/* Движение манипулятора (перенос груза из 0 на указанный угол) по нажатой кнопке
 * с использованием класса. Controller проверяет пришли ли моторы в определённый угол
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

    void go_step(bool input, bool m1_is_0, bool m1_is_180, bool m2_is_40, bool m2_is_100, bool m2_is_115, 
        bool m2_is_140, bool m3_is_0, bool m3_is_180, bool m4_is_10, bool m4_is_15, bool m5_is_0, bool m5_is_90, 
        bool m6_is_10, bool m6_is_73) {

        if (state == BEGIN) {
            state = GO_WAIT;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A140;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A0;
            m6_angle = M6Angle::A73;

        } else if (state == GO_WAIT && m1_is_0 && m2_is_140 && m3_is_0 && m4_is_10 && m5_is_0 && m6_is_73) {
            state = WAIT;

        } else if (state == WAIT && input) {
            state = GO_UP;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A15;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP && m1_is_0 && m2_is_115 && m3_is_0 && m4_is_15 && m5_is_90 && m6_is_73) {
            state = GO_LOW;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A100;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_LOW && m1_is_0 && m2_is_100 && m3_is_0 && m4_is_10 && m5_is_90 && m6_is_73) {
            state = GO_PICKUP;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A100;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_PICKUP && m1_is_0 && m2_is_100 && m3_is_0 && m4_is_10 && m5_is_90 && m6_is_10) {
            state = GO_UP_PICKUP;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A15;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_PICKUP && m1_is_0 && m2_is_115 && m3_is_0 && m4_is_15 && m5_is_90 && m6_is_10) {
            state = GO_UP_OUT_PICKUP;
            m1_angle = M1Angle::A180;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A15;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_OUT_PICKUP && m1_is_180 && m2_is_115 && m3_is_0 && m4_is_15 && m5_is_90 && m6_is_10) {
            state = GO_LOW_OUT_PICKUP;
            m1_angle = M1Angle::A180;
            m2_angle = M2Angle::A100;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_LOW_OUT_PICKUP && m1_is_180 && m2_is_100 && m3_is_0 && m4_is_10 && m5_is_90 && m6_is_10) {
            state = GO_DROP;
            m1_angle = M1Angle::A180;
            m2_angle = M2Angle::A100;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_DROP && m1_is_180 && m2_is_100 && m3_is_0 && m4_is_10 && m5_is_90 && m6_is_73) {
            state = GO_UP_OUT;
            m1_angle = M1Angle::A180;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A15;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_OUT && m1_is_180 && m2_is_115 && m3_is_0 && m4_is_15 && m5_is_90 && m6_is_73) {
            state = GO_DONE;
            m1_angle = M1Angle::A180;
            m2_angle = M2Angle::A140;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_DONE && m1_is_180 && m2_is_140 && m3_is_0 && m4_is_10 && m5_is_90 && m6_is_73) {
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
    iter++;
    bool but = digitalRead(BUTTON_PIN);

    Serial.println();
    Serial.print("m1 = "+String(base.read()));
    Serial.print(" m2 = "+String(shoulder.read()));
    Serial.print(" m3 = "+String(elbow.read()));
    Serial.print(" m4 = "+String(wrist_ver.read()));
    Serial.print(" m5 = "+String(wrist_rot.read()));
    Serial.print(" m6 = "+String(gripper.read()));
    Serial.println();

    bool m1_0 = base.read() == 0; 
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
    bool m6_10 = gripper.read() == 10;
    bool m6_73 = gripper.read() == 73;

    Serial.println("[INPUTS]");
    Serial.print("[" + String(iter) + "] " + "[but = " + String(but) + "] ");
    Serial.print("[m1_0 = "+ String(m1_0)+"] ");
    Serial.print("[m1_180 = "+ String(m1_180)+"] ");
    Serial.print("[m2_40 = "+ String(m2_40)+"] ");
    Serial.print("[m1_0m2_100 = "+ String(m2_100)+"] ");
    Serial.print("[m2_115 = "+ String(m2_115)+"] ");
    Serial.print("[m2_140 = "+ String(m2_140)+"] ");
    Serial.print("[m3_0 = "+ String(m3_0)+"] ");
    Serial.print("[m3_180 = "+ String(m3_180)+"] ");
    Serial.print("[m4_10 = "+ String(m4_10)+"] ");
    Serial.print("[m4_15 = "+ String(m4_15)+"] ");
    Serial.print("[m5_0 = "+ String(m5_0)+"] ");
    Serial.print("[m5_90 = "+ String(m5_90)+"] ");
    Serial.print("[m6_10 = "+ String(m6_10)+"] ");
    Serial.println("[m6_73 = "+ String(m6_73)+"] "); 
    Serial.println();

    control.go_step(but, m1_0, m1_180, m2_40, m2_100, m2_115, m2_140, m3_0, m3_180, m4_10, m4_15, m5_0, m5_90, m6_10, m6_73);

    Serial.println("[OUTPUTS]");
    Serial.print("[" + String(iter) + "] " + "[but = " + String(but) + "] ");
    Serial.print("[m1_0 = "+ String(m1_0)+"] ");
    Serial.print("[m1_180 = "+ String(m1_180)+"] ");
    Serial.print("[m2_40 = "+ String(m2_40)+"] ");
    Serial.print("[m1_0m2_100 = "+ String(m2_100)+"] ");
    Serial.print("[m2_115 = "+ String(m2_115)+"] ");
    Serial.print("[m2_140 = "+ String(m2_140)+"] ");
    Serial.print("[m3_0 = "+ String(m3_0)+"] ");
    Serial.print("[m3_180 = "+ String(m3_180)+"] ");
    Serial.print("[m4_10 = "+ String(m4_10)+"] ");
    Serial.print("[m4_15 = "+ String(m4_15)+"] ");
    Serial.print("[m5_0 = "+ String(m5_0)+"] ");
    Serial.print("[m5_90 = "+ String(m5_90)+"] ");
    Serial.print("[m6_10 = "+ String(m6_10)+"] ");
    Serial.println("[m6_73 = "+ String(m6_73)+"] ");
    Serial.println(); 

}

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT);
    Braccio.begin();
}

void loop() {
    iter++;

    bool but = digitalRead(BUTTON_PIN);

    Serial.println();
    Serial.print("m1 = "+String(base.read()));
    Serial.print(" m2 = "+String(shoulder.read()));
    Serial.print(" m3 = "+String(elbow.read()));
    Serial.print(" m4 = "+String(wrist_ver.read()));
    Serial.print(" m5 = "+String(wrist_rot.read()));
    Serial.print(" m6 = "+String(gripper.read()));
    Serial.println();
    
    bool m1_0 = base.read() == 0; 
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
    bool m6_73 = gripper.read() == 180-73;

    Serial.println("[INPUTS]");
    Serial.print("[" + String(iter) + "] " + "[but = " + String(but) + "] ");
    Serial.print("[m1_0 = "+ String(m1_0)+"] ");
    Serial.print("[m1_180 = "+ String(m1_180)+"] ");
    Serial.print("[m2_40 = "+ String(m2_40)+"] ");
    Serial.print("[m1_0m2_100 = "+ String(m2_100)+"] ");
    Serial.print("[m2_115 = "+ String(m2_115)+"] ");
    Serial.print("[m2_140 = "+ String(m2_140)+"] ");
    Serial.print("[m3_0 = "+ String(m3_0)+"] ");
    Serial.print("[m3_180 = "+ String(m3_180)+"] ");
    Serial.print("[m4_10 = "+ String(m4_10)+"] ");
    Serial.print("[m4_15 = "+ String(m4_15)+"] ");
    Serial.print("[m5_0 = "+ String(m5_0)+"] ");
    Serial.print("[m5_90 = "+ String(m5_90)+"] ");
    Serial.print("[m6_10 = "+ String(m6_10)+"] ");
    Serial.println("[m6_73 = "+ String(m6_73)+"] "); 
    Serial.println();

    control.go_step(but, m1_0, m1_180, m2_40, m2_100, m2_115, m2_140, m3_0, m3_180, m4_10, m4_15, m5_0, m5_90, m6_10, m6_73);

    Serial.println("[OUTPUTS]");
    Serial.print("[" + String(iter) + "] " + "[but = " + String(but) + "] ");
    Serial.print("[m1_0 = "+ String(m1_0)+"] ");
    Serial.print("[m1_180 = "+ String(m1_180)+"] ");
    Serial.print("[m2_40 = "+ String(m2_40)+"] ");
    Serial.print("[m1_0m2_100 = "+ String(m2_100)+"] ");
    Serial.print("[m2_115 = "+ String(m2_115)+"] ");
    Serial.print("[m2_140 = "+ String(m2_140)+"] ");
    Serial.print("[m3_0 = "+ String(m3_0)+"] ");
    Serial.print("[m3_180 = "+ String(m3_180)+"] ");
    Serial.print("[m4_10 = "+ String(m4_10)+"] ");
    Serial.print("[m4_15 = "+ String(m4_15)+"] ");
    Serial.print("[m5_0 = "+ String(m5_0)+"] ");
    Serial.print("[m5_90 = "+ String(m5_90)+"] ");
    Serial.print("[m6_10 = "+ String(m6_10)+"] ");
    Serial.println("[m6_73 = "+ String(m6_73)+"] ");
    Serial.println(); 

    
    Braccio.ServoMovement(20, static_cast<int>(control.m1_angle), static_cast<int>(control.m2_angle), static_cast<int>(control.m3_angle),
     static_cast<int>(control.m4_angle), static_cast<int>(control.m5_angle), static_cast<int>(control.m6_angle), callback);
}
