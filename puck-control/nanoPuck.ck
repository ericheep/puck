// give it some time to breathe
HandshakeID talk;
2.5::second => now;

// initial handshake between ChucK and Arduinos
talk.talk.init();
2.5::second => now;

Puck p[2];

for (0 => int i; i < 2; i++) {
    p[i].init(i);
}

int inc;
while (true) {
    inc + 5 => inc;;
    for (0 => int i; i < 16; i++) {
        p[0].send(i, inc, 255, 255);
        p[1].send(i, inc, 255, 255);
        10::ms => now;
    }
}

1::hour => now;
