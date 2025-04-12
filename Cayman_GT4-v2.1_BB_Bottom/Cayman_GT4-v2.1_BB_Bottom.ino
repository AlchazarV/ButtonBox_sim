#include <Joystick.h>
#include <Adafruit_NeoPixel.h>

#define NUM_LEDS 4
#define LED_PIN 2

#define SW1_PIN 15
#define SW2_PIN 7
#define SW3_PIN 8
#define SW4_PIN 14

#define BUTTON_PINS {3, 4, 5, 6, A3, A2, A1, A0}

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 12, 0, false, false, false, false, false, false, false, false, false, false, false);

bool switchStates[4] = {false, false, false, false};
bool previousSwitchStates[4] = {false, false, false, false}; // Track the previous states
int buttonDelay = 50;

void setup() {

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  pinMode(SW1_PIN, INPUT_PULLUP);
  pinMode(SW2_PIN, INPUT_PULLUP);
  pinMode(SW3_PIN, INPUT_PULLUP);
  pinMode(SW4_PIN, INPUT_PULLUP);

  int buttonPins[] = BUTTON_PINS;

  for (int i = 0; i < 8; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  Joystick.begin();
}

 
void loop() {
  updateSwitches();
  updateButtons();
  //updateLEDs(1, LOW);
}

 
void updateSwitches() {
  bool currentStates[4] = {
    digitalRead(SW1_PIN) == LOW,
    digitalRead(SW2_PIN) == LOW,
    digitalRead(SW3_PIN) == LOW,
    digitalRead(SW4_PIN) == LOW
  };

  // for (int i = 0; i < 4; i++) {
  //   if (currentStates[i] != switchStates[i]) {
  //     switchStates[i] = currentStates[i];
  //     Joystick.setButton(i, switchStates[i]);
  //     updateLEDs(i, switchStates[i]);
  //   }
  // }
  for (int i = 0; i < 4; i++) {
    if (currentStates[i] != previousSwitchStates[i]) {
      // Switch state changed (press or release)
      if (currentStates[i] == LOW) {
        Joystick.setButton(i, true);  // Simulate button press (short signal)
        delay(buttonDelay);
        Joystick.setButton(i, false);
      } else {
        Joystick.setButton(i, true); // Simulate button release (short signal)
        delay(buttonDelay);
        Joystick.setButton(i, false);
      }

      previousSwitchStates[i] = currentStates[i]; // Update previous state
      updateLEDs(i, currentStates[i]);
    }
  }
}

 
void updateLEDs(int index, bool state) {
  if (state) {
    strip.setPixelColor(index, strip.Color(0, 255, 0)); // Green
  } else {
    strip.setPixelColor(index, strip.Color(255, 0, 0)); // Red
  }
  strip.show();
}


void updateButtons() {
  int buttonPins[] = BUTTON_PINS;
  for (int i = 0; i < 8; i++) {
    Joystick.setButton(i + 4, digitalRead(buttonPins[i]) == LOW);
  }
}