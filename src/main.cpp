#include <Arduino.h>
#include <FastLED.h>
#include <Visualization.h>
#include <Spinner.h>
#include <Sparkle.h>

// FAST LED
#define NUM_LEDS 24
#define DISPLAY_LED_PIN 22

CRGB leds[NUM_LEDS];
CRGB off;

void clear();
void setAll(CRGB color);

#define NUM_STREAKS 1
#define NUM_SPARKELS 1

CRGB pink = 0xFF0B20;
CRGB blue = 0x0BFFDD;

CRGB colors[3];
Spinner * spinner;
Sparkle * sparkle;

void setup() {
  delay(2000);

  // Serial.begin(9600);
  // Serial.println("setup started");

  // DISPLAY STUFF
  colors[1] = pink;
  colors[2] = blue;

  off = 0x000000;
  FastLED.addLeds<NEOPIXEL, DISPLAY_LED_PIN>(leds, NUM_LEDS).setCorrection( Typical8mmPixel );;

  FastLED.setBrightness(24);
  setAll(0xFFFFFF);
  FastLED.show();
  delay(2000);

  spinner = new Spinner(NUM_LEDS, leds, blue);
  spinner->setLengthMinMax(12, 12);
  spinner->setIntervalMinMax(30, 30);

  sparkle = new Sparkle(1, NUM_LEDS, leds, pink, 450);

  // Serial.println("setup complete");
}

void loop() {
  clear();  // this only clears the array, not the LEDs, it's fine at the top

  unsigned long currentTime = millis();

  spinner->display(currentTime);

  sparkle->display();

  // setAll(green);
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
