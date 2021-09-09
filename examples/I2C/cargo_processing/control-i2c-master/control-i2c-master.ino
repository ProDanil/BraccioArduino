#include <Wire.h>

#include "Controller1.hpp"
#include "Controller2.hpp"
// #include "Controller3.hpp"

// Button pins
#define PIN_INPUT_TO_Z12 A0  // button pins for transverring cargo from Z0 to Z1 or Z2 and reset button
#define PIN_Z12_TO_OUT A1    // button pins for transverring cargo from Z1 or Z2 to Z3
// Led pins
#define PIN_LED_Z0_Z1 2          // the cargo on Z0 needs to be transferred on Z1
#define PIN_LED_Z0_Z2 4          // the cargo on Z0 needs to be transferred on Z2
#define PIN_LED_PROCESSED_Z1 A2  // the processed cargo on Z1 needs to be transferred on Z3
#define PIN_LED_PROCESSED_Z2 A3  // the processed cargo on Z2 needs to be transferred on Z3
#define PIN_LED_Z1 7             // the cargo on Z1
#define PIN_LED_Z2 8             // the cargo on Z2
// I2C slave addresses
#define I2C_ADDR_SLAVE_X1 11
#define I2C_ADDR_SLAVE_X2 12

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

    } else if (analogRead(PIN_INPUT_TO_Z12) >= 500 && analogRead(PIN_INPUT_TO_Z12) < 800) {
        want_cargo_on_Z2 = true;
        digitalWrite(PIN_LED_Z0_Z2, HIGH);

    } else if (analogRead(PIN_INPUT_TO_Z12) >= 800) {
        want_cargo_on_Z1 = true;
        digitalWrite(PIN_LED_Z0_Z1, HIGH);
    }

    // Z1 and Z2
    if (analogRead(PIN_Z12_TO_OUT) >= 100 && analogRead(PIN_Z12_TO_OUT) < 700) {
        cargo_is_processed_Z2 = true;
        analogWrite(PIN_LED_PROCESSED_Z2, 1023);

    } else if (analogRead(PIN_Z12_TO_OUT) >= 700) {
        cargo_is_processed_Z1 = true;
        analogWrite(PIN_LED_PROCESSED_Z2, 1023);

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

        // Execute the controller
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

        } else if (control1.state == ControllerX1::GO_DROP_Z1) {
            cargo_on_Z1 = true;
            digitalWrite(PIN_LED_Z1, HIGH);

        } else if (control1.state == ControllerX1::GO_DROP_Z2) {
            cargo_on_Z2 = true;
            digitalWrite(PIN_LED_Z2, HIGH);
        }

        // Send controls to the plant
        target_angle_X1[0] = (int)out.go_base;
        target_angle_X1[1] = (int)out.go_shoulder;
        target_angle_X1[2] = (int)out.go_elbow;
        target_angle_X1[3] = (int)out.go_wrist_ver;
        target_angle_X1[4] = (int)out.go_wrist_rot;
        target_angle_X1[5] = (int)out.go_gripper;
        send_target_angles(I2C_ADDR_SLAVE_X1, target_angle_X1);
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

        // Execute the controller
        auto out = control2.go_step(
            cargo_is_processed_Z1, cargo_is_processed_Z2,
            is_done_m1, is_done_m2,
            is_done_m3, is_done_m4,
            is_done_m5, is_done_m6);

        if (control2.state == ControllerX2::GO_UP_PICKUP_Z1) {
            cargo_is_processed_Z1 = false;
            analogWrite(PIN_LED_PROCESSED_Z1, 0);
            cargo_on_Z1 = false;
            digitalWrite(PIN_LED_Z1, LOW);

        } else if (control2.state == ControllerX2::GO_UP_PICKUP_Z1) {
            cargo_is_processed_Z2 = false;
            analogWrite(PIN_LED_PROCESSED_Z2, 0);
            cargo_on_Z2 = true;
            digitalWrite(PIN_LED_Z2, HIGH);
        }

        // Send controls to the plant
        target_angle_X2[0] = (int)out.go_base;
        target_angle_X2[1] = (int)out.go_shoulder;
        target_angle_X2[2] = (int)out.go_elbow;
        target_angle_X2[3] = (int)out.go_wrist_ver;
        target_angle_X2[4] = (int)out.go_wrist_rot;
        target_angle_X2[5] = (int)out.go_gripper;
        send_target_angles(I2C_ADDR_SLAVE_X2, target_angle_X2);
    }
}
