/*
 * Station.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: Matheus (X-warrior) Bratfisch (matheusbrat@gmail.com)
 */

#include "Station.h"


int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}


Station::Station() {
    GenericStation();
    flag = NOTHING;
    forward = 0;
    for (int i = 0; i < 5; i++) {
        idSelection[i] = 0;
        levelSelection[i] = 0;
    }
    control = 0;

    PRINTln("DISCOVER WHO IS LISTENING");
    bool askConfig = false;
    long sent_time;
    PMessage p[5] = { 0, 0, 0, 0, 0 };
    while (!askConfig) {
        sendWhoListen();
        sent_time = millis();
        while ((millis() - sent_time) < (256 * 100)) {
            update(p);
        }
        askConfig = sendAskConfig();
        update(p);
    }

    update(p);
    sent_time = millis();
    update(p);
    while (id == 0x00) {
        update(p);
        update(p);
        update(p);
        update(p);
        update(p);
    }
    PRINTln("Finish ASKING FOR CONFIGURATION");
    PRINTln("SETUP START REGULAR TASK");
    print();
    PRINT("FREE RAM");
    PRINTln(freeRam());
}

void Station::sendWhoListen() {
    PMessage c = PMessage(PMessage::PROTOCOL, PMessage::WHO_LISTEN, BROADCAST,
            (uint8_t) 0x00, (uint8_t) 0x00, (uint8_t) 0x00, (uint8_t) 0x00);
    writeProtocol(c);
}

bool Station::sendAskConfig() {
    uint8_t temp = 0;
    uint8_t tempId = 0;
    flag = WAITING;
    for (int x = 0; x < control; x++) {
        for (int y = 0; y < control - 1; y++) {
            if (levelSelection[y] > levelSelection[y + 1]) {
                temp = levelSelection[y + 1];
                tempId = idSelection[y + 1];
                levelSelection[y + 1] = levelSelection[y];
                idSelection[y + 1] = idSelection[y];
                levelSelection[y] = temp;
                idSelection[y] = tempId;
            }
        }
    }
    if (idSelection[0] != 0x00) {
        PRINT(idSelection[0], HEX);
        PRINT(" ");
        PRINT(idSelection[1], HEX);
        PRINT(" ");
        PRINT(idSelection[2], HEX);
        PRINT(" ");
        PRINT(idSelection[3], HEX);
        PRINT(" ");
        PRINT(idSelection[4], HEX);
        PRINT(" ");
        PMessage c = 0;
        // THIS IF IS JUST FOR TESTING PURPOSE!
        /*if (idSelection[1] != 0) {
         c = PMessage(PMessage::PROTOCOL, PMessage::ASK_CONFIG, idSelection[1],
         idSelection[0], idSelection[2], idSelection[3], idSelection[4]);
         } else {*/
        c = PMessage(PMessage::PROTOCOL, PMessage::ASK_CONFIG, idSelection[0],
                idSelection[1], idSelection[2], idSelection[3], idSelection[4]);
        if (idSelection[0] == 0x01) {
            c.proto = SET_MSG_DEST(c.proto, 1);
        }
        //}
        writeProtocol(c);
        return true;
    }
    return false;
}

void Station::receivedWhoListen(PMessage p) {
    if (id != 0) {
        unsigned long wait = millis();
        while ((millis() - wait) < (unsigned int) (id * 100)) {
            delay(5);
        }
        PMessage c = PMessage(PMessage::PROTOCOL, PMessage::I_LISTEN,
                (uint8_t) 0x00, (uint8_t) id, 0x00, (uint8_t) level,
                (uint8_t) 0x00);
        writeProtocol(c);
    }
}

void Station::receivedIListen(PMessage p) {
    if (p.id_dest == 0x00 && id == 0x00) {
        if (control > 4) {
            int bigger;
            uint8_t temp = 0;
            for (int i = 0; i < 4; i++) {
                if (levelSelection[i] > temp) {
                    temp = levelSelection[i];
                    bigger = i;
                }
            }
            if (p.value2 < levelSelection[bigger]) {
                levelSelection[bigger] = p.value2;
                idSelection[bigger] = p.id_from;
            }
        } else {
            idSelection[control] = p.id_from;
            levelSelection[control] = p.value2;
            control++;
            PRINT("CONTROL: ");
            PRINTln(control);
        }
    }
}

void Station::receivedAskConfig(PMessage p) {
    if (p.id_dest == id || indirectChild(p.id_dest)) {
        flag = RETRANSMIT;
        if (parentPipe == 0x01) {
            p.proto = SET_MSG_DEST(p.proto, 1);
        }
        if (p.id_dest == id) {
            flag = RETRANSMIT_FIRST;
        }
        PRINTln("retransmit ASK_CONFIG");
        //forward = p.id_dest;
        //p.id_dest = parentPipe;
        writeProtocol(p);
    }

}

void Station::receivedSetConfig(PMessage p) {
    bool condition = (findChildPipe(p.value) == 0xFF);
    if (flag == RETRANSMIT || flag == RETRANSMIT_FIRST) {
        PRINTln("retransmit SET_CONFIG");
        flag = NOTHING;
        //p.id_dest = forward;
        if (p.value3 == id && condition) {
            // I'm his parent, open new pipe;
            registerPipe(findOpenPipe(), p.value);
        }
        if (indirectChild(p.value3) && condition) {
            registerIndirecChild(p.value3, p.value);
        }
        if (flag == RETRANSMIT_FIRST) {
            p.proto = SET_MSG_DEST(p.proto, 1);
        }
        writeProtocol(p);
        print();
    } else if (flag == WAITING && GET_MSG_DEST(p.proto) == 1) {
        PRINTln("Received set_config");
        flag = NOTHING;
        parentPipe = p.value3; //c.id_from;
        id = p.value;
        level = p.value2;
        radio.openReadingPipe(1, ID_TO_PIPE(id));
    } else if (flag == NOTHING) {
        if (p.value3 == id && condition) {
            PRINTln("REGISTER CHILD");
            // I'm his parent, open new pipe;
            registerPipe(findOpenPipe(), p.value);
        }
        if (indirectChild(p.value3) && condition) {
            registerIndirecChild(p.value3, p.value);
        }
        print();
    }
}

Station::~Station() {
// TODO Auto-generated destructor stub
}
