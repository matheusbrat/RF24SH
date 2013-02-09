/*
 * MasterStation.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: Matheus (X-warrior) Bratfisch (matheusbrat@gmail.com)
 */

#include "MasterStation.h"

MasterStation::MasterStation() {
    GenericStation();
    for (int i = 0; i < 10; i++) {
        levels[i] = 0xFF;
        for (int x = 0; x < 5; x++) {
            canTalkTo[i][x] = 0;
            if (x != 4) {
                talkingTo[i][x] = 0;
            }
        }
    }
    levels[1] = 0;
    level = 0;
    id = 1;
    nextId = 2;
    lastAskConfig = 0;
    radio.openReadingPipe(1, ID_TO_PIPE(0x00));
}

uint8_t MasterStation::freePipes(uint8_t id) {
    uint8_t temp = 0;
    for (int i = 0; i < 4; i++) {
        if (talkingTo[id][i] == 0)
            temp++;
    }
    return temp;
}
uint8_t MasterStation::getLevel(uint8_t id) {
    return levels[id];
}

MasterStation::~MasterStation() {
    // TODO Auto-generated destructor stub
}

uint8_t MasterStation::getParent(PMessage p) {
    uint8_t parent = 0x00;
    uint16_t weight = 0xFFFF;
    for (int i = 0; i < 5; i++) {
        if (canTalkTo[nextId][i] != 0) {
            uint8_t nFreePipe = freePipes(canTalkTo[nextId][i]);
            if (nFreePipe > 0) {
                uint16_t temp = (4 - nFreePipe)
                        + getLevel(canTalkTo[nextId][i]);
                if (weight > temp) {
                    parent = canTalkTo[nextId][i];
                    weight = temp;
                }
            }
        }
    }
    return parent;
    // THIS IS FOR TESTING PURPOSE AS WELL!
    // TODO: ADD SUPPORT TO MAX NODES VERIFICATION
    //if (childPipes[0] != 0x00) {
    //  return childNodes[0][0];
    //}
    //return p.id_dest;
}

void MasterStation::receivedWhoListen(PMessage p) {
    unsigned long wait = millis();
    while ((millis() - wait) < (unsigned int) (id * 100)) {
        delay(5);
    }
    PMessage c = PMessage(PMessage::PROTOCOL, PMessage::I_LISTEN,
            (uint8_t) 0x00, (uint8_t) id, 0x00, (uint8_t) level,
            (uint8_t) 0x00);
    PRINTln("sending I_LISTEN");
    writeProtocol(c);
}

void MasterStation::receivedIListen(PMessage p) {
    // MASTER DOESN'T DO ANYTHING WHEN RECEIVE THIS!
}
void MasterStation::testMessage() {
    PMessage t = PMessage(PMessage::TUSER, PMessage::CUSER, (nextId - 1), 0x01,
            233, 255, 100);
    uint8_t pipe = findChildPipe(nextId - 1);
    if (pipe != 0xFF) {
        writePipe(findChildPipe(nextId - 1), t);
    }
}

void MasterStation::receivedAskConfig(PMessage p) {
    PRINT("MY ID: ");
    PRINT(id);
    PRINT(" DEST ID ");
    PRINT(p.id_dest);
    PRINT(" MSG DEST ");
    PRINTln(p.proto, HEX);
    if (p.id_dest == id || GET_MSG_DEST(p.proto) == 1) {
        if ((millis() - lastAskConfig) > 2000) {
            PRINTln("ENTROU AQUI");
            canTalkTo[nextId][0] = p.id_dest;
            canTalkTo[nextId][1] = p.id_from;
            canTalkTo[nextId][2] = p.value;
            canTalkTo[nextId][3] = p.value2;
            canTalkTo[nextId][4] = p.value3;
            PRINTln("sending set_config");
            uint8_t parent = getParent(p);
        lastParent = parent;
            PMessage r = PMessage(PMessage::PROTOCOL, PMessage::SET_CONFIG,
                    (uint8_t) 0x00, (uint8_t) id, nextId,
                    (uint8_t) (getLevel(parent) + 1), parent);
            levels[nextId] = getLevel(parent) + 1;
            for (int i = 0; i < 4; i++) {
                if (talkingTo[parent][i] == 0x00) {
                    talkingTo[parent][i] = nextId;
                    break;
                }
            }

            if (parent == id) {
                registerPipe(findOpenPipe(), nextId);
            } else {
                registerIndirecChild(parent, nextId);
            }

            if (GET_MSG_DEST(p.proto) == 1) {
                r.proto = SET_MSG_DEST(r.proto, 1);
            }
            //delay(5);
            writeProtocol(r);
            lastAskConfig = millis();
            delay(5);
            nextId++;
        } else {
            PMessage r = PMessage(PMessage::PROTOCOL, PMessage::SET_CONFIG,
                    (uint8_t) 0x00, (uint8_t) id, (uint8_t) (nextId-1),
                    (uint8_t) (getLevel(lastParent) + 1), lastParent);
            if (GET_MSG_DEST(p.proto) == 1) {
                r.proto = SET_MSG_DEST(r.proto, 1);
            }
            writeProtocol(r);
            delay(5);
        }
        print();
    }
}
void MasterStation::receivedSetConfig(PMessage p) {
    // MASTER DOESN'T DO ANYTHING WHEN RECEIVE THIS!
}

