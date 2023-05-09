//nastavení pinů tlačítek a deklarace globálních proměnných
const int StepButtonPin = A5;
const int GroupButtonPin = 4;
const int ModeButtonPin = A4;

bool lastStepButton = false;
bool lastModeButton = false;
bool lastGroupButton = false;


int mode = 2;
int groups = 0;
int modeGroupGroup = 4;

int modeYesNoChance = 50;

int modeRandomGroupData[5];
int modeRandomGroupIndex = 0;

void resetLEDs() {
  // digitalWrite(A0, false);
  // digitalWrite(A1, false);
  // digitalWrite(A2, false);
  digitalWrite(7, false);
  digitalWrite(6, false);
  digitalWrite(3, false);
  digitalWrite(11, false);
  digitalWrite(9, false);
}

//točení/výběr módu
void advanceMode() {
  resetLEDs();
  mode = (mode + 1) % 3;

  analogWrite(A0, 0 == mode ? 128 : 0);
  analogWrite(A1, 1 == mode ? 128 : 0);
  analogWrite(A2, 2 == mode ? 128 : 0);
}
//točení/výběr skupin
void advanceGroup() {
  resetLEDs();
  groups += 1;
  if (groups == 5) {
    groups = 0;
  }
  digitalWrite(7, groups == 0);
  digitalWrite(6, groups == 1);
  digitalWrite(3, groups == 2);
  digitalWrite(11, groups == 3);
  digitalWrite(9, groups == 4);
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

void switchLedFromNum(int num) {
  switch (num) {
    case 0:
      digitalWrite(7, true);
      break;
    case 1:
      digitalWrite(6, true);
      break;
    case 2:
      digitalWrite(3, true);
      break;
    case 3:
      digitalWrite(11, true);
      break;
    case 4:
      digitalWrite(9, true);
      break;
  }
}

//funkce při stisknutí hlavního krokového tlačítka
void step() {
  resetLEDs();
  switch(mode) {
    case 0:
      if (modeYesNo()) {
        digitalWrite(9, true);
      } else {
        digitalWrite(7, true);
      }
      break;
    case 1:
      modeGroupGroup = (modeGroupGroup + 1) % (groups + 1);
      switchLedFromNum(modeGroupGroup);
      break;
    case 2:
      switchLedFromNum(modeRandomGroup());
      break;
  }
}

void setup() {
  Serial.begin(9600);
  //nastavení ledek na zobrazení výstupu módů
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(9, OUTPUT);
  
  //nastavení ledek na zobrazení vybraného módu
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);

  //nastavení vstupu tlačítek
  pinMode(StepButtonPin, INPUT);
  pinMode(GroupButtonPin, INPUT);
  pinMode(ModeButtonPin, INPUT);

  //nastavení prvního módu při startu
  advanceMode();

  //nastavení seedu z nezapojeného analogového pinu
  randomSeed(analogRead(A3));
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

  lastStepButton = stepButton;
  lastGroupButton = groupButton;
  lastModeButton = modeButton;

  delay(50);
}