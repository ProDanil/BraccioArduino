/* Скетч основных позиций Braccio
   1. Start стартовое положение
   2. Work1 захват груза на 0 и перенос по базе на 180
   3. Work2 захват груза на 90 и перенос через себя на 270*/

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

Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_ver;
Servo wrist_rot;
Servo gripper;

#define BUTTON_PIN 2
volatile bool butt = false;

void setup() {
  Braccio.begin();
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), pushButton, RISING);
  Serial.println("setup Done");
}

volatile int counter = 0;

void pushButton(){
  detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
  butt = true;
  counter += 1;
  Serial.print("Pressed! Counter = ");
  Serial.println(counter);
  delay(1000);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), pushButton, RISING);
}

void loop() {
  counter = 0;
  while (butt == false){
    //Serial.println(butt);
    }
  butt = false;


  /////////////// START //////////////////////

  //1 Стартовая точка
                      //sd  M1, M2, M3, M4, M5, M6
  Braccio.ServoMovement(30,  0, 40, 180, 170, 0, 10); 
  delay(100);

  ////////////////////////////////////////////
  
  while (butt == false){
    //Serial.println(butt);
  }
  butt = false;
  

  /////////////// WORK 1 //// take 0 -> give 180 
           
  //2 Начало на базе 0 град
                      //sd  M1, M2, M3, M4, M5, M6
  Braccio.ServoMovement(30,  0, 100, 0, 10, 90, 10); 
  delay(100); 

  while (butt == false){
    //Serial.println(butt);
  }
  butt = false;
  
  //3 Захват на базе 0 град
                      //sd  M1, M2, M3, M4, M5, M6
  Braccio.ServoMovement(30,  0, 100, 0, 10, 90, 73); 
  delay(100);

  while (butt == false){
    //Serial.println(butt);
  }
  butt = false;

  //4 Prepos переноса, база 0 град
                      //sd  M1, M2, M3, M4, M5, M6
  Braccio.ServoMovement(30,  0, 115, 0, 15, 90, 73);
  delay(100);

  while (butt == false){
    //Serial.println(butt);
  }
  butt = false;

  //5 Перенос, база 180
                      //sd  M1, M2, M3, M4, M5, M6
  Braccio.ServoMovement(30,  180, 115, 0, 15, 90, 73);
  delay(100);

  while (butt == false){
    //Serial.println(butt);
  }
  butt = false;

  //6 Конечная точка на базе 180
                      //sd  M1, M2, M3, M4, M5, M6
  Braccio.ServoMovement(30,  180, 100, 0, 10, 90, 73); 
  delay(100); 

  while (butt == false){
    //Serial.println(butt);
  }
  butt = false;

  //7 Положить на базе 180 град
                      //sd  M1, M2, M3, M4, M5, M6
  Braccio.ServoMovement(30,  180, 100, 0, 10, 90, 10); 
  delay(100);

  ////////////////////////////////////////////
  
  while (butt == false){
    //Serial.println(butt);
    }
  butt = false;

  /////////////// WORK 2 //// take 90 -> give 270 
           
  //8 Начало на базе 90 град
                      //sd  M1, M2, M3, M4, M5, M6
  Braccio.ServoMovement(30,  90, 100, 0, 10, 90, 10); 
  delay(100); 

  while (butt == false){
    //Serial.println(butt);
  }
  butt = false;

  //9 Захват на базе 90 град
                      //sd  M1, M2, M3, M4, M5, M6
  Braccio.ServoMovement(30,  90, 100, 0, 10, 90, 73); 
  delay(100);

  while (butt == false){
    //Serial.println(butt);
  }
  butt = false;

  //10 Prepos переноса, база 90 град
                      //sd  M1, M2, M3, M4, M5, M6
  Braccio.ServoMovement(30,  90, 115, 0, 15, 90, 73);
  delay(100);

  while (butt == false){
    //Serial.println(butt);
  }
  butt = false;

  //11 Перенос на 270
                      //sd  M1, M2, M3, M4, M5, M6
  Braccio.ServoMovement(30,  90, 65, 180, 165, 90, 73);
  delay(100);

  while (butt == false){
    //Serial.println(butt);
  }
  butt = false;

  //12 Конечная точка на 270
                      //sd  M1, M2, M3, M4, M5, M6
  Braccio.ServoMovement(30,  90, 80, 180, 170, 90, 73); 
  delay(100); 

  while (butt == false){
    //Serial.println(butt);
  }
  butt = false;

  //13 Положить на 270
                      //sd  M1, M2, M3, M4, M5, M6
  Braccio.ServoMovement(30,  90, 80, 180, 170, 90, 10); 
  delay(100);

  ////////////////////////////////////////////

}
