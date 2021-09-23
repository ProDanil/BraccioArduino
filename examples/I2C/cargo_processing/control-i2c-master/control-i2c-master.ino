#include <Wire.h>

#include "Controller1.hpp"
#include "Controller2.hpp"
// #include "Controller3.hpp"

/*Step Delay: a milliseconds delay between the movement of each servo.  Allowed values from 10 to 30 msec.
  M1 = base degrees from 0 to 180
  M2 = shoulder degrees from 20 to 160
  M3 = elbow degrees from 0 to 180
  M4 = vertical wrist degrees from 0 to 180
  M5 = rotatory wrist degrees from 0 to 180
  M6 = gripper degrees from 15 to 80 degrees*/

// Button pins
#define PIN_INPUT_TO_Z12 A0  // button pins for transverring cargo from Z0 to Z1 or Z2 and reset button
#define PIN_Z12_TO_OUT A1    // button pins for transverring cargo from Z1 or Z2 to Z3
// Led pins
#define PIN_LED_Z0_Z1 2          // the cargo on Z0 needs to be transferred on Z1
#define PIN_LED_Z0_Z2 4          // the cargo on Z0 needs to be transferred on Z2
#define PIN_LED_PROCESSED_Z1 5   // the processed cargo on Z1 needs to be transferred on Z3
#define PIN_LED_PROCESSED_Z2 6   // the processed cargo on Z2 needs to be transferred on Z3
#define PIN_LED_Z1 7             // the cargo on Z1
#define PIN_LED_Z2 8             // the cargo on Z2
// I2C slave addresses
#define I2C_ADDR_SLAVE_X1 11
#define I2C_ADDR_SLAVE_X2 12
#define TIMER 1000 // ms logs timing

ControllerX1 control1;
ControllerX2 control2;
// ControllerX3 control3;

uint8_t current_angle_X1[6];
uint8_t current_angle_X2[6];
uint8_t target_angle_X1[6];
uint8_t target_angle_X2[6];

bool want_cargo_on_Z1 = false;
bool want_cargo_on_Z2 = false;
bool cargo_on_Z1 = false;
bool cargo_on_Z2 = false;
bool cargo_is_processed_Z1 = false;
bool cargo_is_processed_Z2 = false;
bool printed_done[2] = {false, false};
unsigned long timing;

void setup() {
    Wire.begin();  // begin as master
    Serial.begin(9600);
    // setup pins
    pinMode(PIN_INPUT_TO_Z12, INPUT);
    pinMode(PIN_Z12_TO_OUT, INPUT);
    pinMode(PIN_LED_Z0_Z1, OUTPUT);
    pinMode(PIN_LED_Z0_Z2, OUTPUT);
    pinMode(PIN_LED_PROCESSED_Z1, OUTPUT);
    pinMode(PIN_LED_PROCESSED_Z2, OUTPUT);
    pinMode(PIN_LED_Z1, OUTPUT);
    pinMode(PIN_LED_Z2, OUTPUT);
}

void print_array(uint8_t (&data)[6]) {
    Serial.print("[");
    for (size_t i = 0; i < 6; i++) {
        Serial.print(data[i]);
        Serial.print(", ");
    }
    Serial.println("]");
}

void  print_input_control(
        int controller,
        uint8_t current_angle[6],
        bool is_done_m1, bool is_done_m2,
        bool is_done_m3, bool is_done_m4,
        bool is_done_m5, bool is_done_m6){
    bool all_is_done = is_done_m1 && is_done_m2 && is_done_m3 && is_done_m4 && is_done_m5 && is_done_m6;
    if (!all_is_done){
        printed_done[controller-1] = false;
        if (millis() - timing > TIMER){
            timing = millis();
            Serial.print("     INPUTS CONTROLLER");
            Serial.println(controller);

            if (controller == 1){
                Serial.println("Want cargo on: Z1 "+String(want_cargo_on_Z1)+" Z2 "+String(want_cargo_on_Z2));
                Serial.println("Cargo on: Z1 "+String(cargo_on_Z1)+" Z2 "+String(cargo_on_Z2));
            } else {
                Serial.println("Cargo is processed: Z1 "+String(cargo_is_processed_Z1)+
                                                  " Z2 "+String(cargo_is_processed_Z2));
            }

            Serial.print("Current angles: (");
            Serial.print(String(current_angle[0]) +", "+ String(current_angle[1]) +", "+
                         String(current_angle[2]) +", "+ String(current_angle[3]) +", "+
                         String(current_angle[4]) +", "+ String(current_angle[5]));
            Serial.println(")");

            Serial.println("[M1.done] "+ String(is_done_m1) +" [M2.done] "+ String(is_done_m2) +
                          " [M3.done] "+ String(is_done_m3) +" [M4.done] "+ String(is_done_m4) +
                          " [M5.done] "+ String(is_done_m5) +" [M6.done] "+ String(is_done_m6));
        }
    } else if ((controller == 1 && !printed_done[0]) || ((controller == 2 && !printed_done[1]))){
        Serial.println("[M1.done] "+ String(is_done_m1) +" [M2.done] "+ String(is_done_m2) +
                      " [M3.done] "+ String(is_done_m3) +" [M4.done] "+ String(is_done_m4) +
                      " [M5.done] "+ String(is_done_m5) +" [M6.done] "+ String(is_done_m6));
        printed_done[controller-1] = true;
    }

}

/*void  print_input_control2(
        bool cargo_is_processed_Z1, bool cargo_is_processed_Z2,
        uint8_t current_angle[6],
        bool is_done_m1, bool is_done_m2,
        bool is_done_m3, bool is_done_m4,
        bool is_done_m5, bool is_done_m6); {
    Serial.println("     INPUTS CONTROLLER2");

    Serial.println("Cargo is processed: Z1 "+String(cargo_is_processed_Z1)+" Z2 "+String(cargo_is_processed_Z2));

    Serial.print("Current angles: (");
    Serial.print(String(current_angle[0]) +", "+ String(current_angle[1]) +", "+ String(current_angle[2]) +", "+
    String(current_angle[3]) +", "+ String(current_angle[4]) +", "+ String(current_angle[5]));
    Serial.println(")");

    Serial.println("[M1.done] "+ String(is_done_m1) +" [M2.done] "+ String(is_done_m2) +" [M3.done] "+ String(is_done_m3) +" [M4.done] "+ String(is_done_m4) +" [M5.done] "+ String(is_done_m5) +" [M6.done] "+ String(is_done_m6));
}*/

void print_out_control(int controller, uint8_t target_angle[6]){
    Serial.print("     INPUTS CONTROLLER");
    Serial.println(controller);
    Serial.print("Target angles: (");
    Serial.print(String(target_angle[0]) +", "+ String(target_angle[1]) +", "+ String(target_angle[2]) +", "+
    String(target_angle[3]) +", "+ String(target_angle[4]) +", "+ String(target_angle[5]));
    Serial.println(")");
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


void loop() {
    // Read buttons

    // Z0
    if (analogRead(PIN_INPUT_TO_Z12) >= 200 && analogRead(PIN_INPUT_TO_Z12) < 500) {
        want_cargo_on_Z1 = false;
        want_cargo_on_Z2 = false;
        digitalWrite(PIN_LED_Z0_Z1, LOW);
        digitalWrite(PIN_LED_Z0_Z2, LOW);
        Serial.println("*** The cargo is removed from Z0 pos ***");

    } else if (analogRead(PIN_INPUT_TO_Z12) >= 500 && analogRead(PIN_INPUT_TO_Z12) < 800) {
        want_cargo_on_Z2 = true;
        digitalWrite(PIN_LED_Z0_Z2, HIGH);
        Serial.println("*** REQUEST: move the cargo to pos Z2 ***");

    } else if (analogRead(PIN_INPUT_TO_Z12) >= 800) {
        want_cargo_on_Z1 = true;
        digitalWrite(PIN_LED_Z0_Z1, HIGH);
        Serial.println("*** REQUEST: move the cargo to pos Z1 ***");
    }

    // Z1 and Z2 processed
    if (analogRead(PIN_Z12_TO_OUT) >= 100 && analogRead(PIN_Z12_TO_OUT) < 700 && cargo_on_Z2) {
        cargo_is_processed_Z2 = true;
        digitalWrite(PIN_LED_PROCESSED_Z2, HIGH);
        Serial.println("*** The cargo on Z2 is processed! ***");

    } else if (analogRead(PIN_Z12_TO_OUT) >= 700 && cargo_on_Z1) {
        cargo_is_processed_Z1 = true;
        digitalWrite(PIN_LED_PROCESSED_Z1, HIGH);
        Serial.println("*** The cargo on Z1 is processed! ***");

    } /*else if (analogRead(PIN_Z12_TO_OUT) >= 800){
        cargoZ1_is_processed = false;
        cargoZ2_is_processed = false;
        digitalWrite(PIN_LED_Z1, LOW);
        digitalWrite(PIN_LED_Z2, LOW);
    } */

    // X1
    {
        // Read the state of the plant
        read_current_angles(I2C_ADDR_SLAVE_X1, current_angle_X1);

        // Calculate inputs for the controller
        bool is_done_m1 = (current_angle_X1[0] == target_angle_X1[0]);
        bool is_done_m2 = (current_angle_X1[1] == target_angle_X1[1]);
        bool is_done_m3 = (current_angle_X1[2] == target_angle_X1[2]);
        bool is_done_m4 = (current_angle_X1[3] == target_angle_X1[3]);
        bool is_done_m5 = (current_angle_X1[4] == target_angle_X1[4]);
        bool is_done_m6 = (current_angle_X1[5] == target_angle_X1[5]);

        // Print inputs controller 1
        print_input_control(
            1, current_angle_X1,
            is_done_m1, is_done_m2,
            is_done_m3, is_done_m4,
            is_done_m5, is_done_m6);

        // Execute the controller
//        Serial.print("----contril1(");
//        Serial.print(want_cargo_on_Z1);
//        Serial.print(", ");
//        Serial.print(want_cargo_on_Z2);
//        Serial.print(", ");
//        Serial.print(cargo_on_Z1);
//        Serial.print(", ");
//        Serial.print(cargo_on_Z2);
//        Serial.print(", ");
//        Serial.print(is_done_m1);
//        Serial.print(", ");
//        Serial.print(is_done_m2);
//        Serial.print(", ");
//        Serial.print(is_done_m3);
//        Serial.print(", ");
//        Serial.print(is_done_m4);
//        Serial.print(", ");
//        Serial.print(is_done_m5);
//        Serial.print(", ");
//        Serial.print(is_done_m6);
//        Serial.println(")----");
        auto out = control1.go_step(
            want_cargo_on_Z1, want_cargo_on_Z2,
            cargo_on_Z1, cargo_on_Z2,
            is_done_m1, is_done_m2,
            is_done_m3, is_done_m4,
            is_done_m5, is_done_m6);

        if (control1.state == ControllerX1::GO_UP_PICKUP_Z01 || control1.state == ControllerX1::GO_UP_PICKUP_Z02) {
            want_cargo_on_Z1 = false;
            want_cargo_on_Z2 = false;
            digitalWrite(PIN_LED_Z0_Z1, LOW);
            digitalWrite(PIN_LED_Z0_Z2, LOW);
            Serial.println("*** The cargo pickuped from Z0 pos ***");

        } else if (control1.state == ControllerX1::GO_DROP_Z1) {
            cargo_on_Z1 = true;
            digitalWrite(PIN_LED_Z1, HIGH);
            Serial.println("*** The cargo on Z1 pos ***");

        } else if (control1.state == ControllerX1::GO_DROP_Z2) {
            cargo_on_Z2 = true;
            digitalWrite(PIN_LED_Z2, HIGH);
            Serial.println("*** The cargo on Z2 pos ***");
        }

        // Checking for new target
        bool new_target = target_angle_X1[0] == (int)out.go_base && target_angle_X1[1] == (int)out.go_shoulder &&
                          target_angle_X1[2] == (int)out.go_elbow && target_angle_X1[3] == (int)out.go_wrist_ver &&
                          target_angle_X1[4] == (int)out.go_wrist_rot && target_angle_X1[5] == (int)out.go_gripper;

        // Send controls to the plant
        target_angle_X1[0] = (int)out.go_base;
        target_angle_X1[1] = (int)out.go_shoulder;
        target_angle_X1[2] = (int)out.go_elbow;
        target_angle_X1[3] = (int)out.go_wrist_ver;
        target_angle_X1[4] = (int)out.go_wrist_rot;
        target_angle_X1[5] = (int)out.go_gripper;
        send_target_angles(I2C_ADDR_SLAVE_X1, target_angle_X1);

        // Print out controller 1
        if (new_target){
            print_out_control(1, target_angle_X1);
        }

    }

    // X2
    {
        // Read the state of the plant
        read_current_angles(I2C_ADDR_SLAVE_X2, current_angle_X2);

        // Calculate inputs for the controller
        bool is_done_m1 = (current_angle_X2[0] == target_angle_X2[0]);
        bool is_done_m2 = (current_angle_X2[1] == target_angle_X2[1]);
        bool is_done_m3 = (current_angle_X2[2] == target_angle_X2[2]);
        bool is_done_m4 = (current_angle_X2[3] == target_angle_X2[3]);
        bool is_done_m5 = (current_angle_X2[4] == target_angle_X2[4]);
        bool is_done_m6 = (current_angle_X2[5] == target_angle_X2[5]);

        // Print inputs controller 2
        print_input_control(
            2, current_angle_X2,
            is_done_m1, is_done_m2,
            is_done_m3, is_done_m4,
            is_done_m5, is_done_m6);

        // Execute the controller
        auto out = control2.go_step(
            cargo_is_processed_Z1, cargo_is_processed_Z2,
            is_done_m1, is_done_m2,
            is_done_m3, is_done_m4,
            is_done_m5, is_done_m6);

        if (control2.state == ControllerX2::GO_UP_PICKUP_Z1) {
            cargo_is_processed_Z1 = false;
            digitalWrite(PIN_LED_PROCESSED_Z1, LOW);
            cargo_on_Z1 = false;
            digitalWrite(PIN_LED_Z1, LOW);

        } else if (control2.state == ControllerX2::GO_UP_PICKUP_Z2) {
            cargo_is_processed_Z2 = false;
            digitalWrite(PIN_LED_PROCESSED_Z2, LOW);
            cargo_on_Z2 = false;
            digitalWrite(PIN_LED_Z2, LOW);
        }

        // Checking for new target
        bool new_target = target_angle_X1[0] == (int)out.go_base && target_angle_X1[1] == (int)out.go_shoulder &&
                          target_angle_X1[2] == (int)out.go_elbow && target_angle_X1[3] == (int)out.go_wrist_ver &&
                          target_angle_X1[4] == (int)out.go_wrist_rot && target_angle_X1[5] == (int)out.go_gripper;

        // Send controls to the plant
        target_angle_X2[0] = (int)out.go_base;
        target_angle_X2[1] = (int)out.go_shoulder;
        target_angle_X2[2] = (int)out.go_elbow;
        target_angle_X2[3] = (int)out.go_wrist_ver;
        target_angle_X2[4] = (int)out.go_wrist_rot;
        target_angle_X2[5] = (int)out.go_gripper;
        send_target_angles(I2C_ADDR_SLAVE_X2, target_angle_X2);

        // Print out controller 2
        if (new_target){
            print_out_control(2, target_angle_X2);
        }
    }
    delay(100);
}
