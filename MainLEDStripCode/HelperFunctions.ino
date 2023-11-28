uint8_t fadeCalc2(uint8_t target, uint8_t current, uint8_t speed) {
  int retval = current;
  float ratio = (float)speed / (float)100;
  if (target != current) {
    if (current > target) {
      retval = current - (uint8_t)(ratio * (current - target)) - 1;
      if (retval < target) {
        retval = target;
      }
    } else {
      retval = current + (uint8_t)(ratio * (target - current)) + 1;
      if (retval > target) {
        retval = target;
      }
    }
  }
  return (uint8_t)retval;
}

uint32_t evenBetterFade(uint32_t pixelColor, uint8_t Rt, uint8_t Gt, uint8_t Bt, uint8_t speed) {
  uint8_t Rc = (pixelColor >> 16) & 0xFF;
  uint8_t Gc = (pixelColor >> 8) & 0xFF;
  uint8_t Bc = pixelColor & 0xFF;
  uint8_t Rr = fadeCalc2(Rt, Rc, speed);
  uint8_t Gr = fadeCalc2(Gt, Gc, speed);
  uint8_t Br = fadeCalc2(Bt, Bc, speed);
  return strip0.Color(Rr, Gr, Br);
}


uint8_t fadeCalc(uint8_t target, uint8_t current, uint8_t speed) {
  int retval = current;
  if (target != current) {
    if (current > target) {
      retval = current - speed;
      if (retval < target) {
        retval = target;
      }
    } else {
      retval = current + speed;
      if (retval > target) {
        retval = target;
      }
    }
  }
  return (uint8_t)retval;
}

uint32_t betterFade(uint32_t pixelColor, uint8_t Rt, uint8_t Gt, uint8_t Bt, uint8_t speed) {
  uint8_t Rc = (pixelColor >> 16) & 0xFF;
  uint8_t Gc = (pixelColor >> 8) & 0xFF;
  uint8_t Bc = pixelColor & 0xFF;
  uint8_t Rr = fadeCalc(Rt, Rc, speed);
  uint8_t Gr = fadeCalc(Gt, Gc, speed);
  uint8_t Br = fadeCalc(Bt, Bc, speed);
  return strip0.Color(Rr, Gr, Br);
}

uint32_t fade(uint32_t pixelColor, int r1, int g1, int b1, uint8_t speed) {
  int r = (pixelColor >> 16) & 0xFF;
  int g = (pixelColor >> 8) & 0xFF;
  int b = pixelColor & 0xFF;
  if ((r - r1) < speed) {
    r = 0;
  }
  if ((g - g1) < speed) {
    g = 0;
  }
  if ((b - b1) < speed) {
    b = 0;
  }
  return strip0.Color((uint8_t)(r - (r - r1) / speed), (uint8_t)(g - (g - g1) / speed), (uint8_t)(b - (b - b1) / speed));
}

void setColor(uint8_t arrayIn[3], uint8_t r, uint8_t g, uint8_t b) {
  arrayIn[0] = g;
  arrayIn[1] = r;
  arrayIn[2] = b;
}

colorType createColor(uint8_t r, uint8_t g, uint8_t b) {
  colorType retval = {};
  retval.r = r;
  retval.g = g;
  retval.b = b;
  return retval;
}

void clearArray(uint8_t arrayIn[NUMPIXELS][3]) {
  for (int i = 0; i < NUMPIXELS; i++) {
    arrayIn[i][0] = 0;
    arrayIn[i][1] = 0;
    arrayIn[i][2] = 0;
  }
}

void clearArrayAdv(uint8_t arrayIn[NUMPIXELS][3], uint numofpixles) {
  for (int i = 0; i < numofpixles; i++) {
    arrayIn[i][0] = 0;
    arrayIn[i][1] = 0;
    arrayIn[i][2] = 0;
  }
}

void clearAllArrays() {
  for (int i = 0; i < NUMSTRIPS; i++) {
    clearArray(striparrays[i]);
    ledStrips[i].clear();
    stripStates[i].animation = IDLE_ANIMATION;
    ledStrips[i].show();
  }
}

void orbFunction(Adafruit_NeoPixel &ledStrip, uint8_t striparray[NUMSTRIPARRAY][3], uint8_t stripId, uint rate, uint spacing, uint direction, colorType orbColor1, colorType orbColor2) {

  static unsigned long nextTime[NUMSTRIPS] = {};
  static int position[NUMSTRIPS] = {};
  if (rate > UPDATE_RATE) {
    rate = UPDATE_RATE;
  }
  if (millis() - ((1000 / rate) + 1) > nextTime[stripId]) {
    position[stripId] = 0;
    nextTime[stripId] = millis();
  }
  if (millis() > nextTime[stripId]) {
    if (direction == 0) {
      position[stripId]++;
    } else {
      position[stripId]--;
    }
    nextTime[stripId] = nextTime[stripId] + 1000 / rate;
  }
  if (position[stripId] >= spacing || position[stripId] < 0) {
    if (direction == 0) {
      position[stripId] = 0;
    } else {
      position[stripId] = spacing - 1;
    }
  }

  clearArrayAdv(striparray, NUMPIXELS + 10);

  for (int i = 0; i < NUMPIXELS + 10; i++) {
    if (i % spacing == position[stripId]) {
      if (direction == 0) {
        //setColor(striparray[i + 7], 100, 100, 100);
        setColor(striparray[i + 6], orbColor1.r, orbColor1.g, orbColor1.b);
        setColor(striparray[i + 5], orbColor1.r, orbColor1.g, orbColor1.b);
        setColor(striparray[i + 4], orbColor1.r, orbColor1.g, orbColor1.b);
        setColor(striparray[i + 3], orbColor2.r, orbColor2.g, orbColor2.b);
        setColor(striparray[i + 2], orbColor2.r, orbColor2.g, orbColor2.b);
        setColor(striparray[i + 1], orbColor2.r, orbColor2.g, orbColor2.b);
      } else {
        //setColor(striparray[i + 1], 100, 100, 100);
        setColor(striparray[i + 2], orbColor1.r, orbColor1.g, orbColor1.b);
        setColor(striparray[i + 3], orbColor1.r, orbColor1.g, orbColor1.b);
        setColor(striparray[i + 4], orbColor1.r, orbColor1.g, orbColor1.b);
        setColor(striparray[i + 5], orbColor2.r, orbColor2.g, orbColor2.b);
        setColor(striparray[i + 6], orbColor2.r, orbColor2.g, orbColor2.b);
        setColor(striparray[i + 7], orbColor2.r, orbColor2.g, orbColor2.b);
      }
    }
  }
  for (int i = 0; i < NUMPIXELS; i++) {
    uint32_t pixelColor = ledStrip.getPixelColor(i);

    //if (striparray[i + 10][0] != 0 || striparray[i + 10][1] != 0 || striparray[i + 10][2] != 0) {
    //  ledStrip.setPixelColor(i, ledStrip.Color(striparray[i + 10][0], striparray[i + 10][1], striparray[i + 10][2]));
    //} else {
    //ledStrip.setPixelColor(i, betterFade(pixelColor, striparray[i + 10][0], striparray[i + 10][1], striparray[i + 10][2], 10));
    ledStrip.setPixelColor(i, evenBetterFade(pixelColor, striparray[i + 10][0], striparray[i + 10][1], striparray[i + 10][2], 20));
    //}
  }
}

void playBarFunction(Adafruit_NeoPixel &ledStrip, uint8_t striparray[NUMSTRIPARRAY][3], uint8_t stripId, uint rate, uint direction, uint startLed, uint endLed, colorType orbColor1, colorType orbColor2) {
  static unsigned long nextTime[NUMSTRIPS] = {};
  static uint position[NUMSTRIPS] = {};
  if (rate > UPDATE_RATE) {
    rate = UPDATE_RATE;
  }
  if (millis() - ((1000 / rate) + 1) > nextTime[stripId]) {
    if (direction == 0) {
      position[stripId] = startLed;
    } else {
      position[stripId] = startLed + 9;
    }
    nextTime[stripId] = millis() + 1000 / rate;
    clearArrayAdv(striparray, NUMPIXELS + 10);
  }
  if (millis() > nextTime[stripId]) {
    if (direction == 0) {
      if (position[stripId] <= NUMPIXELS + 10) {
        position[stripId]++;
      }
    } else {
      if (position[stripId] != 0) {
        position[stripId]--;
      }
    }
    nextTime[stripId] = nextTime[stripId] + 1000 / rate;
  }

  for (int i = 0; i < NUMPIXELS + 10; i++) {
    if (direction == 0) {
      if (i == position[stripId]) {
        setColor(striparray[i + 7], 100, 100, 100);
        setColor(striparray[i + 6], orbColor1.r, orbColor1.g, orbColor1.b);
        setColor(striparray[i + 5], orbColor1.r, orbColor1.g, orbColor1.b);
        setColor(striparray[i + 4], orbColor1.r, orbColor1.g, orbColor1.b);
        setColor(striparray[i + 3], orbColor2.r, orbColor2.g, orbColor2.b);
        setColor(striparray[i + 2], orbColor2.r, orbColor2.g, orbColor2.b);
        setColor(striparray[i + 1], orbColor2.r, orbColor2.g, orbColor2.b);
      }
    } else {
      if (i == position[stripId]) {
        setColor(striparray[i + 1], 100, 100, 100);
        setColor(striparray[i + 2], orbColor1.r, orbColor1.g, orbColor1.b);
        setColor(striparray[i + 3], orbColor1.r, orbColor1.g, orbColor1.b);
        setColor(striparray[i + 4], orbColor1.r, orbColor1.g, orbColor1.b);
        setColor(striparray[i + 5], orbColor2.r, orbColor2.g, orbColor2.b);
        setColor(striparray[i + 6], orbColor2.r, orbColor2.g, orbColor2.b);
        setColor(striparray[i + 7], orbColor2.r, orbColor2.g, orbColor2.b);
      }
    }
  }
  if (direction == 0) {
    for (int i = startLed; i < endLed; i++) {
      uint32_t pixelColor = ledStrip.getPixelColor(i);
      ledStrip.setPixelColor(i, evenBetterFade(pixelColor, striparray[i + 10][0], striparray[i + 10][1], striparray[i + 10][2], 20));
    }
  } else {
    for (int i = endLed; i < startLed; i++) {
      uint32_t pixelColor = ledStrip.getPixelColor(i);
      ledStrip.setPixelColor(i, evenBetterFade(pixelColor, striparray[i + 10][0], striparray[i + 10][1], striparray[i + 10][2], 20));
    }
  }
}

void solidColorAlternating(Adafruit_NeoPixel &ledStrip, uint8_t striparray[NUMSTRIPARRAY][3], uint8_t stripId, uint rate, colorType Color1, colorType Color2) {
  static unsigned long nextTime[NUMSTRIPS] = {};
  static bool state[NUMSTRIPS] = {};

  if (millis() > nextTime[stripId]) {
    state[stripId] = !state[stripId];
    nextTime[stripId] = millis() + rate;
  }

  for (int i = 0; i < NUMPIXELS; i++) {
    uint32_t pixelColor = ledStrip.getPixelColor(i);
    if (state[stripId]) {
      setColor(striparray[i], Color1.r, Color1.g, Color1.b);
    } else {
      setColor(striparray[i], Color2.r, Color2.g, Color2.b);
    }
    ledStrip.setPixelColor(i, betterFade(pixelColor, striparray[i][0], striparray[i][1], striparray[i][2], 10));
  }
}

void solidColor(Adafruit_NeoPixel &ledStrip, uint8_t striparray[NUMPIXELS][3], colorType color) {

  for (int i = 0; i < NUMPIXELS; i++) {
    uint32_t pixelColor = ledStrip.getPixelColor(i);
    setColor(striparray[i], color.r, color.g, color.b);
    ledStrip.setPixelColor(i, betterFade(pixelColor, striparray[i][0], striparray[i][1], striparray[i][2], 2));
  }
}

uint32_t lastPixelState[NUMSTRIPS][NUMPIXELS];

// Function to check if the NeoPixels array has changed
bool hasPixelStateChanged(Adafruit_NeoPixel &ledStrip, uint8_t stripId) {
  for (int i = 0; i < ledStrip.numPixels(); i++) {
    if (ledStrip.getPixelColor(i) != lastPixelState[stripId][i]) {
      return true;  // Return true if any pixel has changed
    }
  }
  return false;  // Return false if no changes detected
}

// Function to update the extra copy of the NeoPixels array
void storePixelState(Adafruit_NeoPixel &ledStrip, uint8_t stripId) {
  for (int i = 0; i < ledStrip.numPixels(); i++) {
    lastPixelState[stripId][i] = ledStrip.getPixelColor(i);
  }
}


void parseData(String data) {
  // Print the received data
  // Serial.print("Received Data: ");
  // Serial.println(data);

  // Reset the array and counter
  numValues = 0;
  memset(parsedValues, 0, sizeof(parsedValues));

  // Tokenize the data based on commas
  char *token = strtok(const_cast<char *>(data.c_str()), ",");
  while (token != nullptr && numValues < MAX_VALUES) {
    // Convert the token to an integer and store in the array
    parsedValues[numValues++] = atoi(token);

    // Move to the next token
    token = strtok(nullptr, ",");
  }

  // Print the parsed values
  // Serial.print("Parsed Values: ");
  // for (int i = 0; i < numValues; ++i) {
  //   Serial.print(parsedValues[i]);
  //   Serial.print(" ");
  // }
  // Serial.println();
}

void clearEvents() {
  eventCount = 0;
}

void resetEvents() {
  for (int j = 0; j < eventCount; j++) {
    stripEvents[j].hasrun = false;
  }
}

void printStripState(const stripStateType &state) {
  Serial.println("Strip State:");
  Serial.print("Animation: ");
  Serial.println(state.animation);
  Serial.print("Idle Color (R, G, B): ");
  Serial.print(state.idleColor.r);
  Serial.print(", ");
  Serial.print(state.idleColor.g);
  Serial.print(", ");
  Serial.println(state.idleColor.b);
  Serial.print("Solid Color (R, G, B): ");
  Serial.print(state.solidColor.r);
  Serial.print(", ");
  Serial.print(state.solidColor.g);
  Serial.print(", ");
  Serial.println(state.solidColor.b);
  Serial.print("Orb Rate: ");
  Serial.println(state.orb_rate);
  Serial.print("Orb Spacing: ");
  Serial.println(state.orb_spacing);
  Serial.print("Orb Direction: ");
  Serial.println(state.orb_direction);
  Serial.print("Orb Color 1 (R, G, B): ");
  Serial.print(state.orb_color1.r);
  Serial.print(", ");
  Serial.print(state.orb_color1.g);
  Serial.print(", ");
  Serial.println(state.orb_color1.b);
  Serial.print("Orb Color 2 (R, G, B): ");
  Serial.print(state.orb_color2.r);
  Serial.print(", ");
  Serial.print(state.orb_color2.g);
  Serial.print(", ");
  Serial.println(state.orb_color2.b);
  Serial.print("Playbar Rate: ");
  Serial.println(state.playbar_rate);
  Serial.print("Playbar Direction: ");
  Serial.println(state.playbar_direction);
  Serial.print("Playbar Color 1 (R, G, B): ");
  Serial.print(state.playbar_color1.r);
  Serial.print(", ");
  Serial.print(state.playbar_color1.g);
  Serial.print(", ");
  Serial.println(state.playbar_color1.b);
  Serial.print("Playbar Color 2 (R, G, B): ");
  Serial.print(state.playbar_color2.r);
  Serial.print(", ");
  Serial.print(state.playbar_color2.g);
  Serial.print(", ");
  Serial.println(state.playbar_color2.b);
  Serial.print("Alternating Rate: ");
  Serial.println(state.alternating_rate);
  Serial.print("Alternating Color 1 (R, G, B): ");
  Serial.print(state.alternating_color1.r);
  Serial.print(", ");
  Serial.print(state.alternating_color1.g);
  Serial.print(", ");
  Serial.println(state.alternating_color1.b);
  Serial.print("Alternating Color 2 (R, G, B): ");
  Serial.print(state.alternating_color2.r);
  Serial.print(", ");
  Serial.print(state.alternating_color2.g);
  Serial.print(", ");
  Serial.println(state.alternating_color2.b);
  Serial.println();
}

void printEventType(const eventType &event) {
  Serial.println("Event Type:");
  Serial.print("Has Run: ");
  Serial.println(event.hasrun);
  Serial.print("Strip Number: ");
  Serial.println(event.stripNum);
  Serial.print("Start Time: ");
  Serial.println(event.startTime);
  Serial.println("Strip State: (Excluded)");
  Serial.println();
}

void rainbowTest(Adafruit_NeoPixel &ledStrip, uint8_t stripId) {
  static int firstPixelHue[NUMSTRIPS] = {};  // First pixel starts at red (hue 0)

  ledStrip.clear();
  for (int b = 0; b < 3; b++) {  //  'b' counts from 0 to 2...
                                 //   Set all pixels in RAM to 0 (off)
    // 'c' counts up from 'b' to end of strip in increments of 3...
    for (int c = b; c < ledStrip.numPixels(); c += 3) {
      // hue of pixel 'c' is offset by an amount to make one full
      // revolution of the color wheel (range 65536) along the length
      // of the strip (strip.numPixels() steps):
      int hue = firstPixelHue[stripId] + c * 65536L / (ledStrip.numPixels() / 2);
      uint32_t color = ledStrip.gamma32(ledStrip.ColorHSV(hue));  // hue -> RGB
      ledStrip.setPixelColor(c, color);                           // Set pixel 'c' to value 'color'
    }
    firstPixelHue[stripId] += 65536 / 200;  // One cycle of color wheel over 90 frames
  }
}