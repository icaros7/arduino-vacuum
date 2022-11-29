/*
 * Embedded project
 * Remote control robot vaccum
 *
 * MT0: Left side DC Motor
 * MT1: Right side DC Motor
 * MT2: [TODO] Water mop DC Motor
 * US0: Left side ultrasonic sensor
 * US1: Right side ultrasonic sensor
 * US2: Rear side ultrasonic sensor
 *
 * 이지윤, 이호민
 *
 * TODO: If no more has pin, then do not use LCD
*/
#include <LiquidCrystal.h>
#include <IRremote.h>
#include <SoftwareSerial.h>

#define IR_RX 11  // TODO: Duplicate pin number
#define BT_RX 2   // TODO: Duplicate pin number
#define BT_TX 3   // TODO: Duplicate pin number
#define US0_TRI 3 // TODO: Duplicate pin number
#define US0_ECH 2 // TODO: Duplicate pin number
#define US1_TRI 3 // TODO: Duplicate pin number
#define US1_ECH 2 // TODO: Duplicate pin number
#define US2_TRI 3 // TODO: Duplicate pin number
#define US2_ECH 2 // TODO: Duplicate pin number
#define LCD_0 12  // TODO: Duplicate pin number
#define LCD_1 11  // TODO: Duplicate pin number
#define LCD_2 5   // TODO: Duplicate pin number
#define LCD_3 4   // TODO: Duplicate pin number
#define LCD_4 3   // TODO: Duplicate pin number
#define LCD_5 2   // TODO: Duplicate pin number

LiquidCrystal lcd(LCD_0, LCD_1, LCD_2, LCD_3, LCD_4, LCD_5);
IRrecv irrecv(IR_RX);
SoftwareSerial BTSerial(BT_RX, BT_TX);

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
    if (da & 0x08) { digitalWrite(MT0[i], HIGHT); }
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
    if (da & 0x08) { digitalWrite(MT0[i], HIGHT); }
    else { digitalWrite(MT0[i], LOW); }
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
    if (mt0)) {
      Serial.println("Motor0 to forward");

      mt0PwmCtrl(0);  // Stop
      mt0Ctrl(5);     // Forward rotation
      mt0 = true;
    }
    else {
      Serial.println("Motor0 to forward");
      
      mt0PwmCtrl(0);  // Stop
      mt0Ctrl(10);    // Reverse rotation
      mt0 = false;
    }
  }
  else if (mt == 1) { // Right side DC motor control
    if (mt1)) {
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

  long[3] temp = { pulseIn(US0_ECH, IGHT) / 58.2, pulseIn(US1_ECH, IGHT) / 58.2, pulseIn(US2_ECH, IGHT) / 58.2 };

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
  
}
