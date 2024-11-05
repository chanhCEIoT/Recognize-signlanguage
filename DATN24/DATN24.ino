#define ADDRESS 0x1E //0011110b, I2C 7bit address of HMC5883
#include <Wire.h> 
#include <SimpleKalmanFilter.h>
#include <U8g2lib.h>
#include <ITG3200.h>
#include <string.h>
#include <SoftwareSerial.h>

//Create software serial object to communicate with HC-05
SoftwareSerial mySerial(3, 2); //HC-05 Tx & Rx is connected to Arduino #3 & #2
ITG3200 gyro = ITG3200();

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 
SimpleKalmanFilter filter_Hmcx0(3, 2, 0.01);
SimpleKalmanFilter filter_Hmcy0(3, 2, 0.01);
SimpleKalmanFilter filter_Hmcz0(3, 2, 0.01);
SimpleKalmanFilter filter_Hmcx1(3, 2, 0.01);
SimpleKalmanFilter filter_Hmcy1(3, 2, 0.01);
SimpleKalmanFilter filter_Hmcz1(3, 2, 0.01);
SimpleKalmanFilter filter_flex0(3, 2, 0.05);
SimpleKalmanFilter filter_flex1(3, 2, 0.05);
SimpleKalmanFilter filter_flex2(3, 2, 0.05);
SimpleKalmanFilter filter_flex3(3, 2, 0.05);
SimpleKalmanFilter filter_gX(5, 2, 0.05);
SimpleKalmanFilter filter_gY(5, 2, 0.05);
SimpleKalmanFilter filter_gZ(5, 2, 0.05);
int hmc_0[3],hmc_1[3],flexSensor[4],d_hmc[3],dConstant_hmc[3];
float ix, iy, iz;
void readFlexSensor(int *flexSensor){
  flexSensor[0] = analogRead(A0);
  flexSensor[1] = analogRead(A1);
  flexSensor[2] = analogRead(A2);
  flexSensor[3] = analogRead(A3);
}

void readHMCsensor(int addressi2c, int *hmc){
  //Tell the HMC5883 where to begin reading data
  Wire.beginTransmission(ADDRESS);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();
 //Read data from each axis, 2 registers per axis
  Wire.requestFrom(ADDRESS, 6);
  if(6<=Wire.available()){
    hmc[0] = Wire.read()<<8; //X msb
    hmc[0] |= Wire.read(); //X lsb
    hmc[1] = Wire.read()<<8; //Z msb
    hmc[1] |= Wire.read(); //Z lsb
    hmc[2] = Wire.read()<<8; //Y msb
    hmc[2] |= Wire.read(); //Y lsb
  }
}

void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(0x70);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void caliBrate(int *hmc_0, int *hmc_1, int *dif){
  SimpleKalmanFilter filter_00(3, 2, 0.001);
  SimpleKalmanFilter filter_01(3, 2, 0.001);
  SimpleKalmanFilter filter_02(3, 2, 0.001);
  SimpleKalmanFilter filter_10(3, 2, 0.001);
  SimpleKalmanFilter filter_11(3, 2, 0.001);
  SimpleKalmanFilter filter_12(3, 2, 0.001);
  int check = 1;
  while(check){
    for(int i = 0; i < 1000; i++){
      tcaselect(0);
      readHMCsensor(ADDRESS,hmc_0);
      hmc_0[0] = filter_00.updateEstimate(hmc_0[0]);
      hmc_0[1] = filter_01.updateEstimate(hmc_0[1]);
      hmc_0[2] = filter_02.updateEstimate(hmc_0[2]);
      tcaselect(1);
      readHMCsensor(ADDRESS,hmc_1);
      hmc_1[0] = filter_10.updateEstimate(hmc_1[0]);
      hmc_1[1] = filter_11.updateEstimate(hmc_1[1]);
      hmc_1[2] = filter_12.updateEstimate(hmc_1[2]);
      delay(1);
    }
    dif[0] = hmc_1[0] - hmc_0[0];
    dif[1] = hmc_1[1] - hmc_0[1];
    dif[2] = hmc_1[2] - hmc_0[2];
    for(int j = 0;j<100;j++){
    tcaselect(0);
    readHMCsensor(ADDRESS,hmc_0);
    hmc_0[0] = filter_Hmcx0.updateEstimate(hmc_0[0]);
    hmc_0[1] = filter_Hmcy0.updateEstimate(hmc_0[1]);
    hmc_0[2] = filter_Hmcz0.updateEstimate(hmc_0[2]);
    
    tcaselect(1);
    readHMCsensor(ADDRESS,hmc_1);
    hmc_1[0] = filter_Hmcx1.updateEstimate(hmc_1[0]);
    hmc_1[1] = filter_Hmcy1.updateEstimate(hmc_1[1]);
    hmc_1[2] = filter_Hmcz1.updateEstimate(hmc_1[2]);

    filter_00.updateEstimate(hmc_0[0]);
    filter_01.updateEstimate(hmc_0[1]);
    filter_02.updateEstimate(hmc_0[2]);
    filter_10.updateEstimate(hmc_1[0]);
    filter_11.updateEstimate(hmc_1[1]);
    filter_12.updateEstimate(hmc_1[2]);

    delay(10);
    }
    d_hmc[0] = hmc_1[0] - hmc_0[0] - dif[0];
    d_hmc[1] = hmc_1[1] - hmc_0[1] - dif[1];
    d_hmc[2] = hmc_1[2] - hmc_0[2] - dif[2];
      if(abs(d_hmc[0])>10 || abs(d_hmc[1])>10 || abs(d_hmc[2])>10 ){
        check = 1;
        u8g2.clearBuffer();
        u8g2.setFont( u8g2_font_7x14B_tf);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
        u8g2.drawStr(10,20,"re-calibrating...");	// write something to the internal memory
        u8g2.sendBuffer();
      }
      else{
        check = 0;
      }
      
  }
}


void setup(){  
  //Initialize Serial,I2C,oled communications
  pinMode(LED_BUILTIN, OUTPUT);
  gyro.init(ITG3200_ADDR_AD0_LOW); 
  // SoftwareSerial mySerial(3,2); //HC-05 Tx & Rx is connected to Arduino #3 & #2
  Serial.begin(38400);
  mySerial.begin(38400);
  u8g2.begin();
  //config HMC to read data
  Wire.begin();  
  Wire.beginTransmission(0x70);
  Wire.write(1 << 0);
  Wire.endTransmission();  
  Wire.beginTransmission(ADDRESS); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();

  Wire.beginTransmission(0x70);
  Wire.write(1 << 1);
  Wire.endTransmission();  
  // Serial.println("done.");
  
  Wire.beginTransmission(ADDRESS); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();

  
  // Serial.print("Calibrating...");
  u8g2.clearBuffer();
  u8g2.setFont( u8g2_font_7x14B_tf);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.drawStr(17,20,"Calibrating...");	// write something to the internal memory
  u8g2.sendBuffer();
  gyro.zeroCalibrate(500, 10);
  // caliBrate(hmc_0,hmc_1,dConstant_hmc);
    // delay(3000);
  u8g2.clearBuffer();
  u8g2.setFont(  u8g2_font_7x14B_tf);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.drawStr(50,20,"Done");
  u8g2.sendBuffer();
  delay(1000);
  

}
int i = 0;

void loop(){
  i++;
  if(i%10 == 0){
    tcaselect(1);
    gyro.readGyro(&ix,&iy,&iz); 
    ix = filter_gX.updateEstimate(ix);
    iy = filter_gY.updateEstimate(iy);
    iz = filter_gZ.updateEstimate(iz);
    readFlexSensor(flexSensor);
    flexSensor[0] = filter_flex0.updateEstimate(flexSensor[0]);
    flexSensor[1] = filter_flex1.updateEstimate(flexSensor[1]);
    flexSensor[2] = filter_flex2.updateEstimate(flexSensor[2]);
    flexSensor[3] = filter_flex3.updateEstimate(flexSensor[3]);
  }
  if(i == 49){
    u8g2.clearBuffer();
    digitalWrite(LED_BUILTIN,LOW);
  }
  if(i%10 ==0){
    char dataString[128];
    
    sprintf(dataString, "%lld\t%lld\t%lld\t%d\t%d\t%d\t%d\n", (long long)(ix),(long long)(iy),(long long)(iz), flexSensor[0], flexSensor[1], flexSensor[2], flexSensor[3]);
    mySerial.print(dataString);
    Serial.print(dataString);
  }
  if(i==50){
    digitalWrite(LED_BUILTIN, HIGH);
    char result[50];
     sprintf(result,"Gx:%d",int(ix));
    u8g2.setFont( u8g2_font_tiny5_tf);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.drawStr(10,6,result);	// write something to the internal memory
    
    sprintf(result,"Gy:%d",int(iy));
    u8g2.setFont( u8g2_font_tiny5_tf);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.drawStr(10,13,result);	// write something to the internal memory
    
    sprintf(result,"Gz:%d",int(iz));
    u8g2.setFont( u8g2_font_tiny5_tf);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.drawStr(10,20,result);	// write something to the internal memory

    sprintf(result,"S1:%d",flexSensor[0]);
    u8g2.setFont( u8g2_font_tiny5_tf);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.drawStr(50,6,result);	// write something to the internal memory

    sprintf(result,"S2:%d",flexSensor[1]);
    u8g2.setFont( u8g2_font_tiny5_tf);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.drawStr(50,13,result);	// write something to the internal memory

    sprintf(result,"S3:%d",flexSensor[2]);
    u8g2.setFont( u8g2_font_tiny5_tf);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.drawStr(50,20,result);	// write something to the internal memory
    
    sprintf(result,"S4:%d",flexSensor[3]);
    u8g2.setFont( u8g2_font_tiny5_tf);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.drawStr(50,27,result);	// write something to the internal memory

    sprintf(result,"DATN");
    u8g2.setFont( u8g2_font_squeezed_b6_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.drawStr(90,10,result);	// write something to the internal memory

    sprintf(result,"2024");
    u8g2.setFont( u8g2_font_squeezed_b6_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.drawStr(90,25,result);	// write something to the internal memory

    u8g2.sendBuffer();
    i = 0;
  
  }
  delayMicroseconds(1);
}

