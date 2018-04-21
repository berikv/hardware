
#include <HM10.h>

HM10SoftwareSerial ble(8, 9);
void setup() {
  Serial.begin(9600);

  while (!ble.ping());
//  ble.factoryReset();
//
//  ble.setIBeaconUUID("f7826da6-4fa2-4e98-8024-bc5b71e0893e");
//  ble.setIBeacon(true);

  ble.setRemoteAccessMode(RemoteAccessModeReadPins);
  
  ble.reboot();
  
  while (!ble.ping());
 
  ble.proxyTo(Serial);
  /*
  ble.getName();
  ble.setName("test");
  
  // Set ibeacon UUID to f7826da6-4fa2-4e98-8024-bc5b71e0893e
  ble.setIBeaconUUID("f7826da6-4fa2-4e98-8024-bc5b71e0893e");
  */
}

void loop() {
//  success = ble.setName("LightSensor");
//  Serial.println(String("Setname ") + (success ? "OK" : "Failed"));
//  Serial.println(ble.getName());
//
//   Serial.println(ble.getIBeaconUUID());
  
//  // Mimic as an Estimote (was 74278BDAB64445208F0C720EAF059935)
//  ble.setIBeaconUUID("B9407F30-F5F8-466E-AFF9-25556B57FE6D");
}
