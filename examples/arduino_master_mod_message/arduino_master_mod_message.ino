#include <SPI.h>
#include "message.h"
#include <GenericStation.h>
#include <MasterStation.h>
#include <config.h>
#include "printf.h"

MasterStation<Message> * p;


Message d[5] = {0, 0, 0, 0, 0};

bool details = false;
long control = millis();

void setup() {
  SERIALBEGIN(57600);
  printf_begin();
  p = new MasterStation<Message>();
}

void testMessage() { 
    Message t = Message(PMessage::TUSER, PMessage::CUSER, (p->nextId - 1), 0x01, 233, 255, 100, 25);
    uint8_t pipe = p->findChildPipe(p->nextId - 1);
    if (pipe != 0xFF) {
        p->writePipe(p->findChildPipe(p->nextId - 1), t);
    } 
}

void loop() {
  if(millis() - control > 1000) {
    testMessage();
    control = millis();
  }
  p->update(d);
}