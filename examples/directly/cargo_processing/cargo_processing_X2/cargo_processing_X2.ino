// ВЗАИМОДЕЙСТВИЕ МАНИПУЛЯТОРОВ //

/* На вход X1 приходит положение Z1 или Z2 и перекладывает груз. Контроллер X2 проверяет состояния Z1 и Z2 и, в зависимости от того, куда пришёл груз, манипулятор перекладывает на позицию Z3.
*/

#include <MyBraccio.h>
#include <Servo.h>

#define Z1_PIN 7 // сигнал с Z1, обработка завершена
#define Z2_PIN 8 // сигнал с Z2, обработка завершена
#define CONTROL_PIN 12

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

unsigned iter = 0;
int Z1_done = 0;
long timer = 0;

void setup() {
    Serial.begin(115200);
    pinMode(Z1_PIN, INPUT_PULLUP);
    pinMode(Z2_PIN, INPUT_PULLUP);
    Braccio.begin(20);
    timer = millis();
    
}

struct Controller {
public:

    enum struct M1Angle {
        A0 = 0,
        A60 = 60,
        A90 = 90,
        A120 = 120,
        A180 = 180
    };
    enum struct M2Angle {
        A65 = 65,
        A90 = 90,
        A100 = 100,
        A115 = 115,
        A140 = 140
    };
    enum struct M3Angle {
        A0 = 0,
        A180 = 180
    };
    enum struct M4Angle {
        A10 = 10,
        A15 = 15,
        A165 = 165
    };
    enum struct M5Angle {
        A0 = 0,
        A90 = 90
    };
    enum struct M6Angle {
        A10 = 10,
        A73 = 73
    };
    
    M1Angle m1_angle;
    M2Angle m2_angle;
    M3Angle m3_angle;
    M4Angle m4_angle;
    M5Angle m5_angle;
    M6Angle m6_angle;

    enum State {
        BEGIN, GO_WAIT, WAIT, 
        GO_SAFE_Z1, GO_UP_Z1, GO_LOW_Z1, GO_PICKUP_Z1, GO_UP_PICKUP_Z1, GO_SAFE_PICKUP_Z1,
        GO_SAFE_Z2, GO_UP_Z2, GO_LOW_Z2, GO_PICKUP_Z2, GO_UP_PICKUP_Z2, GO_SAFE_PICKUP_Z2,
        GO_SAFE_Z31, GO_SAFE_Z32, GO_UP_PICKUP_Z3, GO_LOW_PICKUP_Z3, GO_DROP_Z3, GO_UP_Z3 
    };
    State state;

    void go_step(bool input_Z1, bool input_Z2, bool m1_is_done, bool m2_is_done, bool m3_is_done, bool m4_is_done, 
        bool m5_is_done, bool m6_is_done) {

        bool m_is_done = m1_is_done && m2_is_done && m3_is_done && m4_is_done && m5_is_done && m6_is_done;

        if (state == BEGIN) {
            state = GO_WAIT;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A140;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A0;
            m6_angle = M6Angle::A10;

        } else if (state == GO_WAIT && m_is_done) {
            state = WAIT;

        } else if (state == WAIT && input_Z1 && Z1_done != 3) {
            state = GO_SAFE_Z1;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A65;
            m3_angle = M3Angle::A180;
            m4_angle = M4Angle::A165;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_SAFE_Z1 && m_is_done) {
            state = GO_UP_Z1;
            m1_angle = M1Angle::A120;
            m2_angle = M2Angle::A65;
            m3_angle = M3Angle::A180;
            m4_angle = M4Angle::A165;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_Z1 && m_is_done) {
            state = GO_LOW_Z1;
            m1_angle = M1Angle::A120;
            m2_angle = M2Angle::A90;
            m3_angle = M3Angle::A180;
            m4_angle = M4Angle::A165;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_LOW_Z1 && m_is_done) {
            state = GO_PICKUP_Z1;
            m1_angle = M1Angle::A120;
            m2_angle = M2Angle::A90;
            m3_angle = M3Angle::A180;
            m4_angle = M4Angle::A165;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_PICKUP_Z1 && m_is_done) {
            state = GO_UP_PICKUP_Z1;
//            Z1_done += 1;
            m1_angle = M1Angle::A120;
            m2_angle = M2Angle::A65;
            m3_angle = M3Angle::A180;
            m4_angle = M4Angle::A165;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_PICKUP_Z1 && m_is_done) {
            state = GO_SAFE_PICKUP_Z1;
            m1_angle = M1Angle::A180;
            m2_angle = M2Angle::A65;
            m3_angle = M3Angle::A180;
            m4_angle = M4Angle::A165;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;
//////////////////////////////////////////////////////////////////

        } else if (state == WAIT && input_Z2) {
            state = GO_SAFE_Z2;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A65;
            m3_angle = M3Angle::A180;
            m4_angle = M4Angle::A165;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_SAFE_Z2 && m_is_done) {
            state = GO_UP_Z2;
            m1_angle = M1Angle::A60;
            m2_angle = M2Angle::A65;
            m3_angle = M3Angle::A180;
            m4_angle = M4Angle::A165;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_Z2 && m_is_done) {
            state = GO_LOW_Z2;
            m1_angle = M1Angle::A60;
            m2_angle = M2Angle::A90;
            m3_angle = M3Angle::A180;
            m4_angle = M4Angle::A165;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_LOW_Z2 && m_is_done) {
            state = GO_PICKUP_Z2;
            m1_angle = M1Angle::A60;
            m2_angle = M2Angle::A90;
            m3_angle = M3Angle::A180;
            m4_angle = M4Angle::A165;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_PICKUP_Z2 && m_is_done) {
            state = GO_UP_PICKUP_Z2;
//            Z1_done = 0;
            m1_angle = M1Angle::A60;
            m2_angle = M2Angle::A65;
            m3_angle = M3Angle::A180;
            m4_angle = M4Angle::A165;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_PICKUP_Z2 && m_is_done) {
            state = GO_SAFE_PICKUP_Z2;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A65;
            m3_angle = M3Angle::A180;
            m4_angle = M4Angle::A165;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;
//////////////////////////////////////////////////////////////////

        } else if (state == GO_SAFE_PICKUP_Z1 && m_is_done) {
            state = GO_SAFE_Z31;
            m1_angle = M1Angle::A180;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A15;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_SAFE_PICKUP_Z2 && m_is_done) {
            state = GO_SAFE_Z32;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A15;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;
            
        } else if ((state == GO_SAFE_Z31 || state == GO_SAFE_Z32) && m_is_done) {
            state = GO_UP_PICKUP_Z3;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A15;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_PICKUP_Z3 && m_is_done) {
            state = GO_LOW_PICKUP_Z3;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A100;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_LOW_PICKUP_Z3 && m_is_done) {
            state = GO_DROP_Z3;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A100;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_DROP_Z3 && m_is_done) {
            state = GO_UP_Z3;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A15;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_Z3 && m_is_done) {
            state = GO_WAIT;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A140;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A0;
            m6_angle = M6Angle::A10;

        }
        
    }

private:

};

Controller control;

void callback(
    int final_base, int final_shoulder, int final_elbow,
    int final_wrist_ver, int final_wrist_rot, int final_gripper,
    int current_base, int current_shoulder, int current_elbow,
    int current_wrist_ver, int current_wrist_rot, int current_gripper
) {
    Serial.print("Desired values: (");
    Serial.print(String(final_base) +", "+ String(final_shoulder) +", "+ String(final_elbow) +", "+
    String(final_wrist_ver) +", "+ String(final_wrist_rot) +", "+ String(final_gripper));
    Serial.println(")");
    Serial.print("Current values: (");
    Serial.print(String(current_base) +", "+ String(current_shoulder) +", "+ String(current_elbow) +", "+
    String(current_wrist_ver) +", "+ String(current_wrist_rot) +", "+ String(current_gripper));
    Serial.println(")");

    //Serial.print("[#iter] [M1.done = FALSE/TRUE]");
    Serial.print("["+String(iter)+"] ");
    if (current_base == final_base){
        Serial.print("[M1.done] TRUE ");
    } else {Serial.print("[M1.done] false ");}

    if (current_shoulder == final_shoulder){
        Serial.print("[M2.done] TRUE ");
    } else {Serial.print("[M2.done] false ");}

    if (current_elbow == final_elbow){
        Serial.print("[M3.done] TRUE ");
    } else {Serial.print("[M3.done] false ");}

    if (current_wrist_ver == final_wrist_ver){
        Serial.print("[M4.done] TRUE ");
    } else {Serial.print("[M4.done] false ");}

    if (current_wrist_rot == final_wrist_rot){
        Serial.print("[M5.done] TRUE ");
    } else {Serial.print("[M5.done] false ");}

    if (current_gripper == final_gripper){
        Serial.println("[M6.done] TRUE ");
    } else {Serial.println("[M6.done] false ");}

}


void loop() {
    //iter++;

    bool input_Z1 = !digitalRead(Z1_PIN);
    bool input_Z2 = !digitalRead(Z2_PIN);

//    if (millis() - timer >= 1000){
//      if (input_Z1){
//        Serial.println("груз на Z1");
//      }
//      if (input_Z2){
//        Serial.println("груз на Z2");
//      }
//      timer = millis();
//    }
    control.go_step(input_Z1, input_Z2, true, true, true, true, true, true);
    
    Braccio.ServoMovement(20, static_cast<int>(control.m1_angle), static_cast<int>(control.m2_angle), static_cast<int>(control.m3_angle),
     static_cast<int>(control.m4_angle), static_cast<int>(control.m5_angle), static_cast<int>(control.m6_angle));
}
