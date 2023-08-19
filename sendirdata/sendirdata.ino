#include <MsTimer2.h>
#include <IRremote.hpp>

#define ADDRESS 0xA15E
#define POWER 0x26
#define SLEEPMODE 0xA6
#define MODE 0x66
#define UP 0xE6
#define DOWN 0x16
#define TIMER 0x98
#define WINDPOWER 0x56
#define WINDDIR 0xD6
#define TURBO 0x36

#define PERIODTIME 1800 //1 CYCLE = 30MINUTE

////INPUT DATA////
#define FIRST_TIME     1 * 1800
#define SECCOND_TIME  3 * 1800

uint8_t cycle = 6;//2 per 1 cycle , explain: if at cycle=4 FIRST_TIME = 1 * 1800, SECCOND_TIME = 2 * 1800, total time = fir time 2 + sec time 2 =(1*1800*2)+(2*1800*2)
//////////////////


const int IR_LED_PIN = 3;
uint32_t after_CNT = 0;
uint32_t duration_CNT = 0;
uint8_t blink = 0;
uint8_t onoff = 0;
uint8_t flag = 0;
uint8_t temp = 0;

IRsend irsend;

enum _state {    // 열거형 정의
    After = 0,         // 초깃값 할당
    Duration,
}state;

void setup() {
  MsTimer2::set(500, counterfunc);
  MsTimer2::start();

  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  irsend.begin(IR_LED_PIN); // Start with IR_SEND_PIN as send pin and enable feedback LED at default feedback LED pin
  
}

void loop() {
  
  if (Serial.available() > 0) {
    temp = Serial.read();
    if(temp == '0'){onoff = 0;}
    else if (temp == '2'){
      irsend.sendNEC(0xA15E26D9, 32);//testdone
      Serial.println("force operation on");
      onoff = 1;
      }
      else if (temp == 'c' || temp == 'C'){
      delay(50);
      Serial.println("enter input cycle value 1~9");
      delay(50);
      while(1){
        if (Serial.available() > 0) {
          cycle = Serial.read()-48;
          break;          
        }
      }
      Serial.print("cycle value is ");
      Serial.println(cycle);
      
      }
    else{onoff = 1;}

  }

  if(blink%2){digitalWrite(LED_BUILTIN, HIGH);  flag = 0;}
  else
  {
    if(flag == 0){
      digitalWrite(LED_BUILTIN, LOW);
      if(onoff == 0)
      {
        after_CNT = 0;
        duration_CNT = 0;
        Serial.println("operation off");
      }
      else{
        Serial.print("operation on. ");
        Serial.print("cycle: ");
        Serial.print(cycle);
        Serial.print("  after_CNT, duration_CNT: ");
        Serial.print(after_CNT);
        Serial.print(", ");
        Serial.println(duration_CNT);
      }
    }
    flag = 1;
  }

  if(after_CNT>FIRST_TIME)
  {
    irsend.sendNEC(0xA15E26D9, 32);//testdone
    Serial.println("FIRST_TIME");
    after_CNT = 0;
    state = Duration;
    cycle--;
  }

  if(duration_CNT>SECCOND_TIME)
  {
    irsend.sendNEC(0xA15E26D9, 32);//testdone //A1=address, 5E=invert address, 26=commend, D9=invert commend
    Serial.println("SECCOND_TIME");
    duration_CNT = 0;
    state = After;
    cycle--;
  }
}

void counterfunc(){
  if(cycle>0){
    if(state == After){after_CNT++;}
    else {duration_CNT++;}
    blink++;  
  }
}
