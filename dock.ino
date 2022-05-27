
// Oversikt over alle pins som brukes.
// 2D array brukes til leds sfor å gruppere pins som hører til
// samme RG-LED.
int scoreleds [5][2] = {{4, -1}, {10, 11}, {6, 9}, {3, 5}, {-1, 2}};
int tiltSensor = 12;
int knapp = 7;

// Oversikt over hvilke pins som går til hvilke LED-pærer
// Rød: 4
// Oransje: R10, G11
// Gul: R6, G9
// Lime: R3, G5
// Grønn: 2

// Variabler for henting og telling av data fra stolryggen
unsigned long serialSiste = millis();
unsigned serialIntervall_ms = 1000;
int serialTotal = 0;
int serialGode = 0;

// Variabler for visning av poengsum.
bool viserScore = false; 
unsigned long visScoreSiste = millis();
unsigned long visScoreLengde_s = 4;
int antallLedsTotal = 0;
int antallLedsPaa = 0;

// Variabel for visning av pausevarsel
int visPause = 0;

void setup() {

	Serial.begin(9600);

	// Setter alle pins som går til LEDs til output.
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 2; j++) {
			if (scoreleds[i][j] != -1) {
				pinMode(scoreleds[i][j],  OUTPUT);
			}
		}
	}

	// Setter knapp til input med pullup-resistor så kretsen kan være
	// enklere med færre resistorer.
	pinMode(knapp, INPUT_PULLUP);
          
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
            serialTotal++;

            if (byte % 2 == 1) {
                serialGode++;
            }

			if (byte > 1) {
				visPause = true;
			}

			// Til debuggings
			Serial.print("Byte: ");
			Serial.print(byte);
            Serial.print(" - Total: ");
            Serial.print(serialTotal);
            Serial.print(" - Gode: ");
            Serial.println(serialGode);

    	}
      
    }

	// Oppgaver om skal utføres om poengsumen vises i denne syklusen.
	if (viserScore) {

		// Sjekker om alle leds som skal på er på, ellers skrur den på
		// neste led som skal på. Dette er hvordan den
		// gradvise/animerte visningen oppnås.
		if (antallLedsPaa < antallLedsTotal) {
			skruPaaScoreLed(antallLedsPaa);
		}

		// Sjekker om det har gått fire sekund siden knappen for å vise
		// poengsum ble trykt på sist og skjuler så poengsummen.
		if (millis() > visScoreSiste + (visScoreLengde_s*1000)) {

			Serial.println("Fjerner lys!");
			skruAvLeds();
			viserScore = false;

		}

	} else if (visPause) {

		digitalWrite(10, 255);
		digitalWrite(3, 255);

	} else {

		skruAvLeds();

	}
  
  	// Registrer knappetrykk for å vise poengsum. Om poengsum allerede
	// vises forlenges heller visningstiden.
    if (!digitalRead(knapp)) {

		if (!viserScore) {

			visScore();
			viserScore = true;
		
		}

		visScoreSiste = millis();

    }
  	
	// Venter
    delay(100);
  
}

// Funksjon for å beregne poengsum og å starte gradvis/animert visning
// av den
void visScore() {

	float score = (float) serialGode / (float) serialTotal * 4.7;

	Serial.print("Ratio: ");
	Serial.print((float) serialGode / (float) serialTotal);
	Serial.print(" - Score: ");
	Serial.println(score);

	antallLedsTotal = score + 1;
	antallLedsPaa = 0;
	skruAvLeds();
	skruPaaScoreLed(antallLedsPaa);

}

// Funksjon for å skru på en LED om gangen, brukes for å animere
// visningen av poengsum
void skruPaaScoreLed(int lednr) {

	if (lednr <= 2) {
		analogWrite(scoreleds[lednr][0], 255);
	}

	if (lednr >= 2) {
		analogWrite(scoreleds[lednr][1], 255);
	}

	antallLedsPaa++;

}

// Funksjon for å skru av alle LEDs når poengsummen ikke skal vises.
void skruAvLeds() {

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