#include <PMessage.h>

class Message: public PMessage {
  public:
      	Message();
      	~Message();
      	Message(PMessage::TYPE _pmsg, PMessage::CMD _t, uint8_t id_dest, uint8_t id_from, uint8_t _value, uint8_t _value2, uint8_t _value3, float _extra);
      	Message(PMessage::TYPE _pmsg, PMessage::CMD _t, uint8_t id_dest, uint8_t id_from, uint8_t _value, uint8_t _value2, uint8_t _value3);
      	Message(uint8_t b[9]);
        float extra;
        void print();
};
