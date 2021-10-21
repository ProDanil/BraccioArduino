// ВЗАИМОДЕЙСТВИЕ МАНИПУЛЯТОРОВ. Mutex - I2C-master //

/* 3 манипулятора перекладывают груз на один выход
*/

#include <Wire.h>

#include "Controller1.hpp"
#include "Controller2.hpp"
#include "Controller3.hpp"

#define DEBUG_LOG
//#define DEBUG_MESSAGE

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

#define INPUT_X1_PIN A0   // input signal about cargo on Z1
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

#define TIMER 1000 // ms logs timing

ControllerX1 control1;
ControllerX2 control2;
ControllerX3 control3;

ControllerX1::Input input_X1;
ControllerX1::Input prev_input_X1;
ControllerX2::Input input_X2;
ControllerX2::Input prev_input_X2;
ControllerX3::Input input_X3;
ControllerX3::Input prev_input_X3;

uint8_t current_angle_X1[6];
uint8_t current_angle_X2[6];
uint8_t current_angle_X3[6];
uint8_t target_angle_X1[6];
uint8_t target_angle_X2[6];
uint8_t target_angle_X3[6];

bool printed_done[3] = {false, false, false};
bool cargo_on_Z1;
bool cargo_on_Z2;
bool cargo_on_Z3;
bool is_prev_passive_X1;
bool is_prev_passive_X2;
bool is_prev_passive_X3;

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
        while (acquired != 0) { /* spin lock */ }
        acquired = i;
    }

    void try_acquire(bool X1, bool X2, bool X3) {
        if (acquired == 0){
            if (X1){
                if (X2){
                    if (mutex_flag == 0){
                        MESSAGE("GO_X1");
                        mutex_flag = 1;
                        acquired = 1;
    
                    } else if (mutex_flag == 1){
                        if (X3){
                            mutex_flag = 2;
                            MESSAGE("GO_X2");
                            acquired = 2;
    
                        } else {
                            mutex_flag = 0;
                            MESSAGE("GO_X2");
                            acquired = 2;
                        }
    
                    } else if (mutex_flag == 2){
                        mutex_flag = 0;
                        if (X3){
                            MESSAGE("GO_X3");
                            acquired = 3;
                        }
                        acquired = 0;
                    }
                } else if (X3){
                    if (mutex_flag == 0){
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
    
            } else if (X2){
                if (X3){
                    if (mutex_flag = 2){
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
    
            } else if (X3){
                MESSAGE("GO_X3");
                mutex_flag = 0;
                acquired = 3;
            }

        } else {
            /*do nothing*/
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

void  print_input_control(
        int controller,
        uint8_t current_angle[6],
        bool want_cargo_on_out, bool is_acquired,
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

            Serial.println("Want cargo on out: "+String(want_cargo_on_out));
            Serial.println("is acquired: "+String(is_acquired));

            Serial.print("Current angles: (");
            Serial.print(String(current_angle[0]) +", "+ String(current_angle[1]) +", "+
                         String(current_angle[2]) +", "+ String(current_angle[3]) +", "+
                         String(current_angle[4]) +", "+ String(current_angle[5]));
            Serial.println(")");

            Serial.println("[M1.done] "+ String(is_done_m1) +" [M2.done] "+ String(is_done_m2) +
                          " [M3.done] "+ String(is_done_m3) +" [M4.done] "+ String(is_done_m4) +
                          " [M5.done] "+ String(is_done_m5) +" [M6.done] "+ String(is_done_m6));
        }

    } else if ((controller == 1 && !printed_done[0]) || (controller == 2 && !printed_done[1])
            || (controller == 3 && !printed_done[2])){
        Serial.println("[M1.done] "+ String(is_done_m1) +" [M2.done] "+ String(is_done_m2) +
                      " [M3.done] "+ String(is_done_m3) +" [M4.done] "+ String(is_done_m4) +
                      " [M5.done] "+ String(is_done_m5) +" [M6.done] "+ String(is_done_m6));
        printed_done[controller-1] = true;
    }

}

void print_out_control(int controller, uint8_t target_angle[6]){
    Serial.print("     OUTPUTS CONTROLLER");
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

void log_everything(int control,
                    bool want_cargo_on_out, bool is_acquired,
                    bool is_done_m1, bool is_done_m2,
                    bool is_done_m3, bool is_done_m4,
                    bool is_done_m5, bool is_done_m6,
                    int st,
                    bool want_to_acquire, bool want_to_release,
                    int base, int shoulder,
                    int elbow, int wrist_ver,
                    int wrist_rot, int gripper){

    Serial.print("[iter # "+String(iter)+"] ");
    Serial.print("CONTROLLER_"+String(control)+" ");
    Serial.print("INPUTS ");
    Serial.print("want_cargo_on_out="+String(want_cargo_on_out)+" ");
    Serial.print("is_acquired="+String(is_acquired)+" ");
    Serial.print("is_done_m1="+String(is_done_m1)+" ");
    Serial.print("is_done_m2="+String(is_done_m2)+" ");
    Serial.print("is_done_m3="+String(is_done_m3)+" ");
    Serial.print("is_done_m4="+String(is_done_m4)+" ");
    Serial.print("is_done_m5="+String(is_done_m5)+" ");
    Serial.print("is_done_m6="+String(is_done_m6));
    Serial.print(" state="+String(st));
    Serial.print(" OUTPUTS ");
    Serial.print("want_to_acquire="+String(want_to_acquire)+" ");
    Serial.print("want_to_release="+String(want_to_release)+" ");
    Serial.print("base="+String(base)+" ");
    Serial.print("shoulder="+String(shoulder)+" ");
    Serial.print("elbow="+String(elbow)+" ");
    Serial.print("wrist_ver="+String(wrist_ver)+" ");
    Serial.print("wrist_rot="+String(wrist_rot)+" ");
    Serial.println("gripper="+String(gripper)+" ");
}


void loop() {
    iter++;

    // Read buttons
    if (analogRead(INPUT_X1_PIN) >= 100 && analogRead(INPUT_X1_PIN) < 700) {
        input_X1.want_cargo_on_out = false;
        digitalWrite(LAMP_X1_PIN, LOW);
        MESSAGE("*** The cargo is removed from Z1 pos ***");
    } else if (analogRead(INPUT_X1_PIN) >= 700) {
        input_X1.want_cargo_on_out = true;
        digitalWrite(LAMP_X1_PIN, HIGH);
        MESSAGE("*** The cargo on Z1 pos ***");
    }

    if (analogRead(INPUT_X2_PIN) >= 100 && analogRead(INPUT_X2_PIN) < 700) {
        input_X2.want_cargo_on_out = false;
        digitalWrite(LAMP_X2_PIN, LOW);
        MESSAGE("*** The cargo is removed from Z2 pos ***");
    } else if (analogRead(INPUT_X2_PIN) >= 700) {
        input_X2.want_cargo_on_out = true;
        digitalWrite(LAMP_X2_PIN, HIGH);
        MESSAGE("*** The cargo on Z2 pos ***");
    }

    if (analogRead(INPUT_X3_PIN) >= 100 && analogRead(INPUT_X3_PIN) < 700) {
        input_X3.want_cargo_on_out = false;
        digitalWrite(LAMP_X3_PIN, LOW);
        MESSAGE("*** The cargo is removed from Z3 pos ***");
    } else if (analogRead(INPUT_X3_PIN) >= 700) {
        input_X3.want_cargo_on_out = true;
        digitalWrite(LAMP_X3_PIN, HIGH);
        MESSAGE("*** The cargo on Z3 pos ***");
    }

    read_current_angles(I2C_ADDR_SLAVE_X1, current_angle_X1);
    input_X1.is_done_m1 = (current_angle_X1[0] == target_angle_X1[0]);
    input_X1.is_done_m2 = (current_angle_X1[1] == target_angle_X1[1]);
    input_X1.is_done_m3 = (current_angle_X1[2] == target_angle_X1[2]);
    input_X1.is_done_m4 = (current_angle_X1[3] == target_angle_X1[3]);
    input_X1.is_done_m5 = (current_angle_X1[4] == target_angle_X1[4]);
    input_X1.is_done_m6 = (current_angle_X1[5] == target_angle_X1[5]);

    // Print inputs controller 1
    #ifdef DEBUG_MESSAGE
        print_input_control(
            1, current_angle_X1,
            input_X1.want_cargo_on_out, input_X1.is_acquired,
            input_X1.is_done_m1, input_X1.is_done_m2,
            input_X1.is_done_m3, input_X1.is_done_m4,
            input_X1.is_done_m5, input_X1.is_done_m6);
    #endif

    // Execute the controllers
        auto out_X1 = control1.go_step(input_X1);

    #ifdef DEBUG_LOG
    bool is_active = (bool)out_X1.active;
    bool is_same_input = (input_X1.want_cargo_on_out == prev_input_X1.want_cargo_on_out &&
                             input_X1.is_acquired == prev_input_X1.is_acquired &&
                             input_X1.is_done_m1 == prev_input_X1.is_done_m1 &&
                             input_X1.is_done_m2 == prev_input_X1.is_done_m2 &&
                             input_X1.is_done_m3 == prev_input_X1.is_done_m3 &&
                             input_X1.is_done_m4 == prev_input_X1.is_done_m4 &&
                             input_X1.is_done_m5 == prev_input_X1.is_done_m5 &&
                             input_X1.is_done_m6 == prev_input_X1.is_done_m6);

    if (is_active || !is_same_input || !is_prev_passive_X1) {
        if (is_active){
            is_prev_passive_X1 = false;
        }
        int st = control1.state;
        log_everything(1, input_X1.want_cargo_on_out,
                          input_X1.is_acquired,
                          input_X1.is_done_m1, input_X1.is_done_m2,
                          input_X1.is_done_m3, input_X1.is_done_m4,
                          input_X1.is_done_m5, input_X1.is_done_m6,
                          st,
                          out_X1.want_to_acquire, out_X1.want_to_release,
                          (int)out_X1.go_base, (int)out_X1.go_shoulder,
                          (int)out_X1.go_elbow, (int)out_X1.go_wrist_ver,
                          (int)out_X1.go_wrist_rot, (int)out_X1.go_gripper);

    } else if (!is_active && !is_prev_passive_X1){
        is_prev_passive_X1 = true;
    }

    prev_input_X1.want_cargo_on_out = input_X1.want_cargo_on_out;
    prev_input_X1.is_acquired = input_X1.is_acquired;
    prev_input_X1.is_done_m1 = input_X1.is_done_m1;
    prev_input_X1.is_done_m2 = input_X1.is_done_m2;
    prev_input_X1.is_done_m3 = input_X1.is_done_m3;
    prev_input_X1.is_done_m4 = input_X1.is_done_m4;
    prev_input_X1.is_done_m5 = input_X1.is_done_m5;
    prev_input_X1.is_done_m6 = input_X1.is_done_m6;
    #endif

    if (control1.state == ControllerX1::GO_UP_CLENCHED_Z0) {
        input_X1.want_cargo_on_out = false;
        digitalWrite(LAMP_X1_PIN, LOW);
        MESSAGE("*** The cargo pickuped from Z1 pos ***");
    }

    read_current_angles(I2C_ADDR_SLAVE_X2, current_angle_X2);
    input_X2.is_done_m1 = (current_angle_X2[0] == target_angle_X2[0]);
    input_X2.is_done_m2 = (current_angle_X2[1] == target_angle_X2[1]);
    input_X2.is_done_m3 = (current_angle_X2[2] == target_angle_X2[2]);
    input_X2.is_done_m4 = (current_angle_X2[3] == target_angle_X2[3]);
    input_X2.is_done_m5 = (current_angle_X2[4] == target_angle_X2[4]);
    input_X2.is_done_m6 = (current_angle_X2[5] == target_angle_X2[5]);

    // Print inputs controller 2
    #ifdef DEBUG_MESSAGE
        print_input_control(
            2, current_angle_X2,
            input_X2.want_cargo_on_out, input_X2.is_acquired,
            input_X2.is_done_m1, input_X2.is_done_m2,
            input_X2.is_done_m3, input_X2.is_done_m4,
            input_X2.is_done_m5, input_X2.is_done_m6);
    #endif

        auto out_X2 = control2.go_step(input_X2);

    #ifdef DEBUG_LOG
    is_active = (bool)out_X2.active;
    is_same_input = (input_X2.want_cargo_on_out == prev_input_X2.want_cargo_on_out &&
                             input_X2.is_acquired == prev_input_X2.is_acquired &&
                             input_X2.is_done_m1 == prev_input_X2.is_done_m1 &&
                             input_X2.is_done_m2 == prev_input_X2.is_done_m2 &&
                             input_X2.is_done_m3 == prev_input_X2.is_done_m3 &&
                             input_X2.is_done_m4 == prev_input_X2.is_done_m4 &&
                             input_X2.is_done_m5 == prev_input_X2.is_done_m5 &&
                             input_X2.is_done_m6 == prev_input_X2.is_done_m6);

    if (is_active || !is_same_input || !is_prev_passive_X2) {
        if (is_active){
            is_prev_passive_X2 = false;
        }
        int st = control2.state;
        log_everything(2, input_X2.want_cargo_on_out,
                          input_X2.is_acquired,
                          input_X2.is_done_m1, input_X2.is_done_m2,
                          input_X2.is_done_m3, input_X2.is_done_m4,
                          input_X2.is_done_m5, input_X2.is_done_m6,
                          st,
                          out_X2.want_to_acquire, out_X2.want_to_release,
                          (int)out_X2.go_base, (int)out_X2.go_shoulder,
                          (int)out_X2.go_elbow, (int)out_X2.go_wrist_ver,
                          (int)out_X2.go_wrist_rot, (int)out_X2.go_gripper);

    } else if (!is_active && !is_prev_passive_X2){
        is_prev_passive_X2 = true;
    }

    prev_input_X2.want_cargo_on_out = input_X2.want_cargo_on_out;
    prev_input_X2.is_acquired = input_X2.is_acquired;
    prev_input_X2.is_done_m1 = input_X2.is_done_m1;
    prev_input_X2.is_done_m2 = input_X2.is_done_m2;
    prev_input_X2.is_done_m3 = input_X2.is_done_m3;
    prev_input_X2.is_done_m4 = input_X2.is_done_m4;
    prev_input_X2.is_done_m5 = input_X2.is_done_m5;
    prev_input_X2.is_done_m6 = input_X2.is_done_m6;
    #endif

    if (control2.state == ControllerX2::GO_UP_CLENCHED_Z0) {
        input_X2.want_cargo_on_out = false;
        digitalWrite(LAMP_X2_PIN, LOW);
        MESSAGE("*** The cargo pickuped from Z2 pos ***");
    }

    read_current_angles(I2C_ADDR_SLAVE_X3, current_angle_X3);
    input_X3.is_done_m1 = (current_angle_X3[0] == target_angle_X3[0]);
    input_X3.is_done_m2 = (current_angle_X3[1] == target_angle_X3[1]);
    input_X3.is_done_m3 = (current_angle_X3[2] == target_angle_X3[2]);
    input_X3.is_done_m4 = (current_angle_X3[3] == target_angle_X3[3]);
    input_X3.is_done_m5 = (current_angle_X3[4] == target_angle_X3[4]);
    input_X3.is_done_m6 = (current_angle_X3[5] == target_angle_X3[5]);

    // Print inputs controller 3
    #ifdef DEBUG_MESSAGE
        print_input_control(
            3, current_angle_X3,
            input_X3.want_cargo_on_out, input_X3.is_acquired,
            input_X3.is_done_m1, input_X3.is_done_m2,
            input_X3.is_done_m3, input_X3.is_done_m4,
            input_X3.is_done_m5, input_X3.is_done_m6);
    #endif

        auto out_X3 = control3.go_step(input_X3);

    #ifdef DEBUG_LOG
    is_active = (bool)out_X3.active;
    is_same_input = (input_X3.want_cargo_on_out == prev_input_X3.want_cargo_on_out &&
                             input_X3.is_acquired == prev_input_X3.is_acquired &&
                             input_X3.is_done_m1 == prev_input_X3.is_done_m1 &&
                             input_X3.is_done_m2 == prev_input_X3.is_done_m2 &&
                             input_X3.is_done_m3 == prev_input_X3.is_done_m3 &&
                             input_X3.is_done_m4 == prev_input_X3.is_done_m4 &&
                             input_X3.is_done_m5 == prev_input_X3.is_done_m5 &&
                             input_X3.is_done_m6 == prev_input_X3.is_done_m6);

    if (is_active || !is_same_input || !is_prev_passive_X3) {
        if (is_active){
            is_prev_passive_X3 = false;
        }
        int st = control3.state;
        log_everything(3, input_X3.want_cargo_on_out,
                          input_X3.is_acquired,
                          input_X3.is_done_m1, input_X3.is_done_m2,
                          input_X3.is_done_m3, input_X3.is_done_m4,
                          input_X3.is_done_m5, input_X3.is_done_m6,
                          st,
                          out_X3.want_to_acquire, out_X3.want_to_release,
                          (int)out_X3.go_base, (int)out_X3.go_shoulder,
                          (int)out_X3.go_elbow, (int)out_X3.go_wrist_ver,
                          (int)out_X3.go_wrist_rot, (int)out_X3.go_gripper);

    } else if (!is_active && !is_prev_passive_X3){
        is_prev_passive_X3 = true;
    }

    prev_input_X3.want_cargo_on_out = input_X3.want_cargo_on_out;
    prev_input_X3.is_acquired = input_X3.is_acquired;
    prev_input_X3.is_done_m1 = input_X3.is_done_m1;
    prev_input_X3.is_done_m2 = input_X3.is_done_m2;
    prev_input_X3.is_done_m3 = input_X3.is_done_m3;
    prev_input_X3.is_done_m4 = input_X3.is_done_m4;
    prev_input_X3.is_done_m5 = input_X3.is_done_m5;
    prev_input_X3.is_done_m6 = input_X3.is_done_m6;
    #endif

    if (control3.state == ControllerX3::GO_UP_CLENCHED_Z0) {
        input_X3.want_cargo_on_out = false;
        digitalWrite(LAMP_X3_PIN, LOW);
        MESSAGE("*** The cargo pickuped from Z3 pos ***");
    }

    
    // Mutex release

    if (out_X1.want_to_release && mutex.acquired == 1){
        MESSAGE("*** X1 released ***");
        digitalWrite(MUTEX_LAMP_PIN, LOW);
        input_X1.is_acquired = false;
        mutex.release();
    }

    if (out_X2.want_to_release && mutex.acquired == 2){
        MESSAGE("*** X2 released ***");
        digitalWrite(MUTEX_LAMP_PIN, LOW);
        input_X2.is_acquired = false;
        mutex.release();
    }

    if (out_X3.want_to_release && mutex.acquired == 3){
        MESSAGE("*** X3 released ***");
        digitalWrite(MUTEX_LAMP_PIN, LOW);
        input_X3.is_acquired = false;
        mutex.release();
    }

    
    // Try acquire

    mutex.try_acquire(out_X1.want_to_acquire, out_X2.want_to_acquire, out_X3.want_to_acquire);

    if (mutex.acquired == 1){
        input_X1.is_acquired = true;
        digitalWrite(MUTEX_LAMP_PIN, HIGH);
        MESSAGE("*** X1 is acquired ***");
    } else if (mutex.acquired == 2){
        input_X2.is_acquired = true;
        digitalWrite(MUTEX_LAMP_PIN, HIGH);
        MESSAGE("*** X2 is acquired ***");
    } else if (mutex.acquired == 3){
        input_X3.is_acquired = true;
        digitalWrite(MUTEX_LAMP_PIN, HIGH);
        MESSAGE("*** X3 is acquired ***");
    }

    // Checking for new target
    bool new_target = target_angle_X1[0] == (int)out_X1.go_base && target_angle_X1[1] == (int)out_X1.go_shoulder &&
                      target_angle_X1[2] == (int)out_X1.go_elbow && target_angle_X1[3] == (int)out_X1.go_wrist_ver &&
                      target_angle_X1[4] == (int)out_X1.go_wrist_rot && target_angle_X1[5] == (int)out_X1.go_gripper;

    // Send controls to the plant
    target_angle_X1[0] = (int)out_X1.go_base;
    target_angle_X1[1] = (int)out_X1.go_shoulder;
    target_angle_X1[2] = (int)out_X1.go_elbow;
    target_angle_X1[3] = (int)out_X1.go_wrist_ver;
    target_angle_X1[4] = (int)out_X1.go_wrist_rot;
    target_angle_X1[5] = (int)out_X1.go_gripper;
    send_target_angles(I2C_ADDR_SLAVE_X1, target_angle_X1);

    // Print out controller 1
    #ifdef DEBUG_MESSAGE
        if (new_target){
            print_out_control(1, target_angle_X1);
        }
    #endif

    // Checking for new target
    new_target = target_angle_X2[0] == (int)out_X2.go_base && target_angle_X2[1] == (int)out_X2.go_shoulder &&
                 target_angle_X2[2] == (int)out_X2.go_elbow && target_angle_X2[3] == (int)out_X2.go_wrist_ver &&
                 target_angle_X2[4] == (int)out_X2.go_wrist_rot && target_angle_X2[5] == (int)out_X2.go_gripper;

    target_angle_X2[0] = (int)out_X2.go_base;
    target_angle_X2[1] = (int)out_X2.go_shoulder;
    target_angle_X2[2] = (int)out_X2.go_elbow;
    target_angle_X2[3] = (int)out_X2.go_wrist_ver;
    target_angle_X2[4] = (int)out_X2.go_wrist_rot;
    target_angle_X2[5] = (int)out_X2.go_gripper;
    send_target_angles(I2C_ADDR_SLAVE_X2, target_angle_X2);

    // Print out controller 2
    #ifdef DEBUG_MESSAGE
        if (new_target){
            print_out_control(2, target_angle_X2);
        }
    #endif

    // Checking for new target
    new_target = target_angle_X3[0] == (int)out_X3.go_base && target_angle_X3[1] == (int)out_X3.go_shoulder &&
                 target_angle_X3[2] == (int)out_X3.go_elbow && target_angle_X3[3] == (int)out_X3.go_wrist_ver &&
                 target_angle_X3[4] == (int)out_X3.go_wrist_rot && target_angle_X3[5] == (int)out_X3.go_gripper;

    target_angle_X3[0] = (int)out_X3.go_base;
    target_angle_X3[1] = (int)out_X3.go_shoulder;
    target_angle_X3[2] = (int)out_X3.go_elbow;
    target_angle_X3[3] = (int)out_X3.go_wrist_ver;
    target_angle_X3[4] = (int)out_X3.go_wrist_rot;
    target_angle_X3[5] = (int)out_X3.go_gripper;
    send_target_angles(I2C_ADDR_SLAVE_X3, target_angle_X3);

    // Print out controller 3
    #ifdef DEBUG_MESSAGE
        if (new_target){
            print_out_control(3, target_angle_X3);
        }
    #endif

    delay(100);
}
