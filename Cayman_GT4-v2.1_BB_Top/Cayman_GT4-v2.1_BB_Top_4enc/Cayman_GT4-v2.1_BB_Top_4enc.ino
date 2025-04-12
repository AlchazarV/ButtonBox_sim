#include <Joystick.h>
#include <Adafruit_NeoPixel.h>

// Define the number of LEDs and the pin they are connected to
#define NUM_LEDS 4
#define LED_PIN 2

// Define the pins for the rotary encoders and their buttons
#define RSW1_PIN_A 3
#define RSW1_PIN_B 4
#define RSW1_BTN 5
#define RSW2_PIN_A 6
#define RSW2_PIN_B 7
#define RSW2_BTN 8
#define RSW3_PIN_A 9
#define RSW3_PIN_B 10
#define RSW3_BTN 16
#define RSW4_PIN_A 14
#define RSW4_PIN_B 15
#define RSW4_BTN A0

// --- LED brightness control ---
#define MIN_BRIGHTNESS 10
#define MAX_BRIGHTNESS 255
#define BRIGHTNESS_STEP 10

int led1_brightness = 100;
int led2_brightness = 100;
int led3_brightness = 100;
int led4_brightness = 100;

Adafruit_NeoPixel leds(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                   12, 0, // 12 buttons, 0 hats
                   false, false, false, // No axes
                   false, false, false,
                   false, false, false,
                   false, false);

// --- Encoder state tracking ---
int last_rsw1_state = 0;
int last_rsw2_state = 0;
int last_rsw3_state = 0;
int last_rsw4_state = 0;

void setup() {
  pinMode(RSW1_PIN_A, INPUT_PULLUP);
  pinMode(RSW1_PIN_B, INPUT_PULLUP);
  pinMode(RSW1_BTN,   INPUT_PULLUP);

  pinMode(RSW2_PIN_A, INPUT_PULLUP);
  pinMode(RSW2_PIN_B, INPUT_PULLUP);
  pinMode(RSW2_BTN,   INPUT_PULLUP);

  pinMode(RSW3_PIN_A, INPUT_PULLUP);
  pinMode(RSW3_PIN_B, INPUT_PULLUP);
  pinMode(RSW3_BTN,   INPUT_PULLUP);

  pinMode(RSW4_PIN_A, INPUT_PULLUP);
  pinMode(RSW4_PIN_B, INPUT_PULLUP);
  pinMode(RSW4_BTN,   INPUT_PULLUP);

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
  leds.setPixelColor(0, leds.Color(0, led1_brightness, 0));  //green leds.setPixelColor(index, color)
  leds.setPixelColor(1, leds.Color(led2_brightness, 0, 0)); //red
  leds.setPixelColor(2, leds.Color(0, 0, led3_brightness)); //blue
  leds.setPixelColor(3, leds.Color(led4_brightness, led4_brightness, led4_brightness)); //white
  leds.show();
}

void loop() {
  // Handle encoder direction and brightness
  handleEncoder(RSW1_PIN_A, RSW1_PIN_B, last_rsw1_state, 1, 2, led1_brightness);
  handleEncoder(RSW2_PIN_A, RSW2_PIN_B, last_rsw2_state, 4, 5, led2_brightness);
  handleEncoder(RSW3_PIN_A, RSW3_PIN_B, last_rsw3_state, 7, 8, led3_brightness);
  handleEncoder(RSW4_PIN_A, RSW4_PIN_B, last_rsw4_state, 10, 11, led4_brightness);
  // Handle buttons
  updateButton(RSW1_BTN, 0);
  updateButton(RSW2_BTN, 3);
  updateButton(RSW3_BTN, 6);
  updateButton(RSW4_BTN, 9);


  updateLEDs();
}
