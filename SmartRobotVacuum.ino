/*
 * Embedded project
 * Remote control robot vacuum
 *
 * MT0: Left side DC Motor
 * MT1: Right side DC Motor
 * MT2: Water Vacuum Motor
 * US0: Left side ultrasonic sensor
 * US1: Right side ultrasonic sensor
 * US2: Rear side ultrasonic sensor
 *
 * 이지윤, 이호민
*/
#include <SoftwareSerial.h>
#include <IRremote.h>

#define US0_TRI 23  // 왼쪽 초음파 센서 핀
#define US0_ECH 22 
#define US1_TRI 25  // 오른쪽 초음파 센서 핀
#define US1_ECH 24
#define US2_TRI 27  // 후방 초음파 센서 핀
#define US2_ECH 26
#define IR_RX 13    // IR 센서 PWM 컨트롤 피

int Dir1Pin_m0 = 38; // 왼쪽 모터 in1
int Dir2Pin_m0 = 39; // 왼쪽 모터 in2
int SpeedPin_m0 = 12;// 왼쪽 모터 enable & PWM 컨트롤

int Dir1Pin_m1 = 40; // 오른쪽 모터 in1
int Dir2Pin_m1 = 41; // 오른쪽 모터 in2
int SpeedPin_m1 = 11;// 오른쪽 모터 enable & PWM 컨트롤

int Dir1Pin_m2 = 46; // 청소 모터 in1
int Dir2Pin_m2 = 47; // 청소 모터 in2
int SpeedPin_m2 = 10;// 청소 모터 enable & PWM 컨트롤

long dis[3];                // 초음파 센서 데이터 저장용 배열
int btn;    // IR 리모콘 데이터 저장 변수
char cmd;   // BT Serial 통신 텍스트 저장 변수
bool mov;   // 마지막 진행 방향 저장 변수 (0: 후진, 1: 전진)

IRrecv irrecv(IR_RX); // IR 객체

void setup() {
  Serial.println("INFO: Call setup()");

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
  Serial3.begin(9600);  // 메가 2506의 BT 시리얼 통신을 위해 Serial3 (14, 15번 핀) 사용
  IrReceiver.begin(IR_RX);
}

/* 블루투스를 통해 텍스트 읽어오기 */
char btCmdIn() {
  if (Serial3.available()) {  // 통신이 가능한 상태면
    cmd = Serial3.read();     // 블루투스 TX로부터 데이터를 받아와 cmd에 저장

    Serial.println(cmd);      // 로깅용 Serial 모니터 출력

    return cmd;
  }
  else { return "e"; }        // 실패시 e 반환
}

/* IR 리모콘을 통해 값 읽어오기 */
int irRx() {
  if (IrReceiver.decode()) {  // IR로부터 데이터를 읽어와 객체 내에 저장
    IrReceiver.resume();      // 다음 값 수신 준비
    int temp = IrReceiver.decodedIRData.command; // temp에 버튼 고유값 가져오기

    Serial.println(temp);

    return temp;
  }
}


/* 차량 전진 메서드 */
void forward(int temp) {
  if( temp == 1) { stop(); }

  digitalWrite(Dir1Pin_m0, HIGH);
  digitalWrite(Dir2Pin_m0, LOW);
  analogWrite(SpeedPin_m0, 255);  // SpeedPin은 PWM 컨트롤 해야하므로 analogWrite 사용
  digitalWrite(Dir1Pin_m1, HIGH);
  digitalWrite(Dir2Pin_m1, LOW);
  analogWrite(SpeedPin_m1, 255);
}

/* 차량 우회전 메서드 */
void right(void) {
  digitalWrite(Dir1Pin_m0, LOW);  // left side 역회전
  digitalWrite(Dir2Pin_m0, HIGH);
  analogWrite(SpeedPin_m0, 255);
  digitalWrite(Dir1Pin_m1, HIGH); // right side 정회전
  digitalWrite(Dir2Pin_m1, LOW);
  analogWrite(SpeedPin_m1, 255);
}

/* 차량 좌회전 메서드 */
void left(void) {
  digitalWrite(Dir1Pin_m0, HIGH); // left side 정회전
  digitalWrite(Dir2Pin_m0, LOW);
  analogWrite(SpeedPin_m0, 255);
  digitalWrite(Dir1Pin_m1, LOW);  // right side 역회전
  digitalWrite(Dir2Pin_m1, HIGH);
  analogWrite(SpeedPin_m1, 255);
}

/* 차량 후진 메서드 */
void reverse(int temp) {
  if( temp == 1) { stop(); }  // 매개변수로 1이 넘어오면 완전 정지 후 출발

  digitalWrite(Dir1Pin_m0, LOW);  // left side 역회전
  digitalWrite(Dir2Pin_m0, HIGH);
  analogWrite(SpeedPin_m0, 255);
  digitalWrite(Dir1Pin_m1, LOW);  // right side 역회전
  digitalWrite(Dir2Pin_m1, HIGH);
  analogWrite(SpeedPin_m1, 255);
}

/* 차량 정지 메서드 */
void stop() {
  analogWrite(SpeedPin_m0,0);
  analogWrite(SpeedPin_m1,0);
}

/* 초음파 센서 값 읽기 메서드 */
void ultrasonic() {
  //초음파 센서를 초기화 하는 과정 US0
  digitalWrite(US0_TRI, LOW);
  delayMicroseconds(2);
  digitalWrite(US0_TRI, HIGH);
  delayMicroseconds(10);
  digitalWrite(US0_TRI, LOW);

  //초음파 센서를 초기화 하는 과정 US1
  digitalWrite(US1_TRI, LOW);
  delayMicroseconds(2);
  digitalWrite(US1_TRI, HIGH);
  delayMicroseconds(10);
  digitalWrite(US1_TRI, LOW);

  //초음파 센서를 초기화 하는 과정 US2
  digitalWrite(US2_TRI, LOW);
  delayMicroseconds(2);
  digitalWrite(US2_TRI, HIGH);
  delayMicroseconds(10);
  digitalWrite(US2_TRI, LOW);

  // 트리커 핀에서 나온 펄스를 받아 cm 단위로 거리 계산
  dis[0] = pulseIn(US0_ECH, HIGH) / 58.2;
  dis[1] = pulseIn(US1_ECH, HIGH) / 58.2;
  dis[2] = pulseIn(US2_ECH, HIGH) / 58.2;

  Serial.print(dis[0]);
  Serial.print(", ");
  Serial.print(dis[1]);
  Serial.print(", ");
  Serial.println(dis[2]);
}

/* 물걸래 청소기 작동 컨트롤 */
void vacuumOnOff(bool state) {
  if (state) {
    digitalWrite(Dir1Pin_m2, LOW);
    digitalWrite(Dir2Pin_m2, HIGH);
    analogWrite(SpeedPin_m2, 255);
  }
  else {
    digitalWrite(Dir1Pin_m2, LOW);
    digitalWrite(Dir2Pin_m2, HIGH);
    analogWrite(SpeedPin_m2, 0);
  }
}

void loop() {
  cmd = btCmdIn();
  btn = irRx();

  if (cmd == 'O' || btn == 94) { vacuumOnOff(true); }
  if (cmd == 'F' || btn == 74) { vacuumOnOff(false); }
  if (cmd == 'D' || btn == 90) { right(); }
  if (cmd == 'C' || btn == 8) { left(); }
  if (cmd == 'S' || btn == 28) { stop(); }
  if (cmd == 'A' || btn == 24) {
    if (!mov) { forward(1); }
    else { forward(0); }

    mov = true;
  }
  if (cmd == 'B' || btn == 82) {
    if (mov) { reverse(1); }
    else { reverse(0); }

    mov = false;
  }
}
