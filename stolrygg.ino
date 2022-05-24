int vib1 = 3;
int vib2 = 5;
int sensor1 = 2;
int sensor2 = 7;
int femvolt2 = 12; 

int trykk1 = LOW;
int trykk2 = LOW;
int holdning = HIGH;

int counter = 0;
int varsle = LOW;

int sendCount = 0;

void setup() {
    
    pinMode(vib1, OUTPUT);
    pinMode(vib2, OUTPUT);
    pinMode(sensor1, INPUT);
    pinMode(sensor2, INPUT);

    pinMode(femvolt2, OUTPUT);
    digitalWrite(femvolt2, HIGH);

    Serial.begin(9600);
    
}

void loop() {

    // Leser av inndata fra trykksensorer
    trykk1 = digitalRead(sensor1);
    trykk2 = digitalRead(sensor2);
    
    // Sjekker om kun en trykksensor gir input om gangen. Om begge 
    // er på sitter brukeren riktig, om ingen er på sitter ikke
    // brukeren på stolen, og om kun en er på sitter brukeren
    // antakeligvis med dårlig holdning.
    if (trykk1 != trykk2) {

        holdning = LOW;
        
        // Teller hvor lenge brukeren har sittet med dårlig holdning.
        if (counter < 10) {

            counter++;

        } else {
          
            varsle = HIGH;

            if (trykk1) {
                analogWrite(vib1, 255);
            } else {
                analogWrite(vib2, 255);
            }

        }
    
    // Brukeren sitter enten med god holdning eller ikke på stolen.
    } else {
          
        holdning = HIGH;

        // Slutter å varsle og teller ned hvor lenge brukeren har sittet med dårlig holdning.      
        if (counter > 0) {
          
            if (varsle == HIGH) {

              varsle = LOW;
              analogWrite(vib1, 0);
              analogWrite(vib2, 0);
            
            }
          
            counter--;

        }
      
    }
      
    if (sendCount < 10) {
        
        sendCount++;
    
    } else {

        if ((trykk1 || trykk2)) {

            Serial.write(holdning);

            sendCount = 0;

        }
    }

    Serial.print("Sensor 1: ");
    Serial.print(trykk1);
    Serial.print(" - Sensor 2: ");
    Serial.print(trykk2);
    Serial.print(" - Count: ");
    Serial.println(counter);
  
    delay(300);
}