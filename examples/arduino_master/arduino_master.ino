#include <SPI.h>
#include <PMessage.h>
#include <GenericStation.h>
#include <MasterStation.h>
#include <config.h>
#include "printf.h"

MasterStation<PMessage> * p;

PMessage d[5] = {0, 0, 0, 0, 0};
bool details = false;
long control = millis();

void setup() {
  SERIALBEGIN(57600);
  printf_begin();
  p = new MasterStation<PMessage>();
}


void loop() {
  if(millis() - control > 1000) {
    p->testMessage();
    control = millis();
  }
  p->update(d);
}