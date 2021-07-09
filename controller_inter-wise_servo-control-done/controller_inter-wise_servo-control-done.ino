/* Движение манипулятора (перенос груза из 0 на указанный угол) по нажатой кнопке
 * с использованием класса. Controller проверяет закончили ли моторы движение
*/

#include <MyBraccio.h>
#include <Servo.h>

#define BUTTON_PIN 2
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

struct Controller {
public:

    enum struct M1Angle {
        A0 = 0,
        A180 = 180
    };
    enum struct M2Angle {
        A40 = 40,
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
        A15 = 15
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
        BEGIN, GO_WAIT, WAIT, GO_UP, GO_LOW, GO_PICKUP, GO_UP_PICKUP,
        GO_UP_OUT_PICKUP, GO_LOW_OUT_PICKUP, GO_DROP, GO_UP_OUT, GO_DONE, DONE
    };
    State state;

    void go_step(bool input, bool m1_is_done, bool m2_is_done, bool m3_is_done, bool m4_is_done, bool m5_is_done,
        bool m6_is_done) {

        bool m_is_done = m1_is_done && m2_is_done && m3_is_done && m4_is_done && m5_is_done && m6_is_done;

        if (state == BEGIN) {
            state = GO_WAIT;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A140;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A0;
            m6_angle = M6Angle::A73;

        } else if (state == GO_WAIT && m_is_done) {
            state = WAIT;

        } else if (state == WAIT && input) {
            state = GO_UP;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A15;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP && m_is_done) {
            state = GO_LOW;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A100;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_LOW && m_is_done) {
            state = GO_PICKUP;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A100;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_PICKUP && m_is_done) {
            state = GO_UP_PICKUP;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A15;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_PICKUP && m_is_done) {
            state = GO_UP_OUT_PICKUP;
            m1_angle = M1Angle::A180;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A15;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_OUT_PICKUP && m_is_done) {
            state = GO_LOW_OUT_PICKUP;
            m1_angle = M1Angle::A180;
            m2_angle = M2Angle::A100;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_LOW_OUT_PICKUP && m_is_done) {
            state = GO_DROP;
            m1_angle = M1Angle::A180;
            m2_angle = M2Angle::A100;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_DROP && m_is_done) {
            state = GO_UP_OUT;
            m1_angle = M1Angle::A180;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A15;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_OUT && m_is_done) {
            state = GO_DONE;
            m1_angle = M1Angle::A180;
            m2_angle = M2Angle::A140;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_DONE && m_is_done) {
            state = DONE;


        } else if (state == DONE) {
            state = GO_WAIT;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A140;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A0;
            m6_angle = M6Angle::A73;

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

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT);
    Braccio.begin();
}

void loop() {
    iter++;

    bool but = digitalRead(BUTTON_PIN);

    /*Serial.println();
    Serial.print("m1 = "+String(base.read()));
    Serial.print(" m2 = "+String(shoulder.read()));
    Serial.print(" m3 = "+String(elbow.read()));
    Serial.print(" m4 = "+String(wrist_ver.read()));
    Serial.print(" m5 = "+String(wrist_rot.read()));
    Serial.print(" m6 = "+String(gripper.read()));
    Serial.println();*/
    
    /*bool m1_0 = base.read() == 0;
    bool m1_180 = base.read() == 180;
    bool m2_40 = shoulder.read() == 40;
    bool m2_100 = shoulder.read() == 100;
    bool m2_115 = shoulder.read() == 115;
    bool m2_140 = shoulder.read() == 140;
    bool m3_0 = elbow.read() == 0;
    bool m3_180 = elbow.read() == 180;
    bool m4_10 = wrist_ver.read() == 10;
    bool m4_15 = wrist_ver.read() == 15;
    bool m5_0 = wrist_rot.read() == 0;
    bool m5_90 = wrist_rot.read() == 90;
    bool m6_10 = gripper.read() == 180-10;
    bool m6_73 = gripper.read() == 180-73;*/


    control.go_step(but, true, true, true, true, true, true);
    
    Braccio.ServoMovement(20, static_cast<int>(control.m1_angle), static_cast<int>(control.m2_angle), static_cast<int>(control.m3_angle),
     static_cast<int>(control.m4_angle), static_cast<int>(control.m5_angle), static_cast<int>(control.m6_angle), callback);
}
