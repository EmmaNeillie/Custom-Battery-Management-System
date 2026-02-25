/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fdcan.h
  * @brief   This file contains all the function prototypes for
  *          the fdcan.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FDCAN_H__
#define __FDCAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */
#include "stm32g4xx_it.h"

extern FDCAN_HandleTypeDef hfdcan1;

extern FDCAN_HandleTypeDef hfdcan2;

/* CAN Message IDs */
// TODO: Update these IDs as per final CAN specification
#define CAN_MSG_ID_BMS_STATUS           0x100  // BMS status message
#define CAN_MSG_ID_CELL_VOLTAGES_1      0x102  // Cell voltages 1-4
#define CAN_MSG_ID_CELL_VOLTAGES_2      0x103  // Cell voltages 5-8
#define CAN_MSG_ID_CELL_VOLTAGES_3      0x104  // Cell voltages 9-12
#define CAN_MSG_ID_TEMPERATURES         0x105  // Temperature readings

#define CAN_MSG_ID_PACK_STATS           0x201  // Pack voltage and current
#define CAN_MSG_ID_FAULT_STATUS         0x207  // Fault/warning flags

/* Accept IDs 0x100 .. 0x208 (standard 11-bit IDs).
  Lower 3 bits vary, so match bits 10..3. Mask = 0b11111111000 = 0x7F8 */
#define FDCAN_ACCEPT_ID   0x100U
#define FDCAN_ACCEPT_MASK 0x7F8U

/* CAN Message Cache Structure */

typedef struct {
    uint16_t    packVoltage_V;      // Pack voltage in volts
    int16_t     packCurrent_A;      // Pack current in amps (signed for charge/discharge)
    uint8_t     packSOH;            // State of Health in percentage (0-100)
    uint8_t     packSOC;            // State of Charge in percentage (0-100)
    uint8_t     boardTemp;          // Board temperature in °C
    uint8_t     SDCstatus;          // SDC status (0 = fault, 1 = normal)
} CAN_MSG_PackStats_t;

typedef struct {
    int8_t segNo;              // Segment Number
    int8_t voltages[6];        // Cell voltages in millivolts
    int8_t checksum;           // Checksum for data integrity
} CAN_MSG_CellVoltages_t;

typedef struct {
    uint8_t segNo;              // Segment Number
    int8_t minTemp;             // Minimum Temperature in tenths of °C
    int8_t maxTemp;             // Maximum Temperature in tenths of °C
    int8_t averageTemp;         // Average Temperature in tenths of °C
    uint8_t thermCount;         // Thermistor count
    uint8_t highestThermID;     // ID of highest thermistor
    uint8_t lowestThermID;      // ID of lowest thermistor
    uint8_t checksum;           // Checksum for data integrity
} CAN_MSG_SegTemps_t;

void MX_FDCAN1_Init(void);
void MX_FDCAN2_Init(void);

/**
 * @brief Configure FDCAN filters and start reception
 * @param hfdcan pointer to FDCAN handle
 * @retval HAL status
 */
HAL_StatusTypeDef FDCAN_Config(FDCAN_HandleTypeDef *hfdcan);

/**
 * @brief Initialize FDCAN1 with filtering and interrupts
 */
void FDCAN1_Config_And_Start(void);

/**
 * @brief Initialize FDCAN2 with filtering and interrupts
 */
void FDCAN2_Config_And_Start(void);

/**
 * @brief Transmit CAN message
 * @param canId CAN identifier
 * @param data Pointer to data buffer
 * @param dataLen Length of data (0-8 bytes)
 * @retval HAL status
 */
HAL_StatusTypeDef CAN_Transmit(uint32_t canId, uint8_t *data, uint8_t dataLen);

#ifdef __cplusplus
}
#endif

#endif /* __FDCAN_H__ */

