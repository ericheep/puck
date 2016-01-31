// give it some time to breathe
HandshakeID talk;
2.5::second => now;

// initial handshake between ChucK and Arduinos
talk.talk.init();
2.5::second => now;

Puck p;
p.setPort(0);

int inc;

p.IDCheck(0) => int check;
<<< check >>>;

while (true) {
    inc + 5 => inc;;
    for (0 => int i; i < 16; i++) {
        p.send(i, 512, 122, 40);
        10::ms => now;
    }
}

1::hour => now;
