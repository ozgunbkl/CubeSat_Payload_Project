#include "payload_service.h"
#include <stdlib.h>

// Simulate a sensor reading (e.g. temperature or radiation level)
void PayloadSim_GenerateData(uint8_t *buffer, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        // Generate pseudo-random "science" data
        buffer[i] = (uint8_t)(rand() % 256);
    }
}

