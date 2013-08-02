#include "message.h"

Message::Message(PMessage::TYPE _pmsg, PMessage::CMD _t, uint8_t id_dest, uint8_t id_from, uint8_t _value, uint8_t _value2, uint8_t _value3, uint8_t _extra) : PMessage(_pmsg, _t, id_dest, id_from, _value, _value2, _value3) {
  extra = _extra;
}

Message::Message(PMessage::TYPE _pmsg, PMessage::CMD _t, uint8_t id_dest, uint8_t id_from, uint8_t _value, uint8_t _value2, uint8_t _value3) : PMessage(_pmsg, _t, id_dest, id_from, _value, _value2, _value3) {
}

Message::Message(uint8_t b[6]) {
	proto = 0;
	proto = b[0];
	id_dest = b[1];
	id_from = b[2];
	value = b[3];
	value2 = b[4];
	value3 = b[5];
        extra = b[6];
}
void Message::print() {
  PMessage::print();
  PRINTln(extra);
}
Message::Message() {
}
Message::~Message() {
	// TODO Auto-generated destructor stub
}
