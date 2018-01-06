#include <avr/wdt.h>

int button;
int ArrayData[12];
long int x = 0;
int bat = 0;
int otkl = 0;
int zaliv = 0;
int good = 0;
int opn = 0;
int cld = 0;
int chk = 0;
String cmd = "";
bool strComplete = false;
unsigned long prev = 0;

///////////////////////////////////////////////////////////////////////////////
void setup() {
  MCUSR = 0;
  wdt_disable();
  Serial.begin(115200);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(2, 1);
  digitalWrite(3, 1);
  digitalWrite(4, 1);
  delay(5000);
  wdt_enable(WDTO_4S);
}
///////////////////////////////////////////////////////////////////////////////
void loop() {
  wdt_reset();
  Buffer(); //Снимаем показания
  if (strComplete) {
    if (cmd.substring(0, 1) == "B") {
      if (cmd.substring(1, 2) == "1") { //открыть
        digitalWrite(4, 0);
        delay(200);
        digitalWrite(4, 1);
        delay(1000);
      }
      else if (cmd.substring(1, 2) == "2") { //закрыть
        digitalWrite(3, 0);
        delay(200);
        digitalWrite(3, 1);
        delay(1000);
      }
      else if (cmd.substring(1, 2) == "3") { // отключить датчики на 60 минут
        digitalWrite(2, 0);
        delay(200);
        digitalWrite(2, 1);
        delay(1000);
      }
    }
    cmd = "";
    strComplete = false;
  }

  if (millis() - 2000 > prev) {
    prev = millis();
    for (int i = 0; i < 12; i++) {
      Serial.print(ArrayData[i]);
      if (i != 11) {
        Serial.print(';');
      }
    }
    Serial.print('\n');
  }
}

///////////////////////////////////////////////////////////////////////////////
void Buffer() {
  bat = analogRead(A1);
  cld = analogRead(A2);
  opn = analogRead(A3);
  otkl = analogRead(A4);
  good = analogRead(A5);
  zaliv = analogRead(A6);
  chk = analogRead(A7);
  if (bat > 100) {
    ArrayData[0] = 1;
  }   else ArrayData[0] = 0; //Батарея
  if (otkl > 100) {
    ArrayData[1] = 1;
  }  else ArrayData[1] = 0; //Отключение датчиков на 60 минут
  if (zaliv > 100) {
    ArrayData[2] = 1;
  } else ArrayData[2] = 0;
  if (good > 100) {
    ArrayData[3] = 1;
  }  else ArrayData[3] = 0; //Готов
  if (chk > 100) {
    ArrayData[4] = 1;
  }   else ArrayData[4] = 0; //Неисправность
  ArrayData[5] = 0; //Неисправность 1
  ArrayData[6] = 0; //Неисправность 2
  ArrayData[7] = 0; //Неисправность 3
  ArrayData[8] = 0; //Неисправность 4
  ArrayData[9] = 0; //Неисправность 5
  if (opn > 100) {
    ArrayData[10] = 1;
  }  else ArrayData[10] = 0; //Открыто
  if (cld > 100) {
    ArrayData[11] = 1;
  }  else ArrayData[11] = 0; //Закрыто
}
///////////////////////////////////////////////////////////////////////////////
void serialEvent() {
  while (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    cmd += inChar;
    if (inChar == '\n') {
      strComplete = true;
    }
  }
}
