
// Oversikt over alle pins som brukes.
int ledpins[] = {6, 5, 4, 3, 2};
int fargepins[] = {10, 11};
int tiltSensor = 12;
int scoreKnapp = 7;
int snoozeKnapp = 8;

// Variabler for henting og telling av data fra stolryggen
int holdningTotal = 1;
int holdningGode = 1;

// Variabler for visning av poengsum.
bool viserScore = false; 
unsigned long visScoreStart = millis();
unsigned visScoreLengde_ms = 4000;
int antallLedsTotal = 0;
int antallLedsPaa = 0;

// Variabler for visning av pausevarsel
bool pauseLysPaa = false;
bool pauseBlink = false;
int pauseBlinkCounter = 0;
int pauseFerdig = false;

// Variabler for stillemodus
bool stillemodus = false;
unsigned long stillemodusStart = millis();
int stillemodusLengdeMin = 1; // 30 min

void setup() {

	Serial.begin(9600);

	// Setter alle pins som går til LEDs til output, og til HIGH, fordi
	// LOW skrur lysene på i denne kretsen
	for (int pin : ledpins) {
		pinMode(pin,  OUTPUT);
		digitalWrite(pin, HIGH);
	}
	
	// To analoge pins styrer fargen hos alle 5 leds. Settes til output.
	pinMode(fargepins[0], OUTPUT);
	pinMode(fargepins[1], OUTPUT);

	// Setter knapper til input med pullup-resistor. Pullup-resitor ble
	// brukt så kretsen kunne være enklere med færre resistorer.
	pinMode(scoreKnapp, INPUT_PULLUP);
	pinMode(snoozeKnapp, INPUT_PULLUP);

	settFarge (130, 70);
          
}

// Hovedloop hvor mange ting sjekkes 10 ganger i sekundet.
void loop() {
	
	if (Serial.available() > 0) {

		int byte = Serial.read();

		if (byte == 1 || byte == 2) {

			holdningTotal++;
			if(byte == 2) {
				holdningGode++;
			}

		} else {

			switch (byte) {

				// pauseblink
				case 3:
					pauseLysPaa = true;
					pauseBlink = true;
					pauseBlinkCounter = 0;
					pauseFerdig = false;
					break;

				// konstant lys
				case 4:
					pauseLysPaa = true;
					pauseBlink = false;
					pauseFerdig = false;
					break;

				// pause ferdig
				case 5:
					pauseFerdig = true;
					break;

				// pause avsluttet
				case 6:
					pauseLysPaa = false;
					if (!viserScore) skruAvLeds();
					break;

			}

		}

		// Til debugging
		/*Serial.print("Byte: ");
		Serial.print(byte);
		Serial.print(" - Total: ");
		Serial.print(holdningTotal);
		Serial.print(" - Gode: ");
		Serial.println(holdningGode);*/

	}

	// Oppgaver om skal utføres om poengsumen skal vises i denne syklusen.
	if (viserScore) {

		// Sjekker om alle leds som skal på er på. Hvis ikke skrur den på
		// neste led som skal på. Dette er hvordan den
		// gradvise/animerte visningen oppnås.

		/*Serial.print("Leds som skal på: ");
		Serial.print(antallLedsTotal);
		Serial.print(" - Leds som er på: ");
		Serial.println(antallLedsPaa);*/

		if (antallLedsPaa < antallLedsTotal) {
			skruPaaScoreLed(antallLedsPaa);
		}

		// Sjekker om det har gått fire sekund siden knappen for å vise
		// poengsum ble trykt på sist, og skjuler så poengsummen.
		if (millis() > visScoreStart + (visScoreLengde_ms)) {

			skruAvLeds();
			settFarge(130, 70);
			viserScore = false;

			if (stillemodus) {
				settFarge(50, 0);
				digitalWrite(ledpins[2], LOW);
			}

		}

	} else if (pauseLysPaa && !stillemodus) {

		if (pauseBlink) {

			settFarge(130, 70);

			if (pauseBlinkCounter == 0) {
				digitalWrite(ledpins[0], LOW);
				digitalWrite(ledpins[4], LOW);;
            } else if (pauseBlinkCounter == 10) {
				skruAvLeds();
            }

            pauseBlinkCounter += pauseBlinkCounter == 19 ? -19 : 1;

		} else {
			
			if (pauseFerdig) {
				settFarge(0, 255);
			} else {
				settFarge(130, 70);
			}

			skruPaaScoreLed(0);
			skruPaaScoreLed(4);

		}

	}
  
  	// Registrer knappetrykk for å vise poengsum. Om poengsum allerede
	// vises forlenges heller visningstiden.
    if (!digitalRead(scoreKnapp)) {

		if (!viserScore) {

			visScore();
			viserScore = true;
		
		}

		visScoreStart = millis();

    }

	// Stillemodus-knapp
	if (!digitalRead(snoozeKnapp)) {

		stillemodus = !stillemodus;

		skruAvLeds();
		

		if (stillemodus) {
			
			stillemodusStart = millis();
			
			pauseLysPaa = false;

			settFarge(50, 0);
			for (int i = 0; i < 2; i++) {
				digitalWrite(ledpins[2], 0);
				delay(100);
				digitalWrite(ledpins[1], i);
				digitalWrite(ledpins[3], i);
				delay(100);
				digitalWrite(ledpins[0], i);
				digitalWrite(ledpins[4], i);
				delay(100);
			}

		} else {
			
			settFarge(0, 255);
			for (int i = 0; i < 2; i++) {
				digitalWrite(ledpins[0], i);
				digitalWrite(ledpins[4], i);
				
				delay(100);
				digitalWrite(ledpins[1], i);
				digitalWrite(ledpins[3], i);
				delay(100);
				digitalWrite(ledpins[2], i);
				delay(100);
			}
			
		}

		Serial.write(stillemodus);

	} else {

		/*if (stillemodus && ((millis() - stillemodusStart) > (stillemodusLengdeMin*60*1000))) {
			stillemodus = false;
			Serial.write(stillemodus);
		}*/

	}
  	
	// Venter
    delay(100);
  
}

// Funksjon for å beregne poengsum og å starte gradvis/animert visning
// av den.
void visScore() {
	
	skruAvLeds();

	float score = (float) holdningGode / (float) holdningTotal * 4.7;

	// Debugging
	/*Serial.print("Ratio: ");
	Serial.print((float) holdningGode / (float) holdningTotal);
	Serial.print(" - Score: ");
	Serial.println(score);*/

	antallLedsTotal = score + 1;
	antallLedsPaa = 0;
	
	int farger[5][2] = {{255, 0}, {255, 50}, {170, 220}, {50, 255}, {0, 255}};
	settFarge(farger[antallLedsTotal-1][0], farger[antallLedsTotal-1][1]);

	skruPaaScoreLed(antallLedsPaa);

}

// Funksjon for å skru på én LED om gangen. Brukes for å animere
// visningen av poengsum
void skruPaaScoreLed(int lednr) {

	digitalWrite(ledpins[lednr], LOW);
	antallLedsPaa++;

}

// Funksjon for å skru av alle LEDs når poengsummen ikke skal vises.
void skruAvLeds() {

	for (int pin : ledpins) {
		digitalWrite(pin, HIGH);
	}

	antallLedsTotal = 0;
	antallLedsPaa = 0;

}

// Funksjon for å sette fargen til alle fem LEDs.
void settFarge(int r, int g) {
    analogWrite(fargepins[0], r);
    analogWrite(fargepins[1], g);
}

