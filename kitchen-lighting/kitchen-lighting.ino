// New Lights
const int BUTTON_LED_PIN = 0;
const int PANEL_PIN = 1;
const int BUTTON_PIN = 2;

// Old Lights
/*
const int BUTTON_LED_PIN = 2;
const int PANEL_PIN = 1;
const int BUTTON_PIN = 0;
*/

// LED panel variables
const int PANEL_CHANGE_DELAY = 1;        // ms to wait before changing panel output
enum lightModes{OFF, DIM, MEDIUM, FULL};
enum lightModes panelMode = FULL;
static const int MODE_BRIGHTNESS [] = {
  [OFF] = 0,
  [DIM] = 60,
  [MEDIUM] = 150,
  [FULL] = 255
};
int panelCurrentOutput = 0;
bool panelEnabled = false;

// Button LED variables
const int BUTTON_LED_HIGH = 200;
const int BUTTON_LED_LOW = 10;
bool buttonLedOutput = HIGH;
bool buttonLedEnabled = true;
const int BUTTON_LED_DISPLAY_DELAY = 1250;
bool freshButtonLedChange = true;

// Push-button state tracking
const int PRESS_DELAY = 20;       // ms to ignore input between button state changes (filtering)
bool buttonState = LOW;
bool lastButtonState = LOW;
long millisOfLastChange;
long millisSinceLastChange;
long millisSinceLastPush;
const int RAPID_PUSH_DELAY = 500; // ms between pushes to count as rapid pushes
bool freshButtonDown = true;      // Was the button freshly pressed, or is it being held down?
bool freshButtonUp = true;        // Was the button freshly released, or has it always been up?

void setup() {
  millisOfLastChange = millis();

  pinMode(BUTTON_PIN, INPUT);
  pinMode(PANEL_PIN, OUTPUT);
  pinMode(BUTTON_LED_PIN, OUTPUT);

  buttonState = digitalRead(BUTTON_PIN);
  lastButtonState = buttonState;

  analogWrite(PANEL_PIN, LOW);
  analogWrite(BUTTON_LED_PIN, BUTTON_LED_LOW);
}

void loop() {
  checkButtonState();
  setButtonLed();
  setPanel();
}

// Adjust light panel to appropriate output
void setPanel() {
  int panelLastOutput = panelCurrentOutput;
  switch(panelMode) {
    case FULL:
      if (panelCurrentOutput < MODE_BRIGHTNESS[FULL]) {
        panelCurrentOutput++;
      }
      break;
    
    case MEDIUM:
      if (panelCurrentOutput < MODE_BRIGHTNESS[MEDIUM]) {
        panelCurrentOutput++;
      } else if (panelCurrentOutput > MODE_BRIGHTNESS[MEDIUM]) {
        panelCurrentOutput--;
      }
      break;
    
    case DIM:
      if (panelCurrentOutput < MODE_BRIGHTNESS[DIM]) {
        panelCurrentOutput++;
      } else if (panelCurrentOutput > MODE_BRIGHTNESS[DIM]) {
        panelCurrentOutput--;
      }
      break;
      
    case OFF:
      if (panelCurrentOutput > MODE_BRIGHTNESS[OFF]) {
        panelCurrentOutput--;
      }
  }
  analogWrite(PANEL_PIN, panelCurrentOutput);
  if (panelLastOutput != panelCurrentOutput) {
    delay(PANEL_CHANGE_DELAY);
  }
}

// Adjust button LED to appropriate output
void setButtonLed() {
  if (buttonLedEnabled) {
    if (buttonState == HIGH) {
      analogWrite(BUTTON_LED_PIN, BUTTON_LED_HIGH);
    } else {
      analogWrite(BUTTON_LED_PIN, BUTTON_LED_LOW);
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
      if (freshButtonDown) {
        buttonDown();
      } else {
        buttonHold();
      }
    } else if (buttonState == LOW && freshButtonUp)
    {
      buttonUp();
    }
  }
}

// The button was just pressed down
void buttonDown() {
  freshButtonDown = false;
  freshButtonUp = true;
}

// The button is being held down
void buttonHold() {
  // Check to see if we're disabling the button led
  if (millisSinceLastChange > BUTTON_LED_DISPLAY_DELAY &&
      freshButtonLedChange) {
    buttonLedEnabled = !buttonLedEnabled;
    freshButtonLedChange = false;
  }
}

// The button was released
void buttonUp() {
  freshButtonDown = true;
  freshButtonUp = false;

  if (freshButtonLedChange) {
    // Differentiate a rapid push from a normal push
    if (millis() - millisSinceLastPush < RAPID_PUSH_DELAY &&
        panelMode != OFF) {
      toggleLightMode();
    } else {
      togglePanel();
    }
  }
  
  freshButtonLedChange = true;
  millisSinceLastPush = millis();
}

void togglePanel() {
  if (panelMode != OFF) {
    panelMode = OFF;
  } else {
    panelMode = FULL;
  }
}

void toggleLightMode() {
  switch(panelMode) {
    case FULL:
      panelMode = MEDIUM;
      break;
    case MEDIUM:
      panelMode = DIM;
      break;
    case DIM:
      panelMode = FULL;
      break;
  }
}
