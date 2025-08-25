#ifndef REFLOW_LOGIC_H
#define REFLOW_LOGIC_H

#include <Arduino.h>
#include <PID_v1.h>

// ***** TYPE DEFINITIONS *****
// Reflow state machine types
enum ReflowState {
  REFLOW_STATE_IDLE,
  REFLOW_STATE_PREHEAT,
  REFLOW_STATE_SOAK,
  REFLOW_STATE_REFLOW,
  REFLOW_STATE_COOL,
  REFLOW_STATE_COMPLETE,
  REFLOW_STATE_TOO_HOT,
  REFLOW_STATE_ERROR
};

enum ReflowStatus {
  REFLOW_STATUS_OFF,
  REFLOW_STATUS_ON
};

enum DebounceState {
  DEBOUNCE_STATE_IDLE,
  DEBOUNCE_STATE_CHECK,
  DEBOUNCE_STATE_RELEASE
};

enum Switch {
  SWITCH_NONE,
  SWITCH_1
};

// ***** CONSTANTS *****
// Temperature constants
#define TEMPERATURE_ROOM 50
#define TEMPERATURE_COOL_MIN 50
#define TEMPERATURE_REFLOW_MAX 250

// PID constants
#define PID_KP_PREHEAT 100
#define PID_KI_PREHEAT 0.025
#define PID_KD_PREHEAT 20

#define PID_KP_SOAK 300
#define PID_KI_SOAK 0.05
#define PID_KD_SOAK 250

#define PID_KP_REFLOW 300
#define PID_KI_REFLOW 0.05
#define PID_KD_REFLOW 350

// Timing constants
#define PID_SAMPLE_TIME 1000
#define SENSOR_SAMPLING_TIME 1000
#define SOAK_MICRO_PERIOD 9000
#define SOAK_TEMPERATURE_STEP 5
#define DEBOUNCE_PERIOD_MIN 50

// ***** EXTERNAL VARIABLES *****
// Reflow state variables (extern declarations)
extern ReflowState reflowState;
extern ReflowStatus reflowStatus;
extern DebounceState debounceState;
extern long lastDebounceTime;
extern Switch switchStatus;
extern int timerSeconds;

// Function declaration
void reflow_main();

#endif
