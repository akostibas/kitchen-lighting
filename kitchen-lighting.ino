const int BUTTON_PIN = 0;
const int BUTTON_LED_PIN = 2;
const int LED_PIN = 1;

bool leftPanelOutput = LOW;
bool lastButtonState = LOW;

int panelMode = 0;
int buttonLedMode = 0;

long leftLastChange;

void setup() {
  leftLastChange = millis();
  // put your setup code here, to run once:
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  digitalWrite(LED_PIN  , leftPanelOutput);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    if (lastButtonState == LOW) {
      lastButtonState = HIGH;
      buttonPress();
    } 
  } else {
    lastButtonState = LOW;
  }
  
  digitalWrite(LED_PIN, leftPanelOutput);
}

void buttonPress() {
  if ((millis() - leftLastChange) > 100)
  {
    leftPanelOutput = !leftPanelOutput;
    leftLastChange = millis();
  }
}
