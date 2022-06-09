
int ledpins[] = {6, 5, 4, 3, 2};
int fargepins[] = {10, 11};

void setup() {
  
    for (int pin = 0; pin < sizeof(ledpins); pin++) {
        pinMode(ledpins[pin], OUTPUT);
        digitalWrite(ledpins[pin], HIGH);
    }
    
    pinMode(7, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);
    
    pinMode(fargepins[0], OUTPUT);
    pinMode(fargepins[1], OUTPUT);
    settFarge(255, 0);
 
}


void loop() {
 
    if (!digitalRead(7)) {

        int farger[5][2] = {{255, 0}, {200, 50}, {170, 170}, {50, 200}, {0, 255}};

        for (int i = 0; i < 5; i++) {
            digitalWrite(ledpins[i], LOW);
            delay(100);
        }

        delay(3500);

        for (int i = 0; i < 5; i++) {
            digitalWrite(ledpins[i], HIGH);
        }

    }
  
    delay(50);

    /*if (!digitalRead(8)) {
        settFarge(0, 255);
    } else {
        settFarge(255, 0);
    }*/
  
}
  

void settFarge(int r, int g) {
    analogWrite(fargepins[0], r);
    analogWrite(fargepins[1], g);
}