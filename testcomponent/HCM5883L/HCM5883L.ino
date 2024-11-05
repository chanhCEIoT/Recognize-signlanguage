#define address 0xD //0011110b, I2C 7bit address of HMC5883
#include <Wire.h> 
#include <SimpleKalmanFilter.h>
SimpleKalmanFilter bo_loc1(3, 2, 0.05);
SimpleKalmanFilter bo_loc2(3, 2, 0.05);
SimpleKalmanFilter bo_loc3(3, 2, 0.05);
int xf,yf,zf;
void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(0x70);
  Wire.write(1 << i);
  Wire.endTransmission();  
}
void setup(){  
  //Initialize Serial and I2C communications
  Serial.begin(230400);
  tcaselect(1);
  Wire.begin();  
  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(address); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();
}

void loop(){
  
  int x,y,z; //triple axis data
  int xmin,xmax,ymin,ymax,zmin,zmax;
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
  
  xf = bo_loc1.updateEstimate(x);
  yf = bo_loc2.updateEstimate(y);
  zf = bo_loc3.updateEstimate(z);
  Serial.print(-500);
  Serial.print("\t");
  Serial.print(xf);
  Serial.print("\t");
  Serial.print(yf);
  Serial.print("\t");
  Serial.print(zf);
  Serial.print("\t");  
  Serial.print(500);
  Serial.println();
  delay(10);
}

