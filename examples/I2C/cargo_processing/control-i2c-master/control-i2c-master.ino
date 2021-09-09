#include <Wire.h>

#include "Controller1.hpp"
#include "Controller2.hpp"
// #include "Controller3.hpp"

//button pins
#define PIN_INPUT_TO_Z12 A0 // button pins for transverring cargo from Z0 to Z1 or Z2 and reset button
#define PIN_Z12_TO_OUT A1   // button pins for transverring cargo from Z1 or Z2 to Z3
// Lamp pins
#define PIN_LAMP_Z0_Z1 2 // the cargo on Z0 needs to be transferred on Z1
#define PIN_LAMP_Z0_Z2 4 // the cargo on Z0 needs to be transferred on Z2
#define PIN_LAMP_PROCESSED_Z1 A2 // the processed cargo on Z1 needs to be transferred on Z3
#define PIN_LAMP_PROCESSED_Z2 A3 // the processed cargo on Z2 needs to be transferred on Z3
#define PIN_LAMP_Z1 7    // the cargo on Z1
#define PIN_LAMP_Z2 8    // the cargo on Z2

// X1
#define I2C_SLAVE1_ADDRESS 11

// X2
#define I2C_SLAVE2_ADDRESS 12

// X3
// TODO

ControllerX1 control1;
ControllerX2 control2;
// ControllerX3 control3;

int current_angle_X1[6] = {0, 0, 0, 0, 0, 0};
int current_angle_X2[6] = {0, 0, 0, 0, 0, 0};

uint8_t send_angle_X1[6] = {0, 0, 0, 0, 0, 0};
uint8_t send_angle_X2[6] = {0, 0, 0, 0, 0, 0};

bool want_cargo_on_Z1 = false;
bool want_cargo_on_Z2 = false;
bool cargo_on_Z1 = false;
bool cargo_on_Z2 = false;
bool cargo_is_processed_Z1 = false;
bool cargo_is_processed_Z2 = false;

void setup() {
    Wire.begin(); // begin as master
    Serial.begin(9600);
        // setup pins
    pinMode(PIN_INPUT_TO_Z12, INPUT);
    pinMode(PIN_Z12_TO_OUT, INPUT);
    pinMode(PIN_LAMP_Z0_Z1, OUTPUT);
    pinMode(PIN_LAMP_Z0_Z2, OUTPUT);
    pinMode(PIN_LAMP_PROCESSED_Z1, OUTPUT);
    pinMode(PIN_LAMP_PROCESSED_Z2, OUTPUT);
    pinMode(PIN_LAMP_Z1, OUTPUT);
    pinMode(PIN_LAMP_Z2, OUTPUT);

}

void read_angles(slave_address){

    Wire.requestFrom(slave_address, 6);

    if (slave_address == 11){
        for (int i=0; i<6; i++){
            current_angle_X1[i] = Wire.read();
        }

        Serial.println("recieved state X1: ");
        for (int i=0; i<6; i++){
            Serial.print(current_angle_X1[i]);
            Serial.print(" ");
        }
        Serial.println();

    } else {
        for (int i=0; i<6; i++){
            current_angle_X2[i] = Wire.read();
        }

        Serial.println("recieved state X2: ");
        for (int i=0; i<6; i++){
            Serial.print(current_angle_X2[i]);
            Serial.print(" ");
        }
        Serial.println();
    }

}

void send_angles(slave_address){

    Wire.beginTransmission(slave_address);

    if (slave_address == 11){
        send_angle_X1[0] = (int)out.go_base;
        send_angle_X1[1] = (int)out.go_shoulder;
        send_angle_X1[2] = (int)out.go_elbow;
        send_angle_X1[3] = (int)out.go_wrist_ver;
        send_angle_X1[4] = (int)out.go_wrist_rot;
        send_angle_X1[5] = (int)out.go_gripper;

        Wire.write(send_angle_X1, 6);

    }  else {
        send_angle_X2[0] = (int)out.go_base;
        send_angle_X2[1] = (int)out.go_shoulder;
        send_angle_X2[2] = (int)out.go_elbow;
        send_angle_X2[3] = (int)out.go_wrist_ver;
        send_angle_X2[4] = (int)out.go_wrist_rot;
        send_angle_X2[5] = (int)out.go_gripper;

        Wire.write(send_angle_X2, 6);

    }

    Wire.endTransmission();

}

void loop() {
    // Read buttons

    // Z0
    if (analogRead(PIN_INPUT_TO_Z12) => 200 && analogRead(PIN_INPUT_TO_Z12) < 500){
        want_cargo_on_Z1 = false;
        want_cargo_on_Z2 = false;
        digitalWrite(PIN_LAMP_Z0_Z1, LOW);
        digitalWrite(PIN_LAMP_Z0_Z2, LOW);

    } else if (analogRead(PIN_INPUT_TO_Z12) => 500 && analogRead(PIN_INPUT_TO_Z12) < 800){
        want_cargo_on_Z2 = true;
        digitalWrite(PIN_LAMP_Z0_Z2, HIGH);

    } else if (analogRead(PIN_INPUT_TO_Z12) => 800){
        want_cargo_on_Z1 = true;
        digitalWrite(PIN_LAMP_Z0_Z1, HIGH);
    }

    // Z1 and Z2
    if (analogRead(PIN_Z12_TO_OUT) => 100 && analogRead(PIN_Z12_TO_OUT) < 700){
        cargo_is_processed_Z2 = true;
        analogWrite(PIN_LAMP_PROCESSED_Z2, 1023);

    } else if (analogRead(PIN_Z12_TO_OUT) => 700) {
        cargo_is_processed_Z1 = true;
        analogWrite(PIN_LAMP_PROCESSED_Z2, 1023);

    } /*else if (analogRead(PIN_Z12_TO_OUT) => 800){
        cargoZ1_is_processed = false;
        cargoZ2_is_processed = false;
        digitalWrite(PIN_LAMP_Z1, LOW);
        digitalWrite(PIN_LAMP_Z2, LOW);
    } */


    // X1
    {
        // Read the state of the plant
        read_angles(I2C_SLAVE1_ADDRESS);

        // Calculate inputs for the controller
        bool is_done_m1 = (current_angle_X1[0] == send_angle_X1[0]);
        bool is_done_m2 = (current_angle_X1[1] == send_angle_X1[1]);
        bool is_done_m3 = (current_angle_X1[2] == send_angle_X1[2]);
        bool is_done_m4 = (current_angle_X1[3] == send_angle_X1[3]);
        bool is_done_m5 = (current_angle_X1[4] == send_angle_X1[4]);
        bool is_done_m6 = (current_angle_X1[5] == send_angle_X1[5]);

        // Execute the controller
        auto out = control1.go_step(
            want_cargo_on_Z1, want_cargo_on_Z2,
            cargo_on_Z1, cargo_on_Z2,
            is_done_m1, is_done_m2,
            is_done_m3, is_done_m4,
            is_done_m5, is_done_m6);

        if (control1.state == GO_UP_PICKUP_Z01 || control1.state == GO_UP_PICKUP_Z02){
            want_cargo_on_Z1 = false;
            want_cargo_on_Z2 = false;
            digitalWrite(PIN_LAMP_Z0_Z1, LOW);
            digitalWrite(PIN_LAMP_Z0_Z2, LOW);

        } else if (control1.state == GO_DROP_Z1){
            cargo_on_Z1 = true;
            digitalWrite(PIN_LAMP_Z1, HIGH);

        } else if (control1.state == GO_DROP_Z2){
            cargo_on_Z2 = true;
            digitalWrite(PIN_LAMP_Z2, HIGH);
        }

        // Send controls to the plant
        send_angles(I2C_SLAVE1_ADDRESS);

    }

    // X2
    {
        // Read the state of the plant
        read_angles(I2C_SLAVE2_ADDRESS);

        // Calculate inputs for the controller
        bool is_done_m1 = (current_angle_X2[0] == send_angle_X2[0]);
        bool is_done_m2 = (current_angle_X2[1] == send_angle_X2[1]);
        bool is_done_m3 = (current_angle_X2[2] == send_angle_X2[2]);
        bool is_done_m4 = (current_angle_X2[3] == send_angle_X2[3]);
        bool is_done_m5 = (current_angle_X2[4] == send_angle_X2[4]);
        bool is_done_m6 = (current_angle_X2[5] == send_angle_X2[5]);

        // Execute the controller
        auto out = control2.go_step(
            cargo_is_processed_Z1, cargo_is_processed_Z2,
            is_done_m1, is_done_m2,
            is_done_m3, is_done_m4,
            is_done_m5, is_done_m6);

        if (control2.state == GO_UP_PICKUP_Z1){
            cargo_is_processed_Z1 = false;
            analogWrite(PIN_LAMP_PROCESSED_Z1, 0);
            cargo_on_Z1 = false;
            digitalWrite(PIN_LAMP_Z1, LOW);

        } else if (control2.state == GO_UP_PICKUP_Z1){
            cargo_is_processed_Z2 = false;
            analogWrite(PIN_LAMP_PROCESSED_Z2, 0);
            cargo_on_Z2 = true;
            digitalWrite(PIN_LAMP_Z2, HIGH);

        }

        // Send controls to the plant
        send_angles(I2C_SLAVE2_ADDRESS);
    }
}

