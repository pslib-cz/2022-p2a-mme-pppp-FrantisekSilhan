bool dataLED[5] = {false, false, false, false, false};
//nastavení pinů tlačítek a deklarace globálních proměnných
const int StepButtonPin = 9;
const int GroupButtonPin = 10;
const int ModeButtonPin = 11;

bool lastStepButton = false;
bool lastModeButton = false;
bool lastGroupButton = false;


int mode = 2;
int groups = 0;
int modeGroupGroup = 4;

int modeYesNoChance = 50;

int modeRandomGroupData[5];
int modeRandomGroupIndex = 0;

//točení/výběr módu
void advanceMode() {
  mode = (mode + 1) % 3;

  digitalWrite(A1, 0 == mode);
  digitalWrite(A2, 1 == mode);
  digitalWrite(A3, 2 == mode);
}
//točení/výběr skupin
void advanceGroup() {
  groups = (groups + 1) % sizeof(dataLED);
  for (int i = 0; i < sizeof(dataLED); i++) {
    dataLED[i] = (i <= groups);
  }
}

void updateDataLED(int number) {
  for (int i = 0; i < sizeof(dataLED); i++) {
    dataLED[i] = (i == number);
  }
}


//funkce s jednotlivými módy
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

int modeRandomGroup() {
  for (int i = 0; i <= groups; i++) {
    int r = random(groups+1);
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
      if (modeRandomGroupIndex > groups) {
        modeRandomGroupIndex = 0;
        memset(modeRandomGroupData, 0, sizeof(modeRandomGroupData));
      }
      return r;
    }
  }
}

//funkce při stisknutí hlavního krokového tlačítka
void step() {
  switch(mode) {
    case 0:
      if (modeYesNo()) {
        dataLED[0] = false;
        dataLED[1] = false;
        dataLED[2] = false;
        dataLED[3] = true;
        dataLED[4] = true;
      } else {
        dataLED[0] = true;
        dataLED[1] = true;
        dataLED[2] = false;
        dataLED[3] = false;
        dataLED[4] = false;
      }
      break;
    case 1:
      modeGroupGroup = (modeGroupGroup + 1) % (groups + 1);
      updateDataLED(modeGroupGroup);
      break;
    case 2:
      updateDataLED(modeRandomGroup());
      break;
  }
}

void setup() {
  //nastavení ledek na zobrazení výstupu módů
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  
  //nastavení ledek na zobrazení vybraného módu
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);

  //nastavení vstupu tlačítek
  pinMode(StepButtonPin, INPUT);
  pinMode(GroupButtonPin, INPUT);
  pinMode(ModeButtonPin, INPUT);

  //nastavení prvního módu při startu
  advanceMode();

  //nastavení seedu z nezapojeného analogového pinu
  randomSeed(analogRead(A4));
}

void loop() {
  const bool stepButton = digitalRead(StepButtonPin);
  const bool groupButton = digitalRead(GroupButtonPin);
  const bool modeButton = digitalRead(ModeButtonPin);

  if (lastStepButton != stepButton && stepButton) {
    step();
  }
  if (lastGroupButton != groupButton && groupButton) {
    advanceGroup();
    modeGroupGroup = -1;

    memset(modeRandomGroupData, 0, sizeof(modeRandomGroupData));
    modeRandomGroupIndex = 0;
  }
  if (lastModeButton != modeButton && modeButton) {
    advanceMode();
    modeGroupGroup = -1;

    memset(modeRandomGroupData, 0, sizeof(modeRandomGroupData));
    modeRandomGroupIndex = 0;
  }

  for (int i = 0; i < 5; i++) {
    digitalWrite(6 - i, dataLED[i]);
  }

  lastStepButton = stepButton;
  lastGroupButton = groupButton;
  lastModeButton = modeButton;

  delay(50);
}