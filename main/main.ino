#include <FastLED.h>

//piny
#define MODE_LEDS_DATA_PIN 3
#define GROUP_LEDS_DATA_PIN 2
const int modeButtonPin = 6;
const int groupButtonPin = 5;
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
int modeRandomGroupData[GROUP_LEDS];
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
  advanceGroup();
  for (int i = 0; i < GROUP_LEDS; i++) {
    modeRandomGroupData[i] = i;
  }
  shuffleArray(modeRandomGroupData, GROUP_LEDS);
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
    modeGroupGroup = modAdvance(modeGroupGroup, group+1);
    number = modeGroupGroup;
  }
  for (int i = 0; i < GROUP_LEDS; i++) {
    group_leds[i] = i == number ? colors[i] : CRGB::Black;
  }
}

void shuffleArray(int* array, int size) {
  if (group == 1) return;
  int last = modeRandomGroupData[GROUP_LEDS-1];
  for (int i = size - 1; i > 0; i--) {
    int j = random(i + 1);
    int temp = array[i];
    array[i] = array[j];
    array[j] = temp;
  }
  if (array[0] == last) {
    int j = random(1, GROUP_LEDS+1);
    int temp = array[0];
    array[0] = array[j];
    array[j] = temp;
  }
}

int modeRandomGroup() {
  if (group == 0) {
    return 0;
  }
  if (modeRandomGroupIndex >= group) {
    shuffleArray(modeRandomGroupData, GROUP_LEDS);
  }

  do {
    modeRandomGroupIndex = modAdvance(modeRandomGroupIndex, GROUP_LEDS);
  } while (modeRandomGroupData[modeRandomGroupIndex] > group);

  return modeRandomGroupData[modeRandomGroupIndex];
}

void clearLeds() {
  FastLED.clear();
  FastLED.show();
  mode_leds[0] = colors[mode];
}

void step() {
  clearLeds();
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
      modeGroup(modeRandomGroup());
      break;
  }
}

void loop() {
  const bool modeButton = digitalRead(modeButtonPin);
  if (modeButton != lastModeButton && modeButton) {
    advanceMode();

    modeGroupGroup = -1;
    modeRandomGroupIndex == GROUP_LEDS-1;
    modeYesNoChance = 50;
  }
  lastModeButton = modeButton;

  const bool groupButton = digitalRead(groupButtonPin);
  if (groupButton != lastGroupButton && groupButton) {
    advanceGroup();

    modeGroupGroup = -1;
    modeRandomGroupIndex == GROUP_LEDS-1;
    modeYesNoChance = 50;
  }
  lastGroupButton = groupButton;

  const bool stepButton = digitalRead(stepButtonPin);
  if (stepButton != lastStepButton && stepButton) {
    step();
  }
  lastStepButton = stepButton;

  delay(50);
  FastLED.show();
}
