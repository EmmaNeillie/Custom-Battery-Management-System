#include "utilities.h"

float EMA_Filter(float new_sample, float prev_filtered, float alpha)
{
    return (alpha * new_sample) + ((1.0f - alpha) * prev_filtered);
}

float Median_Filter(float* samples, uint8_t sample_count)
{
    // Simple bubble sort to sort the samples
    for (uint8_t i = 0; i < sample_count - 1; i++)
    {
        for (uint8_t j = 0; j < sample_count - i - 1; j++)
        {
            if (samples[j] > samples[j + 1])
            {
                float temp = samples[j];
                samples[j] = samples[j + 1];
                samples[j + 1] = temp;
            }
        }
    }

    // Return the median value
    if (sample_count % 2 == 0)
    {
        return (samples[sample_count / 2 - 1] + samples[sample_count / 2]) / 2.0f;
    }
    else
    {
        return samples[sample_count / 2];
    }
}