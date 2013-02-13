#include <SPI.h>
#include <PMessage.h>
#include <Station.h>
#include <GenericStation.h>

GenericStation * p;
PMessage d[5] = {0, 0, 0, 0, 0};
bool details = false;
long control = millis();

void setup() {
  Serial.begin(57600);
  p = new Station();
}


void loop() {
  if(p->update(d) > 0) {
    Serial.println("RECEBI");
    d[0].print();   
  }
}