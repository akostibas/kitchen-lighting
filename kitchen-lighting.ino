const int BUTTON_LED_PIN = 0;
const int LED_PIN = 1;
const int BUTTON_PIN = 2;

enum lightModes{low, medium, high};

// Delay to ignore input between button state changes
const int PRESS_DELAY = 20;

// LED panel variables
enum lightModes panelMode = high;
bool freshModeChange = true;
bool panelEnabled = false;
int panelFancyDelay = 2500;

// Button LED variables
const int BUTTON_LED_HIGH = 155;
const int BUTTON_LED_LOW = 70;
bool buttonLedOutput = HIGH;
bool buttonLedEnabled = true;

// Number of millis needed to hold button down to disable button LEDs
int buttonLedDisableDelay = 1250;
bool freshButtonLedChange = true;

bool buttonState = LOW;
bool lastButtonState = LOW;
// The time of the last state change
long millisOfLastChange;
// The number of millis since the last state change
long millisSinceLastChange;

// Was the button freshly pressed (or is it being held down)?
bool freshButtonPress = true;
// Was the button freshly pressed (or has it always been up)?
bool freshButtonRelease = true;

void setup() {
  millisOfLastChange = millis();
  freshButtonLedChange = true;

  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_LED_PIN, OUTPUT);

  buttonState = digitalRead(BUTTON_PIN);
  lastButtonState = buttonState;

  digitalWrite(LED_PIN, LOW);
  analogWrite(BUTTON_LED_PIN, BUTTON_LED_HIGH);
}

void loop() {
  checkButtonState();
  setButtonLed();
  setPanel();
}

// Adjust light panel to appropriate output
void setPanel() {
  if (panelEnabled) {
    switch(panelMode) {
      case high:
        analogWrite(LED_PIN, 255);
        break;
      case medium:
        analogWrite(LED_PIN, 100);
        break;
      case low:
        analogWrite(LED_PIN, 60);
        break;
    }
  } else {
    digitalWrite(LED_PIN, 0);
  }
}

// Adjust button LED to appropriate output
void setButtonLed() {
  if (buttonLedEnabled) {
    if (buttonState == HIGH) {
      analogWrite(BUTTON_LED_PIN, BUTTON_LED_LOW);
    } else {
      analogWrite(BUTTON_LED_PIN, BUTTON_LED_HIGH);
    }
  } else {
    analogWrite(BUTTON_LED_PIN, 0);
  }
}

void checkButtonState() {
  buttonState = digitalRead(BUTTON_PIN);

  if (buttonState != lastButtonState) {
    millisOfLastChange = millis();
    lastButtonState = buttonState;
  }
  millisSinceLastChange = millis() - millisOfLastChange;

  if (millisSinceLastChange > PRESS_DELAY) {
    // We only take actions if the last change was some time ago
    // this smooths out random bounces in input signal from the
    // button mechanism.
    if (buttonState == HIGH)
    {
      if (freshButtonPress) {
        buttonDown();
      } else {
        buttonHold();
      }
    } else if (buttonState == LOW && freshButtonRelease)
    {
      buttonUp();
    }
  }
}

// The button was just pressed down
void buttonDown() {
  toggleLights();
  freshButtonPress = false;
  freshButtonRelease = true;
}

// The button is being held down
void buttonHold() {
  // Check to see if we're disabling the button led
  if (millisSinceLastChange > buttonLedDisableDelay &&
      freshButtonLedChange) {
    buttonLedEnabled = !buttonLedEnabled;
    freshButtonLedChange = false;
  }

  if (millisSinceLastChange > panelFancyDelay &&
      freshModeChange) {
    freshModeChange = false;
    toggleLightMode();
  }
}

// The button was released
void buttonUp() {
  freshButtonPress = true;
  freshButtonRelease = false;
  freshButtonLedChange = true;
  freshModeChange = true;
}

void toggleLights() {
  panelEnabled = !panelEnabled;
}

void toggleLightMode() {
  switch(panelMode) {
    case high:
      panelMode = medium;
      break;
    case medium:
      panelMode = low;
      break;
    case low:
      panelMode = high;
      break;
  }
}
