
#include <HM10.h>
#include <SoftwareSerial.h>

//HM10SoftwareSerial ble(8, 9);
void setup() {
  Serial.begin(9600);

//  while (!ble.ping());
  /*
  ble.factoryReset();

  ble.setIBeaconUUID("f7826da6-4fa2-4e98-8024-bc5b71e0893e");
  ble.setIBeacon(true);

  ble.setRemoteAccessMode(RemoteAccessModeReadPins);

  ble.setName("Curtain");
  ble.reboot();
  
  while (!ble.ping());
  */
 
  //ble.proxyTo(Serial);

    SoftwareSerial serial(8, 9);
    serial.begin(9600);
    while (true) {
        while (Serial.available()) {
            char s = Serial.read();
            if (s == '\0' || s == '\r' || s == '\n') {
              continue;
            }
            serial.write(s);
        }

        while (serial.available() != 0) {
          char s = serial.read();
          Serial.write(s);
        }
    }
  
  /*
  ble.getName();
  ble.setName("test");
  
  // Set ibeacon UUID to f7826da6-4fa2-4e98-8024-bc5b71e0893e
  ble.setIBeaconUUID("f7826da6-4fa2-4e98-8024-bc5b71e0893e");
  */
}

void loop() {

//  long time = millis();
//  ble.setFlag((uint8_t) time);
//  delay(1000);

//  success = ble.setName("LightSensor");
//  Serial.println(String("Setname ") + (success ? "OK" : "Failed"));
//  Serial.println(ble.getName());
//
//   Serial.println(ble.getIBeaconUUID());
  
//  // Mimic as an Estimote (was 74278BDAB64445208F0C720EAF059935)
//  ble.setIBeaconUUID("B9407F30-F5F8-466E-AFF9-25556B57FE6D");

}
