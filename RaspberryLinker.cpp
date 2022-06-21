//
// Created by lilian on 10/06/2021.
//

#include "RaspberryLinker.h"

RaspberryLinker::RaspberryLinker(long unsigned int baudrate) {
    Serial.begin(baudrate);
}

int RaspberryLinker::available() {
    return Serial.available();
}

void RaspberryLinker::sendCommand(const String& command) {
    Serial.println(command);
    Serial.flush();
}

void RaspberryLinker::sendIdentity() {
    sendCommand("identity " NAME);
}

String RaspberryLinker::receiveCommand(bool blocking) {
    if (!blocking && !available())
        return "";
    return Serial.readStringUntil('\n');
}

int RaspberryLinker::parseCommand(String & command, char** & parameters) {
    String received = receiveCommand();
    if (received == "")
        return -1;
    if (received.startsWith("identify")) {
        sendIdentity();
        return parseCommand(command, parameters);
    }
    char* pch = strtok((char*) received.c_str(), " \n");
    command = String(pch);
    parameters = static_cast<char **>(calloc(sizeof(char *), 13));
    pch = strtok(nullptr, " \n");
    int parameter_count = 0;
    while (pch != nullptr) {
        if (parameter_count >= 13)
            parameters = static_cast<char **>(realloc(parameters, sizeof(char *) * (parameter_count + 1)));
        parameters[parameter_count] = static_cast<char *>(calloc(strlen(pch)+1, sizeof(char)));
        strcpy(parameters[parameter_count], pch);
        ++parameter_count;
        pch = strtok(nullptr, " \n");
    }
    return parameter_count;
}
