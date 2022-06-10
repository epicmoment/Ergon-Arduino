int vib1 = 3;
int vib2 = 5;
int sensor1 = 2;
int sensor2 = 7;
int femvolt2 = 12;

int trykk1 = LOW;
int trykk2 = LOW;

int klokkehastighet = 5;
int sendeCount = 0;
bool stillemodus = false;

int holdningVarselSek = 15;
int holdning = HIGH;
int holdningCounter = 0;
int holdningVarsle = LOW;
int holdningVarselIntervallSek = 8;

int pauseEtterMin = 2;
int pauseVarighetMin = 1; // Pass på at denne går opp i pauseEtterMin
int pauseCounter = 0;
bool taPause = false;
bool tarPause = false;
int pauseVibCounter = 0;
bool varslePause = false;
bool pauseFerdig = false;



void setup() {
    
    pinMode(vib1, OUTPUT);
    pinMode(vib2, OUTPUT);
    pinMode(sensor1, INPUT);
    pinMode(sensor2, INPUT);

    pinMode(femvolt2, OUTPUT);
    digitalWrite(femvolt2, HIGH);

    Serial.begin(9600);
    
}

// Kjører 5 ganger i sekundet
void loop() {

    // Leser av inndata fra trykksensorer
    trykk1 = digitalRead(sensor1);
    trykk2 = digitalRead(sensor2);

    //Serial.print(Serial.available());
    
    if (Serial.available() > 0) {

        int byte = Serial.read();

        //Serial.println(byte == 1);

        switch (byte) {

            // start stillemodus
            case 1:
                //Serial.println("Stillemodus paa!");
                stillemodus = true;
                analogWrite(vib1, LOW);
                analogWrite(vib1, LOW);
                break;

            // stopp stillemodus
            case 0:
                //Serial.println("Stillemodus av!");
                stillemodus = false;
                break;

        }

    }

    // Sjekker om kun en trykksensor gir input om gangen. Om begge 
    // er på sitter brukeren riktig, om ingen er på sitter ikke
    // brukeren på stolen, og om kun en er på sitter brukeren
    // antakeligvis med dårlig holdning.
    holdning = !(trykk1 != trykk2);


    // Hvis stillemodus ikke er på
    if (!stillemodus) {


        // HOLDNING

        if (!holdning) {
        
            // Teller hvor lenge brukeren har sittet med dårlig holdning.
            if (holdningCounter < holdningVarselSek*klokkehastighet) {

                holdningCounter++;

            } else {

                holdningVarsle = HIGH;

                int motor;
                    
                if (trykk1) {
                    int motor = vib1;
                } else {
                    int motor = vib2;
                }

                // Gi varslingsmønster
                analogWrite(motor, 255);
                delay(100);
                analogWrite(motor, 0);
                delay(100);
                analogWrite(motor, 255);
                delay(300);
                analogWrite(motor, 0);

                // Sett teller litt tilbake for å vente litt før neste varsel
                holdningCounter -= holdningVarselIntervallSek*klokkehastighet;

            }

        // Brukeren sitter enten med god holdning eller ikke på stolen.
        } else {
    
            // Slutter å varsle og teller ned hvor lenge brukeren har sittet med dårlig holdning.      
            if (holdningCounter > 0) {

                holdningCounter--;
            
                if (holdningVarsle == HIGH) {

                    holdningVarsle = LOW;

                    // Godkjenningsvibrasjon
                    analogWrite(vib2, 200);
                    delay(200);
                    analogWrite(vib2, 0);
                
                }

            }

        }  

        // PAUSE

        // Bruker sitter på stolen
        if (trykk1 || trykk2) { 

            // Tell opp hvis brukeren sitter og ikke har begynt pause
            // hver count representerer 1/5(klokkehastighet) sekunder
            if (!taPause) {
                pauseCounter += 2;
            }

            if (pauseCounter >= pauseEtterMin*60*klokkehastighet) {

                // Start pausevarsling
                if (!taPause) {
                    taPause = true;

                    // Send kode 3: blink
                    Serial.write(3);
                    varslePause = true;
                    pauseVibCounter = 0;

                }

            }

            // Vibrer og blink om bruker setter seg ned i løpet av pause
            if (taPause && tarPause) {

                // Send kode 3: blink
                Serial.write(3);
                varslePause = true;
                pauseVibCounter = 0;

            }

            if (varslePause) {

                if (pauseVibCounter == 0) {
                    analogWrite(vib1, 255);
                    analogWrite(vib2, 255);
                } else if (pauseVibCounter == 5) {
                    analogWrite(vib1, 0);
                    analogWrite(vib2, 0);
                }

                pauseVibCounter += pauseVibCounter == 9 ? -9 : 1;

            }

            if (pauseFerdig) {
                    pauseFerdig = false;

                    // Send kode 6: pause avsluttet
                    Serial.write(6);
                    varslePause = false;
                    
            }

            tarPause = false;

        // Bruker sitter ikke på stolen
        } else { 

            // Sjekk om bruker nettopp reiste seg
            if (taPause && !tarPause) {

                // Send kode 4: lys konstant
                Serial.write(4);

                varslePause = false;
                analogWrite(vib1, 0);
                analogWrite(vib2, 0);

            }

            if (pauseCounter > 0) {

                // Tell ned slik at en hel pause ender opp på 0.
                // Ganger med 60 for mer nøyaktig int-divisjon
                int tellNedMed = pauseEtterMin/pauseVarighetMin;

                if (pauseCounter > tellNedMed) {
                    pauseCounter -= tellNedMed *2;
                } else {
                    pauseCounter = 0;

                }


            } else {

                if (taPause) {
                    taPause = false;
                    pauseFerdig = true;

                    // Send kode 5: pause ferdig
                    Serial.write(5);
                    varslePause = false;
                    
                }

            }

            tarPause = true;
            
        }

    }

    // Send data om holdning til docken hver 11. kjøresyklus
    // 1: Dårlig holdning
    // 2: God holdning
    if (sendeCount < 10) {
        sendeCount++;
    } else {
        if ((trykk1 || trykk2)) {
            Serial.write(1 + holdning);
            sendeCount = 0;
        }      
    }

    /*Serial.print("Sensor 1: ");
    Serial.print(trykk1);
    Serial.print(" - Sensor 2: ");
    Serial.print(trykk2);
    Serial.print(" - HCount: ");
    Serial.print(holdningCounter);
    Serial.print(" - PCount: ");
    Serial.print(pauseCounter);
    Serial.print(" - Varsle pause: ");
    Serial.println(taPause);*/

    delay(200);

}