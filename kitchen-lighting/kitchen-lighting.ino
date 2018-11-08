const int BUTTON_LED_PIN = 0;
const int LED_PIN = 1;
const int BUTTON_PIN = 2;

enum lightModes{off, low, medium, high};

// Delay to ignore input between button state changes
const int PRESS_DELAY = 20;

// LED panel variables
const int PANEL_CHANGE_DELAY = 1;  // ms to wait before changing panel output
enum lightModes panelMode = high;
int panelCurrentOutput = 0;
bool panelEnabled = false;

// Button LED variables
const int BUTTON_LED_HIGH = 155;
const int BUTTON_LED_LOW = 70;
bool buttonLedOutput = HIGH;
bool buttonLedEnabled = true;
const int BUTTON_LED_TOGGLE_DELAY = 1250;
bool freshButtonLedChange = true;

// Push-button state tracking
bool buttonState = LOW;
bool lastButtonState = LOW;
long millisOfLastChange;
long millisSinceLastChange;
long millisSinceLastPush;
const int RAPID_PUSH_DELAY = 750; // ms between pushes to count as rapid pushes
bool freshButtonDown = true;      // Was the button freshly pressed, or is it being held down?
bool freshButtonUp = true;        // Was the button freshly released, or has it always been up?

void setup() {
  millisOfLastChange = millis();

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
  int panelLastOutput = panelCurrentOutput;
  switch(panelMode) {
    case high:
      if (panelCurrentOutput < 255) {
        panelCurrentOutput++;
      }
      break;
    
    case medium:
      if (panelCurrentOutput < 150) {
        panelCurrentOutput++;
      } else if (panelCurrentOutput > 150) {
        panelCurrentOutput--;
      }
      break;
    
    case low:
      if (panelCurrentOutput < 60) {
        panelCurrentOutput++;
      } else if (panelCurrentOutput > 60) {
        panelCurrentOutput--;
      }
      break;
      
    case off:
      if (panelCurrentOutput > 0) {
        panelCurrentOutput--;
      }
  }
  analogWrite(LED_PIN, panelCurrentOutput);
  if (panelLastOutput != panelCurrentOutput) {
    delay(PANEL_CHANGE_DELAY);
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
  if (millisSinceLastChange > BUTTON_LED_TOGGLE_DELAY &&
      freshButtonLedChange) {
    
    buttonLedEnabled = !buttonLedEnabled;
    freshButtonLedChange = false;
  }
}

// The button was released
void buttonUp() {
  freshButtonDown = true;
  freshButtonUp = false;

  // Don't toggle the lights if we've just changed the button LED
  if (freshButtonLedChange) {
    // Detect a rapid push vs a normal push
    if (millis() - millisSinceLastPush < RAPID_PUSH_DELAY) {
      toggleLightMode();
    } else {
      togglePanel();
    }
  }
  
  freshButtonLedChange = true;
  millisSinceLastPush = millis();
}

void togglePanel() {
  if (panelMode != off) {
    panelMode = off;
  } else {
    panelMode = high;
  }
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
