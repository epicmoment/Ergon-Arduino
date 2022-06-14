
// Oversikt over alle pins som brukes.
int ledpins[] = {6, 5, 4, 3, 2};
int fargepins[] = {10, 11};
int tiltSensor = 12;
int scoreKnapp = 7;
int snoozeKnapp = 8;

// Variabler for telling av data fra stolryggen
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
// Data fra stolryggen leses av, knappetrykk håndteres og varslinger vises.
void loop() {
	

	// Sjekker om data fra stolryggen er mottatt og leser det inn i variabelen byte.
	if (Serial.available() > 0) {

		int byte = Serial.read();

		// Hvis byten er 1 eller 2 handler dataen om holdning, enten god eller dårlig.
		// Docken lagrer dataen i variabler til beregning av poengsum.
		if (byte == 1 || byte == 2) {

			holdningTotal++;
			if(byte == 2) {
				holdningGode++;
			}

		// Tall over 2 handler om pauser
		} else {

			switch (byte) {

				// Brukeren må reise seg og ta pause, lys skal blinke
				case 3:
					pauseLysPaa = true;
					pauseBlink = true;
					pauseBlinkCounter = 0;
					pauseFerdig = false;
					break;

				// Brukeren har reist seg og lysene skal slutte å blinke men fortsatt lyse
				case 4:
					pauseLysPaa = true;
					pauseBlink = false;
					pauseFerdig = false;
					break;

				// Pausen er ferdig, lysene skal bli grønne
				case 5:
					pauseFerdig = true;
					break;

				// Pause avsluttet, slutt å lys
				case 6:
					pauseLysPaa = false;
					if (!viserScore) skruAvLeds();
					break;

			}

		}

	}

	// Oppgaver som skal utføres om poengsumen skal vises i denne syklusen.
	if (viserScore) {

		// Sjekker om alle leds som skal på er på. Hvis ikke skrur den på
		// neste led. Dette er hvordan den gradvise/animerte visningen oppnås.
		if (antallLedsPaa < antallLedsTotal) {
			skruPaaScoreLed(antallLedsPaa);
		}

		// Sjekker om det har gått fire sekund siden knappen ble trykt på, og skjuler så poengsummen.
		if (millis() > visScoreStart + (visScoreLengde_ms)) {

			skruAvLeds();
			settFarge(130, 70);
			viserScore = false;

			// Hvis stillemodus er på skal det røde lyset skrus på igjen.
			if (stillemodus) {
				settFarge(50, 0);
				digitalWrite(ledpins[2], LOW);
			}

		}

	// Hvis docken ikke skal vise poengsum, sjekk om den skal vise lys for pause og stillemodus ikke er på
	} else if (pauseLysPaa && !stillemodus) {

		// Hvis brukeren må reise seg skal lysene blinke
		if (pauseBlink) {

			settFarge(130, 70);

			// En teller brukes for å holde styr på blinkingen
			if (pauseBlinkCounter == 0) {
				digitalWrite(ledpins[0], LOW);
				digitalWrite(ledpins[4], LOW);;
            } else if (pauseBlinkCounter == 10) {
				skruAvLeds();
            }

			// Tilbakestiller telleren om den er 19, altså om det har gått 20 sykler eller 2 sekunder
            pauseBlinkCounter += pauseBlinkCounter == 19 ? -19 : 1;

		// Hvis brukeren har reist seg skal ikke pauselysene blinke, bare lyse
		} else {
			
			// De lyser grønt om pausen er ferdig, og ellers gult
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

		// 
		stillemodus = !stillemodus;

		// Lysanimasjon for stillemodus skal vises så LEDs skrus av
		skruAvLeds();
		if (stillemodus) {
			
			stillemodusStart = millis();
			pauseLysPaa = false;

			// Lysanimasjon for å vise at stillemodus skrus på. i er først 0 så 1,
			// som er det samme som LOW og så HIGH, og LOW skrur lysene på i denne kretsen, mens
			// HIGH skrur de av. ledpin 2 skrives det kun 0 til fordi den skal forbli på.
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
			
			// Lysanimasjon for å vise at stillemodus skrus av
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

		// Beskjed om stillemodus sendes til stolryggen.
		Serial.write(stillemodus);

	} else {

		// Hvis tiden for stillemodus har løpt ut skrus stillemodus av
		if (stillemodus && ((millis() - stillemodusStart) > (stillemodusLengdeMin*60*1000))) {
			stillemodus = false;
			Serial.write(stillemodus);
		}

	}
  	
	// Venter 1/10 sekund før kjøring av neste syklus
    delay(100);
  
}

// Funksjon for å beregne poengsum og å starte gradvis/animert visning
// av den.
void visScore() {
	
	skruAvLeds();

	float score = (float) holdningGode / (float) holdningTotal * 4.7;

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

