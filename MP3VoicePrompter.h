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
    |	This is not a complete library; in fact, not all of the module's functionalities have been implemented—only those I needed for my project.

*/


#ifndef MP3VoicePrompter_h
#define MP3VoicePrompter_h

#include <Arduino.h>
#include <SoftwareSerial.h>

// Definizione dei byte di controllo
#define CMD_START           0x7E    // Inizio del pacchetto di comando
#define CMD_END             0xEF    // Fine del pacchetto di comando
#define CMD_OK              0x41    // Comando ricevuto con successo
#define DATA_ZERO           0x00    // Valore 0

// Definizione dei comandi per il controllo del modulo MP3
#define CMD_NEXT            0x01   // Passa alla traccia successiva
#define CMD_PREVIOUS        0x02   // Torna alla traccia precedente
#define CMD_VOLUME_ADD      0x04   // Aumenta il volume
#define CMD_VOLUME_SUB      0x05   // Diminuisce il volume
#define CMD_VOLUME_SET      0x06   // Imposta il livello del volume
#define CMD_RESET           0x0C   // Resetta il modulo MP3
#define CMD_START_PLAY      0x0D   // Avvia la riproduzione
#define CMD_PAUSE           0x0E   // Mette in pausa la riproduzione
#define CMD_STOP_PLAY       0x16   // Ferma la riproduzione
#define CMD_GET_STATE       0x42   // Ottiene lo stato della riproduzione
#define CMD_GET_VERSION     0x46   // Ottiene la versione del firmware
#define CMD_PLAY_SPECIFIC_FILE_IN_SPECIFIC_FOLDER  0x0F   // Riproduce uno specifico file in una specifica cartella

// Definizione dei possibili stati di riproduzione
#define PLAYBACK_STATUS_PLAYING     1
#define PLAYBACK_STATUS_PAUSED      2
#define PLAYBACK_STATUS_STOPPED     3

// Definizione del timeout (in ms) per il processamento dei comandi da parte del modulo
#define TIME_OUT            1000

// Classe per il controllo del modulo MP3 Voice Prompter
class MP3VoicePrompter {

public:

    // Costruttore che riceve un'istanza di SoftwareSerial
    MP3VoicePrompter(SoftwareSerial &serial);
    
    // Inizializza la comunicazione seriale con il modulo
    void begin();
    
    // Imposta il volume (0-30)
    bool setVolume(uint8_t volume);
    
    // Avvia la riproduzione
    bool play();
    
    // Mette in pausa la riproduzione
    bool pause();
    
    // Ferma la riproduzione
    bool stop();
    
    // Passa alla traccia successiva
    bool nextTrack();
    
    // Torna alla traccia precedente
    bool previousTrack();

    // Ottiene la versione del firmware
    uint16_t getFirmwareVersion();

    // Ottiene lo stato della riproduzione (1: Riproduzione, 2: Pausa, 3: Stop, -1: Errore)
    int8_t getPlaybackStatus();

    // Riproduce un file specifico in una cartella specifica
    bool playSpecifFileInSpecificFolder(uint8_t folder, uint8_t file);

private:
    
    // Riferimento all'oggetto SoftwareSerial
    SoftwareSerial &mp3Serial;
    
    // Funzione interna per inviare comandi seriali al modulo
    bool sendCommand(byte command, byte data1, byte data2, bool detectACK);

    // Funzione interna per svuotare il buffer seriale del modulo (TX del modulo, RX del MCU)
    void clearSerialBuffer();

    // Metodo per DEBUG
    void PrintSerialRaw();
    
};

#endif