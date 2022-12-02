// MT0: 10(E), 9(IN1), 8(IN2)
// MT1: 13(E), 12(IN3), 11(IN4)
// MT2: 7(E), 6(IN1), 5(IN2) 

//메소드 써서 loop 
//m0 = left side , m1 = right side 
// US0 = left ,US1 = right, US2 = rear

int Dir1Pin_m0 = 9; // in1
int Dir2Pin_m0 = 8; // in2
int SpeedPin_m0 = 10; // enable

int Dir1Pin_m1 = 12; //in1 
int Dir2Pin_m1 = 11; //in2
int SpeedPin_m1 = 13; //enable

int Dir1Pin_m2 = 6; //in1
int Dir2Pin_m2 = 5; //in2
int SpeedPin_m2 = 7; //enable

int US0_TRI = 23; //senor1
int US0_ECH = 22; 
int US1_TRI = 25; //senor2
int US1_ECH = 24;
int US2_TRI = 27; //senor3
int US2_ECH = 26;

long duration1, distance1; 
long duration2, distance2;
long duration3, distance3;

void setup() {
    //motor pinMode
    pinMode(Dir1Pin_m0, OUTPUT);
    pinMode(Dir2Pin_m0, OUTPUT);
    pinMode(SpeedPin_m0, OUTPUT);
    pinMode(Dir1Pin_m1, OUTPUT);
    pinMode(Dir2Pin_m1, OUTPUT);
    pinMode(SpeedPin_m1, OUTPUT);
    pinMode(Dir1Pin_m2, OUTPUT);
    pinMode(Dir2Pin_m2, OUTPUT);
    pinMode(SpeedPin_m2, OUTPUT);
    // senor pinMode
    pinMode(US0_TRI, OUTPUT);
    pinMode(US0_ECH, INPUT);
    pinMode(US1_TRI, OUTPUT);
    pinMode(US1_ECH, INPUT);
    pinMode(US2_TRI, OUTPUT);
    pinMode(US2_ECH, INPUT);

    Serial.begin(9600);

}
//마지막에 작동했던 것을 기억하는 전역변수 선언.

//정방향 메소드
void forward(int temp) {
    if( temp == 1) {
        analogWrite(SpeedPin_m0,0);
        analogWrite(SpeedPin_m1,0);
        delay(1000);
    } 
    digitalWrite(Dir1Pin_m0,HIGH);
    digitalWrite(Dir1Pin_m0,LOW);
    analogWrite(SpeedPin_m0,128);
    digitalWrite(Dir1Pin_m1, HIGH);
    digitalWrite(Dir1Pin_m1,LOW);
    analogWrite(SpeedPin_m1,128);
}

//좌회전 메소드
void left(int temp) {
    if( temp == 1) {
        analogWrite(SpeedPin_m0,0);
        analogWrite(SpeedPin_m1,0);
        delay(1000);
    }
    digitalWrite(Dir1Pin_m0,LOW); //left side 역회전
    digitalWrite(Dir1Pin_m0,HIGH);
    analogWrite(SpeedPin_m0,80);
    digitalWrite(Dir1Pin_m1,HIGH); //right side 정회전
    digitalWrite(Dir1Pin_m1,LOW);
    analogWrite(SpeedPin_m1,51);
}
//우회전 메소드
void right(int temp) {
    if( temp == 1) {
        analogWrite(SpeedPin_m0,0);
        analogWrite(SpeedPin_m1,0);
        delay(1000);
    }
    digitalWrite(Dir1Pin_m0,HIGH); //left side 정회전
    digitalWrite(Dir1Pin_m0,LOW);
    analogWrite(SpeedPin_m0,80); 
    digitalWrite(Dir1Pin_m1,LOW); //right side 역회전 
    digitalWrite(Dir1Pin_m1,HIGH);
    analogWrite(SpeedPin_m1,51);

}
//역방향 메소드(후진)
void reverse(int temp) {
    if( temp == 1) {
        analogWrite(SpeedPin_m0,0);
        analogWrite(SpeedPin_m1,0);
        delay(1000);
    }
    digitalWrite(Dir1Pin_m0,LOW); //left side 역회전
    digitalWrite(Dir1Pin_m0,HIGH);
    analogWrite(SpeedPin_m0,128);
    digitalWrite(Dir1Pin_m1,LOW);// right side 역회전
    digitalWrite(Dir1Pin_m1,HIGH);
    analogWrite(SpeedPin_m1,128);
}

long[] ultrasonic() { //센서 별로 달라야하지않나?
    Serial.println("INFO: Scanning Ultrasonic sensor");
    
    //초음파 센서를 초기화 하는 과정 US0
    digitalWrite(US0_TRI, LOW);
    delayMicroseconds(2);
    digitalWrite(US0_TRI, HIGH);
    delayMicroseconds(10);
    digitalWrite(US0_TRI, LOW);
    duration1 = pulseIn(US0_ECH, HIGH); // 트리거 핀에서 나온 펄스를 받아서 
    distance1= duration1*0.034/2; // 거리를 측정합니다. 
    //초음파 센서를 초기화 하는 과정 US1
    digitalWrite(US1_TRI, LOW); 
    delayMicroseconds(2);
    digitalWrite(US1_TRI, HIGH);
    delayMicroseconds(10);
    digitalWrite(US1_TRI, LOW);
    duration2 = pulseIn(US1_ECH, HIGH); // 트리거 핀에서 나온 펄스를 받아서 
    distance2= duration1*0.034/2; // 거리를 측정합니다. 
    
    //초음파 센서를 초기화 하는 과정 US2
    digitalWrite(US2_TRI, LOW); 
    delayMicroseconds(2);
    digitalWrite(US2_TRI, HIGH);
    delayMicroseconds(10);
    digitalWrite(US2_TRI, LOW);
    duration3 = pulseIn(US2_ECH, HIGH); // 트리거 핀에서 나온 펄스를 받아서 
    distance3= duration1*0.034/2; // 거리를 측정합니다. 


    // long[3] duration = { pulseIn(US0_ECH, HIGH) / 58.2, pulseIn(US1_ECH, HIGH) / 58.2, pulseIn(US2_ECH, HIGH) / 58.2 };
}

void loop() {
    
    //센서1과 센서2 (좌측 우측 센서) 9이상이면 전진
    if( distance1 > 9 && distance2 > 9 ) {
        forward(); //전진

        Serial.print ( "Sensor1 : "); //센서 1에
        Serial.print ( distance1);// 거리 값
        Serial.println("cm"); // cm를 출력합니다.

        Serial.print ( "Sensor2 : "); //센서 2에
        Serial.print ( distance2);// 거리 값
        Serial.println("cm"); // cm를 출력합니다.
    }
    delay(2000);

    //센서1이 9보다 작고 센서2가 9보다 크다는 건 왼쪽 장애물 -> 우회전!
    else if ( distance1 < 9 && distance2 > 9) {
        right();
        
        Serial.print ( "Sensor1 : "); //센서 1에
        Serial.print ( distance1);// 거리 값
        Serial.println("cm"); // cm를 출력합니다.

        Serial.print ( "Sensor2 : "); //센서 2에
        Serial.print ( distance2);// 거리 값
        Serial.println("cm"); // cm를 출력합니다.

    }
    delay(2000);

    //오른쪽에 장애물이 있으면  좌회전!
    else if ( distance1 > 9 && distance2 < 9) {
        left();

        Serial.print ( "Sensor1 : "); //센서 1에
        Serial.print ( distance1);// 거리 값
        Serial.println("cm"); // cm를 출력합니다.

        Serial.print ( "Sensor2 : "); //센서 2에
        Serial.print ( distance2);// 거리 값
        Serial.println("cm"); // cm를 출력합니다.
    }
    delay(2000);
    
    //왼쪽과 오른쪽 senor가 둘다 9 이하이고 후방이 15이상이면 후진!!
   else if ( distance3 > 15 & distance1 < 9 && distance2 < 9) 
   {
    reverse();

    Serial.print ( "Sensor1 : "); //센서 1에
    Serial.print ( distance1);// 거리 값
    Serial.println("cm"); // cm를 출력합니다.

    Serial.print ( "Sensor2 : "); //센서 2에
    Serial.print ( distance2);// 거리 값
    Serial.println("cm"); // cm를 출력합니다.

    Serial.print ( "Sensor3 : "); //센서 3에
    Serial.print ( distance3);// 거리 값
    Serial.println("cm"); // cm를 출력합니다.
   }
   delay(2000);
}


    } 

}


