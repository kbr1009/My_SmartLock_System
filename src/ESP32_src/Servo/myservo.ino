#include <Servo.h>

Servo myservo; //Servoオブジェクトを作成

void setup() {
  myservo.attach(13); //13ピンにサーボ制御線（オレンジ）を接続
}

void loop() {
  myservo.write(90); //180度へ回転 
  delay(1000);
  myservo.write(0); //元に戻る
  delay(1000);
}
