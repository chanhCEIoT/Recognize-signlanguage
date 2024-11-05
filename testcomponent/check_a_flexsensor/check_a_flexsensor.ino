#include <SimpleKalmanFilter.h>
SimpleKalmanFilter bo_loc1(3, 2, 0.05);
void setup() {
  // put your setup code here, to run once:

 Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int flex1 = analogRead(A0);
   flex1 = bo_loc1.updateEstimate(flex1);
   Serial.print(1000);
   Serial.print("\t");
   Serial.print(flex1);
   Serial.print("\t");
   Serial.print(0);
   Serial.println();
  delay(10); 
}
