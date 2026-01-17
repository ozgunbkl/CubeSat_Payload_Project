# 📦 CubeSat Payload Subsystem & Simulator

A modular, state-driven Payload Subsystem designed for a CubeSat Flight Software (FSW) stack.  
This module simulates a generic scientific instrument (e.g., optical camera, radiation sensor, RF payload) and manages the full payload data lifecycle—from generation to on-board archiving—within a Software-in-the-Loop (SIL) environment.

> **Note:** The focus of this project is flight software architecture and subsystem interaction, not payload-specific science algorithms.

---

## 🎯 Project Objective

The objective of this subsystem is to demonstrate a deterministic, safety-aware payload software design suitable for integration into a full CubeSat FSW running on an RTOS.

The payload is designed to:
- Operate only in valid, command-driven modes  
- Generate deterministic science data  
- Reliably route data to the On-Board Data Archive  
- Detect and isolate local faults without performing system-level recovery  

---

## 🧠 Architectural Overview

The Payload Subsystem is implemented as a **Finite State Machine (FSM)** combined with a software-based payload simulator.

### 1️⃣ Finite State Machine (FSM)

The payload operates in five deterministic states:

| State    | Description                                      |
|----------|--------------------------------------------------|
| **OFF**      | Initial state, no activity                     |
| **STANDBY**  | Electronics initialized, awaiting commands     |
| **ACTIVE**   | Science data generation and archiving          |
| **ERROR**    | Local fault detected, operations halted        |
| **SAFE**     | Minimal functionality mode for power/safety    |

**State transitions are:**
- Command-driven  
- Strictly validated  
- Rejected if invalid, with error reporting  

This guarantees predictable and testable behavior under all conditions.

---

### 2️⃣ Local Fault Detection & Isolation (FDI)

This subsystem implements **local fault detection and isolation**, not full system recovery.

**Local Faults Detected:**
- Archive service write failures  
- Storage full conditions  
- Invalid state transitions  

**Local Response:**
- Transition to `PL_STATE_ERROR`  
- Increment internal error counters  
- Preserve telemetry for system-level inspection  

> **Note:** System-wide recovery actions (mode switching, watchdog resets, power cycling) are intentionally delegated to a global FDIR subsystem.

---

## 🧩 Integrated Features

### 🔁 Cross-Subsystem Interaction
- Direct integration with the On-Board Data Archive Service  
- Payload never downlinks data directly  
- All science data is routed through standardized storage services  

### 🧪 Payload Simulator
- Deterministic / pseudo-random science data generation  
- Configurable output size and generation rate  
- Enables full verification without physical hardware  

### 📡 Command-Driven Design
Supported commands (typically issued by CDHS):
- `INIT`  
- `START`  
- `STOP`  
- `SET_RATE`  
- `RESET`  

> The payload does not parse communication frames and remains independent of the communications layer.

### 📊 Housekeeping Telemetry
```c
typedef struct {
    PayloadState_t current_state;   // FSM state
    uint32_t bytes_generated;       // Total science data produced
    uint16_t last_cmd_received;     // Last processed command
    uint16_t error_counter;         // Detected local faults
    uint8_t  data_rate;             // Data generation rate
} PayloadTelemetry_t;
```

---

## 🛠️ Technical Design Constraints

- **No dynamic memory allocation**
- **Fixed-size buffers only**
- **Deterministic execution**
- **Hardware-independent**
- **HAL-ready interfaces**
- **RTOS-compatible** (task or scheduled service)
- **Software-in-the-Loop (SIL) friendly**

---

## 🧪 Verification & Validation

Testing is performed using the **Unity Test Framework**.

**Verified Scenarios:**

- **Nominal Mission Flow:** INIT → START → DATA_GEN → STOP
- **Invalid Command Rejection:** Attempting START while in OFF
- **Archive Failure Handling:** Automatic transition to ERROR when storage limits are reached
- **Telemetry Consistency:** Error counters and byte counts validated

---

## 🔧 Software-in-the-Loop (SIL) Integration

This project includes a Software-in-the-Loop test that links the Payload Subsystem with the real On-Board Data Archive service to validate cross-subsystem interaction.

### Build Example

**Build Command:**
```bash
gcc -o test_payload \
  src/payload_service.c \
  src/payload_sim.c \
  ../CubeSat_Archive_Project/src/archive_service.c \
  ../CubeSat_Archive_Project/src/utils.c \
  test/test_payload.c \
  test/unity/unity.c \
  -I include \
  -I test/unity \
  -I ../CubeSat_Archive_Project/include
```

**Execution:**
```bash
./test_payload
```

**This build validates:**

- Payload finite state machine correctness
- Deterministic science data generation
- Archive service integration
- Correct error propagation on storage exhaustion


---

## 📂 Project Structure
```
payload/
 ├── include/
 │    ├── payload_service.h   # Public API and FSM
 │    └── payload_sim.h       # Simulator interface
 ├── src/
 │    ├── payload_service.c   # Core logic and fault handling
 │    └── payload_sim.c       # Data generation
 ├── test/
 │    ├── test_payload.c      # Unity-based tests
 │    └── unity/              # Unity framework
 └── README.md
```

---

## 🚀 Future Integration & Extensions

- **Global FDIR Integration:** Connect payload fault reports to a system-level FDIR engine for autonomous recovery.
- **Time Service Integration:** Add CCSDS / ECSS PUS-compliant timestamps to science packets.
