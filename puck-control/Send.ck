// Send.ck

public class Send {

    // static communication object
    Handshake h;

    fun void send(int led, int data) {
        h.talk.note(led, data);
    }
}

