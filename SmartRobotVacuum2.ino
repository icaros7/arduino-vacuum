// MT0: 10(E), 9(IN1), 8(IN2)
// MT1: 13(E), 12(IN3), 11(IN4)
// MT2: 7(E), 6(IN1), 5(IN2) 

//메소드 써서 loop 
//m0 = left side , m1 = right side

int Dir1Pin_m0 = 9; // in1
int Dir2Pin_m0 = 8; // in2
int SpeedPin_m0 = 10; // enable

int Dir1Pin_m1 = 12;
int Dir2Pin_m1 = 11;
int SpeedPin_m1 = 13;

int Dir1Pin_m2 = 6; 
int Dir2Pin_m2 = 5;
int SpeedPin_m2 = 7;

void setup() {
    pinMode(Dir1Pin_m0, OUTPUT);
    pinMode(Dir2Pin_m0, OUTPUT);
    pinMode(SpeedPin_m0, OUTPUT);
    pinMode(Dir1Pin_m1, OUTPUT);
    pinMode(Dir2Pin_m1, OUTPUT);
    pinMode(SpeedPin_m1, OUTPUT);
    pinMode(Dir1Pin_m2, OUTPUT);
    pinMode(Dir2Pin_m2, OUTPUT);
    pinMode(SpeedPin_m2, OUTPUT);
}

//정방향 메소드
void forward() {
    digitalWrite(Dir1Pin_m0,HIGH);
    digitalWrite(Dir1Pin_m0,LOW);
    analogWrite(SpeedPin_m0,128);
    digitalWrite(Dir1Pin_m1, HIGH);
    digitalWrite(Dir1Pin_m1,LOW);
    analogWrite(SpeedPin_m1,128);
}

//좌회전 메소드
void left() {
    digitalWrite(Dir1Pin_m0,LOW); //left side 역회전
    digitalWrite(Dir1Pin_m0,HIGH);
    analogWrite(SpeedPin_m0,80);
    digitalWrite(Dir1Pin_m1,HIGH); //right side 정회전
    digitalWrite(Dir1Pin_m1,LOW);
    analogWrite(SpeedPin_m1,51);
}
//우회전 메소드
void right() {
    digitalWrite(Dir1Pin_m0,HIGH); //left side 정회전
    digitalWrite(Dir1Pin_m0,LOW);
    analogWrite(SpeedPin_m0,80); 
    digitalWrite(Dir1Pin_m1,LOW); //right side 역회전 
    digitalWrite(Dir1Pin_m1,HIGH);
    analogWrite(SpeedPin_m1,51);

}
//역방향 메소드(후진)
void reverse() {
    digitalWrite(Dir1Pin_m0,LOW); //left side 역회전
    digitalWrite(Dir1Pin_m0,HIGH);
    analogWrite(SpeedPin_m0,128);
    digitalWrite(Dir1Pin_m1,LOW);// right side 역회전
    digitalWrite(Dir1Pin_m1,HIGH);
    analogWrite(SpeedPin_m1,128);
}

long[] ultrasonic() {
  Serial.println("INFO: Scanning Ultrasonic sensor");

  digitalWrite(US0_TRI, LOW);
  digitalWrite(US1_TRI, LOW);
  digitalWrite(US2_TRI, LOW);
  delayMicroseconds(2);
  digitalWrite(US0_TRI, HIGH);
  digitalWrite(US1_TRI, HIGH);
  digitalWrite(US2_TRI, HIGH);
  delayMicroseconds(10);
  digitalWrite(US0_TRI, LOW);
  digitalWrite(US1_TRI, LOW);
  digitalWrite(US2_TRI, LOW);

  long[3] temp = { pulseIn(US0_ECH, HIGH) / 58.2, pulseIn(US1_ECH, HIGH) / 58.2, pulseIn(US2_ECH, HIGH) / 58.2 };

  Serial.print("INFO: Ultrasonic scan data = ");
  Serial.println(temp);

  return temp;
}

void loop() {
    // 주변에 아무것도 없을 경우
    if( ultrasonoic < 0) {
        forward
    } 

}


