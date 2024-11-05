// ITG-3200_test
// Copyright 2010-2011 Filipe Vieira & various contributors.
// http://code.google.com/p/itg-3200driver
// Simple test of gyro sensors output using default settings.

#include <Wire.h>
#include <ITG3200.h>
#include <SimpleKalmanFilter.h>
SimpleKalmanFilter filter_gX(3, 2, 0.05);
SimpleKalmanFilter filter_gY(3, 2, 0.05);
SimpleKalmanFilter filter_gZ(3, 2, 0.05);
ITG3200 gyro = ITG3200();
int ix, iy, iz;

void setup(void) {
  Serial.begin(230400);
  Wire.begin();      // if experiencing gyro problems/crashes while reading XYZ values
                     // please read class constructor comments for further info.
  Wire.beginTransmission(0x70);
  Wire.write(1 << 1);
  Wire.endTransmission();  
  delay(1000);
  // Use ITG3200_ADDR_AD0_HIGH or ITG3200_ADDR_AD0_LOW as the ITG3200 address 
  // depending on how AD0 is connected on your breakout board, check its schematics for details
  gyro.init(ITG3200_ADDR_AD0_LOW); 
  
  Serial.print("Calibrating...");
  gyro.zeroCalibrate(100, 10);
  Serial.println("done.");
}

void loop(void) {
    // while (gyro.isRawDataReady()) {
    
    // Reads uncalibrated raw values from the sensor 
    // gyro.readGyroRaw(&ix,&iy,&iz); 
    // Serial.print("\t"); 
    // ix = filter_gX.updateEstimate(ix);
    // Serial.print(ix); 
    // Serial.print("\t"); 
    // iy = filter_gX.updateEstimate(iy);
    // Serial.print(iy); 
    // Serial.print("\t"); 
    // iz = filter_gX.updateEstimate(iz);
    // Serial.println(iz); 
    
     
    
    // Reads calibrated raw values from the sensor 
    gyro.readGyroRawCal(&ix,&iy,&iz); 
    Serial.print("\t"); 
    Serial.print(1000);
    Serial.print("\t"); 
    ix = filter_gX.updateEstimate(ix);
    Serial.print(ix); 
    Serial.print("\t"); 
    iy = filter_gX.updateEstimate(iy);
    Serial.print(iy); 
    Serial.print("\t"); 
    iz = filter_gX.updateEstimate(iz);
    Serial.print(iz); 
    Serial.print("\t"); 
    Serial.println(-1000);
    
     
    // Reads calibrated values in deg/sec    
    // gyro.readGyro(&x,&y,&z); 
    // Serial.print(1000); 
    // Serial.print("\t"); 
    // Serial.print(x); 
    // Serial.print("\t"); 
    // Serial.print(y); 
    // Serial.print("\t"); 
    // Serial.print(z);
    // Serial.print("\t"); 
    // Serial.print(-1000);
    // Serial.println(); 
    delay(100);
  // } 
}



