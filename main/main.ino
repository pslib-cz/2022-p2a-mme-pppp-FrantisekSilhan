#include <FastLED.h>

//piny
#define MODE_LEDS_DATA_PIN 3
#define GROUP_LEDS_DATA_PIN 2
const int modeButtonPin = 5;
const int groupButtonPin = 6;
const int stepButtonPin = 4;

//globální proměnné
#define MODE_LEDS 1
#define GROUP_LEDS 5
bool lastModeButton = false;
bool lastGroupButton = false;
bool lastStepButton = false;

const int modes = 3;
int mode = -1;
int group = -1;

int modeRandomGroupData[5];
int modeRandomGroupIndex = 0;

CRGB colors[5] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Cyan, CRGB::Yellow};

CRGB mode_leds[MODE_LEDS];
CRGB group_leds[GROUP_LEDS];

int modeYesNoChance = 50;
int modeGroupGroup = -1;
bool yes;

//funkce
void setup() { 
  FastLED.addLeds<WS2812B, MODE_LEDS_DATA_PIN, RGB>(mode_leds, MODE_LEDS);
  FastLED.addLeds<WS2812B, GROUP_LEDS_DATA_PIN, RGB>(group_leds, GROUP_LEDS);
  FastLED.setBrightness(50);

  advanceMode();
}

int modAdvance(int number, int mod) {
  return (number + 1) % mod;
}

void advanceMode() {
  mode = modAdvance(mode, modes);
  mode_leds[0] = colors[mode];
}

void advanceGroup() {
  group = modAdvance(group, GROUP_LEDS);
  for (int i = 0; i < GROUP_LEDS; i++) {
    group_leds[i] = i <= group ? colors[i] : CRGB::Black;
  }
}


bool modeYesNo() {
  const int number = random(0, 100);
  if (number < modeYesNoChance) {
    modeYesNoChance -= 10;
    return false;
  } else {
    modeYesNoChance = 50;
    return true;
  }
}


void modeGroup(int number) {
  if (number == -2) {
    modeGroupGroup = modAdvance(modeGroupGroup, GROUP_LEDS);
    number = modeGroupGroup;
  }
  for (int i = 0; i < GROUP_LEDS; i++) {
    group_leds[i] = i == number ? colors[i] : CRGB::Black;
  }
}

int generateRandomGroup() {
  for (int i = 0; i <= group; i++) {
    int r = random(group+1);
    bool found = false;

    for (int j = 0; j < modeRandomGroupIndex; j++) {
      if (modeRandomGroupData[j] == r) {
        found = true;
        break;
      }
    }
    if (!found) {
      modeRandomGroupData[modeRandomGroupIndex] = r;
      modeRandomGroupIndex++;
      if (modeRandomGroupIndex > group) {
        modeRandomGroupIndex = 0;
        memset(modeRandomGroupData, 0, sizeof(modeRandomGroupData));
      }
      return r;
    }
  }
}

void step() {
  switch(mode) {
    case 0:
      yes = modeYesNo();
      for (int i = 0; i < GROUP_LEDS; i++) {
          group_leds[i] = yes ? colors[0] : colors[1];
      }
      break;
    case 1:
      modeGroup(-2);
      break;
    case 2:
      modeGroup(generateRandomGroup());
      break;
  }
}

void loop() {
  const bool modeButton = digitalRead(modeButtonPin);
  if (modeButton != lastModeButton && modeButton) {
    advanceMode();

    modeGroupGroup = -1;
  }
  lastModeButton = modeButton;

  const bool groupButton = digitalRead(groupButtonPin);
  if (groupButton != lastGroupButton && groupButton) {
    advanceGroup();

    modeGroupGroup = -1;
  }
  lastGroupButton = groupButton;

  const bool stepButton = digitalRead(stepButtonPin);
  if (stepButton != lastStepButton && stepButton) {
    step();
  }
  lastStepButton = stepButton;

  delay(100);
  FastLED.show();
}
