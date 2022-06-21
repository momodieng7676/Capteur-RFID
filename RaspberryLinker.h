//
// Created by lilian on 10/06/2021.
//

#ifndef RASPBERRYLINKER_H

#define RASPBERRYLINKER_H

#define NAME "Rfid"

#include <Arduino.h>

class RaspberryLinker {
public:
    RaspberryLinker() : RaspberryLinker(9600) {};
    RaspberryLinker(long unsigned int baudrate);

    int available();
    void sendCommand(const String& command);
    void sendIdentity();

    String receiveCommand(bool blocking=false);
    int parseCommand(String & command, char** & parameters);

    ~RaspberryLinker() {
        Serial.end();
    }
};

#endif //RASPBERRYLINKER_H
