/* Движение манипулятора (перенос груза) по нажатой кнопке
 * с использованием класса. Переписано к виду КА pose-wise архитектуры, 
 * в которой состояниями являются позы манипулятора.
 
  * wait
  * low/up
  * pickup/drop
  */

#include <MyBraccio.h>
#include <Servo.h>

#define BUTTON_PIN 2

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

enum State{IDLE_, WAIT, UP, LOW_, PICKUP, UP_PICKUP, UP_OUT_PICKUP, LOW_OUT_PICKUP, DROP, DONE};
State state = IDLE_;


void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);
  Serial.println("going to wait...");
  Braccio.begin();
  Serial.println("reached wait state");
  state = WAIT;
}

class Manipulator{
  public:
    Manipulator(){};

    // переход в позицию ожидания вход сигнала
    void go_wait(){ 
      Braccio.ServoMovement(20,  0, 140, 0, 10, 0, 10); 
      delay(100);
    }
    
    // перенос груза с позиции 0 на 180
    void go(bool input, int deg){
      
      if (state == WAIT && input == 1){
        state = state + 1;
        Serial.println("going to up...");
        _go_up();
        Serial.println("reached and position up");
        delay(100); 
        
      } else if (state == UP){
        state = state + 1;
        Serial.println("going to low...");
        _go_low();
        Serial.println("reached and position low");
        delay(100);
        
      } else if (state == LOW_){
        state = state + 1;
        Serial.println("going to pickup...");
        _go_pickup();
        Serial.println("reached and position pickup");
        delay(100);
        
      } else if (state == PICKUP){
        state = state + 1;
        Serial.println("going to up_pickup...");
        _go_up_pickup();
        Serial.println("reached and position up_pickup");
        delay(100);

      } else if (state == UP_PICKUP){
        state = state + 1;
        Serial.println("going to up_out_pickup...");
        _go_up_out_pickup(deg);
        Serial.println("reached and position up_out_pickup");
        delay(100);
        
      } else if (state == UP_OUT_PICKUP){
        state = state + 1;
        Serial.println("going to low_out_pickup...");
        _go_low_out_pickup(deg);
        Serial.println("reached and position low_out_pickup");
        delay(100);

      } else if (state == LOW_OUT_PICKUP){
        state = state + 1;
        Serial.println("going to drop...");
        _go_drop(deg);
        Serial.println("reached and position drop");
        delay(100);

      } else if (state == DROP){
        state = state + 1;
        Serial.println("DONE!");
        
      } else if (state == DONE){
        state = WAIT;
        Serial.println("going to wait...");
        go_wait();
        Serial.println("reached wait state");
      }

    }
    
  private:
    void _go_up(){
      Braccio.ServoMovement(20, 0, 115,  0, 15, 90, 10);
    }
    
    void _go_low(){
      Braccio.ServoMovement(30, 0, 100,  0, 10, 90, 10);
    }
    
    void _go_pickup(){
      Braccio.ServoMovement(30, 0, 100,  0, 10, 90, 73);
    }
    
    void _go_up_pickup(){
      Braccio.ServoMovement(30, 0, 115,  0, 15, 90, 73);
    }
    //////////////////////////////////////////////////
    void _go_up_out_pickup(int deg){
      if (deg <= 180){
        Braccio.ServoMovement(20, deg, 115,  0, 15, 90, 73);
      } else {
        Braccio.ServoMovement(20, deg-180, 65, 180, 165, 90, 73);
      } 
    }
    
    void _go_low_out_pickup(int deg){
      if (deg <= 180){
        Braccio.ServoMovement(30, deg, 100,  0, 10, 90, 73);
      } else {
        Braccio.ServoMovement(30, deg-180, 80, 180, 170, 90, 73);
      } 
    }
    
    void _go_drop(int deg){
      if (deg <= 180){
        Braccio.ServoMovement(30, deg, 100,  0, 10, 90, 10);
      } else {
        Braccio.ServoMovement(30,  deg-180, 80, 180, 170, 90, 10);
      }
    }
};

Manipulator man;

void loop() {
  man.go(digitalRead(BUTTON_PIN),210);
}
