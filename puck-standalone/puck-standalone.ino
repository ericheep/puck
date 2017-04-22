#include "Tlc5940.h"
#include "colors.h"
#include "math.h" 

#define NUM_LEDS 16
#define NUM_TLCS 3

float inc;
float sinInc, brightInc, sinBright;

// three per led
int LEDS[NUM_LEDS][3];

void setColor(int ledNum, LedRGB lrgb) {
  ledNum = ledNum % NUM_LEDS;
  int red_pin = LEDS[ledNum][0];
  int green_pin = LEDS[ledNum][1];
  int blue_pin = LEDS[ledNum][2];
  Tlc.set(red_pin, lrgb.r);
  Tlc.set(green_pin, lrgb.g);
  Tlc.set(blue_pin, lrgb.b);
}

void setColor(int ledNum, RGB rgb) {
  LedRGB lrgb = RGBtoLED(rgb);
  setColor(ledNum, lrgb);
}

void setColor(int ledNum, HSV hsv) {
  if (hsv.h > 359) hsv.h = hsv.h % 360;
  RGB rgb = HSVtoRGB(hsv);
  setColor(ledNum, rgb);
}

// converts RGB values from HSV conversion to TLC5940 values
LedRGB RGBtoLED(RGB rgb) {
  if (rgb.r > 1 || rgb.g > 1 || rgb.b > 1) {
    Serial.print("Exceeds expected RGB values");
  }
  else {
    LedRGB lrgb = {
      rgb.r * 4000, rgb.g * 4000, rgb.b * 4000
    };
    return lrgb;
  }
}

// converts HSV to RGB values to be passed into LedRGB
RGB HSVtoRGB(HSV hsv) {
  // algorithm from http://en.wikipedia.org/wiki/HSL_and_HSV#Converting_to_RGB
  RGB rgb;
  RGB rgb_p;

  float chroma = hsv.v * hsv.s;
  float sector = float(hsv.h) / 60.0;

  // remainder is sector mod 2 in the real number sense
  float remainder = sector - ((int(sector) / 2) * 2) ;
  float x = chroma * (1 - abs(remainder - 1));
  switch (int(sector)) {
    case 0:
      rgb_p = (RGB) {
        chroma, x, 0
      };
      break;
    case 1:
      rgb_p = (RGB) {
        x, chroma, 0
      };
      break;
    case 2:
      rgb_p = (RGB) {
        0, chroma, x
      };
      break;
    case 3:
      rgb_p = (RGB) {
        0, x, chroma
      };
      break;
    case 4:
      rgb_p = (RGB) {
        x, 0, chroma
      };
      break;
    case 5:
      rgb_p = (RGB) {
        chroma, 0, x
      };
      break;
    default:
      rgb_p = (RGB) {
        0, 0, 0
      };
  }

  float m = hsv.v - chroma;
  rgb = (RGB) {
    rgb_p.r + m, rgb_p.g + m, rgb_p.b + m
  };
  return rgb;
}

void setup() {
  for (int i = 0; i < NUM_LEDS; i++) {
    for (int j = 0; j < 3; j++) {
      LEDS[i][j] = i * 3 + j;
    }
  }
  Serial.begin(9600);
  Tlc.init();
}

// loop code
// float inc;

// returns a random HSV
HSV randomHSV() {
  return  randomHSV(0, 360, .4, 1, .4, 1);
}

HSV randomHSV(int minH, int maxH, float minS, float maxS, float minV, float maxV) {
  int h = random(minH, maxH);
  float s = (random(int(minS * 1000), int(maxS * 1000)) / 1000.0);
  float v = (random(int(minV * 1000), int(maxV * 1000)) / 1000.0);
  return (HSV) {
    h, s, v
  };
}

float incrementTime(float milliseconds) { 
  return (2.0 * PI)/milliseconds;
}

const float SECOND = 100;
const float MINUTE = 60 * SECOND;
const float HOUR = 60 * MINUTE;
const float DAY = 24 * MINUTE;

float cycle = 0.0;
float color = 0.0;
float range = 360;
float offset = 0.0;

float incrementSize = incrementTime(DAY);

void loop() {
  cycle = fmod(cycle + incrementSize, 2.0 * PI);
  color = offset + range * (sin(cycle) + 1.0) * 0.5;
  
  for (int i = 0; i < 16; i++) {
    HSV hsv = {
      color, 1.0, 0.005
    };
    setColor(i, hsv);
    Tlc.update();
  }

  Serial.println(color);
  delay(10);
}

