
#include <SimpleKalmanFilter.h>

SimpleKalmanFilter bo_loc1(2, 2, 0.1);
SimpleKalmanFilter bo_loc2(2, 2, 0.1);
SimpleKalmanFilter bo_loc3(2, 2, 0.1);
SimpleKalmanFilter bo_loc4(5, 2, 0.0005);
// SimpleKalmanFilter bo_loc5(2, 2, 0.1);
unsigned long previousMillis = 0;  // Biến lưu trữ thời gian trước đó
const long interval = 49;   // Khoảng thời gian giữa các lần in ra màn hình (50ms)
int i = 0;
void setup() {
  Serial.begin(230400);
  // pinMode(A0,OUTPUT);
   pinMode(LED_BUILTIN, OUTPUT);

}


void loop() {
  
  unsigned long currentMillis = millis();
  // Đọc giá trị từ 5 cảm biến Flex Sensor
  int flex1 = analogRead(A0);
  int flex2 = analogRead(A1);
  int flex3 = analogRead(A2);
  int flex4 = analogRead(A3);
  // int flex5 = analogRead(A5);

  flex1 = bo_loc1.updateEstimate(flex1);
  flex2 = bo_loc2.updateEstimate(flex2);
  flex3 = bo_loc3.updateEstimate(flex3);
  flex4 = bo_loc4.updateEstimate(flex4);
  // flex5 = bo_loc5.updateEstimate(flex5);

    // Kiểm tra nếu đã đủ thời gian để in ra màn hình
  Serial.print(flex1);
  Serial.print("\t");
  Serial.print(flex2);
  Serial.print("\t");
  Serial.print(flex3);
  Serial.print("\t");
  Serial.print(flex4);
  // Serial.print("\t");
  // Serial.print(flex5);
  Serial.println();
  if(i%2 == 0)
  // analogWrite(A0,0);
  digitalWrite(LED_BUILTIN, HIGH);
  else
  // analogWrite(A0,1023);
  digitalWrite(LED_BUILTIN, LOW); 
  i++; 
  delay(10);  // Đợi 50ms trước khi đọc lại dữ liệu
}
