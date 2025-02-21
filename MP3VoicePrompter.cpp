/*
    AUTHOR: Alessio Luffarelli
    WEB: www.alessioluffarelli.it

    ITALIANO

    |	Questa è una libreria per interfacciarsi con il dispositivo "Voice Prompt Module with Integrated MP3 Player and Speaker" (SKU: DFR1173)
    |	prodotto da DF Robot e acquistabile dal link https://www.dfrobot.com/product-2862.html.
    |	
    |	Ho scritto da zero questa libreria in quanto il produttore non fornisce alcuna libreria ufficiale per controllare il modulo. Inoltre la documentazione
    |	messa a disposizione dal produttore (un manuale in PDF reperibile al link https://github.com/DFRobotdl/DFROBOTDL/tree/main/DFR1173),
    |	oltre ad essere molto scarna, mal impaginata e mal scritta, contiene anche errori e imprecisioni.
    |	
    |	Infati alcune risposte del modulo inviate attraverso la seriale, non corrispondono a quelle riportate nel manuale, e alcune funzionalità non sono documentate. 
    |	Nel codice di questa libreria potete trovare alcuni esempi.
    |	
    |	Non escludo che il modulo in mio possesso possa avere un firmware troppo vecchio o troppo nuovo rispetto a quello utilizzato per scrivere il manuale.
    |	Purtroppo nel manuale non è riportata la versione del firmware a cui fa riferimento.
    |	
    |	Per il motivo appena descritto La libreria è stata testata con successo con il modulo in mio possesso, ma potrebbe non funzionare con altre versioni del modulo.
    |	
    |	Questa da me scritta non è una libreria completa, infatti non tutte le funzionalità del modulo sono state implementate, ma solo quelle che
    |	servivano a me per il mio progetto.


    ENGLISH (translated with ChatGPT)

    |	This is a library for interfacing with the "Voice Prompt Module with Integrated MP3 Player and Speaker" (SKU: DFR1173),
    |	manufactured by DFRobot and available for purchase at: https://www.dfrobot.com/product-2862.html.
    |	
    |	I wrote this library from scratch because the manufacturer does not provide any official library to control the module. Additionally,
    |	the documentation provided by the manufacturer (a PDF manual available at https://github.com/DFRobotdl/DFROBOTDL/tree/main/DFR1173)
    |	is not only very sparse, poorly formatted, and badly written, but also contains errors and inaccuracies.
    |	
    |	For instance, some responses sent by the module through the serial interface do not match those reported in the manual, and certain
    |	functionalities are undocumented. You can find some examples in this library's code.
    |	
    |	It is possible that the module I own has a firmware version that is either too old or too new compared to the one used to write
    |	the manual. Unfortunately, the manual does not specify the firmware version it refers to.
    |	
    |	For this reason, the library has been successfully tested with the module I own, but it may not work with other versions of the module.
    |	
    |	This is not a complete library; in fact, not all of the module's functionalities have been implemented — only those I needed for my project.

*/


#include "MP3VoicePrompter.h"

// Costruttore: inizializza il riferimento alla porta seriale
MP3VoicePrompter::MP3VoicePrompter(SoftwareSerial &serial) : mp3Serial(serial) {}

// Resetta il modulo e avvia la comunicazione con alla velocità specificata
void MP3VoicePrompter::begin() {
    sendCommand(CMD_RESET, DATA_ZERO, DATA_ZERO, true); // Prima di ogni operazione resetto il modulo
    mp3Serial.begin(9600); // Mentre aspetto che il modulo si inizializzi (impiega circa 700ms), inizializzo la SoftwareSerial
    stop(); // In alcuni, dopo il reset, il modulo inizia subito la riproduzione, per cui la fermo
}

// Invia un comando seriale al modulo ed eventualmente controlla se il modulo risponde con un ACK
bool MP3VoicePrompter::sendCommand(byte command, byte data1, byte data2, bool detectACK) {

    clearSerialBuffer(); // Svuoto il buffer seriale (TX del modulo, RX del MCU) prima di inviare un comando. Il buffer infatti potrebbe contenere dati non ancora letti (un caso tipico è quando termina la riproduzione di un brano e viene inviata la notifica)
    
    byte packet[] = {CMD_START, command, 0x00, 0x02, data1, data2, CMD_END};
    mp3Serial.write(packet, sizeof(packet));

    // Aspetto che il modulo processi il comando inviato (alcune operazioni, come la riproduzione specifica di un file all'interno di una cartella, impiegano più tempo)
    uint32_t startTime = millis();
    byte min_packet_size = 3; // ACK
    if (!detectACK) min_packet_size = 7; // Risposta a comandi come CMD_GET_VERSION o CMD_GET_STATE
    while ((millis() - startTime < TIME_OUT) && (mp3Serial.available() < min_packet_size)) ; // Aspetto, per un tempo massimo di TIME_OUT, che il modulo processi il comando rispondendomi con un numero di byte pari ad almeno min_packet_size

    if (detectACK) {
        if (mp3Serial.available() != 3) return false; // Mi aspetto esattamente 3 byte (ACK)

        // Leggo i byte di risposta
        byte startByte = mp3Serial.read();
        byte ackByte = mp3Serial.read();
        byte endByte = mp3Serial.read();

        // Verifico che il modulo risponda con un ACK (7E 41 EF)
        if (startByte != CMD_START) return false;
        if (ackByte != CMD_OK) return false;
        if (endByte != CMD_END) return false;
    }

    return true;
}


// Restituisce la versione del firmware del modulo
uint16_t MP3VoicePrompter::getFirmwareVersion() {

    if (!sendCommand(CMD_GET_VERSION, DATA_ZERO, DATA_ZERO, false)) return 0;

    if (mp3Serial.available() != 7) return 0; // Mi aspetto esattamente 7 byte

    // Con il modulo in mio possesso ottengo questo: 7E 46 02 00 05 10 EF

    // Leggo i byte di risposta
    byte startByte = mp3Serial.read();
    byte byte2 = mp3Serial.read();
    byte byte3 = mp3Serial.read();
    byte byte4 = mp3Serial.read();
    byte byte5 = mp3Serial.read();
    byte byte6 = mp3Serial.read();
    byte endByte = mp3Serial.read();

    if (startByte != CMD_START) return 0;
    if (byte2 != CMD_GET_VERSION) return 0;
    if (byte3 != 0x02) return 0;
    if (byte4 != 0x00) return 0;
    if (endByte != CMD_END) return 0;

    return ((uint16_t)byte5 << 8) | byte6;
}

// Ottiene lo stato della riproduzione (1: Riproduzione, 2: Pausa, 3: Stop, -1: Errore)
int8_t MP3VoicePrompter::getPlaybackStatus() {

    if (!sendCommand(CMD_GET_STATE, DATA_ZERO, DATA_ZERO, false)) return -1;

    if (mp3Serial.available() != 7) return -1; // Mi aspetto esattamente 7 byte

    // Leggo i byte di risposta
    byte startByte = mp3Serial.read();
    byte byte2 = mp3Serial.read();
    byte byte3 = mp3Serial.read();
    byte byte4 = mp3Serial.read();
    mp3Serial.read(); // byte5 is foo
    byte byte6 = mp3Serial.read();
    byte endByte = mp3Serial.read();

    /*
        PLAY:   7E 42 02 00 02 01 EF
        PAUSE:  7E 42 02 00 02 02 EF
        STOP:   7E 42 02 00 02 00 EF

        ATTENZIONE:
        il manuale riporta che i byte 3 e 4 dovrebbero essere 0x00 e 0x02, 
        ma con il dispositivo in mio possesso ho ottenuto 0x02 e 0x00
    */

    if (startByte != CMD_START) return -1;
    if (byte2 != CMD_GET_STATE) return -1;
    if (byte3 != 0x02) return -1;
    if (byte4 != 0x00) return -1;
    if (endByte != CMD_END) return -1;

    // (1: Riproduzione, 2: Pausa, 3: Stop, -1: Errore)
    if (byte6 == 0x01) return PLAYBACK_STATUS_PLAYING;
    else if (byte6 == 0x02) return PLAYBACK_STATUS_PAUSED;
    else if (byte6 == 0x00) return PLAYBACK_STATUS_STOPPED;
    else return -1;

}

// Riproduce un file specifico in una cartella specifica
// La cartella deve chiamarsi "XX-QUALSIASI_COSA", dove XX è un numero tra 01 e 99
// Il file deve chiamarsi "YYY-QUALSIASI_COSA.mp3", dove YYY è un numero tra 001 e 255
// L'intero nome della cartella (compreso "XX-") deve essere massimo di 12 caratteri
// L'intero nome del file (compreso "YY-" e ".mp3") deve essere massimo di 12 caratteri
bool MP3VoicePrompter::playSpecifFileInSpecificFolder(uint8_t folder, uint8_t file) {

    if (folder < 1 || folder > 99) return false;
    if (file < 1 || file > 255) return false;

    return sendCommand(CMD_PLAY_SPECIFIC_FILE_IN_SPECIFIC_FOLDER, folder, file, true);
}

// Imposta il volume (valore tra 0 e 30)
bool MP3VoicePrompter::setVolume(uint8_t volume) {
    if (volume > 30) volume = 30;
    return sendCommand(CMD_VOLUME_SET, DATA_ZERO, volume, true);
}

// Avvia la riproduzione
bool MP3VoicePrompter::play() {
    return sendCommand(CMD_START_PLAY, DATA_ZERO, DATA_ZERO, true);
}

// Mette in pausa la riproduzione
bool MP3VoicePrompter::pause() {
    return sendCommand(CMD_PAUSE, DATA_ZERO, DATA_ZERO, true);
}

// Ferma la riproduzione
bool MP3VoicePrompter::stop() {
    return sendCommand(CMD_STOP_PLAY, DATA_ZERO, DATA_ZERO, true);
}

// Passa alla traccia successiva
bool MP3VoicePrompter::nextTrack() {
    return sendCommand(CMD_NEXT, DATA_ZERO, DATA_ZERO, true);
}

// Torna alla traccia precedente
bool MP3VoicePrompter::previousTrack() {
    return sendCommand(CMD_PREVIOUS, DATA_ZERO, DATA_ZERO, true);
}

// Funzione interna per svuotare il buffer seriale del modulo (TX del modulo, RX del MCU)
void MP3VoicePrompter::clearSerialBuffer() {
    while (mp3Serial.available()) {
        mp3Serial.read();
        if (mp3Serial.available() == 0) delay(30); // Quando non ci sono più byte disponibili, aspetto ulteriori 30ms prima di uscire dalla while, per dare tempo nel caso ne arrivino altri
    }
}

// Metodo per DEBUG
void MP3VoicePrompter::PrintSerialRaw() {

    unsigned long timestamp[100];
    byte packet[100];
    int c = 0;

    // Leggo i byte disponibili sulla seriale
    while (mp3Serial.available() && c < 30) {

        timestamp[c] = millis();
        packet[c] = mp3Serial.read();

        c++;

        if (mp3Serial.available() == 0) delay(30); // Quando non ci sono più byte disponibili, aspetto ulteriori 30ms prima di uscire dalla while, per dare tempo nel caso ne arrivino altri
    }

    // Stampo i byte letti
    for (int i = 0; i < c; i++) {

        if (packet[i] == CMD_START) {
            Serial.print("MS ");
            Serial.print(timestamp[i]);
            Serial.print(": ");
        }

        if (packet[i] < 16) Serial.print("0"); // Aggiungo uno "0" per avere sempre due cifre esadecimali
        Serial.print(packet[i], HEX);
        Serial.print(" ");

        if (packet[i] == CMD_END) {
            Serial.println("");
        }

    }

    if (c == 0) {
        Serial.print("MS ");
        Serial.print(millis());
        Serial.println(": <EMPTY>");
    }
}