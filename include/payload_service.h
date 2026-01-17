#ifndef PAYLOAD_SERVICE_H
#define PAYLOAD_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

// Payload States
typedef enum {
    PL_STATE_OFF,
    PL_STATE_STANDBY,
    PL_STATE_ACTIVE,
    PL_STATE_ERROR,
    PL_STATE_SAFE
} PayloadState_t;

// Payload Commands
typedef enum {
    PL_CMD_INIT,
    PL_CMD_START,
    PL_CMD_STOP,
    PL_CMD_SET_RATE,
    PL_CMD_RESET
} PayloadCmd_t;

// Status codes for the CDHS (Higher-level system)
typedef enum {
    PL_OK,
    PL_ERR_INVALID_STATE,
    PL_ERR_ALREADY_INITIALIZED,
    PL_ERR_HAL_FAILURE
} PayloadStatus_t;

// Telemetry for Housekeeping
typedef struct {
    PayloadState_t current_state;
    uint32_t bytes_generated;
    uint16_t last_cmd_received;
    uint16_t error_counter;
    uint8_t data_rate; // e.g., 1-10 Hz
} PayloadTelemetry_t;

// Public API
void Payload_Init(void);
PayloadStatus_t Payload_ProcessCommand(PayloadCmd_t cmd, uint8_t param);
void Payload_Update(void); // To be called in the main loop
PayloadTelemetry_t Payload_GetTelemetry(void);

#endif