#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

#define doBeep false

#define seaLevelPressure 101325  // [Pa]
#define logsPerSecond 10
#define minutesAtStopLogging 2
#define positiveDelay 100
#define negativeDelay 3000
#define landOffDelay 1000
#define MPU_addr 0x68
#define SD_sckPin 10
#define buzzerPin 6

Adafruit_BMP085 bmp;
char completeFileName[32], fileName[16] = "FLIGHT";
int16_t dataIndex = 0;
int16_t AccX, AccY, AccZ;
int32_t Pressure;
float Temperature, Altitude;


void setup() {
  boolean BMP180_flag = false;
  boolean GY_521_flag = false;
  boolean SD_flag = false;

  pinMode(buzzerPin, OUTPUT);

  // Wait for Serial
  while (!Serial) {}
  Serial.begin(9600);

  // Wait for BMP180
  do {
    BMP180_flag = bmp.begin();
    if (BMP180_flag) {
      Serial.println("BMP180 sensor connected!");
      doBuzzer(positiveDelay);
    } else {
      Serial.println("Could not find BMP180 sensor!");
      doBuzzer(negativeDelay);
      delay(1000);
    }
  } while (!BMP180_flag);
  delay(1000);

  // Wait for GY_521
  do {
    Wire.begin();
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(0);     // set to zero (wakes up the MPU-6050)
    GY_521_flag = Wire.endTransmission(true);
    if (GY_521_flag == 0) {
      Serial.println("GY-521 sensor connected!");
      doBuzzer(positiveDelay);
    } else {
      Serial.println("Could not find GY-521 sensor!");
      doBuzzer(negativeDelay);
      delay(1000);
    }
  } while (GY_521_flag != 0);
  delay(1000);


  // Wait for SD
  do {
    SD_flag = SD.begin(SD_sckPin);
    if (SD_flag) {
      Serial.println("SD module connected!");
      doBuzzer(positiveDelay);
    } else {
      Serial.println("Could not find SD module!");
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
  unsigned long stopLoggingMillis = minutesAtStopLogging * 60UL * 1000UL;

  while (millis() < stopLoggingMillis) {

    // BMP180
    Temperature = bmp.readTemperature();
    Pressure = bmp.readPressure();
    Altitude = bmp.readAltitude(seaLevelPressure);

    // GY_521
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
    delay(1000 / logsPerSecond);
  }

  while (true) {
    digitalWrite(buzzerPin, HIGH);
    delay(landOffDelay);
    digitalWrite(buzzerPin, LOW);
    delay(landOffDelay);
  }
}


void doBuzzer(uint16_t delayMs) {
  if (doBeep) {
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
    Serial.println("File created: " + String(completeFileName));
    dataFile.close();
  } else {
    Serial.println("Error creating file!");
    completeFileName[0] = '\0';  // Empty the completeFileName string
  }
}


bool writeToFile(const char* dataString, const char* fileName) {
  File dataFile = SD.open(fileName, FILE_WRITE);

  if (dataFile) {
    Serial.println(dataString);
    dataFile.println(dataString);
    dataFile.close();
    return true;
  } else {
    Serial.println("Error writing to " + String(fileName));
    return false;
  }
}
