/*
    Tuan Nguyen (http://www.iotsharing.com) or (nha.tuan84@gmail.com)
*/
#include <Arduino.h>
#include "EspLed7SegNDigit.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define numBehindDot 4
#define FORMAT  "%." STR(numBehindDot) "f"

EspLed7SegNDigit::EspLed7SegNDigit(int SCLK, int RCLK, int DIO, int numOfDigits):numOfDigits(numOfDigits){
    this->SCLK = SCLK; //pulse
    this->RCLK = RCLK; //latch
    this->DIO = DIO;  //data
    pinMode(SCLK, OUTPUT);
    pinMode(RCLK, OUTPUT);
    pinMode(DIO, OUTPUT);
    data = new int *[numOfDigits];
    for(int i=0; i<numOfDigits; i++){
        data[i] = new int[2];
        data[i][0] = -1;
    }
    reFreshInMs = 400;
    this->updateCb = NULL;
}
EspLed7SegNDigit::~EspLed7SegNDigit(){
    for(int i=0; i<numOfDigits; i++){
        delete(data[i]);
    }
    delete(data);
}
//trigger to transfer data to IC74HC595 
void EspLed7SegNDigit::pulse(){
  digitalWrite(SCLK, LOW);
  digitalWrite(SCLK, HIGH);
}

//latch data  
void EspLed7SegNDigit::latch(void){
  digitalWrite(RCLK, LOW);
  digitalWrite(RCLK, HIGH);
}

//send data to DIO pin  
void EspLed7SegNDigit::high(void){
  digitalWrite(DIO, HIGH);
} 
void EspLed7SegNDigit::low(void){
  digitalWrite(DIO, LOW);
}
void EspLed7SegNDigit::clearDisplay(void){
    for(int i=0; i<numOfDigits; i++){
        data[i][0] = -1;
    }
}
int EspLed7SegNDigit::mapToSeg(unsigned char chr){
    switch(chr){
        case '0':
            return 0;
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case '0.':
            return 10;
        case '1.':
            return 11;
        case '2.':
            return 12;
        case '3.':
            return 13;
        case '4.':
            return 14;
        case '5.':
            return 15;
        case '6.':
            return 16;
        case '7.':
            return 17;
        case '8.':
            return 18;
        case '9.':
            return 19; 
        case '-':
            return 20;
        case 'a':
        case 'A':
            return 21;
        case 'b':
        case 'B':
            return 22;
        case 'c':
        case 'C':
            return 23;
        case 'd':
        case 'D':
            return 24;
        case 'e':
        case 'E':
            return 25;
        case 'f':
        case 'F':
            return 26;
        case 'g':
        case 'G':
            return 27;
        case 'h':
        case 'H':
            return 28;
        case 'i':
        case 'I':
            return 29;
        case 'j':
        case 'J':
            return 30;
        case 'k':
        case 'K':
            return 31;
        case 'l':
        case 'L':
            return 32;
        case 'm':
        case 'M':
            return 33;
        case 'n':
        case 'N':
            return 34;
        case 'o':
        case 'O':
            return 35;
        case 'p':
        case 'P':
            return 36;
        case 'q':
        case 'Q':
            return 37;
        case 'r':
        case 'R':
            return 38;
        case 's':
        case 'S':
            return 39;
        case 't':
        case 'T':
            return 40;
        case 'u':
        case 'U':
            return 41;
        case 'v':
        case 'V':
            return 42;
        case 'w':
        case 'W':
            return 43;
        case 'x':
        case 'X':
            return 44;
        case 'y':
        case 'Y':
            return 45;
        case 'z':
        case 'Z':
            return 46;
        default:
            return 47;
    }
}

int EspLed7SegNDigit::setCharAt(int pos, unsigned char chr){
    if(pos <= numOfDigits) {
        data[pos-1][0] = seg7[mapToSeg(chr)];
        data[pos-1][1] = led[pos-1];
    }
}
int EspLed7SegNDigit::setRefreshTime(int ms){
    this->reFreshInMs = ms;
}

void EspLed7SegNDigit::displayNum(float number, int nrBehindDot){
    int pos = 0;
    char newFm[8];
    memset(newFm, 0, 8);
    if(nrBehindDot > numOfDigits-2) {
        nrBehindDot = 4;
    }
    newFm[0] = '%';
    newFm[1] = '.';
    sprintf(&newFm[2], "%df", nrBehindDot);
    char buf[numOfDigits+nrBehindDot+2];
    sprintf(buf, newFm, number);

    int len = strlen(buf);  

    for(int i=len-1; i>=0; i--){
        if(buf[i] == '0'){
            buf[i] = 0;
        } else {
            if((i > 0) && (buf[i] == '.')){
                buf[i] = 0;
            } 
            break;
        }       
    } 
    len = strlen(buf); 
    boolean dot = 0;
    for(int i=len-1; i>=0; i--){
        if(buf[i] == '.'){
            dot = 1;
            continue;
        } else {
            int idx = mapToSeg(buf[i]);
            if(dot == 1){
                dot = 0;
                //9 is index distance between 0 and 0. in seg7
                data[pos][0] = seg7[9 + idx];
            } else {
                data[pos][0] = seg7[idx];
            }
            data[pos][1] = led[pos];
            pos++;
        }
    }
}
void EspLed7SegNDigit::setUpdateCb(updateDisplay cb){
    this->updateCb = cb;
}

void EspLed7SegNDigit::scan_led(void){
    for(int k=0; k<numOfDigits; k++){
        if(data[k][0] != -1){
            for(int i=0; i<2; i++){
                //shift each bit of a byte to DIO
                int tmp = data[k][i];
                for (int j=0; j<8; j++){
                  if(tmp & 128){
                    high();
                  } else {
                    low();  
                  }   
                  pulse(); 
                  tmp = tmp << 1;
                }
            }
            latch();
            delayMicroseconds(40);
        } 
    }
}
void EspLed7SegNDigit::loop(void){
    scan_led();
    long now = millis();
    if (now - lastTick > reFreshInMs) {
        lastTick = now;
        //callback
        if(this->updateCb != NULL){
            this->updateCb();
        }
    }

}
