#include "EspLed7SegNDigit.h"

int SCLK = 14; //pulse
int RCLK = 27; //latch
int DIO = 12;  //data

float num = 0.12;

EspLed7SegNDigit ledm(SCLK, RCLK, DIO, 8);

void updateDisplayCb(void){
  ledm.clearDisplay();
  ledm.setCharAt(8, 'd');
  ledm.setCharAt(7, 's');
  ledm.setCharAt(6, 't');
  num = num + 1;
  ledm.displayNum(num);
}

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  ledm.setUpdateCb(updateDisplayCb);
  ledm.setCharAt(8, 'd');
  ledm.setCharAt(7, 's');
  ledm.setCharAt(6, 't');
}

void loop() {
  // put your main code here, to run repeatedly:
  ledm.loop();
}
