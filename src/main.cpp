#include <Arduino.h>
#include <FastLED.h>
#include <Visualization.h>
#include <Spinner.h>
#include <Sparkle.h>
#include <SoundReaction.h>
#include <TeensyAudioFFT.h>

// FAST LED
#define NUM_LEDS 24
// #define DISPLAY_LED_PIN 22  // VERSION 1
#define DISPLAY_LED_PIN 4      // VERSION 2
// #define DISPLAY_LED_PIN 23  // STAFF
// #define DISPLAY_LED_PIN 5   // LANTERN
#define SPARKLE_CONTROL_PIN 1
#define SPINNER_CONTROL_PIN 15
#define FLASHLIGHT_CONTROL_PIN 23

#define AUDIO_INPUT_PIN A7  // MEDALLION
// #define AUDIO_INPUT_PIN A0  // STAFF

#define TOUCH_SENSITIVITY 700

CRGB leds[NUM_LEDS];
CRGB off;

void clear();
void setAll(CRGB color);

#define NUM_STREAKS 1
#define NUM_SPARKELS 1

#define SATURATION 244
#define VALUE 255

CRGB pink = 0xFF0B20;
CRGB blue = 0x0BFFDD;
CRGB white = 0x080808;

uint8_t spinnerHue;
uint8_t sparkleHue;

CHSV spinnerHSVColor;
CRGB spinnerRGBColor;

CHSV sparkleHSVColor;
CRGB sparkleRGBColor;

Spinner * spinner;
Sparkle * sparkle;

SoundReaction * soundReaction;


uint16_t interval = 50;
uint32_t nextTime = 0;

bool flashlight = false;
bool spinnerColorCycle = true;
bool sparkleColorCycle = true;

#define SPINNER_SPEED 14009
#define SPARKLE_SPEED 2333

uint32_t randomTimeOffset;   // Just so we don't start with the same colors
                              // every time

void setup() {
  delay(2000);

  Serial.begin(9600);
  Serial.println("setup started");

  randomSeed(analogRead(15));

  // AUDIO setup
  TeensyAudioFFTSetup(AUDIO_INPUT_PIN);
  samplingBegin();

  // DISPLAY STUFF
  off = 0x000000;
  FastLED.addLeds<NEOPIXEL, DISPLAY_LED_PIN>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );;

  FastLED.setBrightness(64);
  FastLED.setDither(0);
  setAll(white);
  FastLED.show();
  delay(2000);

  spinner = new Spinner(NUM_LEDS, leds, blue);
  sparkle = new Sparkle(1, NUM_LEDS, leds, pink, 450);
  soundReaction = new SoundReaction(0, 24, leds, pink, 0x000000);

  randomTimeOffset = random(SPINNER_SPEED*SPARKLE_SPEED);
  Serial.println(randomTimeOffset);
  Serial.println(audioInputPin);
  Serial.println("setup complete");
}

void loop() {
  unsigned long currentTime = millis();
  clear();  // this only clears the array, not the LEDs, it's fine at the top

  if (spinnerColorCycle) {
    spinnerHue = ((currentTime + randomTimeOffset)/SPINNER_SPEED) % 256;
    spinnerHSVColor = CHSV(spinnerHue, SATURATION, VALUE);
    hsv2rgb_rainbow( spinnerHSVColor, spinnerRGBColor);
    spinner->setColor(spinnerRGBColor);
    soundReaction->setOnColor(spinnerRGBColor);
  }

  if (sparkleColorCycle) {
    sparkleHue = ((currentTime + randomTimeOffset)/SPARKLE_SPEED) % 256;
    sparkleHSVColor = CHSV(sparkleHue, SATURATION, VALUE);
    hsv2rgb_rainbow(sparkleHSVColor, sparkleRGBColor);
    sparkle->setColor(sparkleRGBColor);
  }

  // Serial.print(touchRead(1));
  // Serial.print("\t\");
  // Serial.println(touchRead(15));

  if (touchRead(FLASHLIGHT_CONTROL_PIN) > TOUCH_SENSITIVITY) {
    flashlight = true;
    setAll(white);
    FastLED.show();
    delay(1000);
  }

  if (flashlight) {
    if (touchRead(FLASHLIGHT_CONTROL_PIN) > TOUCH_SENSITIVITY ||
        touchRead(SPINNER_CONTROL_PIN) > TOUCH_SENSITIVITY ||
        touchRead(SPARKLE_CONTROL_PIN) > TOUCH_SENSITIVITY) {
      flashlight = false;
      setAll(0x000000);
      FastLED.show();
      delay(1000);
    } else {
      setAll(white);
      FastLED.show();
      return;
    }
  }

  if (touchRead(SPINNER_CONTROL_PIN) > TOUCH_SENSITIVITY) {
    spinnerColorCycle = false;
    if (currentTime > nextTime) {
      // Serial.print(currentTime);
      // Serial.print(", ");
      // Serial.println(nextTime);
      nextTime = currentTime + interval;
      spinnerHue = (spinnerHue + 1) % 256;
      spinnerHSVColor = CHSV(spinnerHue, SATURATION, VALUE);
      hsv2rgb_rainbow( spinnerHSVColor, spinnerRGBColor);
      spinner->setColor(spinnerRGBColor);
    }

    setAll(spinnerRGBColor);
    FastLED.show();
    return;
  }

  if (touchRead(SPARKLE_CONTROL_PIN) > TOUCH_SENSITIVITY) {
    sparkleColorCycle = false;
    if (currentTime > nextTime) {
      // Serial.print(currentTime);
      // Serial.print(", ");
      // Serial.println(nextTime);
      nextTime = currentTime + interval;
      sparkleHue = (sparkleHue + 1) % 256;
      sparkleHSVColor = CHSV(sparkleHue, SATURATION, VALUE);
      hsv2rgb_rainbow(sparkleHSVColor, sparkleRGBColor);
      sparkle->setColor(sparkleRGBColor);
    }

    setAll(sparkleRGBColor);
    FastLED.show();
    return;
  }

  float intensity = readRelativeIntensity(currentTime, 2, 4);
  soundReaction->display(min(intensity, 1));

  // spinner->display(currentTime);
  sparkle->display();

  FastLED.show();
}

void setAll(CRGB color) {
  for (uint8_t i=0; i<NUM_LEDS; i++) {
    leds[i] = color;
  }
}

void clear() {
  setAll(off);
}
