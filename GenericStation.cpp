/*
 * GenericStation.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: x-warrior
 */

#include "GenericStation.h"

GenericStation::GenericStation() :
        radio(9, 10) {
    // FIX NUMBER FOUR MAGIC NUMBER
    for (int i = 0; i < 4; i++) {
        childPipes[i] = 0;
        childNodes[i] = 0;
        childNodesSize[i] = 0;
    }
    parentPipe = 0;
    id = 0;
    level = 0;

    radio.begin();
    radio.setRetries(15, 15);
    radio.setPayloadSize(sizeof(PMessage));
    radio.openReadingPipe(0, PROTO_PIPE);
    radio.startListening();
    radio.printDetails();
}

uint8_t GenericStation::findOpenPipe() {
    //TODO: CHECK!
    for (uint8_t i = 0; i < 4; i++) {
        if (childPipes[i] == 0x00) {
            return i;
        }
    }
    return 0xFF;
}

void GenericStation::registerIndirecChild(uint8_t parent, uint8_t id) {
    uint8_t pipe = findChildPipe(parent);
    uint8_t * list = (uint8_t*) realloc(childNodes[pipe],
            (sizeof(uint8_t) * (childNodesSize[pipe] + 1)));
    list[childNodesSize[pipe]] = id;
    childNodes[pipe] = list;
    childNodesSize[pipe] = childNodesSize[pipe] + 1;
}

bool GenericStation::writeProtocol(PMessage p) {
    return writePipe((uint64_t) PROTO_PIPE, p);
}

bool GenericStation::writePipe(uint8_t pipeNumber, PMessage p) {
    if (childPipes[pipeNumber] != 0) {
        return writePipe(ID_TO_PIPE(childPipes[pipeNumber]), p);
    }
    return false;
}

bool GenericStation::writePipe(uint64_t pipe, PMessage p) {
    delay(10);
    radio.stopListening();
    radio.openWritingPipe(pipe);
    short attempts = 5;
    boolean ok = false;
    do {
        ok = radio.write(&p, sizeof(PMessage));
    } while (!ok && --attempts);
    Serial.print("SENDING TO ");
    Serial.print((uint32_t) pipe);
    if(ok)
        Serial.println(" ok...");
    else
        Serial.println(" failed... ");
    p.print();
    radio.startListening();
    return ok;
}

void GenericStation::print() {
    Serial.print("ID ");
    Serial.println(id);
    Serial.print("PARENT ");
    Serial.println(parentPipe);
    for (int i = 0; i < 4; i++) {
        Serial.print("(Indirect?) Child of Pipe: ");
        Serial.println(i);
        for (int x = 0; x < childNodesSize[i]; x++) {
            Serial.print(childNodes[i][x], HEX);
        }
        Serial.println("");
    }
}

int GenericStation::update(PMessage p[5]) {
    readProtocol();
    p[0] = readMaster();
    /* PIPE 0 = PROTOCOL
     * PIPE 1 = MASTER
     * PIPE 2..5 = CHILD
     * */
    int t = 0;
    for (int i = 0; i < 4; i++) {
        if (childPipes[i] != 0x00) {
            PMessage c = 0;
            if(read(i + 2, c)) {
                t++;
                p[i + 1] = processRead(c); // CORRECT VALUES TO PIPE NUMBER
            }
            readProtocol();
        }
    }
    readProtocol();
    return t;
}


PMessage GenericStation::readMaster() {
    if(parentPipe != 0x00) {
        PMessage c = 0;
        if (read((uint8_t) 1, c)) {
            Serial.print("RECEIVED ON MASTER PIPE");
            return processRead(c);
        }
    }
}

bool GenericStation::read(uint8_t pipeNumber, PMessage & p) {
    boolean done = false;
    uint8_t buffer[sizeof(PMessage)];
    if (radio.available(&pipeNumber)) {
        while (!done) {
            done = radio.read(&buffer, sizeof(PMessage));
            p = (PMessage) buffer;
        }
        Serial.println("RECEIVING");
        p.print();
        return true;
    }
    return false;
}

void GenericStation::readProtocol() {
    PMessage c = 0;
    if (read((uint8_t) 0, c)) {
        processReadProtocol(c);
    }
}
PMessage GenericStation::processRead(PMessage p) {
    if (!p.is_protocol()) {
        if (p.id_dest == id) {
            Serial.println("RECEIVED MESSAGE FOR ME");
            return p;
        } else if (indirectChild(p.id_dest)) {
            Serial.println("FORWARDING MESSAGE");
            writePipe(findChildPipe(p.id_dest), p);
            p = 0;
            return p;
        }
    }
    p = 0;
    return p;
}

void GenericStation::processReadProtocol(PMessage p) {
    if (p.is_protocol()) {
        switch (p.get_type()) {
        case PMessage::WHO_LISTEN:
            receivedWhoListen(p);
            break;
        case PMessage::I_LISTEN:
            receivedIListen(p);
            break;
        case PMessage::ASK_CONFIG:
            receivedAskConfig(p);
            break;
        case PMessage::SET_CONFIG:
            receivedSetConfig(p);
            break;
        }
    }
}

bool GenericStation::indirectChild(uint8_t id) {
    for (int i = 0; i < 4; i++) {
        for (int x = 0; x < childNodesSize[i]; x++) {
            uint8_t v = childNodes[i][x];
            if (id == v) {
                return true;
            }
        }
    }
    return false;
}

void GenericStation::registerPipe(uint8_t pipeNumber, uint8_t id) {
    childPipes[pipeNumber] = id;
    /*if (childNodesSize[pipeNumber] == 0) {
     uint8_t * list = (uint8_t*) malloc(
     (sizeof(uint8_t) * (childNodesSize[pipeNumber] + 1)));
     list[childNodesSize[pipeNumber]] = id;
     childNodes[pipeNumber] = list;
     childNodesSize[pipeNumber] = childNodesSize[pipeNumber] + 1;
     } else {*/
    uint8_t * list = (uint8_t*) realloc(childNodes[pipeNumber],
            (sizeof(uint8_t) * (childNodesSize[pipeNumber] + 1)));
    list[childNodesSize[pipeNumber]] = id;
    childNodes[pipeNumber] = list;
    childNodesSize[pipeNumber] = childNodesSize[pipeNumber] + 1;
    radio.openReadingPipe((pipeNumber + 2), ID_TO_PIPE(id));

//	}
}

uint8_t GenericStation::findChildPipe(uint8_t id) {
    for (int i = 0; i < 4; i++) {
        for (int x = 0; x < childNodesSize[i]; x++) {
            uint8_t v = childNodes[i][x];
            if (id == v) {
                return i;
            }
        }
    }
    return 0xFF;
}

void GenericStation::receivedWhoListen(PMessage p) {
    // Class that extend this must implement!
}

void GenericStation::receivedIListen(PMessage p) {
    // Class that extend this must implement!
}

void GenericStation::receivedAskConfig(PMessage p) {
    // Class that extend this must implement!
}

void GenericStation::receivedSetConfig(PMessage p) {
    // Class that extend this must implement!
}

GenericStation::~GenericStation() {
}
