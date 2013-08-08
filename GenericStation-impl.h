
/*
 * GenericStation-impl.h
 *
 *  Created on: Aug 01, 2013
 *      Author: Matheus (X-warrior) Bratfisch (matheusbrat@gmail.com)
 */

template <class MESSAGE_TYPE>
GenericStation<MESSAGE_TYPE>::GenericStation(uint8_t ce, uint8_t csn) :
#if defined(ARDUINO) || defined(__MK20DX128__)
        radio(ce, csn)
#else
#warning Are you sure, you are calling the correct constructor?
#endif
{
    this->startup();
}

template <class MESSAGE_TYPE>
GenericStation<MESSAGE_TYPE>::GenericStation() :
#if defined(ARDUINO) || defined(__MK20DX128__)
        radio(9, 10)
#else
        radio("/dev/spidev0.0",8000000 , 25)
#endif
{
    this->startup();
}

template <class MESSAGE_TYPE>
void GenericStation<MESSAGE_TYPE>::startup() {
	(void)static_cast<PMessage*>((MESSAGE_TYPE*)0);
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
	radio.setPayloadSize(sizeof(MESSAGE_TYPE));
	radio.openReadingPipe(0, PROTO_PIPE);
	radio.startListening();
}


template <class MESSAGE_TYPE>
uint8_t GenericStation<MESSAGE_TYPE>::findOpenPipe() {
    //TODO: CHECK!
    for (uint8_t i = 0; i < 4; i++) {
        if (childPipes[i] == 0x00) {
            return i;
        }
    }
    return 0xFF;
}

template <class MESSAGE_TYPE>
void GenericStation<MESSAGE_TYPE>::registerIndirectChild(uint8_t parent, uint8_t id) {
    uint8_t pipe = findChildPipe(parent);
    uint8_t * list = (uint8_t*) realloc(childNodes[pipe],
            (sizeof(uint8_t) * (childNodesSize[pipe] + 1)));
    list[childNodesSize[pipe]] = id;
    childNodes[pipe] = list;
    childNodesSize[pipe] = childNodesSize[pipe] + 1;
}
template <class MESSAGE_TYPE>
bool GenericStation<MESSAGE_TYPE>::writeProtocol(MESSAGE_TYPE p) {
    return writePipe((uint64_t) PROTO_PIPE, p);
}
template <class MESSAGE_TYPE>
bool GenericStation<MESSAGE_TYPE>::writePipe(uint8_t pipeNumber, MESSAGE_TYPE p) {
    if (childPipes[pipeNumber] != 0) {
        return writePipe(ID_TO_PIPE(childPipes[pipeNumber]), p);
    }
    return false;
}
template <class MESSAGE_TYPE>
bool GenericStation<MESSAGE_TYPE>::write(MESSAGE_TYPE p) {
	// Class that extend this must implement!
	return false;
}
template <class MESSAGE_TYPE>
bool GenericStation<MESSAGE_TYPE>::writePipe(uint64_t pipe, MESSAGE_TYPE p) {
    delay(5);
    radio.stopListening();
    radio.openWritingPipe(pipe);
    short attempts = 5;
    bool ok = false;
    do {
        delay(10 - (attempts * 2) + 1);
        ok = radio.write(&p, sizeof(MESSAGE_TYPE));
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

template <class MESSAGE_TYPE>
void GenericStation<MESSAGE_TYPE>::print() {
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
template <class MESSAGE_TYPE>
int GenericStation<MESSAGE_TYPE>::update(MESSAGE_TYPE p[5]) {
    return 0;
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
template <class MESSAGE_TYPE>
bool GenericStation<MESSAGE_TYPE>::read(uint8_t * pipeNumber, MESSAGE_TYPE & p) {
    bool done = false;
    uint8_t buffer[sizeof(MESSAGE_TYPE)];
    if (radio.available(pipeNumber)) {
        while (!done) {
            done = radio.read(&buffer, sizeof(MESSAGE_TYPE));
            p = (MESSAGE_TYPE) buffer;
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
template <class MESSAGE_TYPE>
MESSAGE_TYPE GenericStation<MESSAGE_TYPE>::processRead(MESSAGE_TYPE p) {
    if (!p.is_protocol()) {
        if (p.id_dest == id) {
            PRINTln("RECEIVED MESSAGE FOR ME");
            return p;
        } else if (indirectChild(p.id_dest)) {
            PRINTln("FORWARDING MESSAGE");
            writePipe(findChildPipe(p.id_dest), p);
            p = MESSAGE_TYPE(MESSAGE_TYPE::TUSER, MESSAGE_TYPE::CUSER, (uint8_t) 0,
                    (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);
            return p;
        } else {
            writePipe(ID_TO_PIPE(this->id), p);
            p = MESSAGE_TYPE(MESSAGE_TYPE::TUSER, MESSAGE_TYPE::CUSER, (uint8_t) 0,
                                (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);
            return p;
        }
    }
    p = MESSAGE_TYPE(MESSAGE_TYPE::TUSER, MESSAGE_TYPE::CUSER, (uint8_t) 0, (uint8_t) 0,
            (uint8_t) 0, (uint8_t) 0, (uint8_t) 0);
    return p;
}
template <class MESSAGE_TYPE>
void GenericStation<MESSAGE_TYPE>::processReadProtocol(MESSAGE_TYPE p) {
    if (p.is_protocol()) {
        switch (p.get_type()) {
        case MESSAGE_TYPE::WHO_LISTEN:
            receivedWhoListen(p);
            break;
        case MESSAGE_TYPE::I_LISTEN:
            receivedIListen(p);
            break;
        case MESSAGE_TYPE::ASK_CONFIG:
            receivedAskConfig(p);
            break;
        case MESSAGE_TYPE::SET_CONFIG:
            receivedSetConfig(p);
            break;
        }
    }
}

template <class MESSAGE_TYPE>
bool GenericStation<MESSAGE_TYPE>::indirectChild(uint8_t id) {
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

template <class MESSAGE_TYPE>
void GenericStation<MESSAGE_TYPE>::registerPipe(uint8_t pipeNumber, uint8_t id) {
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

template <class MESSAGE_TYPE>
uint8_t GenericStation<MESSAGE_TYPE>::findChildPipe(uint8_t id) {
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

template <class MESSAGE_TYPE>
void GenericStation<MESSAGE_TYPE>::receivedWhoListen(MESSAGE_TYPE p) {
// Class that extend this must implement!
}

template <class MESSAGE_TYPE>
void GenericStation<MESSAGE_TYPE>::receivedIListen(MESSAGE_TYPE p) {
// Class that extend this must implement!
}
template <class MESSAGE_TYPE>
void GenericStation<MESSAGE_TYPE>::receivedAskConfig(MESSAGE_TYPE p) {
// Class that extend this must implement!
}
template <class MESSAGE_TYPE>
void GenericStation<MESSAGE_TYPE>::receivedSetConfig(MESSAGE_TYPE p) {
// Class that extend this must implement!
}
template <class MESSAGE_TYPE>
GenericStation<MESSAGE_TYPE>::~GenericStation() {
}

template <class MESSAGE_TYPE>
MESSAGE_TYPE GenericStation<MESSAGE_TYPE>::pickNewMessage(MESSAGE_TYPE m[5]) {
	for (int i = 0; i < 5; i++) {
		if(m[i].id_dest == this->id && m[i].id_from != 0x00) {
			MESSAGE_TYPE r = m[i];
			m[i] = MESSAGE_TYPE(PMessage::TUSER,PMessage::CUSER,0,0,0,0,0);
			return r;
		}
	}
	return  MESSAGE_TYPE(PMessage::TUSER,PMessage::CUSER,0,0,0,0,0);
}


