
// Oversikt over alle pins som brukes.
int ledpins[] = {6, 5, 4, 3, 2};
int fargepins[] = {10, 11};
int tiltSensor = 12;
int scoreKnapp = 7;
int snoozeKnapp = 8;

// Variabler for henting og telling av data fra stolryggen
unsigned long serialSiste = millis();
unsigned serialIntervall_ms = 500;
int serialTotal = 1;
int serialGode = 1;

// Variabler for visning av poengsum.
bool viserScore = false; 
unsigned long visScoreStart = millis();
unsigned visScoreLengde_ms = 4000;
int antallLedsTotal = 0;
int antallLedsPaa = 0;

// Variabler for visning av pausevarsel
int visPause = 0;

void setup() {

	Serial.begin(9600);

	// Setter alle pins som går til LEDs til output, og sett de til HIGH, fordi
	// LOW skrur lysene på i denne kretsen
	for (int i : ledpins) {
		pinMode(ledpins[i],  OUTPUT);
		digitalWrite(ledpins[i], HIGH);
	}
	
	// To analoge pins styrer fargen hos alle 5 leds. Settes til output.
	pinMode(fargepins[0], OUTPUT);
	pinMode(fargepins[1], OUTPUT);

	// Setter knapper til input med pullup-resistor. Pullup-resitor ble
	// brukt så kretsen kunne være enklere med færre resistorer.
	pinMode(scoreKnapp, INPUT_PULLUP);
	pinMode(snoozeKnapp, INPUT_PULLUP);
          
}

// Hovedloop hvor mange ting sjekkes 10 ganger i sekundet.
void loop() {
 
  	// Innlesing av data fra stolrygg via Serial hvert sekund.
	// Hopper over om det er under et sekund siden siste henting.
  	if (millis() > serialSiste + serialIntervall_ms) {
    
    	serialSiste = millis();

		visPause = false;
      
        if (Serial.available() > 0) {

            int byte = Serial.read();
            if (byte < 2) {
				serialTotal++;
				if(byte == 1) {
                	serialGode++;
				}
            }

			if (byte > 2) {
				visPause = true;
			}

			// Til debugging
			Serial.print("Byte: ");
			Serial.print(byte);
            Serial.print(" - Total: ");
            Serial.print(serialTotal);
            Serial.print(" - Gode: ");
            Serial.println(serialGode);

    	}
      
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

			Serial.println("Fjerner lys!");
			skruAvLeds();
			viserScore = false;

		}

	} else if (visPause) {

		digitalWrite(10, 255);
		digitalWrite(3, 255);

	} else {

		//TODO: Legg til sjekk om pauselys er på!
		if (true) {
			skruAvLeds();
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
  	
	// Venter
    delay(100);
  
}

// Funksjon for å beregne poengsum og å starte gradvis/animert visning
// av den.
void visScore() {
	
	skruAvLeds();

	float score = (float) serialGode / (float) serialTotal * 4.7;

	// Debugging
	Serial.print("Ratio: ");
	Serial.print((float) serialGode / (float) serialTotal);
	Serial.print(" - Score: ");
	Serial.println(score);

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
	digitalWrite(ledpins[lednr], LOW);

	antallLedsPaa++;

}

// Funksjon for å skru av alle LEDs når poengsummen ikke skal vises.
void skruAvLeds() {

	for (int i = 0; i < 5; i++) {
		digitalWrite(ledpins[i], HIGH);
	}

	antallLedsTotal = 0;
	antallLedsPaa = 0;

}

// Funksjon for å sette fargen til alle fem LEDs.
void settFarge(int r, int g) {
    analogWrite(fargepins[0], r);
    analogWrite(fargepins[1], g);
}