/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          5             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI SS      SDA(SS)      9             53        D10        10               10
 * SPI SS      SDA(SS)      8             53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

const int numberReaders = 3;
const int ssPins[] = {10, 9, 8};
const int restPin = 5;
const String validIDs[] = {"6324ce12", "a3e999d", "e3f01d11"};
bool readerState[] = {false, false, false};
int servoPosition = 0;

// id at each reader 
String idAtReader[numberReaders];

MFRC522 mfrc522[numberReaders];

Servo myservo;

void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  
  for (int i = 0; i<numberReaders; i++) {
    mfrc522[i].PCD_Init(ssPins[i], restPin);
    delay(100);
  }

  myservo.attach(3);
    
  Serial.println(F("END SETUP"));
}

void loop() {

  // boolean isValid = false;

  for (int i = 0; i<numberReaders; i++) {
    mfrc522[i].PCD_Init();

    String currentID = "";

    if(mfrc522[i].PICC_IsNewCardPresent() && mfrc522[i].PICC_ReadCardSerial()) {
      currentID = getHexUID(mfrc522[i].uid.uidByte, mfrc522[i].uid.size);
      // Serial.print(currentID);
      // Serial.println();
    }

    if(currentID != idAtReader[i]) {
      idAtReader[i] = currentID;
    }

    if(idAtReader[i] == validIDs[i]) {
      readerState[i] = true;
    } else {
      readerState[i] = false;
    }
    
    mfrc522[i].PICC_HaltA();
    mfrc522[i].PCD_StopCrypto1();
  }

  if(isValidIds()) {
    myservo.write(180);
  } else {
    myservo.write(90);
  }
}

String getHexUID(byte * buffer, byte bufferSize) {
  String id = "";
  for (byte i = 0; i < bufferSize; i++) {
    id = id + String(buffer[i], HEX);
  }
  return id;
}

bool isValidIds() {
  return readerState[0] && readerState[1] && readerState[2];
}
