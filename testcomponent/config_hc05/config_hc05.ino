/*
  ***Cài đặt HC-06
  AT: Trả về OK
  AT+NAMExxxx: đặt tên, VD: AT+NAMEtest2 => Tên là test2
  AT+PINxxxx: đặt mật khẩu, VD: AT+PIN1111 => mật khẩu là 1111
  AT+BAUDx: Đặt tốc độ truyền được sử dụng cho giao tiếp UART nối tiếp
            x = số giá trị hex một chữ số từ 1 đến C, vd: AT+BAUD6
  1 ——— 1200
  2 ——— 2400
  3 ——— 4800
  4 ——— 9600 (Mặc định）
  5 ——— 19200
  6 ——— 38400
  7 ——— 57600
  8 ——— 115200
  9 ——— 230400
  A ——— 460800
  B ——— 921600
  C ——— 1382400
Trả lời bằng OKxxxx trong đó xxxx là tốc độ truyền mới. Ví dụ “AT + BAUD4 ”trả lời với“ OK9600 ”
 */

 
#include <SoftwareSerial.h>

SoftwareSerial hc05(3,2); //TX HC06 - RX 2, RX HC06 - TX 3

void setup(){
  //Khởi tạo Serial
  Serial.begin(38400);
  Serial.println("ENTER AT Commands:");
  //Khởi tạo Bluetooth
  hc05.begin(38400);
}

void loop(){
  //Đọc dữ liệu từ HC-06 gửi đến Arduino
  if (hc05.available()){
    Serial.write(hc05.read());
  }
  
  //Đọc dữ liệu từ arduino gửi đến HC-06
  if (Serial.available()){
    hc05.write(Serial.read());
  }  
}
