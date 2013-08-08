/*
 * Station-impl.h
 *
 *  Created on: Aug 01, 2013
 *      Author: Matheus (X-warrior) Bratfisch (matheusbrat@gmail.com)
 */

template <class MESSAGE_TYPE>
Station<MESSAGE_TYPE>::Station(uint8_t ce, uint8_t csn) : GenericStation<MESSAGE_TYPE>(ce, csn) {
	this->startup();
}
template <class MESSAGE_TYPE>
Station<MESSAGE_TYPE>::Station() : GenericStation<MESSAGE_TYPE>() {
	this->startup();
}

template <class MESSAGE_TYPE>
void Station<MESSAGE_TYPE>::startup() {
	(void)static_cast<PMessage*>((MESSAGE_TYPE*)0);
	flag = NOTHING;
	forward = 0;
	for (int i = 0; i < 5; i++) {
		idSelection[i] = 0;
		levelSelection[i] = 0;
	}
	control = 0;
}

template <class MESSAGE_TYPE>
void Station<MESSAGE_TYPE>::begin() {
    PRINTln("DISCOVER WHO IS LISTENING");
    bool askConfig = false;
    long sent_time;
    MESSAGE_TYPE p[5];
    while (!askConfig) {
        sendWhoListen();
        sent_time = millis();
        while ((millis() - sent_time) < (256 * 100)) {
            update(p);
        }
        askConfig = sendAskConfig(false);
        update(p);
    }

    update(p);
    update(p);
    sent_time = millis();
    update(p);
    update(p);
    while (this->id == 0x00) {
        sendAskConfig(true);
        for(int i = 0; i < 50; i++) {
            update(p);
            update(p);
            update(p);
            update(p);
            update(p);
        }
    }
    PRINTln("Finish ASKING FOR CONFIGURATION");
    PRINTln("SETUP START REGULAR TASK");
    this->print();
}

template <class MESSAGE_TYPE>
void Station<MESSAGE_TYPE>::sendWhoListen() {
	MESSAGE_TYPE c = MESSAGE_TYPE(MESSAGE_TYPE::PROTOCOL, MESSAGE_TYPE::WHO_LISTEN, BROADCAST,
            (uint8_t) 0x00, (uint8_t) 0x00, (uint8_t) 0x00, (uint8_t) 0x00);
    writeProtocol(c);
}
template <class MESSAGE_TYPE>
int Station<MESSAGE_TYPE>::update(MESSAGE_TYPE p[5]) {
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
template <class MESSAGE_TYPE>
bool Station<MESSAGE_TYPE>::sendAskConfig(bool resend) {
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
        MESSAGE_TYPE c;
        // THIS IF IS JUST FOR TESTING PURPOSE!
        /*if (idSelection[1] != 0) {
         c = PMessage(PMessage::PROTOCOL, PMessage::ASK_CONFIG, idSelection[1],
         idSelection[0], idSelection[2], idSelection[3], idSelection[4]);
         } else {*/
        c = MESSAGE_TYPE(MESSAGE_TYPE::PROTOCOL, MESSAGE_TYPE::ASK_CONFIG, idSelection[0],
                idSelection[1], idSelection[2], idSelection[3], idSelection[4]);
        if (idSelection[0] == 0x01) {
            c.proto = SET_MSG_DEST(c.proto, 1);
        }
        if(resend) {
            c.proto = SET_MSG_EXTRA(c.proto, 1);
        }
        //}
        writeProtocol(c);
        return true;
    }
    return false;
}
template <class MESSAGE_TYPE>
void Station<MESSAGE_TYPE>::receivedWhoListen(MESSAGE_TYPE p) {
    if (this->id != 0) {
        unsigned long wait = millis();
        while ((millis() - wait) < (unsigned int) (this->id * 100)) {
            delay(5);
        }
        MESSAGE_TYPE c = MESSAGE_TYPE(MESSAGE_TYPE::PROTOCOL, MESSAGE_TYPE::I_LISTEN,
                (uint8_t) 0x00, (uint8_t) this->id, 0x00, (uint8_t) this->level,
                (uint8_t) 0x00);
        writeProtocol(c);
    }
}

template <class MESSAGE_TYPE>
void Station<MESSAGE_TYPE>::receivedIListen(MESSAGE_TYPE p) {
    if (p.id_dest == 0x00 && this->id == 0x00) {
        if (control > 4) {
            int bigger = 0;
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
template <class MESSAGE_TYPE>
void Station<MESSAGE_TYPE>::receivedAskConfig(MESSAGE_TYPE p) {
    if (p.id_dest == this->id || indirectChild(p.id_dest)) {
        flag = RETRANSMIT;
        if (this->parentPipe == 0x01) {
            p.proto = SET_MSG_DEST(p.proto, 1);
        }
        if (p.id_dest == this->id) {
            flag = RETRANSMIT_FIRST;
        }
        PRINTln("retransmit ASK_CONFIG");
        //forward = p.id_dest;
        //p.id_dest = parentPipe;
        writeProtocol(p);
    }

}
template <class MESSAGE_TYPE>
void Station<MESSAGE_TYPE>::receivedSetConfig(MESSAGE_TYPE p) {
    bool condition = (findChildPipe(p.value) == 0xFF);
    if (flag == RETRANSMIT || flag == RETRANSMIT_FIRST) {
        PRINTln("retransmit SET_CONFIG");
        flag = NOTHING;
        //p.id_dest = forward;
        if (p.value3 == this->id && condition) {
            // I'm his parent, open new pipe;
            registerPipe(this->findOpenPipe(), p.value);
        }
        if (indirectChild(p.value3) && condition) {
        	this->registerIndirectChild(p.value3, p.value);
        }
        if (flag == RETRANSMIT_FIRST) {
            p.proto = SET_MSG_DEST(p.proto, 1);
        }
        writeProtocol(p);
        this->print();
    } else if (flag == WAITING && GET_MSG_DEST(p.proto) == 1) {
        PRINTln("Received set_config");
        flag = NOTHING;
        this->parentPipe = p.value3; //c.id_from;
        this->id = p.value;
        this->level = p.value2;
        this->radio.openReadingPipe(1, ID_TO_PIPE(this->id));
    } else if (flag == NOTHING) {
        if (p.value3 == this->id && condition) {
            PRINTln("REGISTER CHILD");
            // I'm his parent, open new pipe;
            registerPipe(this->findOpenPipe(), p.value);
        }
        if (indirectChild(p.value3) && condition) {
            this->registerIndirectChild(p.value3, p.value);
        }
        this->print();
    }
}
template <class MESSAGE_TYPE>
Station<MESSAGE_TYPE>::~Station() {
// TODO Auto-generated destructor stub
}

template <class MESSAGE_TYPE>
bool Station<MESSAGE_TYPE>::write(MESSAGE_TYPE msg) {
    uint8_t pipe = this->findChildPipe(msg.id_dest);
    PRINT("PIPE---->");
    PRINTln(pipe, HEX);
    if (pipe != 0xFF) {
        return writePipe(ID_TO_PIPE(pipe), msg);
    } else {
    	PRINT("PARENT---->");
    	PRINTln(this->parentPipe);
    	msg.print();
    	return writePipe(ID_TO_PIPE(this->id),msg);
    }
    return false;
}
