#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#define seaLevelPressure_hPa 1013.25

Adafruit_BMP085 bmp;
const int MPU_addr = 0x68;  // I2C address of the MPU-6050
int16_t AccX, AccY, AccZ;
int32_t Pressure;
float Temperature, Altitude;
const int chipSelect = 10;
const int ledPin = LED_BUILTIN;  // the number of the LED pin


void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);


  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1)
      digitalWrite(ledPin, HIGH);
  }
  Serial.println("card initialized.");
  writeToFile("Temperature;Pressure;Altitude;AccX;AccY;AccZ");
}


void loop() {

  Temperature = bmp.readTemperature();
  Pressure = bmp.readPressure();
  Altitude = bmp.readAltitude(seaLevelPressure_hPa * 100);

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true);  // request a total of 14 registers

  AccX = Wire.read() << 8 | Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AccY = Wire.read() << 8 | Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AccZ = Wire.read() << 8 | Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)


  String dataString = "";
  dataString += String(Temperature);
  dataString += ";";
  dataString += String(Pressure);
  dataString += ";";
  dataString += String(Altitude);
  dataString += ";";
  dataString += String(AccX);
  dataString += ";";
  dataString += String(AccY);
  dataString += ";";
  dataString += String(AccZ);

  if (writeToFile(dataString))
    digitalWrite(ledPin, LOW);
  else
    digitalWrite(ledPin, HIGH);

  delay(1000);
}


bool writeToFile(String dataString) {
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
    return false;
  }

  return true;
}