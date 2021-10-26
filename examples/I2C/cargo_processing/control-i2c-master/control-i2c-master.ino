#include <Wire.h>

#include "Controller1.hpp"
#include "Controller2.hpp"

#define DEBUG_LOG
#define DEBUG_MESSAGE

#ifdef DEBUG_LOG
#define LOG(mess) Serial.print(mess)
#else
#define LOG(mess)
#endif

#ifdef DEBUG_MESSAGE
#define MESSAGE(mess) Serial.print(mess)
#else
#define MESSAGE(mess)
#endif

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
#define PIN_LED_Z0_Z1 2         // the cargo on Z0 needs to be transferred on Z1
#define PIN_LED_Z0_Z2 4         // the cargo on Z0 needs to be transferred on Z2
#define PIN_LED_PROCESSED_Z1 5  // the processed cargo on Z1 needs to be transferred on Z3
#define PIN_LED_PROCESSED_Z2 6  // the processed cargo on Z2 needs to be transferred on Z3
#define PIN_LED_Z1 7            // the cargo on Z1
#define PIN_LED_Z2 8            // the cargo on Z2
// I2C slave addresses
#define I2C_ADDR_SLAVE_X1 11
#define I2C_ADDR_SLAVE_X2 12
#define TIMER 1000  // ms logs timing

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

bool request_Z1 = false;
bool request_Z2 = false;

bool occupied_Z1 = false;
bool occupied_Z2 = false;

bool processed_Z1 = false;
bool processed_Z2 = false;

bool is_prev_passive_X1;
bool is_prev_passive_X2;

bool printed_done[2] = {false, false};

unsigned long timing;
unsigned long iter = 0;

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

void print_input_control(
    int controller,
    uint8_t current_angle[6],
    bool is_done_m1, bool is_done_m2,
    bool is_done_m3, bool is_done_m4,
    bool is_done_m5, bool is_done_m6) {
    bool all_is_done = is_done_m1 && is_done_m2 && is_done_m3 && is_done_m4 && is_done_m5 && is_done_m6;
    if (!all_is_done) {
        printed_done[controller - 1] = false;
        if (millis() - timing > TIMER) {
            timing = millis();
            Serial.print("     INPUTS CONTROLLER");
            Serial.println(controller);

            if (controller == 1) {
                Serial.println("Want cargo on: Z1 " + String(last_input_X1.want_cargo_on_Z1) +
                               " Z2 " + String(last_input_X1.want_cargo_on_Z2));
                Serial.println("Cargo on: Z1 " + String(last_input_X1.cargo_on_Z1) + " Z2 " + String(last_input_X1.cargo_on_Z2));
            } else {
                Serial.println("Cargo is processed: Z1 " + String(last_input_X2.cargo_is_processed_Z1) +
                               " Z2 " + String(last_input_X2.cargo_is_processed_Z2));
            }

            Serial.print("Current angles: (");
            Serial.print(String(current_angle[0]) + ", " + String(current_angle[1]) + ", " +
                         String(current_angle[2]) + ", " + String(current_angle[3]) + ", " +
                         String(current_angle[4]) + ", " + String(current_angle[5]));
            Serial.println(")");

            Serial.println("[M1.done] " + String(is_done_m1) + " [M2.done] " + String(is_done_m2) +
                           " [M3.done] " + String(is_done_m3) + " [M4.done] " + String(is_done_m4) +
                           " [M5.done] " + String(is_done_m5) + " [M6.done] " + String(is_done_m6));
        }
    } else if ((controller == 1 && !printed_done[0]) || ((controller == 2 && !printed_done[1]))) {
        Serial.println("[M1.done] " + String(is_done_m1) + " [M2.done] " + String(is_done_m2) +
                       " [M3.done] " + String(is_done_m3) + " [M4.done] " + String(is_done_m4) +
                       " [M5.done] " + String(is_done_m5) + " [M6.done] " + String(is_done_m6));
        printed_done[controller - 1] = true;
    }
}

void print_out_control(int controller, uint8_t target_angle[6]) {
    Serial.print("     OUTPUTS CONTROLLER");
    Serial.println(controller);
    Serial.print("Target angles: (");
    Serial.print(String(target_angle[0]) + ", " + String(target_angle[1]) + ", " + String(target_angle[2]) + ", " +
                 String(target_angle[3]) + ", " + String(target_angle[4]) + ", " + String(target_angle[5]));
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

void update_leds() {
    digitalWrite(PIN_LED_Z0_Z1, request_Z1);
    digitalWrite(PIN_LED_Z0_Z2, request_Z2);
    digitalWrite(PIN_LED_Z1, occupied_Z1);
    digitalWrite(PIN_LED_Z2, occupied_Z2);
    digitalWrite(PIN_LED_PROCESSED_Z1, processed_Z1);
    digitalWrite(PIN_LED_PROCESSED_Z2, processed_Z2);
}

void log_everything(int index,
                    bool want_cargo_on_Z1,
                    bool want_cargo_on_Z2,
                    bool cargo_on_or_processed_Z1,
                    bool cargo_on_or_processed_Z2,
                    bool is_done_m1,
                    bool is_done_m2,
                    bool is_done_m3,
                    bool is_done_m4,
                    bool is_done_m5,
                    bool is_done_m6,
                    String state,
                    int base,
                    int shoulder,
                    int elbow,
                    int wrist_ver,
                    int wrist_rot,
                    int gripper) {
    Serial.print("[iter #" + String(iter) + "]");
    Serial.print(" CONTROLLER_" + String(index));
    if (index == 1){
        Serial.print(" want_cargo_on_Z1=" + String(want_cargo_on_Z1));
        Serial.print(" want_cargo_on_Z2=" + String(want_cargo_on_Z2));
        Serial.print(" cargo_on_Z1=" + String(cargo_on_or_processed_Z1));
        Serial.print(" cargo_on_Z2=" + String(cargo_on_or_processed_Z2));
    } else {
        Serial.print(" cargo_is_processed_Z1=" + String(cargo_on_or_processed_Z1));
        Serial.print(" cargo_is_processed_Z2=" + String(cargo_on_or_processed_Z2));
    }
    Serial.print(" is_done_m1=" + String(is_done_m1));
    Serial.print(" is_done_m2=" + String(is_done_m2));
    Serial.print(" is_done_m3=" + String(is_done_m3));
    Serial.print(" is_done_m4=" + String(is_done_m4));
    Serial.print(" is_done_m5=" + String(is_done_m5));
    Serial.print(" is_done_m6=" + String(is_done_m6));
    Serial.print(" state=" + String(state));
    Serial.print(" base=" + String(base));
    Serial.print(" shoulder=" + String(shoulder));
    Serial.print(" elbow=" + String(elbow));
    Serial.print(" wrist_ver=" + String(wrist_ver));
    Serial.print(" wrist_rot=" + String(wrist_rot));
    Serial.println(" gripper=" + String(gripper));
}


void loop() {
    iter++;

    // Z0
    if (analogRead(PIN_INPUT_TO_Z12) >= 200 && analogRead(PIN_INPUT_TO_Z12) < 500) {
        MESSAGE("*** The cargo is removed from Z0 pos ***");
        request_Z1 = false;
        request_Z2 = false;
    } else if (analogRead(PIN_INPUT_TO_Z12) >= 500 && analogRead(PIN_INPUT_TO_Z12) < 800) {
        MESSAGE("*** REQUEST: move the cargo to pos Z2 ***");
        request_Z2 = true;
    } else if (analogRead(PIN_INPUT_TO_Z12) >= 800) {
        MESSAGE("*** REQUEST: move the cargo to pos Z1 ***");
        request_Z1 = true;
    }

    // Z1 and Z2 processed
    if (analogRead(PIN_Z12_TO_OUT) >= 100 && analogRead(PIN_Z12_TO_OUT) < 700 && occupied_Z2) {
        MESSAGE("*** The cargo on Z2 was processed! ***");
        processed_Z2 = true;
    } else if (analogRead(PIN_Z12_TO_OUT) >= 700 && occupied_Z1) {
        MESSAGE("*** The cargo on Z1 was processed! ***");
        processed_Z1 = true;
    } /*else if (analogRead(PIN_Z12_TO_OUT) >= 800){
        processed_Z1 = false;
        processed_Z2 = false;
    } */

    update_leds();

    // ==========================================

    // Controller 1
    {
        // Inputs
        ControllerX1::Input input;
        input.want_cargo_on_Z1 = request_Z1;
        input.want_cargo_on_Z2 = request_Z2;
        input.cargo_on_Z1 = occupied_Z1;
        input.cargo_on_Z2 = occupied_Z2;
        read_current_angles(I2C_ADDR_SLAVE_X1, current_angle_X1);
        input.is_done_m1 = (current_angle_X1[0] == target_angle_X1[0]);
        input.is_done_m2 = (current_angle_X1[1] == target_angle_X1[1]);
        input.is_done_m3 = (current_angle_X1[2] == target_angle_X1[2]);
        input.is_done_m4 = (current_angle_X1[3] == target_angle_X1[3]);
        input.is_done_m5 = (current_angle_X1[4] == target_angle_X1[4]);
        input.is_done_m6 = (current_angle_X1[5] == target_angle_X1[5]);

// Print inputs controller 1
#ifdef DEBUG_MESSAGE
        print_input_control(
            1, current_angle_X1,
            input.is_done_m1, input.is_done_m2,
            input.is_done_m3, input.is_done_m4,
            input.is_done_m5, input.is_done_m6);
#endif

        // Execute the controller
        bool is_active = control1.go_step(input);
        auto out = control1.out;

#ifdef DEBUG_LOG
        bool is_same_input_X1 = (input == last_input_X1);

        // Log everything, if necessary
        if (is_active || !is_same_input_X1 || !is_prev_passive_X1) {
            if (is_active){
                is_prev_passive_X1 = false;
            }
            log_everything(1,
                            input.want_cargo_on_Z1,
                            input.want_cargo_on_Z2,
                            input.cargo_on_Z1,
                            input.cargo_on_Z2,
                            input.is_done_m1,
                            input.is_done_m2,
                            input.is_done_m3,
                            input.is_done_m4,
                            input.is_done_m5,
                            input.is_done_m6,
                            control1.state2string(control1.state),
                            (int)out.go_base,
                            (int)out.go_shoulder,
                            (int)out.go_elbow,
                            (int)out.go_wrist_ver,
                            (int)out.go_wrist_rot,
                            (int)out.go_gripper);

        }

        is_prev_passive_X1 = !is_active;

#endif

        if (control1.state == ControllerX1::GO_UP_CLENCHED_Z01 || control1.state == ControllerX1::GO_UP_CLENCHED_Z02) {
            MESSAGE("*** The cargo picked up from Z0 pos ***");
            request_Z1 = false;
            request_Z2 = false;
        } else if (control1.state == ControllerX1::GO_DROP_Z1) {
            MESSAGE("*** The cargo on Z1 pos ***");
            occupied_Z1 = true;
        } else if (control1.state == ControllerX1::GO_DROP_Z2) {
            MESSAGE("*** The cargo on Z2 pos ***");
            occupied_Z2 = true;
        }

        // Checking for new target
        bool new_target = target_angle_X1[0] == (int)out.go_base &&
                          target_angle_X1[1] == (int)out.go_shoulder &&
                          target_angle_X1[2] == (int)out.go_elbow &&
                          target_angle_X1[3] == (int)out.go_wrist_ver &&
                          target_angle_X1[4] == (int)out.go_wrist_rot &&
                          target_angle_X1[5] == (int)out.go_gripper;

        // Send controls to the plant
        target_angle_X1[0] = (int)out.go_base;
        target_angle_X1[1] = (int)out.go_shoulder;
        target_angle_X1[2] = (int)out.go_elbow;
        target_angle_X1[3] = (int)out.go_wrist_ver;
        target_angle_X1[4] = (int)out.go_wrist_rot;
        target_angle_X1[5] = (int)out.go_gripper;
        send_target_angles(I2C_ADDR_SLAVE_X1, target_angle_X1);

// Print out controller 1
#ifdef DEBUG_MESSAGE
        if (new_target) {
            print_out_control(1, target_angle_X1);
        }
#endif

        // Update last_input
        last_input_X1 = input;
        // Update last_out
        last_out_X1 = out;
    }

    // Controller 2
    {
        // Inputs
        ControllerX2::Input input;
        input.cargo_is_processed_Z1 = processed_Z1;
        input.cargo_is_processed_Z2 = processed_Z2;
        read_current_angles(I2C_ADDR_SLAVE_X2, current_angle_X2);
        input.is_done_m1 = (current_angle_X2[0] == target_angle_X2[0]);
        input.is_done_m2 = (current_angle_X2[1] == target_angle_X2[1]);
        input.is_done_m3 = (current_angle_X2[2] == target_angle_X2[2]);
        input.is_done_m4 = (current_angle_X2[3] == target_angle_X2[3]);
        input.is_done_m5 = (current_angle_X2[4] == target_angle_X2[4]);
        input.is_done_m6 = (current_angle_X2[5] == target_angle_X2[5]);

#ifdef DEBUG_MESSAGE
        // Print inputs contoller 2
        print_input_control(
            2, current_angle_X2,
            input.is_done_m1, input.is_done_m2,
            input.is_done_m3, input.is_done_m4,
            input.is_done_m5, input.is_done_m6);
#endif

        // Execute the controller
        bool is_active = control2.go_step(input);
        auto out = control2.out;

#ifdef DEBUG_LOG
        bool is_same_input_X2 = (input == last_input_X2);

        if (is_active || !is_same_input_X2 || !is_prev_passive_X2) {
            if (is_active){
                is_prev_passive_X2 = false;
            }
            log_everything(2,
                           false,                        // there isn't input.want_cargo_on_Z1
                           false,                        // there isn't input.want_cargo_on_Z2
                           input.cargo_is_processed_Z1,  // instead of cargo_on_Z1
                           input.cargo_is_processed_Z2,  // instead of cargo_on_Z2
                           input.is_done_m1,
                           input.is_done_m2,
                           input.is_done_m3,
                           input.is_done_m4,
                           input.is_done_m5,
                           input.is_done_m6,
                           control2.state2string(control2.state),
                           (int)out.go_base,
                           (int)out.go_shoulder,
                           (int)out.go_elbow,
                           (int)out.go_wrist_ver,
                           (int)out.go_wrist_rot,
                           (int)out.go_gripper);

        }

        is_prev_passive_X2 = !is_active;

#endif

        if (control2.state == ControllerX2::GO_UP_CLENCHED_Z1) {
            processed_Z1 = false;
            occupied_Z1 = false;
        } else if (control2.state == ControllerX2::GO_UP_CLENCHED_Z2) {
            processed_Z2 = false;
            occupied_Z2 = false;
        }

        // Checking for new target
        bool is_new_target = target_angle_X1[0] == (int)out.go_base &&
                             target_angle_X1[1] == (int)out.go_shoulder &&
                             target_angle_X1[2] == (int)out.go_elbow &&
                             target_angle_X1[3] == (int)out.go_wrist_ver &&
                             target_angle_X1[4] == (int)out.go_wrist_rot &&
                             target_angle_X1[5] == (int)out.go_gripper;

        // Send controls to the plant
        target_angle_X2[0] = (int)out.go_base;
        target_angle_X2[1] = (int)out.go_shoulder;
        target_angle_X2[2] = (int)out.go_elbow;
        target_angle_X2[3] = (int)out.go_wrist_ver;
        target_angle_X2[4] = (int)out.go_wrist_rot;
        target_angle_X2[5] = (int)out.go_gripper;
        send_target_angles(I2C_ADDR_SLAVE_X2, target_angle_X2);

// Print out controller 2
#ifdef DEBUG_MESSAGE
        if (is_new_target) {
            print_out_control(2, target_angle_X2);
        }
#endif

        // Update prev_input
        last_input_X2 = input;
        // Update last_out
        last_out_X2 = out;
    }

    // ==========================================

    update_leds();

    delay(100);
}
