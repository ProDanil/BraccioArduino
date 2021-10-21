// ВЗАИМОДЕЙСТВИЕ МАНИПУЛЯТОРОВ. Mutex - I2C-master //

/* 3 манипулятора перекладывают груз на один выход
*/

#include <Wire.h>

#include "Controller1.hpp"
#include "Controller2.hpp"
#include "Controller3.hpp"

#define DEBUG_LOG
// #define DEBUG_MESSAGE

// #ifdef DEBUG_LOG
// #define LOG(mess) Serial.print(mess)
// #else
// #define LOG(mess)
// #endif

#ifdef DEBUG_MESSAGE
#define MESSAGE(mess) Serial.print(mess)
#else
#define MESSAGE(mess)
#endif

#define INPUT_X1_PIN A0  // input signal about cargo on Z1
#define INPUT_X2_PIN A1
#define INPUT_X3_PIN A2
#define LAMP_X1_PIN 2
#define LAMP_X2_PIN 3
#define LAMP_X3_PIN 4
#define MUTEX_LAMP_PIN 13

// I2C slave addresses
#define I2C_ADDR_SLAVE_X1 11
#define I2C_ADDR_SLAVE_X2 12
#define I2C_ADDR_SLAVE_X3 13

#define TIMER 1000  // ms logs timing

ControllerX1 control1;
ControllerX2 control2;
ControllerX3 control3;

ControllerX1::Input last_input_X1;
ControllerX2::Input last_input_X2;
ControllerX3::Input last_input_X3;

ControllerX1::Out last_out_X1;
ControllerX2::Out last_out_X2;
ControllerX3::Out last_out_X3;

uint8_t current_angle_X1[6];
uint8_t current_angle_X2[6];
uint8_t current_angle_X3[6];
uint8_t target_angle_X1[6];
uint8_t target_angle_X2[6];
uint8_t target_angle_X3[6];

bool occupied_Z1 = false;
bool occupied_Z2 = false;
bool occupied_Z3 = false;

bool is_prev_passive_X1;
bool is_prev_passive_X2;
bool is_prev_passive_X3;

bool printed_done[3] = {false, false, false};

unsigned long timing;
unsigned iter = 0;

void setup() {
    Wire.begin();  // begin as master
    Serial.begin(9600);
    pinMode(INPUT_X1_PIN, INPUT);
    pinMode(INPUT_X2_PIN, INPUT);
    pinMode(INPUT_X3_PIN, INPUT);
    pinMode(LAMP_X1_PIN, OUTPUT);
    pinMode(LAMP_X2_PIN, OUTPUT);
    pinMode(LAMP_X3_PIN, OUTPUT);
    pinMode(MUTEX_LAMP_PIN, OUTPUT);
    digitalWrite(LAMP_X1_PIN, LOW);
    digitalWrite(LAMP_X2_PIN, LOW);
    digitalWrite(LAMP_X3_PIN, LOW);
    digitalWrite(MUTEX_LAMP_PIN, LOW);
}

struct Mutex {
    // `acquired == 0` means "not acquired"
    // `acquired == i` means "acquired for i-th client"
    int acquired = 0;
    byte mutex_flag = 0;

    void acquire_blocking(int i) {
        while (acquired != 0) {
            /* spin lock */
        }
        acquired = i;
    }

    void try_acquire(bool X1, bool X2, bool X3) {
        if (acquired == 0) {
            if (X1) {
                if (X2) {
                    if (mutex_flag == 0) {
                        MESSAGE("GO_X1");
                        mutex_flag = 1;
                        acquired = 1;

                    } else if (mutex_flag == 1) {
                        if (X3) {
                            mutex_flag = 2;
                            MESSAGE("GO_X2");
                            acquired = 2;

                        } else {
                            mutex_flag = 0;
                            MESSAGE("GO_X2");
                            acquired = 2;
                        }

                    } else if (mutex_flag == 2) {
                        mutex_flag = 0;
                        if (X3) {
                            MESSAGE("GO_X3");
                            acquired = 3;
                        }
                        acquired = 0;
                    }
                } else if (X3) {
                    if (mutex_flag == 0) {
                        MESSAGE("GO_X1");
                        mutex_flag = 2;
                        acquired = 1;

                    } else {
                        MESSAGE("GO_X3");
                        mutex_flag = 0;
                        acquired = 3;
                    }

                } else {
                    MESSAGE("GO_X1");
                    acquired = 1;
                }

            } else if (X2) {
                if (X3) {
                    if (mutex_flag = 2) {
                        MESSAGE("GO_X3");
                        mutex_flag = 0;
                        acquired = 3;

                    } else {
                        mutex_flag = 2;
                        MESSAGE("GO_X2");
                        acquired = 2;
                    }

                } else {
                    mutex_flag = 0;
                    MESSAGE("GO_X2");
                    acquired = 2;
                }

            } else if (X3) {
                MESSAGE("GO_X3");
                mutex_flag = 0;
                acquired = 3;
            }

        } else {
            // do nothing
        }
    }

    void release() {
        acquired = 0;
    }
};

Mutex mutex;

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
    bool want_cargo_on_out,
    bool is_acquired,
    bool is_done_m1,
    bool is_done_m2,
    bool is_done_m3,
    bool is_done_m4,
    bool is_done_m5,
    bool is_done_m6) {
    bool all_is_done = is_done_m1 && is_done_m2 && is_done_m3 && is_done_m4 && is_done_m5 && is_done_m6;
    if (!all_is_done) {
        printed_done[controller - 1] = false;
        if (millis() - timing > TIMER) {
            timing = millis();
            Serial.print("     INPUTS CONTROLLER");
            Serial.println(controller);

            Serial.println("Want cargo on out: " + String(want_cargo_on_out));
            Serial.println("is acquired: " + String(is_acquired));

            Serial.print("Current angles: (");
            Serial.print(String(current_angle[0]) + ", " + String(current_angle[1]) + ", " +
                         String(current_angle[2]) + ", " + String(current_angle[3]) + ", " +
                         String(current_angle[4]) + ", " + String(current_angle[5]));
            Serial.println(")");

            Serial.println("[M1.done] " + String(is_done_m1) + " [M2.done] " + String(is_done_m2) +
                           " [M3.done] " + String(is_done_m3) + " [M4.done] " + String(is_done_m4) +
                           " [M5.done] " + String(is_done_m5) + " [M6.done] " + String(is_done_m6));
        }

    } else if ((controller == 1 && !printed_done[0]) || (controller == 2 && !printed_done[1]) || (controller == 3 && !printed_done[2])) {
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

void log_everything(int control,
                    bool want_cargo_on_out,
                    bool is_acquired,
                    bool is_done_m1,
                    bool is_done_m2,
                    bool is_done_m3,
                    bool is_done_m4,
                    bool is_done_m5,
                    bool is_done_m6,
                    String state,
                    bool want_to_acquire,
                    bool want_to_release,
                    int base,
                    int shoulder,
                    int elbow,
                    int wrist_ver,
                    int wrist_rot,
                    int gripper) {
    Serial.print("[iter #" + String(iter) + "]");
    Serial.print(" CONTROLLER_" + String(control));
    Serial.print(" want_cargo_on_out=" + String(want_cargo_on_out));
    Serial.print(" is_acquired=" + String(is_acquired));
    Serial.print(" is_done_m1=" + String(is_done_m1));
    Serial.print(" is_done_m2=" + String(is_done_m2));
    Serial.print(" is_done_m3=" + String(is_done_m3));
    Serial.print(" is_done_m4=" + String(is_done_m4));
    Serial.print(" is_done_m5=" + String(is_done_m5));
    Serial.print(" is_done_m6=" + String(is_done_m6));
    Serial.print(" state=" + state);
    Serial.print(" want_to_acquire=" + String(want_to_acquire));
    Serial.print(" want_to_release=" + String(want_to_release));
    Serial.print(" base=" + String(base));
    Serial.print(" shoulder=" + String(shoulder));
    Serial.print(" elbow=" + String(elbow));
    Serial.print(" wrist_ver=" + String(wrist_ver));
    Serial.print(" wrist_rot=" + String(wrist_rot));
    Serial.println(" gripper=" + String(gripper));
}

/*

- read buttons, capture RISING, cache
- for each controller X:
    - build and fill ControllerX::Input
    - execute the controller
    - retrieve output
    - maybe (if necessary) log the execution state (inputs/outputs in one line)
    - update prev_input_X
    - update last_out_X
- interact with mutex
- send controls to plants

*/

void loop() {
    iter++;

    // Read buttons
    if (analogRead(INPUT_X1_PIN) >= 100 && analogRead(INPUT_X1_PIN) < 700) {
        MESSAGE("*** The cargo is removed from Z1 pos ***");
        occupied_Z1 = false;
        digitalWrite(LAMP_X1_PIN, LOW);
    } else if (analogRead(INPUT_X1_PIN) >= 700) {
        MESSAGE("*** The cargo on Z1 pos ***");
        occupied_Z1 = true;
        digitalWrite(LAMP_X1_PIN, HIGH);
    }

    if (analogRead(INPUT_X2_PIN) >= 100 && analogRead(INPUT_X2_PIN) < 700) {
        MESSAGE("*** The cargo is removed from Z2 pos ***");
        occupied_Z2 = false;
        digitalWrite(LAMP_X2_PIN, LOW);
    } else if (analogRead(INPUT_X2_PIN) >= 700) {
        MESSAGE("*** The cargo on Z2 pos ***");
        occupied_Z2 = true;
        digitalWrite(LAMP_X2_PIN, HIGH);
    }

    if (analogRead(INPUT_X3_PIN) >= 100 && analogRead(INPUT_X3_PIN) < 700) {
        MESSAGE("*** The cargo is removed from Z3 pos ***");
        occupied_Z3 = false;
        digitalWrite(LAMP_X3_PIN, LOW);
    } else if (analogRead(INPUT_X3_PIN) >= 700) {
        MESSAGE("*** The cargo on Z3 pos ***");
        occupied_Z3 = true;
        digitalWrite(LAMP_X3_PIN, HIGH);
    }

    // ==========================================

    // Controller 1
    {
        // Inputs
        ControllerX1::Input input;
        input.want_cargo_on_out = occupied_Z1;
        input.is_acquired = (mutex.acquired == 1);
        read_current_angles(I2C_ADDR_SLAVE_X1, current_angle_X1);
        input.is_done_m1 = (current_angle_X1[0] == target_angle_X1[0]);
        input.is_done_m2 = (current_angle_X1[1] == target_angle_X1[1]);
        input.is_done_m3 = (current_angle_X1[2] == target_angle_X1[2]);
        input.is_done_m4 = (current_angle_X1[3] == target_angle_X1[3]);
        input.is_done_m5 = (current_angle_X1[4] == target_angle_X1[4]);
        input.is_done_m6 = (current_angle_X1[5] == target_angle_X1[5]);

#ifdef DEBUG_MESSAGE
        // Print inputs controller 1
        print_input_control(
            1, current_angle_X1,
            input.want_cargo_on_out, input.is_acquired,
            input.is_done_m1, input.is_done_m2,
            input.is_done_m3, input.is_done_m4,
            input.is_done_m5, input.is_done_m6);
#endif

        // Execute the controller
        bool is_active = control1.go_step(input);
        auto out = control1.out;

#ifdef DEBUG_LOG
        bool is_same_input = (input.want_cargo_on_out == last_input_X1.want_cargo_on_out &&
                              input.is_acquired == last_input_X1.is_acquired &&
                              input.is_done_m1 == last_input_X1.is_done_m1 &&
                              input.is_done_m2 == last_input_X1.is_done_m2 &&
                              input.is_done_m3 == last_input_X1.is_done_m3 &&
                              input.is_done_m4 == last_input_X1.is_done_m4 &&
                              input.is_done_m5 == last_input_X1.is_done_m5 &&
                              input.is_done_m6 == last_input_X1.is_done_m6);

        if (is_active || !is_same_input || !is_prev_passive_X1) {
            if (is_active) {
                is_prev_passive_X1 = false;
            }
            log_everything(1, input.want_cargo_on_out,
                           input.is_acquired,
                           input.is_done_m1,
                           input.is_done_m2,
                           input.is_done_m3,
                           input.is_done_m4,
                           input.is_done_m5,
                           input.is_done_m6,
                           control1.state2string(control1.state),
                           out.want_to_acquire,
                           out.want_to_release,
                           (int)out.go_base,
                           (int)out.go_shoulder,
                           (int)out.go_elbow,
                           (int)out.go_wrist_ver,
                           (int)out.go_wrist_rot,
                           (int)out.go_gripper);
        } else if (!is_active && !is_prev_passive_X1) {
            is_prev_passive_X1 = true;
        }
#endif

        if (control1.state == ControllerX1::GO_UP_CLENCHED_Z0) {
            MESSAGE("*** The cargo pickuped from Z1 pos ***");
            occupied_Z1 = false;
            digitalWrite(LAMP_X1_PIN, LOW);
        }

        // update prev_input
        last_input_X1 = input;
        // update last_out
        last_out_X1 = out;
    }

    // ==========================================

    // Controller 2
    {
        // Inputs
        ControllerX2::Input input;
        input.want_cargo_on_out = occupied_Z2;
        input.is_acquired = (mutex.acquired == 2);
        read_current_angles(I2C_ADDR_SLAVE_X2, current_angle_X2);
        input.is_done_m1 = (current_angle_X2[0] == target_angle_X2[0]);
        input.is_done_m2 = (current_angle_X2[1] == target_angle_X2[1]);
        input.is_done_m3 = (current_angle_X2[2] == target_angle_X2[2]);
        input.is_done_m4 = (current_angle_X2[3] == target_angle_X2[3]);
        input.is_done_m5 = (current_angle_X2[4] == target_angle_X2[4]);
        input.is_done_m6 = (current_angle_X2[5] == target_angle_X2[5]);

#ifdef DEBUG_MESSAGE
        // Print inputs controller 2
        print_input_control(
            2, current_angle_X2,
            input.want_cargo_on_out, input.is_acquired,
            input.is_done_m1, input.is_done_m2,
            input.is_done_m3, input.is_done_m4,
            input.is_done_m5, input.is_done_m6);
#endif

        // Execute the controller
        bool is_active = control2.go_step(input);
        auto out = control2.out;

#ifdef DEBUG_LOG
        bool is_same_input = (input.want_cargo_on_out == last_input_X2.want_cargo_on_out &&
                              input.is_acquired == last_input_X2.is_acquired &&
                              input.is_done_m1 == last_input_X2.is_done_m1 &&
                              input.is_done_m2 == last_input_X2.is_done_m2 &&
                              input.is_done_m3 == last_input_X2.is_done_m3 &&
                              input.is_done_m4 == last_input_X2.is_done_m4 &&
                              input.is_done_m5 == last_input_X2.is_done_m5 &&
                              input.is_done_m6 == last_input_X2.is_done_m6);

        if (is_active || !is_same_input || !is_prev_passive_X2) {
            if (is_active) {
                is_prev_passive_X2 = false;
            }
            log_everything(2, input.want_cargo_on_out,
                           input.is_acquired,
                           input.is_done_m1,
                           input.is_done_m2,
                           input.is_done_m3,
                           input.is_done_m4,
                           input.is_done_m5,
                           input.is_done_m6,
                           control2.state2string(control2.state),
                           out.want_to_acquire,
                           out.want_to_release,
                           (int)out.go_base,
                           (int)out.go_shoulder,
                           (int)out.go_elbow,
                           (int)out.go_wrist_ver,
                           (int)out.go_wrist_rot,
                           (int)out.go_gripper);
        } else if (!is_active && !is_prev_passive_X2) {
            is_prev_passive_X2 = true;
        }
#endif

        if (control2.state == ControllerX2::GO_UP_CLENCHED_Z0) {
            MESSAGE("*** The cargo picked up from Z2 pos ***");
            occupied_Z2 = false;
            digitalWrite(LAMP_X2_PIN, LOW);
        }

        // update prev_input
        last_input_X2 = input;
        // update last_out
        last_out_X2 = out;
    }

    // ==========================================

    // Controller 3
    {
        // Inputs
        ControllerX3::Input input;
        input.want_cargo_on_out = occupied_Z3;
        input.is_acquired = (mutex.acquired == 3);
        read_current_angles(I2C_ADDR_SLAVE_X3, current_angle_X3);
        input.is_done_m1 = (current_angle_X3[0] == target_angle_X3[0]);
        input.is_done_m2 = (current_angle_X3[1] == target_angle_X3[1]);
        input.is_done_m3 = (current_angle_X3[2] == target_angle_X3[2]);
        input.is_done_m4 = (current_angle_X3[3] == target_angle_X3[3]);
        input.is_done_m5 = (current_angle_X3[4] == target_angle_X3[4]);
        input.is_done_m6 = (current_angle_X3[5] == target_angle_X3[5]);

#ifdef DEBUG_MESSAGE
        // Print inputs controller 3
        print_input_control(
            3, current_angle_X3,
            input.want_cargo_on_out, input.is_acquired,
            input.is_done_m1, input.is_done_m2,
            input.is_done_m3, input.is_done_m4,
            input.is_done_m5, input.is_done_m6);
#endif

        // Execute the controller
        bool is_active = control3.go_step(input);
        auto out = control3.out;

#ifdef DEBUG_LOG
        bool is_same_input = (input.want_cargo_on_out == last_input_X3.want_cargo_on_out &&
                              input.is_acquired == last_input_X3.is_acquired &&
                              input.is_done_m1 == last_input_X3.is_done_m1 &&
                              input.is_done_m2 == last_input_X3.is_done_m2 &&
                              input.is_done_m3 == last_input_X3.is_done_m3 &&
                              input.is_done_m4 == last_input_X3.is_done_m4 &&
                              input.is_done_m5 == last_input_X3.is_done_m5 &&
                              input.is_done_m6 == last_input_X3.is_done_m6);

        if (is_active || !is_same_input || !is_prev_passive_X3) {
            if (is_active) {
                is_prev_passive_X3 = false;
            }
            log_everything(3, input.want_cargo_on_out,
                           input.is_acquired,
                           input.is_done_m1,
                           input.is_done_m2,
                           input.is_done_m3,
                           input.is_done_m4,
                           input.is_done_m5,
                           input.is_done_m6,
                           control3.state2string(control3.state),
                           out.want_to_acquire,
                           out.want_to_release,
                           (int)out.go_base,
                           (int)out.go_shoulder,
                           (int)out.go_elbow,
                           (int)out.go_wrist_ver,
                           (int)out.go_wrist_rot,
                           (int)out.go_gripper);
        } else if (!is_active && !is_prev_passive_X3) {
            is_prev_passive_X3 = true;
        }
#endif

        if (control3.state == ControllerX3::GO_UP_CLENCHED_Z0) {
            MESSAGE("*** The cargo picked up from Z3 pos ***");
            occupied_Z3 = false;
            digitalWrite(LAMP_X3_PIN, LOW);
        }

        // update prev_input
        last_input_X3 = input;
        // update last_out
        last_out_X3 = out;
    }

    // ==========================================

    // Release the mutex
    if (last_out_X1.want_to_release && mutex.acquired == 1) {
        MESSAGE("*** Releasing the mutex from X1 ***");
        mutex.release();
    }
    if (last_out_X2.want_to_release && mutex.acquired == 2) {
        MESSAGE("*** Releasing the mutex from X2 ***");
        mutex.release();
    }
    if (last_out_X3.want_to_release && mutex.acquired == 3) {
        MESSAGE("*** Releasing the mutex from X3 ***");
        mutex.release();
    }

    // Try acquire the mutex
    mutex.try_acquire(last_out_X1.want_to_acquire, last_out_X2.want_to_acquire, last_out_X3.want_to_acquire);

    if (mutex.acquired != 0) {
        MESSAGE("*** Mutex was acquired for X" + String(mutex.acquired) + " ***");
        digitalWrite(MUTEX_LAMP_PIN, HIGH);
    } else {
        MESSAGE("*** Mutex is free ***");
        digitalWrite(MUTEX_LAMP_PIN, LOW);
    }

    // ==========================================

    // Checking for new target
    bool is_new_target_X1 = target_angle_X1[0] == (int)last_out_X1.go_base &&
                            target_angle_X1[1] == (int)last_out_X1.go_shoulder &&
                            target_angle_X1[2] == (int)last_out_X1.go_elbow &&
                            target_angle_X1[3] == (int)last_out_X1.go_wrist_ver &&
                            target_angle_X1[4] == (int)last_out_X1.go_wrist_rot &&
                            target_angle_X1[5] == (int)last_out_X1.go_gripper;

    // Send controls to the plant 1
    target_angle_X1[0] = (int)last_out_X1.go_base;
    target_angle_X1[1] = (int)last_out_X1.go_shoulder;
    target_angle_X1[2] = (int)last_out_X1.go_elbow;
    target_angle_X1[3] = (int)last_out_X1.go_wrist_ver;
    target_angle_X1[4] = (int)last_out_X1.go_wrist_rot;
    target_angle_X1[5] = (int)last_out_X1.go_gripper;
    send_target_angles(I2C_ADDR_SLAVE_X1, target_angle_X1);

#ifdef DEBUG_MESSAGE
    // Print out controller 1
    if (is_new_target_X1) {
        print_out_control(1, target_angle_X1);
    }
#endif

    // ==========================================

    // Checking for new target
    bool is_new_target_X2 = target_angle_X2[0] == (int)last_out_X2.go_base &&
                            target_angle_X2[1] == (int)last_out_X2.go_shoulder &&
                            target_angle_X2[2] == (int)last_out_X2.go_elbow &&
                            target_angle_X2[3] == (int)last_out_X2.go_wrist_ver &&
                            target_angle_X2[4] == (int)last_out_X2.go_wrist_rot &&
                            target_angle_X2[5] == (int)last_out_X2.go_gripper;

    // Send controls to the plant 2
    target_angle_X2[0] = (int)last_out_X2.go_base;
    target_angle_X2[1] = (int)last_out_X2.go_shoulder;
    target_angle_X2[2] = (int)last_out_X2.go_elbow;
    target_angle_X2[3] = (int)last_out_X2.go_wrist_ver;
    target_angle_X2[4] = (int)last_out_X2.go_wrist_rot;
    target_angle_X2[5] = (int)last_out_X2.go_gripper;
    send_target_angles(I2C_ADDR_SLAVE_X2, target_angle_X2);

#ifdef DEBUG_MESSAGE
    // Print out controller 2
    if (is_new_target_X2) {
        print_out_control(2, target_angle_X2);
    }
#endif

    // ==========================================

    // Checking for new target
    bool is_new_target_X3 = target_angle_X3[0] == (int)last_out_X3.go_base &&
                            target_angle_X3[1] == (int)last_out_X3.go_shoulder &&
                            target_angle_X3[2] == (int)last_out_X3.go_elbow &&
                            target_angle_X3[3] == (int)last_out_X3.go_wrist_ver &&
                            target_angle_X3[4] == (int)last_out_X3.go_wrist_rot &&
                            target_angle_X3[5] == (int)last_out_X3.go_gripper;

    // Send controls to the plant 3
    target_angle_X3[0] = (int)last_out_X3.go_base;
    target_angle_X3[1] = (int)last_out_X3.go_shoulder;
    target_angle_X3[2] = (int)last_out_X3.go_elbow;
    target_angle_X3[3] = (int)last_out_X3.go_wrist_ver;
    target_angle_X3[4] = (int)last_out_X3.go_wrist_rot;
    target_angle_X3[5] = (int)last_out_X3.go_gripper;
    send_target_angles(I2C_ADDR_SLAVE_X3, target_angle_X3);

#ifdef DEBUG_MESSAGE
    // Print out controller 3
    if (is_new_target_X3) {
        print_out_control(3, target_angle_X3);
    }
#endif

    delay(100);
}
