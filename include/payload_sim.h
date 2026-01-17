#ifndef PAYLOAD_SIM_H
#define PAYLOAD_SIM_H

#include <stdint.h>

/**
 * @brief Fills a buffer with simulated science data.
 * * @param buffer Pointer to the destination buffer
 * @param length Number of bytes to generate
 */
void PayloadSim_GenerateData(uint8_t* buffer, uint16_t length);

#endif