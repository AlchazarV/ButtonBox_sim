#include <Joystick.h>
#include <Adafruit_NeoPixel.h>

// --- Pin definitions ---
#define RSW1_PIN_A 3
#define RSW1_PIN_B 4
#define RSW1_BTN   5

#define RSW2_PIN_A 6
#define RSW2_PIN_B 7
#define RSW2_BTN   8

#define BUT1_PIN   10
#define SW1_PIN    16

#define LED_PIN    2
#define NUM_LEDS   2

// --- LED brightness control ---
#define MIN_BRIGHTNESS 10
#define MAX_BRIGHTNESS 255
#define BRIGHTNESS_STEP 15

int led1_brightness = 100;
int led2_brightness = 100;

Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                   8, 0, // 8 buttons, 0 hats
                   false, false, false, // No axes
                   false, false, false,
                   false, false, false,
                   false, false);

// --- Encoder state tracking ---
int last_rsw1_state = 0;
int last_rsw2_state = 0;

void setup() {
  pinMode(RSW1_PIN_A, INPUT_PULLUP);
  pinMode(RSW1_PIN_B, INPUT_PULLUP);
  pinMode(RSW1_BTN,   INPUT_PULLUP);

  pinMode(RSW2_PIN_A, INPUT_PULLUP);
  pinMode(RSW2_PIN_B, INPUT_PULLUP);
  pinMode(RSW2_BTN,   INPUT_PULLUP);

  pinMode(BUT1_PIN, INPUT_PULLUP);
  pinMode(SW1_PIN, INPUT_PULLUP);

  leds.begin();
  leds.show();

  Joystick.begin();
}

// --- Button handler ---
void updateButton(uint8_t pin, uint8_t buttonIndex) {
  static uint8_t lastState[20];
  uint8_t state = digitalRead(pin);
  uint8_t pressed = !state;
  if (pressed != lastState[pin]) {
    Joystick.setButton(buttonIndex, pressed);
    lastState[pin] = pressed;
  }
}

// --- Toggle switch as momentary press ---
void handleToggleAsMomentary(uint8_t pin, uint8_t buttonIndex) {
  static bool lastState = HIGH;
  bool currentState = digitalRead(pin);
  if (currentState != lastState) {
    Joystick.setButton(buttonIndex, 1);
    delay(100);
    Joystick.setButton(buttonIndex, 0);
  }
  lastState = currentState;
}

// --- Rotary encoder with direction ---
void handleEncoder(int pinA, int pinB, int &lastState, uint8_t btnLeft, uint8_t btnRight, int &brightness) {
  int MSB = digitalRead(pinA); // A
  int LSB = digitalRead(pinB); // B

  int encoded = (MSB << 1) | LSB;
  int sum = (lastState << 2) | encoded;

  // Clockwise
  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    Joystick.setButton(btnRight, 1);
    delay(10);
    Joystick.setButton(btnRight, 0);
    brightness = min(MAX_BRIGHTNESS, brightness + BRIGHTNESS_STEP);
  }

  // Counter-clockwise
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    Joystick.setButton(btnLeft, 1);
    delay(10);
    Joystick.setButton(btnLeft, 0);
    brightness = max(MIN_BRIGHTNESS, brightness - BRIGHTNESS_STEP);
  }

  lastState = encoded;
}

// --- LED update ---
void updateLEDs() {
  leds.setPixelColor(0, leds.Color(led1_brightness, 0, 0));
  leds.setPixelColor(1, leds.Color(led2_brightness, 0, 0));
  leds.show();
}

void loop() {
  // Handle encoder direction and brightness
  handleEncoder(RSW1_PIN_A, RSW1_PIN_B, last_rsw1_state, 1, 2, led1_brightness);
  handleEncoder(RSW2_PIN_A, RSW2_PIN_B, last_rsw2_state, 4, 5, led2_brightness);

  // Handle buttons
  updateButton(RSW1_BTN, 0);
  updateButton(RSW2_BTN, 3);
  updateButton(BUT1_PIN, 6);
  handleToggleAsMomentary(SW1_PIN, 7);

  updateLEDs();
}
