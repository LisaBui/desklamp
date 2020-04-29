#include <FastLED.h>
#include <Arduino.h>

FASTLED_USING_NAMESPACE

// This pattern shows you a rainbowsnake and afterwards it dimming the desk lamp
// slowly up
//
// Based on example by Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    8
#define LED_TYPE    WS2811
#define COLOR_ORDER BRG
#define NUM_LEDS    67
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          255
#define FRAMES_PER_SECOND  120

bool startup = true;
bool worklightReady = true;

void setup() {
  Serial.begin(9600);
  Serial.println("Hello");

  delay(3000); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { sinelon, worklight, setWorklightOn };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  if (startup) {
    gPatterns[gCurrentPatternNumber]();
  } else if (worklightReady) {
    setWorklightOn();
  } else {
    gPatterns[1]();
  }

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  EVERY_N_MILLISECONDS( 7200 ) {
    startup = false;
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void sinelon()
{ 
  Serial.println("doing sinelon Pattern");
  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }

  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS - 1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

int brightness = 0;

void worklight()
{ 
  if (brightness >= 255) return; 
  Serial.println("doing worklight Pattern");
  FastLED.setBrightness(brightness);
  FastLED.show();
  delay(50);
  brightness++;
}

void setWorklightOn()
{ 
  Serial.println("setting up worklight Pattern");
  FastLED.setBrightness(0);
  for (int j = 0; j < NUM_LEDS; j++) {
    leds[j] = TypicalSMD5050 & CarbonArc;
    FastLED.show();
  } 
  worklightReady = false;
}
