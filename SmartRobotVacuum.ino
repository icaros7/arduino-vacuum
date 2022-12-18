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
#define IR_RX 13    // IR 센서 PWM 컨트롤 핀
#define MAX_PWM 125 // 전역 모터 속도 제어 값

int Dir1Pin_m0 = 38; // 왼쪽 모터 in1
int Dir2Pin_m0 = 39; // 왼쪽 모터 in2
int SpeedPin_m0 = 12;// 왼쪽 모터 enable & PWM 컨트롤

int Dir1Pin_m1 = 42; // 오른쪽 모터 in1
int Dir2Pin_m1 = 43; // 오른쪽 모터 in2
int SpeedPin_m1 = 11;// 오른쪽 모터 enable & PWM 컨트롤

int Dir1Pin_m2 = 46; // 청소 모터 in1
int Dir2Pin_m2 = 47; // 청소 모터 in2
int SpeedPin_m2 = 10;// 청소 모터 enable & PWM 컨트롤

int btn;    // IR 리모콘 데이터 저장 변수
int offset = 0;
long dis[3];// 초음파 센서 데이터 저장용 배열
long time, cur;
char cmd;   // BT Serial 통신 텍스트 저장 변수
bool mov;   // 마지막 진행 방향 저장 변수 (0: 후진, 1: 전진)
bool mt_mode = true;  // Turn on manual mode on boot-up

IRrecv irrecv(IR_RX); // IR 객체

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);  // 메가 2506의 BT 시리얼 통신을 위해 Serial3 (14, 15번 핀) 사용
  Serial.println("INFO: Call setup()");

  time = millis();  // Record boot-up millis for management sensing offset

  // IR rx init
  IrReceiver.begin(IR_RX);

  // Motor pinMode init
  pinMode(Dir1Pin_m0, OUTPUT);
  pinMode(Dir2Pin_m0, OUTPUT);
  pinMode(SpeedPin_m0, OUTPUT);
  pinMode(Dir1Pin_m1, OUTPUT);
  pinMode(Dir2Pin_m1, OUTPUT);
  pinMode(SpeedPin_m1, OUTPUT);
  pinMode(Dir1Pin_m2, OUTPUT);
  pinMode(Dir2Pin_m2, OUTPUT);
  pinMode(SpeedPin_m2, OUTPUT);

  // ultrasonic sensor pinMode init
  pinMode(US0_TRI, OUTPUT);
  pinMode(US0_ECH, INPUT);
  pinMode(US1_TRI, OUTPUT);
  pinMode(US1_ECH, INPUT);
  pinMode(US2_TRI, OUTPUT);
  pinMode(US2_ECH, INPUT);

  // Vacuum motor init
  digitalWrite(Dir1Pin_m2, LOW);
  digitalWrite(Dir2Pin_m2, HIGH);
  vacuumOnOff(true);  // Turn on vacuum at boot-on
}

/* 블루투스를 통해 텍스트 읽어오기 */
void btCmdIn() {
  if (Serial3.available()) {  // 통신이 가능한 상태면
    cmd = Serial3.read();     // 블루투스 TX로부터 데이터를 받아와 global var cmd에 저장

    Serial.println(cmd);      // 로깅용 Serial 모니터 출력
  }
}

/* IR 리모콘을 통해 값 읽어오기 */
void irRx() {
  if (IrReceiver.decode()) {  // IR로부터 데이터를 읽어와 객체 내에 저장
    IrReceiver.resume();      // 다음 값 수신 준비
    btn = IrReceiver.decodedIRData.command; // global var btn 에 버튼 고유값 가져오기

    Serial.println(btn);
  }
}

/* 차량 전진 메서드 */
void forward(int temp) {
  if( temp == 1) { stop(); }  // 매개변수로 1이 넘어오면 완전 정지 후 출발

  digitalWrite(Dir1Pin_m0, HIGH);
  digitalWrite(Dir2Pin_m0, LOW);
  analogWrite(SpeedPin_m0, MAX_PWM);  // SpeedPin은 PWM 컨트롤 해야하므로 analogWrite 사용
  digitalWrite(Dir1Pin_m1, HIGH);
  digitalWrite(Dir2Pin_m1, LOW);
  analogWrite(SpeedPin_m1, MAX_PWM);
}

/* 차량 좌회전 메서드 */
void left(void) {
  digitalWrite(Dir1Pin_m0, LOW);  // left side 역회전
  digitalWrite(Dir2Pin_m0, HIGH);
  analogWrite(SpeedPin_m0, MAX_PWM);
  digitalWrite(Dir1Pin_m1, HIGH); // right side 정회전
  digitalWrite(Dir2Pin_m1, LOW);
  analogWrite(SpeedPin_m1, MAX_PWM);
}

/* 차량 우회전 메서드 */
void right(void) {
  digitalWrite(Dir1Pin_m0, HIGH); // left side 정회전
  digitalWrite(Dir2Pin_m0, LOW);
  analogWrite(SpeedPin_m0, MAX_PWM);
  digitalWrite(Dir1Pin_m1, LOW);  // right side 역회전
  digitalWrite(Dir2Pin_m1, HIGH);
  analogWrite(SpeedPin_m1, MAX_PWM);
}

/* 차량 후진 메서드 */
void reverse(int temp) {
  if( temp == 1) { stop(); }  // 매개변수로 1이 넘어오면 완전 정지 후 출발

  digitalWrite(Dir1Pin_m0, LOW);  // left side 역회전
  digitalWrite(Dir2Pin_m0, HIGH);
  analogWrite(SpeedPin_m0, MAX_PWM);
  digitalWrite(Dir1Pin_m1, LOW);  // right side 역회전
  digitalWrite(Dir2Pin_m1, HIGH);
  analogWrite(SpeedPin_m1, MAX_PWM);
}

/* 차량 정지 메서드 */
void stop() {
  analogWrite(SpeedPin_m0, 0);
  analogWrite(SpeedPin_m1, 0);
}

/* 초음파 센서 값 읽기 메서드 */
void ultrasonic() {
  // 초음파 센서를 초기화 하는 과정 US0
  digitalWrite(US0_TRI, LOW);
  delayMicroseconds(2);
  digitalWrite(US0_TRI, HIGH);
  delayMicroseconds(10);
  digitalWrite(US0_TRI, LOW);
  dis[0] = pulseIn(US0_ECH, HIGH) / 58.2;

  // 초음파 센서를 초기화 하는 과정 US1
  digitalWrite(US1_TRI, LOW);
  delayMicroseconds(2);
  digitalWrite(US1_TRI, HIGH);
  delayMicroseconds(10);
  digitalWrite(US1_TRI, LOW);
  dis[1] = pulseIn(US1_ECH, HIGH) / 58.2;

  // 초음파 센서를 초기화 하는 과정 US2
  digitalWrite(US2_TRI, LOW);
  delayMicroseconds(2);
  digitalWrite(US2_TRI, HIGH);
  delayMicroseconds(10);
  digitalWrite(US2_TRI, LOW);
  dis[2] = pulseIn(US2_ECH, HIGH) / 58.2;

  Serial.print(dis[0]);
  Serial.print(", ");
  Serial.print(dis[1]);
  Serial.print(", ");
  Serial.println(dis[2]);
}

/* 물걸래 청소기 작동 컨트롤 */
void vacuumOnOff(bool state) {
  if (state) { analogWrite(SpeedPin_m2, 255); }
  else { analogWrite(SpeedPin_m2, 0); }
}

/* AI 모드 시 센서를 통한 방향 판단 */
void scanWay() {
  if (dis[0] < 10 + offset && dis[1] < 10 + offset) {	// If left and right side both face obstacle
    if (dis[2] > 50) {			  // If rear side not face obstacle
      reverse(1);
      delay(2000);
      mov = false;
    }
    else if (dis[0] < 10 + offset) { right(); }
    else if (dis[1] < 10 + offset) { left(); }
    else {				 // Else go straight and try to overpass obstacle
      forward(1);
      mov = true;
    }
  }
  else if (dis[2] < 7 && dis[1] < 10 + offset) { left(); }
  else if (dis[2] < 7 && dis[0] < 10 + offset) { right(); }
  else if (dis[2] < 7 + offset) {
    left();
    forward(1);
    mov = true;
  }
  else if (dis[0] < 10 + offset) { right(); }
  else if (dis[1] < 10 + offset) { left(); }
  else {
    forward(1);
    mov = true;
  }
}

/* 통합 원격 조정 메서드 */
void hybridRC() {
  if (cmd == 'D' || btn == 90) { right(); }	// Turn right via BT 'D' or IR num_6
  if (cmd == 'C' || btn == 8) { left(); }		// Turn left via BT 'C' or IR num_3
  if (cmd == 'S' || btn == 28) { stop(); }  // Set pwmSpeed 0 via BT 'S' or IR num_5
  if (cmd == 'A' || btn == 24) {            // Chnage direction to forward via BT 'A' or IR num_2
    if (!mov) { forward(1); }
    else { forward(0); }  // If last direction is reverse then set pwmSpeed 0 and change direction
    mov = true;			      // Set last direction to forward
  }
  if (cmd == 'B' || btn == 82) {// Change direction to rear via BT 'B' or IR num_8
    if (mov) { reverse(1); }    // If last direction is reverse then set pwmSpeed 0 and change direction
    else { reverse(0); }
    mov = false;		            // Set last direction to reverse
  }
}

void loop() {
  cur = millis(); // Set now time
  if (cur - time > 180000) { offset += 5; } // Add ultrasonic sensing offset to 3cm per 3 minutes

  btCmdIn();      // Call Bluetooth command
  irRx();         // Call IR command
  ultrasonic();   // Scanning Ultrasonic sensor data to global var

  if (cmd == 'R' || btn == 66) {  // Switch to AI mode via BT 'R' or IR num_1
    mt_mode = false;
    btn = 0;      // Prevent recursive call
    cmd = 'e';
  }
  if (cmd == 'M' || btn == 12) {  // Switch to manual mode via BT 'M" or IR num_7
    mt_mode = true;
    btn = 0;
    cmd = 'e';
  }
  if (cmd == 'O' || btn == 94) {	// Turn on vacuum via BT 'O' or IR num_3
    vacuumOnOff(true);
    btn = 0;
    cmd = 'e';
  }
  if (cmd == 'F' || btn == 74) {	// Turn off vacuum via BT 'F' or IR num_9
    vacuumOnOff(false);
    btn = 0;
    cmd = 'e';
  }

  if (mt_mode) { hybridRC(); }  // 수동 조작 모드면
  else { scanWay(); }           // AI 모드라면
}
