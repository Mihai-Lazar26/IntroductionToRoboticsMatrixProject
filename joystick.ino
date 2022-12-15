#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

const int joyXPin = A0;
const int joyYPin = A1;
const int joySWPin = 13;

const int joyOutsideBigTreshold = 700;
const int joyOutsideSmallTreshold = 300;
const int joyCenterBigTreshold = 600;
const int joyCenterSmallTreshold = 400;

const int buzzerFrequency = 500;
const int buzzerDuration = 200;

const unsigned int minDelay = 200;
const unsigned int defaultDelay = 500;
const unsigned int speedIncrease = 1;

unsigned int delayMove = defaultDelay;
unsigned long lastMoveTime = 0;

bool joyIsNeutral = true;
bool joySWState = HIGH;
bool joyPrevSWState = HIGH;

unsigned long lastSWStateChange = 0;
unsigned int debounceDelay = 50;

unsigned long lastPressTime = 0;
unsigned int longPressTime = 1000 * 3;
byte resetPossible = false;

byte lastSWState = HIGH;
byte currentSWState = HIGH;

byte direction;
byte lastDirection;

void setDelay(const int &newDelay) {
  delayMove = newDelay;  
  if (delayMove < minDelay) {
    delayMove = minDelay;
  }
}

void addDelay(const int &newDelay) {
  setDelay(delayMove + newDelay);
}

void setDefaultDelay() {
  delayMove = defaultDelay;
}

int getSpeed() {
  return defaultDelay - delayMove;
}

void joystickSetup() {
  pinMode(joyXPin, INPUT);
  pinMode(joyYPin, INPUT);
  pinMode(joySWPin, INPUT_PULLUP);
}

void joyStickGameSetup() {
  direction = RIGHT;
  lastDirection = RIGHT;
}

void joystickGameLoop() {
  int joyX = analogRead(joyXPin);
  int joyY = analogRead(joyYPin);

  if (joyY < joyOutsideSmallTreshold && lastDirection != LEFT) {
    direction = RIGHT;
  }
  if (joyY > joyOutsideBigTreshold && lastDirection != RIGHT) {
    direction = LEFT;
  }
  if (joyX < joyOutsideSmallTreshold && lastDirection != DOWN) {
    direction = UP;
  }
  if (joyX > joyOutsideBigTreshold && lastDirection != UP) {
    direction = DOWN;
  }

  if (millis() - lastMoveTime > delayMove) {

    if (direction == LEFT) {
      lastDirection = LEFT;
      moveLeft();
    }
    if (direction == RIGHT) {
      lastDirection = RIGHT;
      moveRight();
    }
    if (direction == DOWN) {
      lastDirection = DOWN;
      moveDown();
    }
    if (direction == UP) {
      lastDirection = UP;
      moveUp();
    }

    lastMoveTime = millis();

    addDelay(-speedIncrease);

    if (soundsOn) {
      tone(buzzerPin, buzzerFrequency, buzzerDuration);
    }
  }
}

void joystickMenuLoop() {
  int joyX = analogRead(joyXPin);
  int joyY = analogRead(joyYPin);

  if (joyX < joyOutsideSmallTreshold && joyIsNeutral) {
    if (soundsOn) {
      tone(buzzerPin, buzzerFrequency, buzzerDuration);
    }
    moveUpMenu();
    joyIsNeutral = false;
  }
  if (joyX > joyOutsideBigTreshold && joyIsNeutral) {
    if (soundsOn) {
      tone(buzzerPin, buzzerFrequency, buzzerDuration);
    }
    moveDownMenu();
    joyIsNeutral = false;
  }
  if (joyY < joyOutsideSmallTreshold && joyIsNeutral) {
    if (soundsOn) {
      tone(buzzerPin, buzzerFrequency, buzzerDuration);
    }
    moveRightMenu();
    joyIsNeutral = false;
  }
  if (joyY > joyOutsideBigTreshold && joyIsNeutral) {
    if (soundsOn) {
      tone(buzzerPin, buzzerFrequency, buzzerDuration);
    }
    moveLeftMenu();
    joyIsNeutral = false;
  }

  if (joyCenterSmallTreshold <= joyY && joyY <= joyCenterBigTreshold && joyCenterSmallTreshold <= joyX && joyX <= joyCenterBigTreshold) {
    joyIsNeutral = true;
  }

  if (debounceSW()) {
    if (soundsOn) {
      tone(buzzerPin, buzzerFrequency, buzzerDuration);
    }
    buttonPress();    
  }  
}

byte debounceSW() {
  byte reading = digitalRead(joySWPin);
  lastSWState = reading;

  return debounceButton(reading, currentSWState, lastSWState, lastSWStateChange, debounceDelay);
}

byte debounceButton(byte &reading, byte &buttonState, byte &lastButtonState, unsigned long &lastDebounceTime, unsigned int &debounceDelay) {
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        return true;
      }
    }
  }
  return false;
}

