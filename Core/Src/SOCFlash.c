#include "stm32g4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "SOCFlash.h"


// STM32G474RE FLASH GEOMETRY
// Cat 3 devices: 4KB in single-bank, 2KB in dual-bank mode. Bank 1 starts at 0x08000000, Bank 2 at 0x08080000 for 512KB total.
// Will be storing records in the LAST PAGE of flash (Linked has been updated)

// Flash size (bytes) from system memory (works on STM32)
static inline uint32_t mcu_flash_size_bytes(void)
{
    return ((uint32_t)(*(__IO uint16_t*)FLASHSIZE_BASE)) * 1024u;
}

// DBANK runtime check: dual-bank => 2KB pages, single-bank => 4KB pages (Cat.3)
static inline bool flash_is_dual_bank(void)
{
#ifdef FLASH_OPTR_DBANK
    return (FLASH->OPTR & FLASH_OPTR_DBANK) != 0u;
#else
    return false;
#endif
}

static inline uint32_t flash_page_size_bytes(void)
{
    return flash_is_dual_bank() ? 2048u : 4096u;
}

// Choose storage page: LAST page of flash
static inline uint32_t soc_storage_page_addr(void)
{
    uint32_t flash_bytes = mcu_flash_size_bytes();     // e.g. 512*1024
    uint32_t page_sz     = flash_page_size_bytes();    // 2048 or 4096
    uint32_t flash_end   = FLASH_BASE + flash_bytes;   // end address (exclusive)
    return flash_end - page_sz;                        // start of last page
}

// Bank selection for erase
static inline uint32_t bank_for_addr(uint32_t addr)
{
    // If dual-bank, banks are split evenly by flash size
    if (flash_is_dual_bank()) {
        uint32_t half = FLASH_BASE + (mcu_flash_size_bytes() / 2u);
        return (addr >= half) ? FLASH_BANK_2 : FLASH_BANK_1;
    }
    return FLASH_BANK_1;
}

// Page index for HAL erase (page number from FLASH_BASE)
static inline uint32_t page_index_for_addr(uint32_t addr)
{
    return (addr - FLASH_BASE) / flash_page_size_bytes();
}

#define SOC_SAVE_MIN_INTERVAL_MS      (2000u)
#define SOC_SAVE_SOC_DELTA_THRESHOLD  (0.25f)
#define SOC_MAGIC  (0x534F4343u) // 'SOCC'

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint32_t seq;
    uint32_t saved_at_ms;

    float currentCapacity_Ah;
    float startingCapacity_Ah;
    float currentSOC_percent;

    uint32_t crc32;
    uint32_t padding; // keep size multiple of 8 for doubleword programming
} SocFlashRecord_t;

_Static_assert(sizeof(SocFlashRecord_t) % 8 == 0, "Record size should be 8-byte aligned for flash programming convenience.");


// CRC32 (small, no table)
static uint32_t crc32_compute(const void *data, uint32_t len)
{
    const uint8_t *p = (const uint8_t *)data;
    uint32_t crc = 0xFFFFFFFFu;
    for (uint32_t i = 0; i < len; i++) {
        crc ^= p[i];
        for (uint32_t b = 0; b < 8; b++) {
            uint32_t mask = -(int32_t)(crc & 1u);
            crc = (crc >> 1) ^ (0xEDB88320u & mask);
        }
    }
    return ~crc;
}

static uint32_t record_crc(const SocFlashRecord_t *r)
{
    // crc covers everything except crc32 field itself
    return crc32_compute(r, (uint32_t)offsetof(SocFlashRecord_t, crc32));
}

// FLASH HELPERS
static inline uint32_t flash_storage_base(void)
{
    return soc_storage_page_addr();
}

static inline uint32_t flash_storage_end(void)
{
    return soc_storage_page_addr() + flash_page_size_bytes();
}

static bool flash_unlock_lock(bool unlock)
{
    return (unlock ? (HAL_FLASH_Unlock() == HAL_OK) : (HAL_FLASH_Lock() == HAL_OK));
}

static bool flash_erase_storage_page(void)
{
    uint32_t addr = flash_storage_base();

    FLASH_EraseInitTypeDef erase = {0};
    uint32_t page_error = 0;

    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Banks     = bank_for_addr(addr);
    erase.Page      = page_index_for_addr(addr);
    erase.NbPages   = 1;

    return (HAL_FLASHEx_Erase(&erase, &page_error) == HAL_OK);
}

// Program flash as double-words (64-bit)
static bool flash_program_bytes(uint32_t dst_addr, const void *src, uint32_t len)
{
    if ((dst_addr & 0x7u) != 0u) return false;
    if ((len % 8u) != 0u) return false;

    const uint64_t *p64 = (const uint64_t*)src;
    for (uint32_t off = 0; off < len; off += 8u) {
        uint64_t v = *p64++;
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, dst_addr + off, v) != HAL_OK) {
            return false;
        }
    }
    return true;
}

static inline bool is_erased_u32(uint32_t v) { return v == 0xFFFFFFFFu; }

static bool record_is_valid(const SocFlashRecord_t *r)
{
    if (r->magic != SOC_MAGIC) return false;
    return (r->crc32 == record_crc(r));
}

static const SocFlashRecord_t* find_latest_record(uint32_t *out_next_write_addr)
{
    const uint32_t start = flash_storage_base();
    const uint32_t end   = flash_storage_end();
    const uint32_t step  = sizeof(SocFlashRecord_t);

    const SocFlashRecord_t *latest = NULL;

    for (uint32_t addr = start; addr + step <= end; addr += step) {
        const SocFlashRecord_t *r = (const SocFlashRecord_t*)addr;

        // Unwritten area starts when magic is erased (0xFFFFFFFF)
        if (is_erased_u32(r->magic)) {
            if (out_next_write_addr) *out_next_write_addr = addr;
            return latest;
        }

        if (record_is_valid(r)) {
            if (!latest || r->seq > latest->seq) {
                latest = r;
            }
        }
    }

    // Page full
    if (out_next_write_addr) *out_next_write_addr = end;
    return latest;
}

static uint32_t g_next_write_addr = 0;
static uint32_t g_last_seq = 0;
static uint32_t g_last_save_ms = 0;
static float    g_last_saved_soc = -999.0f;

bool EEPROM_SOC_Persist_Init(socState_t *out_state)
{
    uint32_t next_addr = flash_storage_base();
    const SocFlashRecord_t *latest = find_latest_record(&next_addr);

    g_next_write_addr = next_addr;

    if (!latest) {
        // No valid records
        g_last_seq = 0;
        g_last_save_ms = 0;
        g_last_saved_soc = -999.0f;
        return false;
    }

    g_last_seq = latest->seq;

    if (out_state) {
        // Restore minimal fields into SOC state
        out_state->startingCapacity   = latest->startingCapacity_Ah;
        out_state->currentCapacity    = latest->currentCapacity_Ah;
        out_state->currentCharge_per  = latest->currentSOC_percent;

        // These are runtime fields; caller can re-init them as needed:
        out_state->startTime = latest->saved_at_ms;
        out_state->startingCharge_per = latest->currentSOC_percent;
        out_state->currentCurrent = 0.0f;
        out_state->averageCurrent = 0.0f;
    }

    g_last_save_ms = latest->saved_at_ms;
    g_last_saved_soc = latest->currentSOC_percent;
    return true;
}

static bool write_record(const SocFlashRecord_t *rec)
{
    // If full, erase and start over
    if (g_next_write_addr == 0) {
        g_next_write_addr = flash_storage_base();
    }

    if (g_next_write_addr + sizeof(SocFlashRecord_t) > flash_storage_end()) {
        if (!flash_unlock_lock(true)) return false;
        bool ok = flash_erase_storage_page();
        (void)flash_unlock_lock(false);
        if (!ok) return false;

        g_next_write_addr = flash_storage_base();
    }

    // Program
    if (!flash_unlock_lock(true)) return false;
    bool ok = flash_program_bytes(g_next_write_addr, rec, sizeof(SocFlashRecord_t));
    (void)flash_unlock_lock(false);

    if (ok) {
        g_next_write_addr += sizeof(SocFlashRecord_t);
    }
    return ok;
}

bool EEPROM_SOC_Persist_ForceSave(const socState_t *state, uint32_t now_ms)
{
    if (!state) return false;

    SocFlashRecord_t rec;
    memset(&rec, 0xFF, sizeof(rec)); // not required, but keeps erased-like padding if any

    rec.magic = SOC_MAGIC;
    rec.seq = g_last_seq + 1u;
    rec.saved_at_ms = now_ms;

    rec.startingCapacity_Ah  = state->startingCapacity;
    rec.currentCapacity_Ah   = state->currentCapacity;
    rec.currentSOC_percent   = state->currentCharge_per;

    rec.crc32 = record_crc(&rec);

    bool ok = write_record(&rec);
    if (ok) {
        g_last_seq = rec.seq;
        g_last_save_ms = now_ms;
        g_last_saved_soc = state->currentCharge_per;
    }
    return ok;
}

void EEPROM_SOC_Persist_SaveIfNeeded(const socState_t *state, uint32_t now_ms)
{
    if (!state) return;

    // First time
    if (g_last_saved_soc < -100.0f) {
        (void)EEPROM_SOC_Persist_ForceSave(state, now_ms);
        return;
    }

    float dsoc = state->currentCharge_per - g_last_saved_soc;
    if (dsoc < 0.0f) dsoc = -dsoc;

    // Rate limit unless SOC moved a lot
    if ((now_ms - g_last_save_ms) < SOC_SAVE_MIN_INTERVAL_MS &&
        dsoc < SOC_SAVE_SOC_DELTA_THRESHOLD) {
        return;
    }

    (void)EEPROM_SOC_Persist_ForceSave(state, now_ms);
}

bool EEPROM_SOC_Persist_EraseAll(void)
{
    if (!flash_unlock_lock(true)) return false;
    bool ok = flash_erase_storage_page();
    flash_unlock_lock(false);

    if (ok) {
        g_next_write_addr = flash_storage_base();
        g_last_seq = 0;
        g_last_save_ms = 0;
        g_last_saved_soc = -999.0f;
    }
    return ok;
}