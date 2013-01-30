/*
 * Protocol.cpp
 *
 *  Created on: Jan 28, 2013
 *      Author: x-warrior
 */

#include "Protocol.h"
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

//#define MASTER

int freeRam() {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

Protocol::Protocol() :
	radio(9, 10) {
	childPipes[0] = 0x00;
	childPipes[1] = 0x00;
	childPipes[2] = 0x00;
	childPipes[3] = 0x00;
	parentPipe = 0x00;
	idSelection[0] = 0x00;
	idSelection[1] = 0x00;
	idSelection[2] = 0x00;
	idSelection[3] = 0x00;
	idSelection[4] = 0x00;
	levelSelection[0] = 0x00;
	levelSelection[1] = 0x00;
	levelSelection[2] = 0x00;
	levelSelection[3] = 0x00;
	levelSelection[4] = 0x00;
	childNodes[0] = 0;
	childNodes[1] = 0;
	childNodes[2] = 0;
	childNodes[3] = 0;
	childNodesSize[0] = 0;
	childNodesSize[1] = 0;
	childNodesSize[2] = 0;
	childNodesSize[3] = 0;
/*	levelSelection[0] = 0xFF;
	levelSelection[1] = 0xFF;
	levelSelection[2] = 0xFF;
	levelSelection[3] = 0xFF;
	levelSelection[4] = 0xFF;*/


	radio.begin();
	radio.setRetries(15, 15);
	radio.setPayloadSize(sizeof(PMessage));
	//radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(0, PROTO_PIPE);
	radio.startListening();
	radio.printDetails();
	flag = NOTHING;
#ifndef MASTER
	id = 0;
	control = 0;
	Serial.println("DISCOVER WHO IS LISTENING");
	who_is_listening();
	long sent_time = millis();
	while ((millis() - sent_time) < (256 * 100)) {
		read();
	}
	Serial.println("Finish discovering who is listening");
	Serial.println("ASKING FOR CONFIGURATION");
	ask_config();
	sent_time = millis();
	while ((millis() - sent_time) < (256 * 100)) {
		read();
	}
	Serial.println("Finish ASKING FOR CONFIGURATION");
	Serial.println("SETUP START REGULAR TASK");
	print();
#else
	level = 0;
	id = 1;
	nextId = 2;
#endif


}

void Protocol::ask_config() {
	uint8_t temp = 0;
	uint8_t tempId = 0;
	flag = WAITING;
	for(int x = 0; x < control; x++) {
    		for(int y = 0; y < control-1; y++) {
		      if(levelSelection[y] > levelSelection[y+1]) {
			      temp = levelSelection[y+1];
				tempId = idSelection[y+1];
			      levelSelection[y+1] = levelSelection[y];
				idSelection[y+1] = idSelection[y];
			      levelSelection[y] = temp;
				idSelection[y] = tempId;
		      }
		}
	}
	Serial.print(idSelection[0], HEX);
	Serial.print(" ");
	Serial.print(idSelection[1], HEX);
	Serial.print(" ");
	Serial.print(idSelection[2], HEX);
	Serial.print(" ");
	Serial.print(idSelection[3], HEX);
	Serial.print(" ");
	Serial.print(idSelection[4], HEX);
	Serial.print(" ");
	PMessage c = 0;
	if(idSelection[1] != 0) {
		c = PMessage(PMessage::PROTOCOL, PMessage::ASK_CONFIG, idSelection[1], idSelection[0], idSelection[2], idSelection[3], idSelection[4]);	
	} else {
		c = PMessage(PMessage::PROTOCOL, PMessage::ASK_CONFIG, idSelection[0], idSelection[1], idSelection[2], idSelection[3], idSelection[4]);
	}
	write(c);
}

bool Protocol::write(PMessage p) {
	delay(5);
	radio.stopListening();
	radio.openWritingPipe(PROTO_PIPE);
	short attempts = 5;
	boolean ok = false;
	do {
		ok = radio.write(&p, sizeof(PMessage));
	} while (!ok && --attempts);
	Serial.println("SENDING");
	p.print();
	radio.startListening();
	delay(5);
	return ok;
}

void Protocol::who_is_listening() {
	PMessage c = PMessage(PMessage::PROTOCOL, PMessage::WHO_LISTEN, BROADCAST, (uint8_t) 0x00, (uint8_t) 0x00, (uint8_t) 0x00,
			(uint8_t) 0x00);
	write(c);
}

void Protocol::i_am_listening() {
	long wait = millis();
	while((millis() - wait) < (id * 100)) {
		Serial.println("SLEEPING for a while");
		delay(10);
	}
	PMessage c = PMessage(PMessage::PROTOCOL, PMessage::I_LISTEN, (uint8_t) 0x00, (uint8_t) id, 0x00, (uint8_t) level,
			(uint8_t) 0x00);
	write(c);
}

uint8_t Protocol::getParent(PMessage p) { 
	if(childPipes[0] != 0x00) {
		return childNodes[0][0];
	} 
	return p.id_dest;
}

bool Protocol::indirectChild(uint8_t id) {
	for(int i = 0; i < 4; i++) {
		for(int x = 0; x < childNodesSize[i]; x++) {
			uint8_t v = childNodes[i][x];
			if(id == v) {
				return true;
			}
		}
	}
	return false;
}
uint8_t Protocol::findOpenPipe() {
	//TODO: CHECK!
	for (uint8_t i = 0; i < 4; i++) {
		if(childPipes[i] == 0x00) {
			return i;
		}
	}
	return 0xFF;
}
void Protocol::registerPipe(uint8_t pipeNumber, uint8_t id) {
	//TODO: CHECK!
	childPipes[pipeNumber] = id;
	childIds[pipeNumber]   = id;
	if(childNodesSize[pipeNumber] == 0) {
		uint8_t * list = (uint8_t*) malloc(sizeof(uint8_t) * (childNodesSize[pipeNumber] + 1));
		list[childNodesSize[pipeNumber]] = id;
		childNodes[pipeNumber] = list;
		childNodesSize[pipeNumber] = childNodesSize[pipeNumber] + 1;
	} else {
		uint8_t * list = (uint8_t*) realloc(childNodes[pipeNumber], sizeof(uint8_t) * (childNodesSize[pipeNumber] + 1));
		list[childNodesSize[pipeNumber]] = id;	
		childNodes[pipeNumber] = list;
		childNodesSize[pipeNumber] = childNodesSize[pipeNumber] + 1;
	}
}

uint8_t Protocol::findChildPipe(uint8_t id) {
	for(int i = 0; i < 4; i++) {
		for(int x = 0; x < childNodesSize[i]; x++) {
			uint8_t v = childNodes[i][x];
			if(id == v) {
				return i;
			}
		}
	}
	return 0xFF;	
}

void Protocol::registerIndirecChild(uint8_t parent, uint8_t id) {
	uint8_t pipe = findChildPipe(parent);
	uint8_t * list = (uint8_t*) realloc(childNodes[pipe], sizeof(uint8_t) * (childNodesSize[pipe] + 1));
	//uint8_t list[childNodesSize[pipe]];
	//uint8_t * tmp = childNodes[pipe];
	//memcpy(list, childNodes[pipe], (childNodesSize[pipe] - 1));
	//delete(tmp);
	list[childNodesSize[pipe]] = id;
	childNodes[pipe] = list;
	childNodesSize[pipe] = childNodesSize[pipe] + 1;
}

void Protocol::read() {
	boolean done = false;
	uint8_t buffer[sizeof(PMessage)];
	PMessage c = 0;
	if (radio.available()) {
		while (!done) {
			done = radio.read(&buffer, sizeof(PMessage));
			c = (PMessage) buffer;
			Serial.println("RECEIVING");
			c.print();
		}
		if (c.is_protocol()) {
			switch	(c.get_type()) {
				case PMessage::WHO_LISTEN:
					Serial.println("SENDING I'm listening");
					i_am_listening();
					break;
				case PMessage::I_LISTEN:
					Serial.println("Received I_LISTEN");
					received_i_am_listening(c);
					break;
				case PMessage::ASK_CONFIG:
#ifndef MASTER
					{
					if(c.id_dest == id) {
						flag = RETRANSMIT;
						Serial.println("retransmit ASK_CONFIG");
						forward = c.id_dest;
						c.id_dest = parentId;
						write(c);
					}
					}
#else
					{
					if(c.id_dest == id) { 
					Serial.println("sending set_config");
					uint8_t parent = getParent(c);
					PMessage r = PMessage(PMessage::PROTOCOL, PMessage::SET_CONFIG, (uint8_t) 0x00, (uint8_t) id, nextId, (uint8_t) level, parent);	
					if(parent == id) { 
						Serial.print("NEXT ID "); Serial.println(nextId);
						registerPipe(findOpenPipe(), nextId);
					} else { 
						registerIndirecChild(parent, nextId);
					}
					write(r);
					nextId++;
					print();
					}
					}
#endif					
					break;
				case PMessage::SET_CONFIG:
#ifndef MASTER
					if(flag == RETRANSMIT) {
						Serial.println("retransmit SET_CONFIG");
						flag = NOTHING;
						c.id_dest = forward;
						if(c.total == id) {
							// I'm his parent, open new pipe;							
							registerPipe(findOpenPipe(), c.value);
						}
						if(indirectChild(c.total)) {
							registerIndirecChild(c.total, c.value);
						}
						write(c);
						print();
					} else if(flag == WAITING) {
						Serial.println("Received set_config");
						flag = NOTHING;
						received_set_config(c);
					}
#endif					
					break;


			}
		}

	}
}
/*	for(int i = 0; i < 4; i++) {
 if() {

 }
 }*/



void Protocol::received_set_config(PMessage c) {
	parentId = c.total; //c.id_from;
	parentPipe = c.total; //c.id_from;
	id = c.value;
	level = c.level;
}

void Protocol::received_i_am_listening(PMessage c) {
	if(c.id_dest == 0x00 && id == 0x00)  {
		if(control > 4) {
			int bigger;
			uint8_t temp = 0;
			for(int i = 0; i < 4; i++) { 
				if(levelSelection[i] > temp) {
					temp = levelSelection[i];
					bigger = i;
				}
			}
			if(c.level < levelSelection[bigger]) {
				levelSelection[bigger]	= c.level;
				idSelection[bigger]	= c.id_from;
			}
		} else {
			idSelection[control] = c.id_from;
			levelSelection[control] = c.level;
			control++;
			Serial.print("CONTROL: ");
			Serial.println(control);
		}
	}
}

bool Protocol::is_direct_child(uint8_t child) {
}

void Protocol::print() { 
	Serial.print("ID ");
	Serial.println(id);
	Serial.print("PARENT ");
	Serial.println(parentId);
	Serial.print("FLAG ");
	Serial.println(flag);
	Serial.println(childNodes[0][0]);
	for (int i = 0; i < 4; i++) {
		Serial.print("(Indirect?) Child of Pipe: ");
		Serial.println(i);
		for (int x = 0; x < childNodesSize[i]; x++) {
			Serial.print(childNodes[i][x], HEX);
		}
		Serial.println("");
	}
}


Protocol::~Protocol() {
	// TODO Auto-generated destructor stub
}

