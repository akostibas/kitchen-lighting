const int BUTTON_PIN = 0;
const int BUTTON_LED_PIN = 2;
const int LED_PIN = 1;
// Delay to ignore input between button state changes
const int PRESS_DELAY = 20;

bool panelOutput = LOW;
int panelMode = 0;

bool buttonState = LOW;
bool lastButtonState = LOW;
long lastStateChange;
long deltaSinceLastChange;
int buttonLedMode = 0;
bool freshButtonPress = true;
bool freshButtonRelease;

void setup() {
  lastStateChange = millis();
  
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
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
  digitalWrite(BUTTON_LED_PIN, LOW);
}

void buttonDown() {
  
}

void buttonReleased() {
  freshButtonPress = true;
  freshButtonRelease = false;
  digitalWrite(BUTTON_LED_PIN, HIGH);
}

void toggleLights() {
  panelOutput = !panelOutput;
  digitalWrite(LED_PIN, panelOutput);
}

