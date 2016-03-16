const int BUTTON_PIN = 0;
const int BUTTON_LED_PIN = 2;
const int LED_PIN = 1;
// Delay to ignore input between button state changes
const int PRESS_DELAY = 20;

bool panelOutput = LOW;
int panelMode = 0;

// Button LED variables
bool buttonLedOutput = HIGH;
bool buttonLedEnabled = true;
int buttonLedDisableDelay = 1250;
bool freshButtonLedChange = true;

bool buttonState = LOW;
bool lastButtonState = LOW;
long lastStateChange;
long deltaSinceLastChange;
int buttonLedMode = 0;
bool freshButtonPress = true;
bool freshButtonRelease;

void setup() {
  lastStateChange = millis();
  freshButtonLedChange = true;

  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_LED_PIN, OUTPUT);
  
  buttonState = digitalRead(BUTTON_PIN);
  lastButtonState = buttonState;

  digitalWrite(LED_PIN, buttonState);
  digitalWrite(BUTTON_LED_PIN, HIGH);
}

void loop() {
  checkButtonState();
}

void checkButtonState() {
    buttonState = digitalRead(BUTTON_PIN);
  
  if (buttonState != lastButtonState) {
    lastStateChange = millis();
    lastButtonState = buttonState;
  }
  deltaSinceLastChange = millis() - lastStateChange;
  
  if (buttonState == HIGH && 
      deltaSinceLastChange > PRESS_DELAY)
  {
    if (freshButtonPress) {
      buttonPressed();
    } else {
      buttonDown();
    }
  }

  if (buttonState == LOW &&
      freshButtonRelease &&
      deltaSinceLastChange > PRESS_DELAY)
  {
    buttonReleased();
  }
}

void buttonPressed() {
  toggleLights();
  // While the button is down, it is not considered a fresh press
  freshButtonPress = false;
  freshButtonRelease = true;
}

void buttonDown() {
  // Check to see if we're disabling the button led
  if (deltaSinceLastChange > buttonLedDisableDelay &&
      freshButtonLedChange) {
    buttonLedEnabled = !buttonLedEnabled;
    freshButtonLedChange = false;
  }

  // Do a 50% dim.. button #2 isn't PWM.
  toggleButtonLed();
}

void buttonReleased() {
  freshButtonPress = true;
  freshButtonRelease = false;
  freshButtonLedChange = true;
  setButtonLed(HIGH);
}

void toggleLights() {
  panelOutput = !panelOutput;
  digitalWrite(LED_PIN, panelOutput);
}

void toggleButtonLed() {
  if (buttonLedEnabled) {
    buttonLedOutput = !buttonLedOutput;
  } else {
    buttonLedOutput = LOW;
  }
  digitalWrite(BUTTON_LED_PIN, buttonLedOutput);
}

void setButtonLed(bool desiredState) {
  if (buttonLedEnabled) {
    buttonLedOutput = desiredState;
  } else {
    buttonLedOutput = LOW;
  }
  digitalWrite(BUTTON_LED_PIN, buttonLedOutput);
}

