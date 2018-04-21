#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>  // Thêm vào sketch thư viện Serial Command
const byte RX = 3;          // Chân 3 được dùng làm chân RX
const byte TX = 2;          // Chân 2 được dùng làm chân TX
 
SoftwareSerial mySerial = SoftwareSerial(RX, TX); 
 
 
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command
 
int red = 4, blue = 5; // led đỏ đối vô digital 4, led xanh đối vô digital 5
int rainSensor = 6; // Chân tín hiệu cảm biến mưa ở chân digital 6 (arduino)
 
const unsigned long CHU_KY_1_LA_BAO_NHIEU = 5000UL; //Cứ sau 5000ms = 5s thì chu kỳ lặp lại
 
void setup() {
  //Khởi tạo Serial ở baudrate 57600 để debug ở serial monitor
  Serial.begin(57600);
 
  //Khởi tạo Serial ở baudrate 57600 cho cổng Serial thứ hai, dùng cho việc kết nối với ESP8266
  mySerial.begin(57600);
  
  //pinMode 2 đèn LED là OUTPUT
  pinMode(red,OUTPUT);
  pinMode(blue,OUTPUT);
 
  pinMode(rainSensor,INPUT);// Đặt chân cảm biến mưa là INPUT, vì tín hiệu sẽ được truyền đến cho Arduino
  
  
  // Một số hàm trong thư viện Serial Command
  sCmd.addCommand("LED",   led); //Khi có lệnh LED thì sẽ thực thi hàm led  
  sCmd.addCommand("RAIN",  rain_detect);//Khi có lệnh RAIN thì sẽ thực thi hàm rain để kiểm tra trị cảm biến mưa
  Serial.println("Da san sang nhan lenh");
}
 
 
unsigned long chuky1 = 0;
void loop() {
  //Khởi tạo một chu kỳ lệnh, chu kỳ là 5000ms
  if (millis() - chuky1 > CHU_KY_1_LA_BAO_NHIEU) {
    chuky1 = millis();
    rain_detect();
  }
  sCmd.readSerial();
  //Bạn không cần phải thêm bất kỳ dòng code nào trong hàm loop này cả
}
 
// hàm led_red sẽ được thực thi khi gửi hàm LED_RED
void led() {
  Serial.println("LED");
  char *json = sCmd.next(); //Chỉ cần một dòng này để đọc tham số nhận đươc
  Serial.println(json);
  StaticJsonBuffer<200> jsonBuffer; //tạo Buffer json có khả năng chứa tối đa 200 ký tự
  JsonObject& root = jsonBuffer.parseObject(json);//đặt một biến root mang kiểu json
 
  int redStatus = root["led"][0];//json -> tham số root --> phần tử thứ 0. Đừng lo lắng nếu bạn không có phần tử này, không có bị lỗi đâu!
  int blueStatus = root["led"][1];//json -> tham số root --> phần tử thứ 0. Đừng lo lắng nếu bạn không có phần tử này, không có bị lỗi đâu!
 
  //kiểm thử giá trị
  Serial.print(F("redStatus "));
  Serial.println(redStatus);
  Serial.print(F("blueStatus "));
  Serial.println(blueStatus);
 
  StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root2 = jsonBuffer2.createObject();
  root2["redStatus"] = redStatus;
  root2["blueStatus"] = blueStatus;
 
  //Tạo một mảng trong JSON
  JsonArray& data = root2.createNestedArray("data");
  data.add(redStatus); 
  data.add(blueStatus);
  
  
  //in ra cổng software serial để ESP8266 nhận
  mySerial.print("LED_STATUS");   //gửi tên lệnh
  mySerial.print('\r');           // gửi \r
  root2.printTo(mySerial);        //gửi chuỗi JSON
  mySerial.print('\r');           // gửi \r
 
  //in ra Serial để debug
  root2.printTo(Serial); //Xuống dòng
  
  //xuất ra màn hình
  digitalWrite(red, redStatus);
  digitalWrite(blue, blueStatus);
}
 
void rain_detect() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  //đọc giá trị cảm biến rồi in ra root
  root["digital"] = digitalRead(rainSensor);
  root["message"] = digitalRead(rainSensor) ? "Khong mua" : "Co mua ne ahihi";
 
 
  //Gửi đi hoy!
  //in ra cổng software serial để ESP8266 nhận
  mySerial.print("RAIN");   //gửi tên lệnh
  mySerial.print('\r');           // gửi \r
  root.printTo(mySerial);        //gửi chuỗi JSON
  mySerial.print('\r'); 
}
