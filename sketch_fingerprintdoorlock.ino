#include <Adafruit_Fingerprint.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); //Inisialisasi serial RX TX modul fingerprint
LiquidCrystal_I2C lcd(0X27, 16, 2); //Inisialisasi I2C LCD
int doorLock = 8; //pin untuk sinyal doorlock

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup()
{
Serial.begin(9600);
while (!Serial);
delay(100);
Serial.println("n\nAdafruit finger detect test");

// set the data rate for the sensor serial port
finger.begin(57600);

if (finger.verifyPassword()) {
Serial.println("Found fingerprint sensor!");
} else {
Serial.println("Did not find fingerprint sensor :(");
while (1) { delay(1); }
}

finger.getTemplateCount();
Serial.print("Sensor contains "); 
Serial.print(finger.templateCount); 
Serial.println(" templates");
Serial.println("Waiting for valid finger...");

lcd.begin();
lcd.clear();

pinMode(doorLock, OUTPUT);
digitalWrite(doorLock, HIGH);
lcd.setBacklight(HIGH);
lcd.setCursor(0,0); //set koordinat LCD
lcd.print("TEMPELKAN");
delay(100);
lcd.setCursor(0,1);
lcd.print("SIDIK JARI");
delay(100);
}


void loop()
{
getFingerprintID();
delay(50);
}


//------------------------------------------------------------
uint8_t getFingerprintID() {
uint8_t p = finger.getImage();
switch (p) {
case FINGERPRINT_OK:
Serial.println("Image taken");
break;
case FINGERPRINT_NOFINGER:
Serial.println("No finger detected");
return p;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println("Communication error");
return p;
case FINGERPRINT_IMAGEFAIL:
Serial.println("Imaging error");
return p;
default:
Serial.println("Unknown error");
return p;
}

// OK success!

p = finger.image2Tz();
switch (p) {
case FINGERPRINT_OK:
Serial.println("Image converted");
break;
case FINGERPRINT_IMAGEMESS:
Serial.println("Image too messy");
return p;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println("Communication error");
return p;
case FINGERPRINT_FEATUREFAIL:
Serial.println("Could not find fingerprint features");
return p;
case FINGERPRINT_INVALIDIMAGE:
Serial.println("Could not find fingerprint features");
return p;
default:
Serial.println("Unknown error");
return p;
}

// OK converted!---------------------------------------------
p = finger.fingerFastSearch();
if (p == FINGERPRINT_OK) { //jika fingerprint terdeteksi
Serial.println("Found a print match!");
lcd.clear();
lcd.setCursor(0,0);
lcd.print("SIDIK JARI BENAR");
delay (100);

digitalWrite(doorLock, LOW);
lcd.setCursor(0,1);
lcd.print("AKAN TERTUTUP : ");
for(int i=5; i>0; i--)
{
lcd.setCursor (15,1); lcd.print(i);
delay (1000);
}
lcd.clear();
lcd.setCursor(0,0);
lcd.print("TEMPELKAN");
delay(100);
lcd.setCursor(0,1);
lcd.print("SIDIK JARI");
delay(100);
digitalWrite(doorLock, HIGH);
}
else if (p == FINGERPRINT_PACKETRECIEVEERR) {
Serial.println("Communication error");
return p;
}
else if (p == FINGERPRINT_NOTFOUND) { //jika fingerprint salah
Serial.println("Did not find a match");

lcd.clear();
lcd.setCursor(0,0);
lcd.print("SIDIK JARI SALAH  ");
delay (100);
lcd.setCursor(0,1);
lcd.print("COBA LAGI");
delay (2000);

lcd.clear();
lcd.setCursor(0,0);
lcd.print("TEMPELKAN");
delay(100);
lcd.setCursor(0,1);
lcd.print("SIDIK JARI");
delay(100);
return p;
}
else {
Serial.println("Unknown error");
lcd.clear();
lcd.setCursor(0,0);
lcd.print("??");
delay (100);
return p;
}

// found a match!
Serial.print("Found ID #"); Serial.print(finger.fingerID);
Serial.print(" with confidence of "); Serial.println(finger.confidence);

return finger.fingerID;
}
//-----------------------------------------------------------------------------
