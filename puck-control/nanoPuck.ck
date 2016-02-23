// give it some time to breathe
HandshakeID talk;
2.5::second => now;

// initial handshake between ChucK and Arduinos
talk.talk.init();
2.5::second => now;

// Puck assignments to ports
Puck p[2];
for (0 => int i; i < 2; i++) {
    p[i].init(i);
}

// variables you are free to change!

// colors, from 0-1023
0 => int lowHue;
1023 => int highHue;

// midi class
NanoKontrol2 n;

// pre-smoothing
int _baseHue[2][16];
int _baseVal[2][16];

// post-smoothing
int baseHue[2][16];
int baseVal[2][16];

int _randomHue[2][16];
int randomHue[2][16];

int _shiftingHue[2][16];
int shiftingHue[2][16];
int shiftingAdd[2];
int shiftingRange[2];
float shiftingInc[2];
float shiftingSine[2];

int _shadowVal[2][16];
int shadowVal[2][16];
int shadowAdd[2];
int shadowRange[2];
float shadowInc[2];
float shadowSine[2];

float hue[2][16];
float sat[2][16];
float val[2][16];

// iniitialize values
for (0 => int i; i < 2; i++) {
    for (0 => int j; j < 16; j++) {
        0 => hue[i][j];
        127 => sat[i][j];
        0 => val[i][j];
    }
}

// overall brightness and hue
fun void staticColors() {
    for (0 => int i; i < 2; i++) {
        for (0 => int j; j < 16; j++) {
            n.slider[i * 4] => _baseVal[i][j];
            n.knob[i * 4] => _baseHue[i][j];
        }
    }
}

// shadow spectrum
fun void shadowColors() {
    for (0 => int i; i < 2; i++) {
        n.knob[1 + i * 4] => shadowAdd[i];
        //(shadowAdd[i]/127.0 * 0.1) % pi +=> shadowInc[i];
        //((Math.sin(shadowInc[i]) + 1.0) * 0.5) * shadowRange[i]/127.0 => shadowSine[i];

        for (0 => int j; j < 16; j++) {
            n.slider[1 + i * 4] * 1.0 => sat[i][j];
            //(shadowRange[i] * j/8.0) $ int => _shadowVal[i][j];
        }
    }
}


// shifting spectrum
fun void shiftingColors() {
    for (0 => int i; i < 2; i++) {
        n.knob[2 + i * 4] => shiftingAdd[i];
        n.slider[2 + i * 4] => shiftingRange[i];
        (shiftingAdd[i]/127.0 * 0.5) % pi +=> shiftingInc[i];
        ((Math.sin(shiftingInc[i]) + 1.0) * 0.5) * shiftingRange[i] => shiftingSine[i];

        for (0 => int j; j < 16; j++) {
            (shiftingRange[i] * j/16.0) $ int => _shiftingHue[i][j];
        }
    }
}

// adds one random led
fun void randomColors() {
    for (0 => int i; i < 2; i++) {
        if (n.knob[3 + i * 4] > Math.random2(0, 127)) {
            Math.random2(0, 16) => int randomLED;
            for (0 => int j; j < 16; j++) {
                if (j == randomLED) {
                    n.slider[3 + i * 4] => randomHue[i][j];
                }
                else {
                    0 => randomHue[i][j];
                }
            }
        }
        if (n.knob[3 + i * 4] == 0) {
            for (0 => int j; j < 16; j++) {
                0 => randomHue[i][j];
            }
        }
    }
}

// smooth transitions
fun void smoothColors() {
    for (0 => int i; i < 2; i++) {
        for (0 => int j; j < 16; j++) {
            if (baseHue[i][j] < _baseHue[i][j]) {
                baseHue[i][j]++;
            }
            else if (baseHue[i][j] > _baseHue[i][j]) {
                baseHue[i][j]--;
            }
            if (baseVal[i][j] < _baseVal[i][j]) {
                baseVal[i][j]++;
            }
            else if (baseVal[i][j] > _baseVal[i][j]) {
                baseVal[i][j]--;
            }
            if (shiftingHue[i][j] < _shiftingHue[i][j]) {
                shiftingHue[i][j]++;
            }
            else if (shiftingHue[i][j] > _shiftingHue[i][j]) {
                shiftingHue[i][j]--;
            }
            if (shadowVal[i][j] < _shadowVal[i][j]) {
                shadowVal[i][j]++;
            }
            else if (shadowVal[i][j] > _shadowVal[i][j]) {
                shadowVal[i][j]--;
            }
        }
    }
}

fun int convert(float input, float scale) {
    return Math.floor(input/127.0 * scale) $ int;
}

fun void updateColors() {
    for (0 => int i; i < 2; i++) {
        for (0 => int j; j < 16; j++) {
            (baseHue[i][j] + randomHue[i][j] + shiftingHue[i][j]) % 127 + shiftingSine[i] => hue[i][j];
            (baseVal[i][j] - shadowVal[i][j]) * (1.0 - shadowSine[i]) => val[i][j];
        }
    }

    for (0 => int i; i < 2; i++) {
        for (0 => int j; j < 16; j++) {
            p[i].send(j, convert(hue[i][j], lowHue + highHue),
                         convert(sat[i][j], 255),
                         convert(val[i][j], 255));
        }
    }
}

while (true) {
    staticColors();
    shadowColors();
    shiftingColors();
    randomColors();
    smoothColors();
    updateColors();
    (1.0/30.0)::second => now;
}
