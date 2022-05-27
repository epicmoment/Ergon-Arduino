int vib1 = 3;
int vib2 = 5;
int sensor1 = 2;
int sensor2 = 7;
int femvolt2 = 12; 

int trykk1 = LOW;
int trykk2 = LOW;

int holdning = HIGH;
int holdningCounter = 0;
int holdningVarsle = LOW;

int pauseCounter = 0;
int pauseVarsle = LOW;
int pauseVarselVib = LOW;

int sendeCount = 0;
int sendeData = 0;

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
    

    // Holdning

    // Sjekker om kun en trykksensor gir input om gangen. Om begge 
    // er på sitter brukeren riktig, om ingen er på sitter ikke
    // brukeren på stolen, og om kun en er på sitter brukeren
    // antakeligvis med dårlig holdning.
    if (trykk1 != trykk2) {

        holdning = LOW;
        
        // Teller hvor lenge brukeren har sittet med dårlig holdning.
        if (holdningCounter < 10) {

            holdningCounter++;

        } else {

            if (!holdningVarsle) {
                
                if (trykk1) {
                    analogWrite(vib1, 255);
                } else {
                    analogWrite(vib2, 255);
                }

                holdningVarsle = HIGH;
            }

        }
    
    // Brukeren sitter enten med god holdning eller ikke på stolen.
    } else {
          
        holdning = HIGH;

        // Slutter å varsle og teller ned hvor lenge brukeren har sittet med dårlig holdning.      
        if (holdningCounter > 0) {

            holdningCounter--;
          
            if (holdningVarsle == HIGH) {

              holdningVarsle = LOW;
              analogWrite(vib1, 0);
              analogWrite(vib2, 0);
            
            }

        }
      
    }

    if (trykk1 || trykk2) {

        if (pauseCounter < 100) {

            pauseCounter++;

        } else {

            if (!pauseVarsle) {

                if (pauseVarselVib) {

                    analogWrite(vib1, 255);
                    analogWrite(vib2, 255);
                    pauseVarselVib = LOW;

                } else {

                    analogWrite(vib1, 0);
                    analogWrite(vib2, 0);
                    pauseVarselVib = HIGH;

                }

            }

        }

    } else {

        if (pauseCounter > 0) {

            pauseCounter -= 10;

            if (pauseVarsle) {
                
                pauseVarsle = LOW;
                analogWrite(vib1, 0);
                analogWrite(vib2, 0);

            }

        }
        
    }

    // Send data om holdning og pause til docken hver 11. kjøresyklus
    // 0: Dårlig holdning
    // 1: God holdning
    // 2: Ta pause, dårlig holdning
    // 3: Ta pause, god holdning
    if (sendeCount < 10) {
        
        sendeCount++;
    
    } else {

        if ((trykk1 || trykk2)) {

            sendeData = 1 + holdning + (pauseVarsle);
        
            Serial.write(sendeData);
            sendeCount = 0;

        }      
        
    }

    Serial.print("Sensor 1: ");
    Serial.print(trykk1);
    Serial.print(" - Sensor 2: ");
    Serial.print(trykk2);
    Serial.print(" - HCount: ");
    Serial.print(holdningCounter);
    Serial.print(" - PCount: ");
    Serial.println(pauseCounter);
  
    delay(300);
}