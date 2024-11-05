// Arduino code and libraries are available to download - link below the video.

// Output for viewing with Serial Oscilloscope: accX,accY,magZ // gyrX, gyrY and gyrZ are commented out

/*
Arduino     MARG GY-85
  A5            SCL
  A4            SDA
  3.3V          VCC
  GND           GND
*/

#include <Wire.h>
#include <ADXL345.h>   // ADXL345 Accelerometer Library
#include <HMC5883L.h>  // HMC5883L Magnetometer Library
#include <ITG3200.h>

ADXL345 acc;  //variable adxl is an instance of the ADXL345 library
HMC5883L compass;
ITG3200 gyro = ITG3200();
float gx, gy, gz;
float gx_rate, gy_rate, gz_rate;
int ix, iy, iz;
float anglegx = 0.0, anglegy = 0.0, anglegz = 0.0;
int ax, ay, az;
int rawX, rawY, rawZ;
float X, Y, Z;
float rollrad, pitchrad;
float rolldeg, pitchdeg;
int error = 0;
float aoffsetX, aoffsetY, aoffsetZ;
float goffsetX, goffsetY, goffsetZ;
unsigned long time, looptime;

void setup() {
  Serial.begin(9600);
  acc.powerOn();
  compass = HMC5883L();
  error = compass.SetScale(1.3);  // Set the scale to +/- 1.3 Ga of the compass
  if (error != 0)                 // If there is an error, print it out.
    Serial.println(compass.GetErrorText(error));

  // Serial.println("Setting measurement mode to continous");
  error = compass.SetMeasurementMode(Measurement_Continuous);  // Set the measurement mode to Continuous
  if (error != 0)                                              // If there is an error, print it out.
    Serial.println(compass.GetErrorText(error));
  for (int i = 0; i <= 200; i++) {
    acc.readAccel(&ax, &ay, &az);
    if (i == 0) {
      aoffsetX = ax;
      aoffsetY = ay;
      aoffsetZ = az;
    }
    if (i > 1) {
      aoffsetX = (ax + aoffsetX) / 2;
      aoffsetY = (ay + aoffsetY) / 2;
      aoffsetZ = (az + aoffsetZ) / 2;
    }
  }
  for (int i = 0; i <= 200; i++) {
    gyro.readGyro(&gx, &gy, &gz);
    if (i == 0) {
      goffsetX = gx;
      goffsetY = gy;
      goffsetZ = gz;
    }
    if (i > 1) {
      goffsetX = (gx + goffsetX) / 2;
      goffsetY = (gy + goffsetY) / 2;
      goffsetZ = (gz + goffsetZ) / 2;
    }
  }


  delay(1000);
  gyro.init(ITG3200_ADDR_AD0_LOW);
  // Serial.print("zero Calibrating...");
  // gyro.zeroCalibrate(2500, 2);
  // Serial.println("done.");
}

void loop() {
  // code fragment for Accelerometer angles (roll and pitch)
  time = millis();
  acc.readAccel(&ax, &ay, &az);  //read the accelerometer values and store them in variables  x,y,z
  rawX = ax - aoffsetX;
  rawY = ay - aoffsetY;
  rawZ = az - (255 - aoffsetZ);
  X = rawX / 256.00;                                      // used for angle calculations
  Y = rawY / 256.00;                                      // used for angle calculations
  Z = rawZ / 256.00;                                      // used for angle calculations
  rollrad = atan(Y / sqrt(X * X + Z * Z));                // calculated angle in radians
  pitchrad = atan(X / sqrt(Y * Y + Z * Z));               // calculated angle in radians
  rolldeg = 180 * (atan(Y / sqrt(X * X + Z * Z))) / PI;   // calculated angle in degrees
  pitchdeg = 180 * (atan(X / sqrt(Y * Y + Z * Z))) / PI;  // calculated angle in degrees

  // Code fragment for Magnetometer heading (yaw)
  MagnetometerRaw raw = compass.ReadRawAxis();
  MagnetometerScaled scaled = compass.ReadScaledAxis();
  int MilliGauss_OnThe_XAxis = scaled.XAxis;  // (or YAxis, or ZAxis)
  float heading = atan2(scaled.YAxis, scaled.XAxis);
  float declinationAngle = 0.0457;
  heading += declinationAngle;
  if (heading < 0)
    heading += 2 * PI;
  if (heading > 2 * PI)
    heading -= 2 * PI;
  float headingDegrees = heading * 180 / M_PI;

  // Code fragment for Gyroscope (roll, pitch, yaw)
  gyro.readGyro(&gx, &gy, &gz);
  looptime = millis() - time;
  gx_rate = (gx - goffsetX) / 14.375;
  gy_rate = (gy - goffsetY) / 14.375;
  gz_rate = (gz - goffsetZ) / 14.375;
  delay(10);
  Serial.print(gx_rate);  // calculated angle in degrees
  Serial.print("\t");
  Serial.print(gy_rate);
  Serial.print("\t");
  Serial.print(gz_rate);
  Serial.println();

  // anglegx = anglegx + (gx_rate * looptime);
  // anglegy = anglegy + (gx_rate * looptime);
  // anglegz = anglegz + (gz_rate * looptime);
  // Accelerometer, Magnetometer and Gyroscope Output
  // Serial.print(rolldeg);
  // Serial.print(",");
  // Serial.print(pitchdeg);  // calculated angle in degrees
  // Serial.print(",");
  // Serial.print(headingDegrees);
  // Serial.println(",");
  // Serial.print(anglegx);
  // Serial.print(",");
  // Serial.print(anglegy);
  // Serial.print(",");
  // Serial.println(anglegz);
  //Serial.println(looptime);
  // Output(raw, scaled, heading, headingDegrees);
}


void Output(MagnetometerRaw raw, MagnetometerScaled scaled, float heading, float headingDegrees) {
  Serial.print("Raw:\t");
  Serial.print(raw.XAxis);
  Serial.print("   ");
  Serial.print(raw.YAxis);
  Serial.print("   ");
  Serial.print(raw.ZAxis);
  Serial.print("   \tScaled:\t");
  Serial.print(scaled.XAxis);
  Serial.print("   ");
  Serial.print(scaled.YAxis);
  Serial.print("   ");
  Serial.println(scaled.ZAxis);
  // Serial.print("   \tHeading:\t");
  // Serial.print(heading);
  // Serial.print(" Radians   \t");
}
