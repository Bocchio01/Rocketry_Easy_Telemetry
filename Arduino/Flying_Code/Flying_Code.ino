/*
  Flying_Code.ino - Arduino code for the rocket flight data logger
  Created by Tommaso Bocchietti, 2023-07-20
  Released into the public domain.

  This code is meant to be used with the following hardware:
  - Arduino Nano
  - BMP180 sensor
  - MPU-6050 sensor
  - SD module
  - Buzzer

  The code is able to log the following data:
  - Time
  - Temperature
  - Pressure
  - Altitude
  - Acceleration on the X, Y and Z axes

  The data is logged on a SD card in a CSV file.
  The buzzer is used to indicate the status of the sensors and the SD module.
  After a period of time the buzzer starts to beep to facilitate the recovery of the rocket.
*/

#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#define DEBUG false  //set to true for debug output, false for no debug output
#define DEBUG_SERIAL \
  if (DEBUG) Serial

#define seaLevelPressure 101325  // [Pa]
// #define logsPerSecond 10
#define minutesBeforeLanding 15  // Logging stopped + Landing buzzer activated
#define positiveDelay 100
#define negativeDelay 3000
#define landOffDelay 1000
#define MPU_addr 0x68
#define SD_sckPin 10
#define buzzerPin 6

Adafruit_BMP085 bmp;
char completeFileName[32],
  fileName[16] = "FLIGHT";
int16_t dataIndex = 0;
int16_t AccX,
  AccY,
  AccZ;
int32_t Pressure;
float Temperature,
  Altitude;


void setup() {
  boolean BMP180_flag = false;
  boolean MPU_6050_flag = false;
  boolean SD_flag = false;

  pinMode(buzzerPin, OUTPUT);

  // Wait for Serial
  if (DEBUG) {
    while (!Serial) {}
  }
  DEBUG_SERIAL.begin(9600);
  // DEBUG_SERIAL.println("Rocket own by Tommaso Bocchietti");
  // DEBUG_SERIAL.println("In case you have found this rocket somewhere, please contact me at tommaso.bocchietti@gmail.com! Thanks.");

  // Wait for BMP180
  do {
    BMP180_flag = bmp.begin();
    if (BMP180_flag) {
      DEBUG_SERIAL.println("BMP180 sensor connected!");
      doBuzzer(positiveDelay);
    } else {
      DEBUG_SERIAL.println("Could not find BMP180 sensor!");
      doBuzzer(negativeDelay);
      delay(1000);
    }
  } while (!BMP180_flag);
  delay(1000);

  // Wait for MPU_6050
  do {
    Wire.begin();
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(0);     // set to zero (wakes up the MPU-6050)
    MPU_6050_flag = Wire.endTransmission(true);
    if (MPU_6050_flag == 0) {
      DEBUG_SERIAL.println("MPU-6050 sensor connected!");
      doBuzzer(positiveDelay);
    } else {
      DEBUG_SERIAL.println("Could not find MPU-6050 sensor!");
      doBuzzer(negativeDelay);
      delay(1000);
    }
  } while (MPU_6050_flag != 0);
  delay(1000);


  // Wait for SD
  do {
    SD_flag = SD.begin(SD_sckPin);
    if (SD_flag) {
      DEBUG_SERIAL.println("SD module connected!");
      doBuzzer(positiveDelay);
    } else {
      DEBUG_SERIAL.println("Could not find SD module!");
      doBuzzer(negativeDelay);
      delay(1000);
    }
  } while (!SD_flag);
  delay(1000);


  // Init new log file on SD card
  initNewFile(fileName);
  if (writeToFile("dataIndex;Time;Temperature;Pressure;Altitude;AccX;AccY;AccZ", completeFileName)) {
    doBuzzer(positiveDelay);
  } else {
    doBuzzer(negativeDelay);
  }
  delay(1000);

  // Setup complete
  for (uint8_t i = 0; i < 5; i++) {
    doBuzzer(100);
    delay(50);
  }
}


void loop() {
  unsigned long stopLoggingMillis = minutesBeforeLanding * 60UL * 1000UL,
                startTimeMillis = millis();

  while (millis() - startTimeMillis < stopLoggingMillis) {

    // BMP180
    Temperature = bmp.readTemperature();
    Pressure = bmp.readPressure();
    Altitude = bmp.readAltitude(seaLevelPressure);

    // MPU_6050
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 14, true);  // request a total of 14 registers

    AccX = Wire.read() << 8 | Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AccY = Wire.read() << 8 | Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AccZ = Wire.read() << 8 | Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

    // Write data to file
    char buffer[128], temperatureBuffer[5], altitudeBuffer[6];
    dtostrf(Temperature, 5, 2, temperatureBuffer);
    dtostrf(Altitude, 4, 2, altitudeBuffer);
    snprintf(buffer, sizeof(buffer), "%d;%lu;%s;%ld;%s;%d;%d;%d", dataIndex, millis(), temperatureBuffer, Pressure, altitudeBuffer, AccX, AccY, AccZ);
    writeToFile(buffer, completeFileName);

    dataIndex++;
    // delay(1000 / logsPerSecond);
  }

  while (true) {
    digitalWrite(buzzerPin, HIGH);
    delay(landOffDelay);
    digitalWrite(buzzerPin, LOW);
    delay(landOffDelay);
  }
}


void doBuzzer(uint16_t delayMs) {
  if (DEBUG == false) {
    digitalWrite(buzzerPin, HIGH);
    delay(delayMs);
    digitalWrite(buzzerPin, LOW);
  }
}


void initNewFile(const char* fileName) {
  int fileNumber = 0;

  do {
    snprintf(completeFileName, sizeof(completeFileName), "%d%s.CSV", fileNumber, fileName);
    fileNumber++;
  } while (SD.exists(completeFileName));

  File dataFile = SD.open(completeFileName, FILE_WRITE);
  if (dataFile) {
    DEBUG_SERIAL.println("File created: " + String(completeFileName));
    dataFile.close();
  } else {
    DEBUG_SERIAL.println("Error creating file!");
    completeFileName[0] = '\0';  // Empty the completeFileName string
  }
}


bool writeToFile(const char* dataString, const char* fileName) {
  File dataFile = SD.open(fileName, FILE_WRITE);

  if (dataFile) {
    DEBUG_SERIAL.println(dataString);
    dataFile.println(dataString);
    dataFile.close();
    return true;
  } else {
    DEBUG_SERIAL.println("Error writing to " + String(fileName));
    return false;
  }
}
