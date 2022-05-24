int scoreleds [5][2] = {{4, -1}, {10, 11}, {6, 9}, {3, 5}, {-1, 2}};
int tiltSensor = 12;
int knapp = 7;

// Rød: 4
// Oransje: R10, G11
// Gul: R6, G9
// Lime: R3, G5
// Grønn: 2


unsigned long serialSiste = millis();
unsigned serialIntervall_ms = 1000;
int serialTotal = 0;
int serialGode = 0;

bool viserScore = false; 
unsigned long visScoreSiste = millis();
unsigned long visScoreLengde_s = 4;
int antallLedsTotal = 0;
int antallLedsPaa = 0;

void setup() {

	Serial.begin(9600);

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 2; j++) {
			if (scoreleds[i][j] != -1) {
				pinMode(scoreleds[i][j],  OUTPUT);
			}
		}
	}

	pinMode(knapp, INPUT_PULLUP);
          
}

void loop() {
  
  	// Innlesing av data fra serial hvert sekund
  	if (millis() > serialSiste + serialIntervall_ms) {
    
    	serialSiste = millis();
      
        if (Serial.available() > 0) {

            int byte = Serial.read();
            serialTotal++;

            if (byte == 1) {
                serialGode++;
            }

			Serial.print("Byte: ");
			Serial.print(byte);
            Serial.print(" - Total: ");
            Serial.print(serialTotal);
            Serial.print(" - Gode: ");
            Serial.println(serialGode);

    	}
      
    }

	if (viserScore) {


		if (antallLedsPaa < antallLedsTotal) {
			skruPaaLed(antallLedsPaa);
		}

		if (millis() > visScoreSiste + (visScoreLengde_s*1000)) {

			Serial.println("Fjerner lys!");
			skjulScore();
			viserScore = false;

		}

	}
  
  	// Knapp for å vise score
    if (!digitalRead(knapp)) {

		if (!viserScore) {

			visScore();
			viserScore = true;
		
		}

		visScoreSiste = millis();

    }
  	
    delay(100);
  
}

void visScore() {

	float score = (float) serialGode / (float) serialTotal * 4.7;

	Serial.print("Ratio: ");
	Serial.print((float) serialGode / (float) serialTotal);
	Serial.print(" - Score: ");
	Serial.println(score);

	antallLedsTotal = score + 1;
	antallLedsPaa = 0;
	skruPaaLed(antallLedsPaa);

}

void skruPaaLed(int lednr) {

	if (lednr <= 2) {
		analogWrite(scoreleds[lednr][0], 255);
	}

	if (lednr >= 2) {
		analogWrite(scoreleds[lednr][1], 255);
	}

	antallLedsPaa++;

}

void skjulScore() {

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 2; j++) {
			if (scoreleds[i][j] != -1) {
				digitalWrite(scoreleds[i][j], 0);
			}
		}
	}

	antallLedsTotal = 0;
	antallLedsPaa = 0;

}