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

int btn;    	     // IR 리모콘 데이터 저장 변수
int Dir1Pin_m0 = 38; // 왼쪽 모터 in1
int Dir2Pin_m0 = 39; // 왼쪽 모터 in2
int SpeedPin_m0 = 12;// 왼쪽 모터 enable & PWM 컨트롤

int Dir1Pin_m1 = 40; // 오른쪽 모터 in1
int Dir2Pin_m1 = 41; // 오른쪽 모터 in2
int SpeedPin_m1 = 11;// 오른쪽 모터 enable & PWM 컨트롤

int Dir1Pin_m2 = 46; // 청소 모터 in1
int Dir2Pin_m2 = 47; // 청소 모터 in2
int SpeedPin_m2 = 10;// 청소 모터 enable & PWM 컨트롤

long dis[3];// 초음파 센서 데이터 저장용 배열
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
void btCmdIn() {
  if (Serial3.available()) {  // 통신이 가능한 상태면
    cmd = Serial3.read();     // 블루투스 TX로부터 데이터를 받아와 cmd에 저장

    Serial.println(cmd);      // 로깅용 Serial 모니터 출력

  }
}

/* IR 리모콘을 통해 값 읽어오기 */
void irRx() {
  if (IrReceiver.decode()) {  // IR로부터 데이터를 읽어와 객체 내에 저장
    IrReceiver.resume();      // 다음 값 수신 준비
    btn = IrReceiver.decodedIRData.command; // temp에 버튼 고유값 가져오기

    Serial.println(temp);
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
  dis[0] = pulseIn(US0_ECH, HIGH) / 58.2;

  //초음파 센서를 초기화 하는 과정 US1
  digitalWrite(US1_TRI, LOW);
  delayMicroseconds(2);
  digitalWrite(US1_TRI, HIGH);
  delayMicroseconds(10);
  digitalWrite(US1_TRI, LOW);
  dis[1] = pulseIn(US1_ECH, HIGH) / 58.2;

  //초음파 센서를 초기화 하는 과정 US2
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
  btCmdIn();	// Call Bluetooth command
  irRx();		// Call IR command

  ultrasonic();		// Scanning Ultrasonic sensor data to global var

  if(dis[0] < 5 && dis[1] < 5) {	// If left and right side both face obstacle
    if (dis[2] > 15) {			  // If rear side not face obstacle
      reverse();
      delay(3000);
      stop();
      mov = false;
      if (dis[0] < 5) { right(); }	  // If left side not face obstacle
      else if (dis[1] < 5) { left(); }   // Else if right side not face obstacle
      else {				 // Else go straight and try to overpass obstacle
	forward(1);
	mov = true;
      }
    }
  }
  else if (dis[0] < 5) { left(); }
  else if (dis[1] < 5) { right(); }

  if (cmd == 'O' || btn == 94) { vacuumOnOff(true); }	// Turn on vacuum via BT 'O' or IR num_3
  if (cmd == 'F' || btn == 74) { vacuumOnOff(false); }	// Turn off vacuum via BT 'F' or IR num_9
  if (cmd == 'D' || btn == 90) { right(); }		// Turn right via BT 'D' or IR num_6
  if (cmd == 'C' || btn == 8) { left(); }		// Turn left via BT 'C' or IR num_3
  if (cmd == 'S' || btn == 28) { stop(); }		// Set pwmSpeed 0 via BT 'S' or IR num_5
  if (cmd == 'A' || btn == 24) {// Chnage direction to forward via BT 'A' or IR num_2
    if (!mov) { forward(1); }	// If last direction is reverse then set pwmSpeed 0 and change direction
    else { forward(0); }

    mov = true;			// Set last direction to forward
  }
  if (cmd == 'B' || btn == 82) {// Change direction to rear via BT 'B' or IR num_8
    if (mov) { reverse(1); }	// If last direction is reverse then set pwmSpeed 0 and change direction
    else { reverse(0); }

    mov = false;		// Set last direction to reverse
  }
}
