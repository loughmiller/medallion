#include <Arduino.h>
#include <FastLED.h>
#include <Visualization.h>
#include <Spinner.h>
#include <Sparkle.h>

// FAST LED
#define NUM_LEDS 24
#define DISPLAY_LED_PIN 4      // VERSION 2
#define SPARKLE_CONTROL_PIN 1
#define SPINNER_CONTROL_PIN 15
#define FLASHLIGHT_CONTROL_PIN 23

#define TOUCH_SENSITIVITY 700

CRGB leds[NUM_LEDS];

#define NUM_STREAKS 1
#define NUM_SPARKELS 1

#define SATURATION 244

uint8_t spinnerHue;
uint8_t sparkleHue;

Visualization * all;
Spinner * spinner;
Sparkle * sparkle;

uint16_t interval = 50;
uint32_t nextTime = 0;

bool flashlight = false;
bool spinnerColorCycle = true;
bool sparkleColorCycle = true;

#define SPINNER_SPEED 14009
#define SPARKLE_SPEED 2333

uint32_t randomTimeOffset;    // Just so we don't start with the same colors
                              // every time

void setup() {
  delay(1000);

  Serial.begin(9600);
  Serial.println("setup started");

  randomSeed(analogRead(15));

  // DISPLAY STUFF
  FastLED.addLeds<NEOPIXEL, DISPLAY_LED_PIN>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );;
  all = new Visualization(NUM_LEDS, 1, 0, 0, leds);

  FastLED.setBrightness(64);
  FastLED.setDither(0);
  all->setAllCRGB(0x303030);
  FastLED.show();
  delay(1000);

  spinner = new Spinner(NUM_LEDS, 0, SATURATION, leds);
  sparkle = new Sparkle(NUM_LEDS, 0, SATURATION, leds, 231);

  randomTimeOffset = random(SPINNER_SPEED*SPARKLE_SPEED);
  Serial.println(randomTimeOffset);
  Serial.println("setup complete");
}

void loop() {
  unsigned long currentTime = millis();
  all->setAllCRGB(0x000000);  // this only clears the array, not the LEDs, it's fine at the top

  if (spinnerColorCycle) {
    spinnerHue = ((currentTime + randomTimeOffset)/SPINNER_SPEED) % 256;
    spinner->setHue(spinnerHue);
  }

  if (sparkleColorCycle) {
    sparkleHue = ((currentTime + randomTimeOffset)/SPARKLE_SPEED) % 256;
    sparkle->setHue(sparkleHue);
  }

  // Serial.print(touchRead(1));
  // Serial.print("\t\");
  // Serial.println(touchRead(15));

  if (touchRead(FLASHLIGHT_CONTROL_PIN) > TOUCH_SENSITIVITY) {
    flashlight = true;
    all->setAllCRGB(0xFFFFFF);
    FastLED.show();
    delay(1000);
  }

  if (flashlight) {
    if (touchRead(FLASHLIGHT_CONTROL_PIN) > TOUCH_SENSITIVITY ||
        touchRead(SPINNER_CONTROL_PIN) > TOUCH_SENSITIVITY ||
        touchRead(SPARKLE_CONTROL_PIN) > TOUCH_SENSITIVITY) {
      flashlight = false;
      all->setAllCRGB(0x000000);
      FastLED.show();
      delay(1000);
    } else {
      all->setAllCRGB(0xFFFFFF);
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
      spinner->setHue(spinnerHue);
    }

    spinner->setAllHue(spinnerHue);
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
      sparkle->setHue(sparkleHue);
    }

    sparkle->setAllHue(sparkleHue);
    FastLED.show();
    return;
  }


  spinner->display(currentTime);

  sparkle->display();

  FastLED.show();
}
