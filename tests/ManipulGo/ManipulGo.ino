/* Движение манипулятора (перенос груза) по нажатой кнопке
 * с использованием класса
 
   методы:
   go_wait() - переход в позицию ожидания
   go180() - перекладывание груза с 0 на 180
   go270() - перекладывание груза с 0 на 270
   
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

class Manipulator{
  public:
    Manipulator(){};

    // переход в позицию ожидания вход сигнала
    void go_wait(){ 
      Braccio.ServoMovement(20,  0, 140, 0, 10, 0, 10); 
      delay(100);
    }
    
    // перенос груза с позиции 0 на 180
    void go180(){ 
      _go_input_up();
      _go_input_low();
      _go_input_low_pickup();
      _go_input_up_pickup();
      _go_output180_up_pickup();
      _go_output180_low_pickup();
      _go_output180_low();
      _go_output180_up();
    }

    // перенос груза с позиции 0 на 270
    void go270(){
      _go_input_up();
      _go_input_low();
      _go_input_low_pickup();
      _go_input_up_pickup();
      _go_output270_up_pickup();
      _go_output270_low_pickup();
      _go_output270_low();
      _go_output270_up();
    }
    
  private:
    void _go_input_up(){
      Braccio.ServoMovement(20, 0, 115,  0, 15, 90, 10);
    }
    void _go_input_low(){
      Braccio.ServoMovement(30, 0, 100,  0, 10, 90, 10);
    }
    void _go_input_low_pickup(){
      Braccio.ServoMovement(30, 0, 100,  0, 10, 90, 73);
    }
    void _go_input_up_pickup(){
      Braccio.ServoMovement(30, 0, 115,  0, 15, 90, 73);
    }
    //////////////////////////////////////////////////
    void _go_output180_up_pickup(){
      Braccio.ServoMovement(20, 180, 115,  0, 15, 90, 73);
    }
    void _go_output180_low_pickup(){
      Braccio.ServoMovement(30, 180, 100,  0, 10, 90, 73);
    }
    void _go_output180_low(){
      Braccio.ServoMovement(30, 180, 100,  0, 10, 90, 10);
    }
    void _go_output180_up(){
      Braccio.ServoMovement(30, 180, 115,  0, 10, 90, 10);
    }
    //////////////////////////////////////////////////
    void _go_output270_up_pickup(){
      Braccio.ServoMovement(20, 90, 65, 180, 165, 90, 73);
    }
    void _go_output270_low_pickup(){
      Braccio.ServoMovement(30, 90, 80, 180, 170, 90, 73);
    }
    void _go_output270_low(){
      Braccio.ServoMovement(30,  90, 80, 180, 170, 90, 10);
    }
    void _go_output270_up(){
      Braccio.ServoMovement(30,  90, 80, 180, 170, 90, 10);
    }
};

Manipulator man;

void setup() {
  Serial.begin(9600);
  Braccio.begin();
  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == 1){
    man.go180();
    delay(100);
  } else {
    man.go_wait();
  }
  
}
