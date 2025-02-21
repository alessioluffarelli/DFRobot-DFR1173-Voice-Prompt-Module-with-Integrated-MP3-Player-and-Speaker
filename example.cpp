#include <Arduino.h>
#include <MP3VoicePrompter.h>
#include <SoftwareSerial.h>

// PIN DFR1173
const byte pDFR1173_TX = A1;
const byte pDFR1173_RX = A2;
const byte pDFR1173_BUSY = A0;

// PIN Speaker
const byte pSpeaker = 11;

SoftwareSerial mySerialMP3(pDFR1173_RX, pDFR1173_TX); // RX, TX
MP3VoicePrompter mp3(mySerialMP3);

void setup() {

  // Emetto un BIP per dare feedback dell'accensione
  tone(pSpeaker, 300, 20);

  Serial.begin(115200);

  Serial.println("");
  Serial.println("- - - - - - - - - - -");
  Serial.println("INIZIO");
  mp3.begin();

  if (mp3.setVolume(1)) Serial.println("VOLUME 1 OK");
  else Serial.println("ERROR");

  delay(50);

}

void loop() {
  
  if (Serial.available()) {  // Se ci sono dati nella seriale
    String command = Serial.readStringUntil('\n');  // Legge fino al carattere di nuova linea
    command.trim();  // Rimuove eventuali spazi o caratteri di nuova linea

    if (command == "PLAY") {
      if (mp3.play()) Serial.println("PLAY OK");
      else Serial.println("ERROR");
    } 
    else if (command == "STOP") {
      if (mp3.stop()) Serial.println("STOP OK");
      else Serial.println("ERROR");
    }
    else if (command == "PAUSE") {
      if (mp3.pause()) Serial.println("PAUSE OK");
      else Serial.println("ERROR");
    }
    else if (command == "NEXT") {
      if (mp3.nextTrack()) Serial.println("NEXT OK");
      else Serial.println("ERROR");
    }
    else if (command == "PREVIOUS") {
      if (mp3.previousTrack()) Serial.println("PREVIOUS OK");
      else Serial.println("ERROR");
    }
    else if (command == "VOLUME 1") {
      if (mp3.setVolume(1)) Serial.println("VOLUME 1 OK");
      else Serial.println("ERROR");
    }
    else if (command == "VOLUME 5") {
      if (mp3.setVolume(5)) Serial.println("VOLUME 5 OK");
      else Serial.println("ERROR");
    }
    else if (command == "VOLUME 10") {
      if (mp3.setVolume(10)) Serial.println("VOLUME 10 OK");
      else Serial.println("ERROR");
    }
    else if (command == "VOLUME 20") {
      if (mp3.setVolume(20)) Serial.println("VOLUME 20 OK");
      else Serial.println("ERROR");
    }
    else if (command == "VOLUME 30") {
      if (mp3.setVolume(30)) Serial.println("VOLUME 30 OK");
      else Serial.println("ERROR");
    }
    else if (command == "VERSION") {
      uint16_t version = mp3.getFirmwareVersion();
      if (version > 0) {
        Serial.print("VERSION: ");
        Serial.println(version);
      }
      else Serial.println("ERROR");
    }
    else if (command == "STATUS") {
      int ret = mp3.getPlaybackStatus();
      if (ret == 1) Serial.println("PLAYING");
      else if (ret == 2) Serial.println("PAUSED");
      else if (ret == 3) Serial.println("STOPPED");
      else Serial.println("ERROR");
    }
    else if (command == "PROVA 1") {
      if (mp3.playSpecifFileInSpecificFolder(1, 3)) Serial.println("RIPRODUZIONE CARTELLA 01 FILE 003.mp3 OK");
      else Serial.println("ERROR");
    }
    else if (command == "PROVA 2") {
      if (mp3.playSpecifFileInSpecificFolder(1, 1)) Serial.println("RIPRODUZIONE CARTELLA 01 FILE 001.mp3 OK");
      else Serial.println("ERROR");
    }
    else if (command == "PROVA 3") {
      if (mp3.playSpecifFileInSpecificFolder(5, 2)) Serial.println("RIPRODUZIONE CARTELLA 05 FILE 002.mp3 OK");
      else Serial.println("ERROR");
    }
    else if (command == "PROVA 4") {
      if (mp3.playSpecifFileInSpecificFolder(1, 99)) Serial.println("RIPRODUZIONE CARTELLA 01 FILE 099.mp3 OK");
      else Serial.println("ERROR");
    }
    else if (command == "PROVA 5") {
      if (mp3.playSpecifFileInSpecificFolder(13, 58)) Serial.println("RIPRODUZIONE CARTELLA 13 FILE 058.mp3 OK");
      else Serial.println("ERROR");
    }
    else if (command == "PROVA 6") {
      if (mp3.playSpecifFileInSpecificFolder(2, 1)) Serial.println("RIPRODUZIONE CARTELLA 2 FILE 001.mp3 OK");
      else Serial.println("ERROR");
    }
    else if (command == "PROVA 7") {
      if (mp3.playSpecifFileInSpecificFolder(4, 2)) Serial.println("RIPRODUZIONE CARTELLA 4 FILE 002.mp3 OK");
      else Serial.println("ERROR");
    }
    else {
        Serial.println("Comando non riconosciuto");
    }
  }

}
