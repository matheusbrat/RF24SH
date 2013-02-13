#include <SPI.h>
#include <PMessage.h>
#include <GenericStation.h>
#include <MasterStation.h>

#define MASTER


GenericStation * p;
PMessage d[5] = {0, 0, 0, 0, 0};
bool details = false;
long control = millis();

void setup() {
  Serial.begin(57600);
  p = new MasterStation();
}

void loop() {
  if(millis() - control > 2000) {
    ((MasterStation*) p)->testMessage();
    control = millis();
  }
  p->update(d);
}