#include "payload_service.h"
#include "payload_sim.h"
#include "archive_service.h"
#include <string.h>
#include <stdio.h>

#define PAYLOAD_RECORD_ID 0x50   // 'P' for Payload
#define SIM_DATA_SIZE 64    // Size of one "Science Packet"


// Private internal state
static PayloadTelemetry_t pl_telemetry;

void Payload_Init(void) {
    memset(&pl_telemetry, 0, sizeof(PayloadTelemetry_t));
    pl_telemetry.current_state = PL_STATE_OFF;
}

void Payload_Update(void){
    PayloadTelemetry_t* tl = &pl_telemetry;

    // 1. Only do work if the payload is ACTIVE
    if(tl->current_state != PL_STATE_ACTIVE){
        return;
    }

    // 2. Logic to handle the data rate 
    // In SIL, we simulate this. Every time Update is called, 
    // we assume one "cycle" has passed.
    static uint32_t cycle_count = 0;
    cycle_count++;

    // If data_rate is 5Hz, and our loop runs at 10Hz, 
    // we produce data every 2 cycles.
    // For now and simplicity: generate data every update
    uint8_t raw_data[SIM_DATA_SIZE];
    PayloadSim_GenerateData(raw_data, SIM_DATA_SIZE);

    // 3. Route the data to the Archive Service
    ArchiveStatus_t arc_status = Archive_WriteRecord(PAYLOAD_RECORD_ID, raw_data, SIM_DATA_SIZE);
    if (arc_status == ARCHIVE_OK) {
        tl->bytes_generated += SIM_DATA_SIZE;
    } else {
        // If the Archive is full, the payload should move to ERROR state
        printf("DEBUG: Archive rejected write! Status Code: %d, Size: %d\n", arc_status, SIM_DATA_SIZE);
        tl->current_state = PL_STATE_ERROR;
        tl->error_counter++;
    } 
}

PayloadStatus_t Payload_ProcessCommand(PayloadCmd_t cmd, uint8_t param) {
    pl_telemetry.last_cmd_received = cmd;

    switch (cmd) {
        case PL_CMD_INIT:
            if (pl_telemetry.current_state == PL_STATE_OFF || pl_telemetry.current_state == PL_STATE_SAFE){
                pl_telemetry.current_state = PL_STATE_STANDBY;
                return PL_OK;
            }
            break;
        
        case PL_CMD_START:
            if (pl_telemetry.current_state == PL_STATE_STANDBY) {
                pl_telemetry.current_state = PL_STATE_ACTIVE;
                return PL_OK;
            }
            break;

        case PL_CMD_STOP:
            if (pl_telemetry.current_state == PL_STATE_ACTIVE) {
                pl_telemetry.current_state = PL_STATE_STANDBY;
                return PL_OK;
            }
            break;

        case PL_CMD_RESET:
            pl_telemetry.current_state = PL_STATE_OFF;
            pl_telemetry.error_counter = 0;
            return PL_OK;

        case PL_CMD_SET_RATE:
            if(param > 0 && param <= 10){   //Limit 10Hz
                pl_telemetry.data_rate = param;
                return PL_OK;
            }
            break;

    }
    
    // If we reach here, the command was invalid for the current state
    return PL_ERR_INVALID_STATE;
}

PayloadTelemetry_t Payload_GetTelemetry(void) {
    return pl_telemetry;
}

