// ВЗАИМОДЕЙСТВИЕ МАНИПУЛЯТОРОВ //

/* Передача груза "из рук в руки" */

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

#include <Wire.h>

#include "Controller1.hpp"
#include "Controller2.hpp"

#define DEBUG_LOG
//#define DEBUG_MESSAGE

#ifdef DEBUG_MESSAGE
#define MESSAGE(mess) Serial.print(mess)
#else
#define MESSAGE(mess)
#endif

#define BUTTONS_PIN A0
#define INPUT_LAMP_PIN 2
#define DONE_PIN 4 // выходной пин для синхронного перехода в положение передачи
#define ACK_PIN 7 // входной пин для DROP и приёма SAFE

// I2C slave addresses
#define I2C_ADDR_SLAVE_X1 11
#define I2C_ADDR_SLAVE_X2 12

ControllerX1 control1;
ControllerX2 control2;

ControllerX1::Input last_input_X1;
ControllerX2::Input last_input_X2;

ControllerX1::Out last_out_X1;
ControllerX2::Out last_out_X2;

uint8_t current_angle_X1[6];
uint8_t current_angle_X2[6];
uint8_t target_angle_X1[6];
uint8_t target_angle_X2[6];

bool occupied_Z1 = false;

bool is_prev_passive_X1;
bool is_prev_passive_X2;

unsigned iter = 0;  

void setup() {
    Wire.begin();  // begin as master

    Serial.begin(9600);

    pinMode(BUTTONS_PIN, INPUT);
    pinMode(INPUT_LAMP_PIN, OUTPUT);
    pinMode(DONE_PIN, OUTPUT);
    pinMode(ACK_PIN, INPUT_PULLUP);

    update_leds();
}

void update_leds() {
    digitalWrite(INPUT_LAMP_PIN, occupied_Z1);
}

void print_array(uint8_t (&data)[6]) {
    Serial.print("[");
    for (size_t i = 0; i < 6; i++) {
        Serial.print(data[i]);
        Serial.print(", ");
    }
    Serial.println("]");
}

void read_current_angles(int slave_address, uint8_t (&current_angle)[6]) {
    Wire.requestFrom(slave_address, 6);
    for (size_t i = 0; i < 6; i++) {
        current_angle[i] = Wire.read();
    }

    Serial.print("recieved current angles from ");
    Serial.print(slave_address);
    Serial.print(": ");
    print_array(current_angle);
}

void send_target_angles(int slave_address, uint8_t (&target_angle)[6]) {
    Serial.print("sending target angles to ");
    Serial.print(slave_address);
    Serial.print(": ");
    print_array(target_angle);

    Wire.beginTransmission(slave_address);
    Wire.write(target_angle, 6);
    Wire.endTransmission();
}

void log_everything(int index,
                    bool want_cargo_on_out,
                    bool in_ack_or_ready,
                    bool is_done_m1,
                    bool is_done_m2,
                    bool is_done_m3,
                    bool is_done_m4,
                    bool is_done_m5,
                    bool is_done_m6,
                    String state,
                    bool out_ack_or_ready,
                    int go_base,
                    int go_shoulder,
                    int go_elbow,
                    int go_wrist_ver,
                    int go_wrist_rot,
                    int go_gripper) {
    Serial.print("[iter #" + String(iter) + "]");
    Serial.print(" CONTROLLER_" + String(index));
    if (index == 1){
        Serial.print(" want_cargo_on_out=" + String(want_cargo_on_out));
        Serial.print(" ack=" + String(in_ack_or_ready));
    } else {
        Serial.print(" X1_ready=" + String(in_ack_or_ready));
    }
    Serial.print(" is_done_m1=" + String(is_done_m1));
    Serial.print(" is_done_m2=" + String(is_done_m2));
    Serial.print(" is_done_m3=" + String(is_done_m3));
    Serial.print(" is_done_m4=" + String(is_done_m4));
    Serial.print(" is_done_m5=" + String(is_done_m5));
    Serial.print(" is_done_m6=" + String(is_done_m6));
    Serial.print(" state=" + String(state));
    if (index == 1){
        Serial.print(" ready=" + String(out_ack_or_ready));
    } else {
        Serial.print(" ack=" + String(out_ack_or_ready));
    }
    Serial.print(" go_base=" + String(go_base));
    Serial.print(" go_shoulder=" + String(go_shoulder));
    Serial.print(" go_elbow=" + String(go_elbow));
    Serial.print(" go_wrist_ver=" + String(go_wrist_ver));
    Serial.print(" go_wrist_rot=" + String(go_wrist_rot));
    Serial.println(" go_gripper=" + String(go_gripper));
}

void loop() {
    iter++;

    // Read buttons
    if (analogRead(BUTTONS_PIN) >= 100 && analogRead(BUTTONS_PIN) < 700) {
        MESSAGE("*** The cargo is removed from Z1 pos ***");
        occupied_Z1 = false;
    } else if (analogRead(BUTTONS_PIN) >= 700) {
        MESSAGE("*** The cargo on Z1 pos ***");
        occupied_Z1 = true;
    }

    update_leds();

    // ==========================================

    // Controller 1
    {
        // Inputs
        ControllerX1::Input input;
        input.want_cargo_on_out = occupied_Z1;
        input.ack = control2.out.ack;
        read_current_angles(I2C_ADDR_SLAVE_X1, current_angle_X1);
        input.is_done_m1 = (current_angle_X1[0] == target_angle_X1[0]);
        input.is_done_m2 = (current_angle_X1[1] == target_angle_X1[1]);
        input.is_done_m3 = (current_angle_X1[2] == target_angle_X1[2]);
        input.is_done_m4 = (current_angle_X1[3] == target_angle_X1[3]);
        input.is_done_m5 = (current_angle_X1[4] == target_angle_X1[4]);
        input.is_done_m6 = (current_angle_X1[5] == target_angle_X1[5]);

        // Execute the controller
        bool is_active = control1.go_step(input);
        auto out = control1.out;

#ifdef DEBUG_LOG
        bool is_same_input = (input == last_input_X1);

        // Log everything, if necessary
        if (is_active || !is_same_input || !is_prev_passive_X1) {
            if (is_active) {
                is_prev_passive_X1 = false;
            }
            log_everything(1,
                           input.want_cargo_on_out,
                           input.ack,
                           input.is_done_m1,
                           input.is_done_m2,
                           input.is_done_m3,
                           input.is_done_m4,
                           input.is_done_m5,
                           input.is_done_m6,
                           control1.state2string(control1.state),
                           out.ready,
                           (int)out.go_base,
                           (int)out.go_shoulder,
                           (int)out.go_elbow,
                           (int)out.go_wrist_ver,
                           (int)out.go_wrist_rot,
                           (int)out.go_gripper);
        }

        is_prev_passive_X1 = !is_active;
#endif

        if (control1.state == ControllerX1::GO_UP_CLENCHED) {
            MESSAGE("*** The cargo pickuped from Z1 pos ***");
            occupied_Z1 = false;
        }

        // Send controls to the plant
        target_angle_X1[0] = (int)out.go_base;
        target_angle_X1[1] = (int)out.go_shoulder;
        target_angle_X1[2] = (int)out.go_elbow;
        target_angle_X1[3] = (int)out.go_wrist_ver;
        target_angle_X1[4] = (int)out.go_wrist_rot;
        target_angle_X1[5] = (int)out.go_gripper;
        send_target_angles(I2C_ADDR_SLAVE_X1, target_angle_X1);

        // Update last_input
        last_input_X1 = input;
        // Update last_out
        last_out_X1 = out;
    }

    // ==========================================

    // Controller 2
    {
        // Inputs
        ControllerX2::Input input;
        input.X1_ready = control1.out.ready;
        read_current_angles(I2C_ADDR_SLAVE_X2, current_angle_X2);
        input.is_done_m1 = (current_angle_X2[0] == target_angle_X2[0]);
        input.is_done_m2 = (current_angle_X2[1] == target_angle_X2[1]);
        input.is_done_m3 = (current_angle_X2[2] == target_angle_X2[2]);
        input.is_done_m4 = (current_angle_X2[3] == target_angle_X2[3]);
        input.is_done_m5 = (current_angle_X2[4] == target_angle_X2[4]);
        input.is_done_m6 = (current_angle_X2[5] == target_angle_X2[5]);

        // Execute the controller
        bool is_active = control2.go_step(input);
        auto out = control2.out;

#ifdef DEBUG_LOG
        bool is_same_input = (input == last_input_X2);

        if (is_active || !is_same_input || !is_prev_passive_X2) {
            if (is_active) {
                is_prev_passive_X2 = false;
            }
            log_everything(2, false,
                           input.X1_ready,
                           input.is_done_m1,
                           input.is_done_m2,
                           input.is_done_m3,
                           input.is_done_m4,
                           input.is_done_m5,
                           input.is_done_m6,
                           control2.state2string(control2.state),
                           out.ack,
                           (int)out.go_base,
                           (int)out.go_shoulder,
                           (int)out.go_elbow,
                           (int)out.go_wrist_ver,
                           (int)out.go_wrist_rot,
                           (int)out.go_gripper);
        }

        is_prev_passive_X2 = !is_active;
#endif

        // Send controls to the plant
        target_angle_X2[0] = (int)out.go_base;
        target_angle_X2[1] = (int)out.go_shoulder;
        target_angle_X2[2] = (int)out.go_elbow;
        target_angle_X2[3] = (int)out.go_wrist_ver;
        target_angle_X2[4] = (int)out.go_wrist_rot;
        target_angle_X2[5] = (int)out.go_gripper;
        send_target_angles(I2C_ADDR_SLAVE_X2, target_angle_X2);

        // Update last_input
        last_input_X2 = input;
        // Update last_out
        last_out_X2 = out;
    }

    // ==========================================

    update_leds();

    delay(100);

}
