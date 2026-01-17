#include <unity.h>
#include "payload_service.h"
#include "archive_service.h"
#include <string.h>


// This function is called before each test
void setUp(void) {
    Archive_Init();
    Payload_Init();
}

void tearDown(void) {
    // Clean up after each test if necessary
}

// --- TEST 1: The Nominal Mission (Happy Path) ---
void test_Payload_NominalTransition(void) {
    PayloadStatus_t status;
    PayloadTelemetry_t tl;

    // 1. Start at OFF , Move to STANDBY
    status = Payload_ProcessCommand(PL_CMD_INIT, 0);
    TEST_ASSERT_EQUAL(PL_OK, status);

    tl = Payload_GetTelemetry();
    TEST_ASSERT_EQUAL(PL_STATE_STANDBY, tl.current_state);
 
    // 2. Move to ACTIVE
    status = Payload_ProcessCommand(PL_CMD_START, 0);
    TEST_ASSERT_EQUAL(PL_OK, status);

    tl = Payload_GetTelemetry();
    TEST_ASSERT_EQUAL(PL_STATE_ACTIVE, tl.current_state);

    // 3. Move back to STANDBY
    status = Payload_ProcessCommand(PL_CMD_STOP, 0);
    TEST_ASSERT_EQUAL(PL_OK, status);

    tl = Payload_GetTelemetry();
    TEST_ASSERT_EQUAL(PL_STATE_STANDBY, tl.current_state);
}

// --- TEST 2: Command Protection (Illegal Actions) ---
void test_Payload_RejectIllegalStart(void) {
    PayloadStatus_t status;

    // Payload starts in OFF state
    // Try to START without INIT
    status = Payload_ProcessCommand(PL_CMD_START, 0);

    // Expectation: The software should REJECT this command
    TEST_ASSERT_EQUAL(PL_ERR_INVALID_STATE, status);

    PayloadTelemetry_t tl = Payload_GetTelemetry();
    TEST_ASSERT_EQUAL(PL_STATE_OFF, tl.current_state);  // Ensure state did not change

}


// --- TEST 3: Parameter Validation ---
void test_Payload_RejectInvalidDataRate(void) {
    // Set rate to 0 or 255 (out of valid range 1-10)
    PayloadStatus_t status = Payload_ProcessCommand(PL_CMD_SET_RATE, 50);

    // Expectation: The software should REJECT this command
    TEST_ASSERT_EQUAL(PL_ERR_INVALID_STATE, status);
}

void test_Payload_GeneratesDataInActiveState(void) {
    // 1. Setup: Move to ACTIVE
    Payload_ProcessCommand(PL_CMD_INIT, 0);
    Payload_ProcessCommand(PL_CMD_START, 0);

    // 2. Action: Call Update (Simulate 1 cycle)
    Payload_Update();

    // 3. Verification: Telemetry should show data was generated
    PayloadTelemetry_t tl = Payload_GetTelemetry();
    TEST_ASSERT_EQUAL(64, tl.bytes_generated);
}

void test_Payload_ArchiveFullTriggersError(void) {
    // 1. Fill the archive (Simulate)
    // We call Update many times until the 4096-byte limit is hit
    for(int i = 0; i < 100; i++) {
        Payload_ProcessCommand(PL_CMD_INIT, 0);
        Payload_ProcessCommand(PL_CMD_START, 0);
        Payload_Update();
    }

    // 2. Eventually, Archive_WriteRecord should return ARCHIVE_ERR_FULL
    // 3. Check if payload moved to ERROR state
    PayloadTelemetry_t tl = Payload_GetTelemetry();
    
    // Once it hits the error, it should stay there
    TEST_ASSERT_EQUAL(PL_STATE_ERROR, tl.current_state);
    TEST_ASSERT_TRUE(tl.error_counter > 0);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_Payload_NominalTransition);
    RUN_TEST(test_Payload_RejectIllegalStart);
    RUN_TEST(test_Payload_RejectInvalidDataRate);
    RUN_TEST(test_Payload_GeneratesDataInActiveState);
    RUN_TEST(test_Payload_ArchiveFullTriggersError);
    
    return UNITY_END();
}