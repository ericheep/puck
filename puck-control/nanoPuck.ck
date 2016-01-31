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
int shiftingBase[2];
int shiftingRange[2];
float shiftingInc[2];

int hue[2][16];
int sat[2][16];
int val[2][16];

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

// shifting spectrum
fun void shiftingColors() {
    for (0 => int i; i < 2; i++) {
        n.slider[2 + i * 4] => shiftingRange[i];
        n.knob[2 + i * 4] => shiftingBase[i];
        if (shiftingInc[i] != 0) {
            (shiftingBase[i]/127.0 * 4.0 + shiftingInc[i])
            % shiftingRange[i] => shiftingInc[i];
        }

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
        }
    }
}

fun int convert(float input, float scale) {
    return Math.floor(input/127.0 * scale) $ int;
}

fun void updateColors() {
    for (0 => int i; i < 2; i++) {
        for (0 => int j; j < 16; j++) {
            (baseHue[i][j] + randomHue[i][j] + shiftingHue[i][j]) % 127 => hue[i][j];
            baseVal[i][j] => val[i][j];
        }
    }

    // for (0 => int i; i < 2; i++) {
    for (0 => int j; j < 16; j++) {
        p[0].send(j, convert(hue[0][j] + shiftingInc[0], lowHue + highHue),
                     convert(sat[0][j], 255),
                     convert(val[0][j], 255));
    }
    // }
}

while (true) {
    staticColors();
    shiftingColors();
    randomColors();
    smoothColors();
    updateColors();
    <<< shiftingInc[0] >>>;
    (1.0/60.0)::second => now;
}
