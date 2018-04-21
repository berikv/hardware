/*
  Software serial multple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo and Micro support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */
#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 9); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Setup..");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  mySerial.println("Hello, world!");
}

bool didReadCommand = false;

void loop() {
  if (mySerial.available()) {
    char c = mySerial.read();
    Serial.write(c);
  }
  
  if (Serial.available()) {
    didReadCommand = true;
        char c = Serial.read();
        Serial.write(c);
 
        // do not send line end characters to the HM-10
        if (c != 10 & c != 13 ) {  
           mySerial.write(c);
        }
 
  } else if (didReadCommand) {
    didReadCommand = false;
    Serial.println();
  }
}


/* Had a great conversation with HM10:
AT
OKAT+NAME?
OK+NAME:HMSoft 
AT+NAMELightSensor
OK+Set:LightSensor
AT+VERS?
HMSoft V549
AT+VERR?
HMSoft V549
AT+NAME?
OK+NAME:LightSensor 
AT+NOTI?
OK+Get:0
AT+NOTI1
OK+Set:1OK+CONNOK+LOST
AT+ROLE?
OK+Get:0OK+CONNOK+LOST
AT+DELO?
AT+DELO1
AT+UUID
AT+UUID?
OK+Get:0xFFE0
AT+IBE0?
OK+Get:0x74278BDA
AT+IBE1?
OK+Get:0xB6444520
AT+IBE2?
OK+Get:0x8F0C720E
AT+IBE3?
OK+Get:0xAF059935
AT+IBEA?
OK+Get:0
AT+IBEA1
OK+Set:1
AT
OK
AT+DELO1
OK+DELO1


 */
