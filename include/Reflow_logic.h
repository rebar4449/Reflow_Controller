#include <PID_v1.h>

extern MCP9600Manager mcp9600;
extern JoystickAxis AXIS_X;
extern JoystickAxis AXIS_Y;

// Profile structure definition (needed for reflow logic)
typedef struct {
  char      title[20];         // "Lead 183"
  char      alloy[20];         // "Sn63/Pb37"
  uint16_t  melting_point;     // 183

  uint16_t  temp_range_0;      // 30
  uint16_t  temp_range_1;      // 235

  uint16_t  time_range_0;      // 0
  uint16_t  time_range_1;      // 340

  char      reference[100];    // "https://www.chipquik.com/datasheets/TS391AX50.pdf"

  uint16_t  stages_preheat_0;  // 30
  uint16_t  stages_preheat_1;  // 100

  uint16_t  stages_soak_0;     // 120
  uint16_t  stages_soak_1;     // 150

  uint16_t  stages_reflow_0;   // 150
  uint16_t  stages_reflow_1;   // 183

  uint16_t  stages_cool_0;     // 240
  uint16_t  stages_cool_1;     // 183
} profile_t;

extern bool isFault;
extern String activeStatus;
extern void loopScreen();
extern int oldTemp;
extern byte state;
extern bool disableMenu;
extern bool profileIsOn;

#define numOfProfiles 10
extern profile_t paste_profile[numOfProfiles];
extern int profileUsed;

// Variables that need to be accessible from main.cpp
extern int windowSize;
extern unsigned long nextCheck;
extern unsigned long nextRead;

// PID control variables
extern double setpoint;
extern double input;
extern double output;
extern double kp;
extern double ki;
extern double kd;
extern int inputInt;
extern unsigned long windowStartTime;
extern unsigned long timerSoak;
extern unsigned long buzzerPeriod;

// Reflow state variables
extern reflowState_t reflowState;
extern reflowStatus_t reflowStatus;
extern debounceState_t debounceState;
extern long lastDebounceTime;
extern switch_t switchStatus;
extern int timerSeconds;
extern int oldTemp;

// Function declarations
// extern void loopScreen(); // TODO: Fix LCD compatibility

/*******************************************************************************
  Title: Reflow Oven Controller
  Version: 1.20
  Date: 26-11-2012
  Company: Rocket Scream Electronics
  Author: Lim Phang Moh
  Website: www.rocketscream.com

  Brief
  =====
  This is an example firmware for our Arduino compatible reflow oven controller.
  The reflow curve used in this firmware is meant for lead-free profile
  (it's even easier for leaded process!). You'll need to use the MAX31855
  library for Arduino if you are having a shield of v1.60 & above which can be
  downloaded from our GitHub repository. Please check our wiki
  (www.rocketscream.com/wiki) for more information on using this piece of code
  together with the reflow oven controller shield.

  Temperature (Degree Celcius)                 Magic Happens Here!
  245-|                                               x  x
      |                                            x        x
      |                                         x              x
      |                                      x                    x
  200-|                                   x                          x
      |                              x    |                          |   x
      |                         x         |                          |       x
      |                    x              |                          |
  150-|               x                   |                          |
      |             x |                   |                          |
      |           x   |                   |                          |
      |         x     |                   |                          |
      |       x       |                   |                          |
      |     x         |                   |                          |
      |   x           |                   |                          |
  30 -| x             |                   |                          |
      |<  60 - 90 s  >|<    90 - 120 s   >|<       90 - 120 s       >|
      | Preheat Stage |   Soaking Stage   |       Reflow Stage       | Cool
   0  |_ _ _ _ _ _ _ _|_ _ _ _ _ _ _ _ _ _|_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
                                                                 Time (Seconds)

  This firmware owed very much on the works of other talented individuals as
  follows:
  ==========================================
  Brett Beauregard (www.brettbeauregard.com)
  ==========================================
  Author of Arduino PID library. On top of providing industry standard PID
  implementation, he gave a lot of help in making this reflow oven controller
  possible using his awesome library.

  ==========================================
  Limor Fried of Adafruit (www.adafruit.com)
  ==========================================
  Author of Arduino MAX6675 library. Adafruit has been the source of tonnes of
  tutorials, examples, and libraries for everyone to learn.

  Disclaimer
  ==========
  Dealing with high voltage is a very dangerous act! Please make sure you know
  what you are dealing with and have proper knowledge before hand. Your use of
  any information or materials on this reflow oven controller is entirely at
  your own risk, for which we shall not be liable.

  Licences
  ========
  This reflow oven controller hardware and firmware are released under the
  Creative Commons Share Alike v3.0 license
  http://creativecommons.org/licenses/by-sa/3.0/
  You are free to take this piece of code, use it and modify it.
  All we ask is attribution including the supporting libraries used in this
  firmware.

  Required Libraries
  ==================
  - Arduino PID Library:
    >> https://github.com/br3ttb/Arduino-PID-Library
  - MAX31855 Library (for board v1.60 & above):
    >> https://github.com/rocketscream/MAX31855
  - MAX6675 Library (for board v1.50 & below):
    >> https://github.com/adafruit/MAX6675-library

  Revision  Description
  ========  ===========
  1.20      Adds supports for v1.60 (and above) of Reflow Oven Controller
            Shield:
            - Uses MAX31855KASA+ chip and pin reassign (allowing A4 & A5 (I2C)
              to be used for user application).
            - Uses analog based switch (allowing D2 & D3 to be used for user
              application).
            Adds waiting state when temperature too hot to start reflow process.
            Corrected thermocouple disconnect error interpretation (MAX6675).
  1.10      Arduino IDE 1.0 compatible.
  1.00      Initial public release.
*******************************************************************************/
// Comment either one the following #define to select your board revision
// Newer board version starts from v1.60 using MAX31855KASA+ chip

// ***** TYPE DEFINITIONS *****
typedef enum REFLOW_STATE
{
  REFLOW_STATE_IDLE,
  REFLOW_STATE_PREHEAT,
  REFLOW_STATE_SOAK,
  REFLOW_STATE_REFLOW,
  REFLOW_STATE_COOL,
  REFLOW_STATE_COMPLETE,
  REFLOW_STATE_TOO_HOT,
  REFLOW_STATE_ERROR
} reflowState_t;

typedef enum REFLOW_STATUS
{
  REFLOW_STATUS_OFF,
  REFLOW_STATUS_ON
} reflowStatus_t;

typedef  enum SWITCH
{
  SWITCH_NONE,
  SWITCH_1,
  SWITCH_2
} switch_t;

typedef enum DEBOUNCE_STATE
{
  DEBOUNCE_STATE_IDLE,
  DEBOUNCE_STATE_CHECK,
  DEBOUNCE_STATE_RELEASE
} debounceState_t;

// ***** CONSTANTS *****
#define TEMPERATURE_ROOM 50
#define TEMPERATURE_SOAK_MIN 150
#define TEMPERATURE_SOAK_MAX 185
#define TEMPERATURE_REFLOW_MAX 220
#define TEMPERATURE_COOL_MIN 100
#define SENSOR_SAMPLING_TIME 1000
#define SOAK_TEMPERATURE_STEP 5
#define SOAK_MICRO_PERIOD 9000
#define DEBOUNCE_PERIOD_MIN 50

// ***** PID PARAMETERS *****
// ***** PRE-HEAT STAGE *****
#define PID_KP_PREHEAT 100
#define PID_KI_PREHEAT 0.025
#define PID_KD_PREHEAT 20
// ***** SOAKING STAGE *****
#define PID_KP_SOAK 300
#define PID_KI_SOAK 0.05
#define PID_KD_SOAK 250
// ***** REFLOW STAGE *****
#define PID_KP_REFLOW 300
#define PID_KI_REFLOW 0.05
#define PID_KD_REFLOW 350
#define PID_SAMPLE_TIME 1000

// ***** PID CONTROL VARIABLES *****
// (These are now declared as extern above)

// PID object declaration (moved to main.cpp)

// Function declaration
void reflow_main();
