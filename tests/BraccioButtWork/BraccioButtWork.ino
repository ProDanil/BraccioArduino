/* Работа манипулятора в 3 режимах:
   * START - стартовая позиция
   * WORK1 - перенос груза из 0 в 180
   * WORK2 - перенос груза из 90 в 270
*/

#include <MyBraccio.h>
#include <Servo.h>

/*Step Delay: a milliseconds delay between the movement of each servo.  Allowed values from 10 to 30 msec.
  M1 = base degrees from 0 to 180
  M2 = shoulder degrees from 15 to 165
  M3 = elbow degrees from 0 to 180
  M4 = vertical wrist degrees from 0 to 180
  M5 = rotatory wrist degrees from 0 to 180
  M6 = gripper degrees from 10 to 73 degrees*/

  /*begin:
    base.write(0);
    shoulder.write(40);
    elbow.write(180);
    wrist_ver.write(170);
    wrist_rot.write(0);
    gripper.write(73);*/
    
#define BUTTON_PIN 2

Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_ver;
Servo wrist_rot;
Servo gripper;

volatile bool butt = false;
volatile uint32_t debounce;

enum State{IDLE_, START, WORK1, WORK2, LAST};
volatile State mode = IDLE_; 

void push_butt(){
  if (millis() - debounce >= 100 && digitalRead(2)) {
    debounce = millis();
    mode = mode +1;
    if (mode == LAST){
      mode = 0;
    }
    Serial.print("Pressed! Mode = ");
    Serial.println(mode);
  }
}

void do_start(){
  Braccio.ServoMovement(30,  0, 40, 180, 170, 0, 10); 
  delay(100);
}

void do_work1(){
                      //sd    M1,  M2, M3, M4, M5, M6
  Braccio.ServoMovement(30,    0, 100,  0, 10, 90, 10); 
  delay(100);
  
  Braccio.ServoMovement(30,    0, 100,  0, 10, 90, 73); 
  delay(100);
  
  Braccio.ServoMovement(30,    0, 115,  0, 15, 90, 73);
  delay(100);
  
  Braccio.ServoMovement(30,  180, 115,  0, 15, 90, 73);
  delay(100);
  
  Braccio.ServoMovement(30,  180, 100,  0, 10, 90, 73); 
  delay(100);
   
  Braccio.ServoMovement(30,  180, 100,  0, 10, 90, 10); 
  delay(100);
}

void do_work2(){
                       //sd  M1,  M2,  M3,  M4, M5, M6
  Braccio.ServoMovement(30,  90, 100,   0,  10, 90, 10); 
  delay(100); 

  Braccio.ServoMovement(30,  90, 100,   0,  10, 90, 73); 
  delay(100);

  Braccio.ServoMovement(30,  90, 115,   0,  15, 90, 73);
  delay(100);

  Braccio.ServoMovement(30,  90,  65, 180, 165, 90, 73);
  delay(100);

  Braccio.ServoMovement(30,  90,  80, 180, 170, 90, 73); 
  delay(100); 

  Braccio.ServoMovement(30,  90,  80, 180, 170, 90, 10); 
  delay(100);
}

void setup() {
  Braccio.begin();
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);
  Serial.println("setup Done");
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), push_butt, RISING);
}

void loop() {

  if (mode == IDLE_){
    
  }else if (mode == START){
    do_start();
  } else if (mode == WORK1){
    do_work1();
  } else if (mode == WORK2){
    do_work2();
  }
}
