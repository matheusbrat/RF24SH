/*
 * GenericStation.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: Matheus (X-warrior) Bratfisch (matheusbrat@gmail.com)
 */

#include "GenericStation.h"

void * station_irq_handler;

GenericStation::GenericStation() :
#if defined(ARDUINO) || defined(__MK20DX128__)
        radio(9, 10) 
#else 
        radio("/dev/spidev0.0",8000000 , 25) 
#endif
{
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
    station_irq_handler = this;
    // CREATE CONSTRUCTOR WITH AND WITHOUT INTERRUPT!
    //radio.set_irq_rx(readInterruptWrapper);
}
void GenericStation::readInterrupt() {
    Serial.println("INTERRUPT");
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
    delay(5);
    radio.stopListening();
    radio.openWritingPipe(pipe);
    short attempts = 5;
    bool ok = false;
    do {
        delay(10 - (attempts * 2) + 1);
        ok = radio.write(&p, sizeof(PMessage));
    } while (!ok && --attempts);
    PRINT("SENDING TO ");
    PRINT((uint32_t) pipe, HEX);
    if (ok)
        PRINTln(" ok...");
    else
        PRINTln(" failed... ");
    p.print();
    radio.startListening();
    return ok;
}

void GenericStation::print() {
    PRINT("ID ");
    PRINTln(id);
    PRINT("PARENT ");
    PRINTln(parentPipe);
    for (int i = 0; i < 4; i++) {
        PRINT("(Indirect?) Child of Pipe: ");
        PRINTln(i);
        for (int x = 0; x < childNodesSize[i]; x++) {
            PRINT(childNodes[i][x], HEX);
        }
        PRINTln("");
    }
}

int GenericStation::update(PMessage p[5]) {
    uint8_t pipeNumber;
    uint8_t quantity = 0;
    for (int i = 0; i < 6; i++) {
        PMessage c;
        if (read(&pipeNumber, c)) {
            /* PIPE 0 = PROTOCOL
             * PIPE 1 = MASTER
             * PIPE 2..5 = CHILD
             * */
            if (pipeNumber == 0) {
                    processReadProtocol(c);
            } else if (pipeNumber == 1) {
                p[0] = processRead(c);
                if (!(p[0].id_dest == 0x00 && p[0].id_from == 0x00)) {
                    quantity++;
                }
            } else if (pipeNumber < 6) {
                uint8_t arrayChildNumber = (pipeNumber - 1);
                p[arrayChildNumber] = processRead(c);
                if (!(p[arrayChildNumber].id_dest == 0x00 && p[arrayChildNumber].id_from == 0x00)) {
                    quantity++;
                }
            }
        }
    }
    return quantity;
}

/*
 PMessage GenericStation::readMaster() {
 if (parentPipe != 0x00) {
 PMessage c;
 if (read((uint8_t) 1, c)) {
 PRINTln("RECEIVED ON MASTER PIPE");
 return processRead(c);
 }
 }
 }
 */

/*bool GenericStation::read(uint8_t pipeNumber, PMessage & p) {
 bool done = false;
 uint8_t buffer[sizeof(PMessage)];
 if (radio.hasData(pipeNumber)) {
 if (radio.available(&pipeNumber)) {
 while (!done) {
 done = radio.read(&buffer, sizeof(PMessage));
 p = (PMessage) buffer;
 }
 PRINT("RECEIVING ON PIPE");
 PRINTln(pipeNumber);
 p.print();
 return true;
 }
 }
 return false;
 }*/

bool GenericStation::read(uint8_t * pipeNumber, PMessage & p) {
    bool done = false;
    uint8_t buffer[sizeof(PMessage)];
    if (radio.available(pipeNumber)) {
        while (!done) {
            done = radio.read(&buffer, sizeof(PMessage));
            p = (PMessage) buffer;
        }
        PRINT("RECEIVING ON PIPE");
        PRINTln(*pipeNumber);
        p.print();
        return true;
    }
    return false;
}

/*void GenericStation::readProtocol() {
 PMessage c;
 if (read((uint8_t) 0, c)) {
 PRINTln("READ PROTOCOL");
 processReadProtocol(c);
 }
 }*/
PMessage GenericStation::processRead(PMessage p) {
    if (!p.is_protocol()) {
        if (p.id_dest == id) {
            PRINTln("RECEIVED MESSAGE FOR ME");
            return p;
        } else if (indirectChild(p.id_dest)) {
            PRINTln("FORWARDING MESSAGE");
            writePipe(findChildPipe(p.id_dest), p);
            p = PMessage(PMessage::TUSER, PMessage::CUSER, (uint8_t) 0,
                    (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);
            return p;
        } else {
            writePipe(ID_TO_PIPE(parentPipe), p);
            p = PMessage(PMessage::TUSER, PMessage::CUSER, (uint8_t) 0,
                                (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);
            return p;
        }
    }
    p = PMessage(PMessage::TUSER, PMessage::CUSER, (uint8_t) 0, (uint8_t) 0,
            (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);
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
