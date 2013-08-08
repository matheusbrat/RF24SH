/*
 * MasterStation-impl.h
 *
 *  Created on: Aug 01, 2013
 *      Author: Matheus (X-warrior) Bratfisch (matheusbrat@gmail.com)
 */

template <class MESSAGE_TYPE>
MasterStation<MESSAGE_TYPE>::MasterStation(uint8_t ce, uint8_t csn) : GenericStation<MESSAGE_TYPE>(ce, csn) {
    this->startup();
}

template <class MESSAGE_TYPE>
MasterStation<MESSAGE_TYPE>::MasterStation() : GenericStation<MESSAGE_TYPE>() {
	this->startup();
}

template <class MESSAGE_TYPE>
void MasterStation<MESSAGE_TYPE>::startup() {
    (void)static_cast<PMessage*>((MESSAGE_TYPE*)0);
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
    this->level = 0;
    this->id = 1;
    nextId = 2;
    lastAskConfig = 0;
    this->radio.openReadingPipe(1, ID_TO_PIPE(0x00));
}


template <class MESSAGE_TYPE>
uint8_t MasterStation<MESSAGE_TYPE>::freePipes(uint8_t id) {
    uint8_t temp = 0;
    for (int i = 0; i < 4; i++) {
        if (talkingTo[id][i] == 0)
            temp++;
    }
    return temp;
}

template <class MESSAGE_TYPE>
uint8_t MasterStation<MESSAGE_TYPE>::getLevel(uint8_t id) {
    return levels[id];
}

template <class MESSAGE_TYPE>
MasterStation<MESSAGE_TYPE>::~MasterStation() {
    // TODO Auto-generated destructor stub
}

template <class MESSAGE_TYPE>
uint8_t MasterStation<MESSAGE_TYPE>::getParent (MESSAGE_TYPE p) {
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
template <class MESSAGE_TYPE>
void MasterStation<MESSAGE_TYPE>::receivedWhoListen(MESSAGE_TYPE p) {
    unsigned long wait = millis();
    while ((millis() - wait) < (unsigned int) (this->id * 100)) {
        delay(5);
    }
    MESSAGE_TYPE c = MESSAGE_TYPE(MESSAGE_TYPE::PROTOCOL, MESSAGE_TYPE::I_LISTEN,
            (uint8_t) 0x00, (uint8_t) this->id, 0x00, (uint8_t) this->level,
            (uint8_t) 0x00);
    PRINTln("sending I_LISTEN");
    writeProtocol(c);
}
template <class MESSAGE_TYPE>
void MasterStation<MESSAGE_TYPE>::receivedIListen(MESSAGE_TYPE p) {
    // MASTER DOESN'T DO ANYTHING WHEN RECEIVE THIS!
}
template <class MESSAGE_TYPE>
void MasterStation<MESSAGE_TYPE>::testMessage() {
	MESSAGE_TYPE t = MESSAGE_TYPE(MESSAGE_TYPE::TUSER, MESSAGE_TYPE::CUSER, (nextId - 1), 0x01,
            233, 255, 100);
    uint8_t pipe = this->findChildPipe(nextId - 1);
    if (pipe != 0xFF) {
        writePipe(this->findChildPipe(nextId - 1), t);
    }
}
template <class MESSAGE_TYPE>
int MasterStation<MESSAGE_TYPE>::update(MESSAGE_TYPE p[5]) {
    uint8_t pipeNumber;
    uint8_t quantity = 0;
    for (int i = 0; i < 6; i++) {
    	MESSAGE_TYPE c;
        if (read(&pipeNumber, c)) {
            /* PIPE 0 = PROTOCOL
             * PIPE 1 = MASTER
             * PIPE 2..5 = CHILD
             * */
            if (pipeNumber == 0) {
                    processReadProtocol(c);
            } else if (pipeNumber == 1) {
//                p[0] = processRead(c);
//                if (!(p[0].id_dest == 0x00 && p[0].id_from == 0x00)) {
//                    quantity++;
//                }
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
template <class MESSAGE_TYPE>
bool MasterStation<MESSAGE_TYPE>::write(MESSAGE_TYPE msg) {
    uint8_t pipe = this->findChildPipe(msg.id_dest);
    if (pipe != 0xFF) {
       return writePipe(pipe, msg);
    }
    return false;
}


template <class MESSAGE_TYPE>
void MasterStation<MESSAGE_TYPE>::receivedAskConfig (MESSAGE_TYPE p) {
    PRINT("MY ID: ");
    PRINT(this->id);
    PRINT(" DEST ID ");
    PRINT(p.id_dest);
    PRINT(" MSG DEST ");
    PRINTln(p.proto, HEX);
    if (p.id_dest == this->id || GET_MSG_DEST(p.proto) == 1) {
        PRINT((millis() - lastAskConfig));
        if ((millis() - lastAskConfig) > 25000 || GET_MSG_EXTRA(p.proto) != 1) {
            PRINTln("ENTROU AQUI");
            canTalkTo[nextId][0] = p.id_dest;
            canTalkTo[nextId][1] = p.id_from;
            canTalkTo[nextId][2] = p.value;
            canTalkTo[nextId][3] = p.value2;
            canTalkTo[nextId][4] = p.value3;
            PRINTln("sending set_config");
            uint8_t parent = getParent(p);
        lastParent = parent;
        MESSAGE_TYPE r = MESSAGE_TYPE(MESSAGE_TYPE::PROTOCOL, MESSAGE_TYPE::SET_CONFIG,
                    (uint8_t) 0x00, (uint8_t) this->id, nextId,
                    (uint8_t) (getLevel(parent) + 1), parent);
            levels[nextId] = getLevel(parent) + 1;
            for (int i = 0; i < 4; i++) {
                if (talkingTo[parent][i] == 0x00) {
                    talkingTo[parent][i] = nextId;
                    break;
                }
            }

            if (parent == this->id) {
                registerPipe(this->findOpenPipe(), nextId);
            } else {
                this->registerIndirectChild(parent, nextId);
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
        	MESSAGE_TYPE r = MESSAGE_TYPE(MESSAGE_TYPE::PROTOCOL, MESSAGE_TYPE::SET_CONFIG,
                    (uint8_t) 0x00, (uint8_t) this->id, (uint8_t) (nextId-1),
                    (uint8_t) (getLevel(lastParent) + 1), lastParent);
            if (GET_MSG_DEST(p.proto) == 1) {
                r.proto = SET_MSG_DEST(r.proto, 1);
            }
            writeProtocol(r);
            delay(5);
        }
        this->print();
    }
}
template <class MESSAGE_TYPE>
void MasterStation<MESSAGE_TYPE>::receivedSetConfig(MESSAGE_TYPE p) {
    // MASTER DOESN'T DO ANYTHING WHEN RECEIVE THIS!
}
