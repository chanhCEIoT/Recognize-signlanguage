#define address 0x1E //0011110b, I2C 7bit address of HMC5883
#include <Wire.h> 
#include <SimpleKalmanFilter.h>
// SimpleKalmanFilter bo_loc1(3, 2, 0.05);
// SimpleKalmanFilter bo_loc2(3, 2, 0.05);
// SimpleKalmanFilter bo_loc3(3, 2, 0.05);
void readHMCsensor(int addressi2c){
    //Put the HMC5883 IC into the correct operating mode

  int x,y,z; //triple axis data
  int xmin,xmax,ymin,ymax,zmin,zmax;
  int xf,yf,zf;
  xmin=0; xmax=0; ymax=0; ymin = 0; zmin=0;zmax=0;
  //Tell the HMC5883 where to begin reading data
  Wire.beginTransmission(address);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();
 //Read data from each axis, 2 registers per axis
  Wire.requestFrom(address, 6);
  if(6<=Wire.available()){
    x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
  }
  
  // xf = bo_loc1.updateEstimate(x);
  // yf = bo_loc2.updateEstimate(y);
  // zf = bo_loc3.updateEstimate(z);
  
  Serial.print("\t");
  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.print(z);
  Serial.print("\t");  

  delay(10);
}

void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(0x70);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void setup(){  
  //Initialize Serial and I2C communications
  Serial.begin(230400);
  Wire.begin();  
  Wire.beginTransmission(0x70);
  Wire.write(1 << 0);
  Wire.endTransmission();  
  Wire.beginTransmission(address); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();

  Wire.beginTransmission(0x70);
  Wire.write(1 << 1);
  Wire.endTransmission();  
  Wire.beginTransmission(address); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();
}

void loop(){
  // Serial.print("sensor 1: \t");
  Serial.print(-500);
  tcaselect(0);
  readHMCsensor(address);
  tcaselect(1);
  readHMCsensor(address);
    Serial.print(500);
  // Serial.print("\t sensor 2: \t");
  // tcaselect(1);
  // readHMCsensor(address);
  Serial.println();
}

