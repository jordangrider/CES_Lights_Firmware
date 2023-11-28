#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define STRIP0_PIN 0
#define STRIP1_PIN 1
#define STRIP2_PIN 2
#define STRIP3_PIN 3
#define STRIP4_PIN 4

#define STOP_COMMAND 1
#define START_COMMAND 2
#define PAUSE_COMMAND 3
#define RESUME_COMMAND 4
#define CONFIGURE_ANIMATION_COMMAND 10
#define CONFIGURE_IDLE_COMMAND 11
#define CLEAR_EVENTS_COMMAND 12

#define IDLE_ANIMATION 0
#define SOLID_COLOR_ANIMATION 1
#define ORBS_ANIMATION 2
#define PLAYBAR_ANIMATION 3
#define ALTERNATING_ANIMATION 4
#define RAINBOW_TEST_ANIMATION 5

#define NUMPIXELS 180
#define NUMSTRIPARRAY NUMPIXELS + 30

#define NUMSTRIPS 5

#define NUMEVENTS 100

#define MAX_UNSIGNED_LONG (2 ^ 32 - 1)

#define UPDATE_RATE (50)  //Update Rate in Hz
#define UPDATE_INTERVAL (1000 / UPDATE_RATE);

Adafruit_NeoPixel strip0(NUMPIXELS, STRIP0_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip1(NUMPIXELS, STRIP1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(NUMPIXELS, STRIP2_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3(NUMPIXELS, STRIP3_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4(NUMPIXELS, STRIP4_PIN, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel ledStrips[] = { strip0, strip1, strip2, strip3, strip4 };

uint8_t striparrays[NUMSTRIPS][NUMSTRIPARRAY][3] = {};

struct colorType {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct stripStateType {
  uint8_t animation;
  colorType idleColor;
  colorType solidColor;
  uint8_t orb_rate;
  uint8_t orb_spacing;
  uint8_t orb_direction;
  colorType orb_color1;
  colorType orb_color2;
  uint8_t playbar_rate;
  uint8_t playbar_direction;
  colorType playbar_color1;
  colorType playbar_color2;
  uint alternating_rate;
  colorType alternating_color1;
  colorType alternating_color2;
};

struct eventType {
  bool hasrun;
  uint8_t stripNum;
  unsigned long startTime;
  stripStateType stripState;
};

stripStateType stripStates[NUMSTRIPS];
eventType stripEvents[NUMEVENTS];

colorType idleColors[NUMSTRIPS];

uint8_t eventCount = 0;

const int MAX_VALUES = 20;     // Maximum number of values to parse
int parsedValues[MAX_VALUES];  // Array to store parsed values
int numValues = 0;             // Variable to track the number of parsed values

colorType orb_color1 = { 0, 100, 100 };
colorType orb_color2 = { 0, 0, 100 };
colorType playbar_color1 = { 0, 100, 100 };
colorType playbar_color2 = { 2, 5, 15 };
colorType black = { 0, 0, 0 };


static int orbSpacing = 30;
static int orbspeed = 1;

void setup() {

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  for (int i = 0; i < NUMSTRIPS; i++) {
    ledStrips[i].begin();
  }

  Serial.begin(115200);

  for (int i = 0; i < NUMSTRIPS; i++) {
    ledStrips[i].show();  // Display the changes
    stripStates[i].idleColor = black;
  }

  // stripStates[0].animation = PLAYBAR_ANIMATION;
  // stripStates[0].orb_rate = 25;
  // stripStates[0].orb_spacing = 25;
  // stripStates[0].orb_color1 = orb_color1;
  // stripStates[0].orb_color2 = orb_color2;
  // stripStates[0].orb_direction = 0;
  // stripStates[0].playbar_rate = 30;
  // stripStates[0].playbar_direction = 0;
  // stripStates[0].playbar_color1 = playbar_color1;
  // stripStates[0].playbar_color2 = playbar_color2;
  // stripStates[1].animation = PLAYBAR_ANIMATION;
  // stripStates[1].orb_rate = 25;
  // stripStates[1].orb_spacing = 25;
  // stripStates[1].orb_color1 = orb_color1;
  // stripStates[1].orb_color2 = orb_color2;
  // stripStates[1].orb_direction = 1;
  // stripStates[1].playbar_rate = 30;
  // stripStates[1].playbar_direction = 1;
  // stripStates[1].playbar_color1 = playbar_color1;
  // stripStates[1].playbar_color2 = playbar_color2;
}

static unsigned long lastTime = 0;
static unsigned long runningTime = 0;
static unsigned long updateTime = 0;
static uint count = 0;
static uint slowCount = 0;
static bool play = false;
void loop() {

  if (Serial.available() > 0) {
    // Read the incoming data until a newline character is received
    String inputString = Serial.readStringUntil('\n');
    // Parse the comma-separated values and store them in the array
    parseData(inputString);

    if (parsedValues[0] == 51) {
      switch (parsedValues[1]) {
        case STOP_COMMAND:
          Serial.println("Received STOP command");
          play = false;
          count = 0;
          clearAllArrays();
          resetEvents();
          break;

        case START_COMMAND:
          Serial.println("Received START command");
          play = true;
          count = 0;
          runningTime = 0;
          lastTime = millis();
          resetEvents();
          break;

        case PAUSE_COMMAND:
          Serial.println("Received PAUSE command");
          play = false;
          break;

        case RESUME_COMMAND:
          Serial.println("Received RESUME command");
          play = true;
          break;

        case CONFIGURE_ANIMATION_COMMAND:
          Serial.println("Received CONFIGURE_ANIMATION command");
          if (true) {
            bool configurationSuccess = true;

            uint8_t stripNum = parsedValues[2];
            if (stripNum >= 0 && stripNum < NUMSTRIPS) {
              stripEvents[eventCount].stripNum = stripNum;
            } else {
              configurationSuccess = false;
            }

            unsigned long startTime = parsedValues[3];
            if (startTime >= 0 && startTime < 600000) {
              stripEvents[eventCount].startTime = startTime;
            }

            uint8_t animation = parsedValues[4];
            stripEvents[eventCount].stripState.animation = animation;

            switch (parsedValues[4]) {
              case SOLID_COLOR_ANIMATION:
                stripEvents[eventCount].stripState.solidColor = createColor(parsedValues[5], parsedValues[6], parsedValues[7]);
                break;

              case ORBS_ANIMATION:
                if (parsedValues[5] >= 1 && parsedValues[5] <= 50) {
                  stripEvents[eventCount].stripState.orb_rate = parsedValues[5];
                } else {
                  configurationSuccess = false;
                }
                if (parsedValues[6] >= 0 && parsedValues[6] <= 1) {
                  stripEvents[eventCount].stripState.orb_direction = parsedValues[6];
                } else {
                  configurationSuccess = false;
                }
                if (parsedValues[7] >= 10 && parsedValues[7] <= NUMPIXELS) {
                  stripEvents[eventCount].stripState.orb_spacing = parsedValues[7];
                } else {
                  configurationSuccess = false;
                }
                stripEvents[eventCount].stripState.orb_color1 = createColor(parsedValues[8], parsedValues[9], parsedValues[10]);
                stripEvents[eventCount].stripState.orb_color2 = createColor(parsedValues[11], parsedValues[12], parsedValues[13]);
                break;

              case PLAYBAR_ANIMATION:
                if (parsedValues[5] >= 1 && parsedValues[5] <= 50) {
                  stripEvents[eventCount].stripState.playbar_rate = parsedValues[5];
                } else {
                  configurationSuccess = false;
                }
                if (parsedValues[6] >= 0 && parsedValues[6] <= 1) {
                  stripEvents[eventCount].stripState.playbar_direction = parsedValues[6];
                } else {
                  configurationSuccess = false;
                }
                stripEvents[eventCount].stripState.playbar_color1 = createColor(parsedValues[7], parsedValues[8], parsedValues[9]);
                stripEvents[eventCount].stripState.playbar_color2 = createColor(parsedValues[10], parsedValues[11], parsedValues[12]);
                break;

              case ALTERNATING_ANIMATION:
                if (parsedValues[5] >= 20 && parsedValues[5] <= 10000) {
                  stripEvents[eventCount].stripState.alternating_rate = parsedValues[5];
                } else {
                  configurationSuccess = false;
                }
                stripEvents[eventCount].stripState.alternating_color1 = createColor(parsedValues[6], parsedValues[7], parsedValues[8]);
                stripEvents[eventCount].stripState.alternating_color2 = createColor(parsedValues[9], parsedValues[10], parsedValues[11]);
                break;

              case RAINBOW_TEST_ANIMATION:
                break;

              default:
                Serial.println("Invalid Animation ID");
                configurationSuccess = false;
                break;
            }

            if (configurationSuccess) {
              Serial.print("Event Registered as #: ");
              Serial.println(eventCount);
              //printStripState(stripEvents[eventCount].stripState);
              //printEventType(stripEvents[eventCount]);
              eventCount++;
            } else {
              Serial.println("Invalid Event Configuration");
            }
          }
          break;

        case CONFIGURE_IDLE_COMMAND:
          Serial.println("Received CONFIGURE_IDLE command");

          if (true) {
            bool configurationSuccess = true;

            uint8_t stripNum = parsedValues[2];
            if (stripNum >= 0 && stripNum < NUMSTRIPS) {
              stripEvents[eventCount].stripNum = stripNum;
            } else {
              configurationSuccess = false;
            }
            idleColors[stripNum] = createColor(parsedValues[3], parsedValues[4], parsedValues[5]);
            if (configurationSuccess) {
              Serial.print("Idle Configuration Set");
            } else {
              Serial.println("Invalid idle Configuration");
            }
          }
          break;

        case CLEAR_EVENTS_COMMAND:
          Serial.println("Received CLEAR_EVENTS command");
          clearEvents();
          break;

        default:
          Serial.println("Unknown command");
          // Your code for handling an unknown command
          break;
      }
    } else {
      Serial.println("Incorrect start of command number.");
    }
  }

  if (play) {

    for (int j = 0; j < eventCount; j++) {
      if (!stripEvents[j].hasrun) {
        if (runningTime > stripEvents[j].startTime) {
          stripEvents[j].hasrun = true;
          stripStates[stripEvents[j].stripNum] = stripEvents[j].stripState;
          //printStripState(stripStates[stripEvents[j].stripNum]);
          // Serial.print("Playing Animation: ");
          // Serial.print(j);
          // Serial.print(" at time ");
          // Serial.println(runningTime);
        }
      }
    }

    for (int i = 0; i < NUMSTRIPS; i++) {
      switch (stripStates[i].animation) {
        case IDLE_ANIMATION:
          solidColor(ledStrips[i], striparrays[i], idleColors[i]);
          break;

        case SOLID_COLOR_ANIMATION:
          solidColor(ledStrips[i], striparrays[i], stripStates[i].solidColor);
          break;

        case ORBS_ANIMATION:
          orbFunction(ledStrips[i], striparrays[i], i, stripStates[i].orb_rate, stripStates[i].orb_spacing, stripStates[i].orb_direction, stripStates[i].orb_color1, stripStates[i].orb_color2);
          break;

        case PLAYBAR_ANIMATION:
          playBarFunction(ledStrips[i], striparrays[i], i, stripStates[i].playbar_rate, stripStates[i].playbar_direction, stripStates[i].playbar_color1, stripStates[i].playbar_color2);
          break;

        case ALTERNATING_ANIMATION:
          solidColorAlternating(ledStrips[i], striparrays[i], i, stripStates[i].alternating_rate, stripStates[i].alternating_color1, stripStates[i].alternating_color2);
          break;

        case RAINBOW_TEST_ANIMATION:
          rainbowTest(ledStrips[i], i);
          break;

        default:
          // Your code for handling an unknown animation type
          break;
      }
    }

    for (int i = 0; i < NUMSTRIPS; i++) {
      if (hasPixelStateChanged(ledStrips[i], i)) {
        ledStrips[i].show();               // Display the changes
        storePixelState(ledStrips[i], i);  // Update the extra copy
      }
    }

    //Serial.println(updateTime - millis());
    while (millis() < updateTime) {}
    updateTime = millis() + UPDATE_INTERVAL;
    runningTime = runningTime + millis() - lastTime;
    //Serial.println(runningTime);
  } else {
    // for (int i = 0; i < NUMSTRIPS; i++) {
    //   solidColor(ledStrips[i], striparrays[i], idleColors[i]);
    // }
    // for (int i = 0; i < NUMSTRIPS; i++) {
    //   if (hasPixelStateChanged(ledStrips[i], i)) {
    //     ledStrips[i].show();               // Display the changes
    //     storePixelState(ledStrips[i], i);  // Update the extra copy
    //   }
    // }
  }
  lastTime = millis();
}
