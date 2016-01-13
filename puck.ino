#include "Tlc5940.h"
#include "colors.h"

#define NUM_SUB_LEDS 16
#define NUM_LEDS 16
#define NUM_TLCS 3

int LEDS[NUM_SUB_LEDS][NUM_TLCS];

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
  if(rgb.r > 1 || rgb.g > 1 || rgb.b > 1) {
    Serial.print("Exceeds expected RGB values");
  }
  else {
    LedRGB lrgb = {
      rgb.r*4000, rgb.g*4000, rgb.b*4000                            
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
  switch(int(sector)) {
  case 0:
    rgb_p = (RGB){
      chroma, x, 0                    
    };
    break;
  case 1:
    rgb_p = (RGB){
      x, chroma, 0                    
    };
    break;
  case 2:
    rgb_p = (RGB){
      0, chroma, x                    
    };
    break;
  case 3:
    rgb_p = (RGB){
      0, x, chroma                    
    };
    break;
  case 4:
    rgb_p = (RGB){
      x, 0, chroma                    
    };
    break;
  case 5:
    rgb_p = (RGB){
      chroma, 0, x                    
    };
    break;
  default:
    rgb_p = (RGB){
      0, 0, 0                    
    };
  }

  float m = hsv.v - chroma;
  rgb = (RGB){
    rgb_p.r + m, rgb_p.g + m, rgb_p.b + m              };
  return rgb;
}

void setup() {
  for (int i = 0; i < NUM_LEDS; i++) {
    for (int j = 0; j < 3; j++) {
      LEDS[i][j] = i * 3 + j;    
    }
  }
  Tlc.init();
}

// loop code
float inc;

// returns a random HSV
HSV randomHSV() {
  return  randomHSV(0, 360, .4, 1, .4, 1);
}


HSV randomHSV(int minH, int maxH, float minS, float maxS, float minV, float maxV) {
  int h = random(minH, maxH);
  float s = (random(int(minS * 1000), int(maxS * 1000)) / 1000.0);
  float v = (random(int(minV * 1000), int(maxV * 1000)) / 1000.0);
  return (HSV){h, s, v};
}

void gradientShifts(float add, float seconds) {
  float milliwait = 10;
  float inc = 0;
  int iterations = seconds / (milliwait * 0.001);
  
  for (int k = 0; k < iterations; k++) {
    inc = inc + add;

    if (inc > 360) {
      inc = inc - 360; 
    }
    Tlc.clear();

    for (int i = 0; i < NUM_LEDS; i++) {
      HSV hsv = {
        240 + i * 5, 1, 1        
      };
      setColor(i, hsv);
    }

    Tlc.update();
    delay(10);
  }
}

void walkDrop() {
  int current_leds[16];
  HSV current_hsvs[16];
  long current_ttls[16];

  int max_ttl = 2000;
  int min_ttl = 2000;
  float minS = .2;
  float maxS = 1;
  float minV = .4;
  float maxV = .6;


  for (int i = 0; i < 16; i++) {
    current_leds[i] = random(0, NUM_LEDS);
    current_hsvs[i] = randomHSV(0, 360, minS, maxS, minV, maxV);
    current_ttls[i] = random(min_ttl, max_ttl);
  }

  Tlc.clear();
  for (int t = 0; t < 3000; t++) {
    Tlc.clear();
    for (int i = 0; i < 16; i++) {
      int change = 0;
      int glow = 0;

      if (glow) {
         current_hsvs[i].v += .01;
         if(current_hsvs[i].v > 1) {
           change = 1;
         }
      }
      else {
         current_hsvs[i].v -= .01;
         if(current_hsvs[i].v < 0) {
           change = 1;
         }
      }
      current_ttls[i] -= 1;
      if(current_ttls[i] < 0) {
         change = 1;
      }

      if (change) {
        current_leds[i] = random(0, NUM_LEDS);
        current_hsvs[i] = randomHSV(0, 360, minS, maxS, minV, maxV);
        current_ttls[i] = random(min_ttl, max_ttl);
      }
      setColor(current_leds[i], current_hsvs[i]);

    }
    Tlc.update();
    delay(10);
  }
}

void cyclingGradients() {
  int c = 0;  
  int loopTime = 30;
  for (int i = 0; i < NUM_LEDS; i++) {
      int n = i % 21;
      Tlc.clear();
      for (int j = 0; j < 150; j++) {
        int base = i * 2;
        Tlc.clear();
        HSV hsv1 = {base + c + j - 5, 1, .5 };
        HSV hsv2 = {base + c + j - 10, 1, .5 };
        HSV hsv3 = {base + c + j - 15, 1, .5 };
        HSV hsv4 = {base + c + j - 20, 1, .5 };
        HSV hsv5 = {base + c + j - 25, 1, .5 };
        HSV hsv6 = {base + c + j - 30, 1, .5 };
        HSV hsv7 = {base + c + j - 35, 1, .5 };
        HSV hsv8 = {base + c + j - 40, 1, .5 };
        if (j > 5 && j < 100) setColor(n, hsv1);
        if (j > 10 && j < 105) setColor(n + 2, hsv2);
        if (j > 15 && j < 110) setColor(n + 4, hsv3);
        if (j > 20 && j < 115) setColor(n + 6, hsv4);
        if (j > 25 && j < 120) setColor(n + 8, hsv5);
        if (j > 30 && j < 125) setColor(n + 10, hsv6);
        if (j > 35 && j < 130) setColor(n + 12, hsv7);
        if (j > 40 && j < 135) setColor(n + 14, hsv8);
        Tlc.update();
        delay(loopTime);  
      }
   }
}

void loop() {
  // walkDrop();
  //cyclingGradients();
  gradientShifts(0.2, 30);
}

