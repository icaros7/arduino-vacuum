/*
 * Embedded project
 * Remote control robot vaccum
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

#define IR_RX 29
#define BT_RX 30   
#define BT_TX 31   
#define US0_TRI 23 
#define US0_ECH 22 
#define US1_TRI 25 
#define US1_ECH 24 
#define US2_TRI 27 
#define US2_ECH 26 
#define LCD_0 32  
#define LCD_1 34  
#define LCD_2 35   
#define LCD_3 36   
#define LCD_4 37   
#define LCD_5 39   

LiquidCrystal lcd(LCD_0, LCD_1, LCD_2, LCD_3, LCD_4, LCD_5);
IRrecv irrecv(IR_RX);
SoftwareSerial BTSerial(BT_RX, BT_TX);

// MT0: 10(E), 9(IN1), 8(IN2)
// MT1: 13(E), 12(IN3), 11(IN4)
// MT2: 7(E), 6(IN1), 5(IN2) 

int MT0[6] = {42, 44, 46, 48, 4, 5};  // TODO: Duplicate pin number
int MT1[6] = {42, 44, 46, 48, 4, 5};  // TODO: Duplicate pin number
int MT2[6] = {42, 44, 46, 48, 4, 5};  // TODO: Duplicate pin number
bool mt0 = true, mt1 = true;
char cmd;

/**
 * Control left side DC motor
 * @param da Data value
 */
void mt0Ctrl(unsigned char da) {
  for (int i = 0; i < 4; i++) {
    if (da & 0x08) { digitalWrite(MT0[i], HIGH); }
    else { digitalWrite(MT0[i], LOW); }
    da *= 2;
  }
}

/**
 * Control right side DC motor
 * @param da Data value
 */
void mt1Ctrl(unsigned char da) {
  for (int i = 0; i < 4; i++) {
    if (da & 0x08) { digitalWrite(MT1[i], HIGH); }
    else { digitalWrite(MT1[i], LOW); }
    da *= 2;
  }
}

/**
 * Control left side DC motor output of power
 * @param spe Speed value
 */
void mt0PwmCtrl(unsigned char spe) {
  Serial.print("INFO: Change Motor0 PWM = ");
  Serial.println(spe);

  analogWrite(MT0[4], spe);
  analogWrite(MT0[5], spe);
}

/**
 * Control right side DC motor output of power
 * @param spe Speed value
 */
void mt1PwmCtrl(unsigned char spe) {
  Serial.print("INFO: Change Motor1 PWM = ");
  Serial.println(spe);

  analogWrite(MT1[4], spe);
  analogWrite(MT1[5], spe);
}

/**
 * Change motor working direction
 * @param mt 0 = left, 1 = right
 */
void mtRotCtrl(int mt) {
  Serial.print("INFO: Motor rotation control = ");

  if (mt == 0) {      // Left side DC motor control
    if (!mt0) {
      Serial.println("Motor0 to forward");

      mt0PwmCtrl(0);  // Stop
      mt0Ctrl(5);     // Forward rotation
      mt0PwmCtrl(128); // set speed 128
      mt0 = true;
    }
    else {
      Serial.println("Motor0 to reverse");
      
      mt0PwmCtrl(0);  // Stop
      mt0Ctrl(10);    // Reverse rotation
      mt0PwmCtrl(128); //set speed max(128)
      mt0 = false;
    }
  }

  
  else if (mt == 1) { // Right side DC motor control
    if (!mt1) {
      Serial.println("Motor1 to forward");

      mt1PwmCtrl(0);  // Stop
      mt1Ctrl(5);     // Forward rotation
      mt1 = true;
    }
    else {
      Serial.println("Motor1 to reverse");

      mt1PwmCtrl(0);  // Stop
      mt1Ctrl(10);    // Reverse rotation
      mt1 = false;
  }
  else {
    Serial.println("ERROR");
    Serial.println("ERROR: mtRotCtrl() invaild motor number");
    }
}

/**
 * Scanning distance via ultrasonic sensor
 * return US0, US1, US2 distance array
 */
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

/**
 * Print char array on Crystal LCD
 * @param str String what you want to print on LCD
 */
void printLcd(char[] str) {
  lcd.print(str);
  Serial.print("INFO: Print on LCD = ");
  Serial.println(str);
}

/**
 * Read command from Bluetooth
 * return Success or not
 */
bool btCmdIn() {
  if (BTSerial.available()) {
    cmd = (char) BTSerial.read();
    Serial.print("INFO: BT command = ");
    Serial.println(cmd);

    return true;
  }

  return false;
}

void setup() {
  Serial.begin(9600);

  // DC Motor init
  for (int i = 0; i < 6; i++) {
    pinMode(MT0[i], OUTPUT);
    pinMode(MT1[i], OUTPUT);
  }

  // Ultrasonic sensor init
  pinMode(US0_TRI, OUTPUT);
  pinMode(US0_ECH, INPUT);
  pinMode(US1_TRI, OUTPUT);
  pinMode(US1_ECH, INPUT);
  pinMode(US2_TRI, OUTPUT);
  pinMode(US2_ECH, INPUT);

  // CrystalLCD init
  lcd.begin(16, 2);

  // IR RX init
  irrecv.enableIRIn();
  irrecv.blink13(true);

  // BT init
  BTSerial.begin(9600);
}

void loop() { 
// 왼쪽: 시계 오른쪽: 반시계 오른쪽회전
// 오른쪽 : 시계 왼쪽: 반시계 왼쪽 회전

if( ultrasonic < 0 ) {
  mt0PwmCtrl(0);
  mt1Ctrl(5);
  mt0Ctrl(5);
  mt0PwmCtrl(128);
  mt1PwmCtrl(128);
}
else if (ultrasonic < 300) //TODO: Duplicate distance
{
  
}


  // 초음파센서가 왼쪽에 장애물이 있다고 판단했을때..를 어떻게...?
  // 오른쪽 회전
  digitalWrite(mt0Ctrl,HIGH); //시계 방향 세팅
  digitalWrite(mt1Ctrl,LOW); //시계 방향 세팅
  
  // 초음파 오른쪽 장애물 판단 시
  // 왼쪽 회전
  digitalWrite(mt0Ctrl,LOW); //반시계 방향 세팅
  digitalWrtie(mt1Ctrl,HIGH); //반시계 방향 세팅

  //초음파 센서 양쪽 다 장애물 판단 시
  //정지
  digitalWrite(mt0Ctrl,LOW);
  digitalWrtie(mt1Ctrl,LOW);
  delay(2000);
  
  mtRotCtrl(0);
  mtRotCtrl(1);

  long [] ultra = ultrasonic();
  printLcd(ultrasonic);
}
