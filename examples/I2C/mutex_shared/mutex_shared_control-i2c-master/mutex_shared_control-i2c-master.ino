// ВЗАИМОДЕЙСТВИЕ МАНИПУЛЯТОРОВ. Mutex - I2C-master //

/* 3 манипулятора перекладывают груз на один выход
*/

#include <Wire.h>

#include "Controller1.hpp"
#include "Controller2.hpp"
#include "Controller3.hpp"

#define DEBUG_LOG
//#define DEBUG_MESSAGE

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

#define OUT_LAMP_PIN 3    // лампа - выходной сигнал с mutex

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

bool cargo_on_Z1;
bool cargo_on_Z2;
bool cargo_on_Z3;

unsigned iter = 0;


void setup() {
    Wire.begin();  // begin as master
    Serial.begin(115200);
    pinMode(OUT_LAMP_PIN, OUTPUT);
    digitalWrite(OUT_LAMP_PIN, LOW);
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

void read_current_angles(int slave_address,
                         uint8_t (&current_angle)[6],
                         bool &cargo_on) {
    Wire.requestFrom(slave_address, 8);
    for (size_t i = 0; i < 7; i++) {
        if (i < 6){
            current_angle[i] = Wire.read();
        } else {
            cargo_on = Wire.read();
        }

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
    iter++;

    read_current_angles(I2C_ADDR_SLAVE_X1, current_angle_X1, cargo_on_Z1);
    input_X1.is_done_m1 = (current_angle_X1[0] == target_angle_X1[0]);
    input_X1.is_done_m2 = (current_angle_X1[1] == target_angle_X1[1]);
    input_X1.is_done_m3 = (current_angle_X1[2] == target_angle_X1[2]);
    input_X1.is_done_m4 = (current_angle_X1[3] == target_angle_X1[3]);
    input_X1.is_done_m5 = (current_angle_X1[4] == target_angle_X1[4]);
    input_X1.is_done_m6 = (current_angle_X1[5] == target_angle_X1[5]);
    input_X1.want_cargo_on_out = cargo_on_Z1;

    // Execute the controller
        auto out_X1 = control1.go_step(input_X1);

    read_current_angles(I2C_ADDR_SLAVE_X2, current_angle_X2, cargo_on_Z2);
    input_X2.is_done_m1 = (current_angle_X2[0] == target_angle_X2[0]);
    input_X2.is_done_m2 = (current_angle_X2[1] == target_angle_X2[1]);
    input_X2.is_done_m3 = (current_angle_X2[2] == target_angle_X2[2]);
    input_X2.is_done_m4 = (current_angle_X2[3] == target_angle_X2[3]);
    input_X2.is_done_m5 = (current_angle_X2[4] == target_angle_X2[4]);
    input_X2.is_done_m6 = (current_angle_X2[5] == target_angle_X2[5]);
    input_X2.want_cargo_on_out = cargo_on_Z2;

    // Execute the controller
        auto out_X2 = control2.go_step(input_X2);

    read_current_angles(I2C_ADDR_SLAVE_X3, current_angle_X3, cargo_on_Z3);
    input_X3.is_done_m1 = (current_angle_X3[0] == target_angle_X3[0]);
    input_X3.is_done_m2 = (current_angle_X3[1] == target_angle_X3[1]);
    input_X3.is_done_m3 = (current_angle_X3[2] == target_angle_X3[2]);
    input_X3.is_done_m4 = (current_angle_X3[3] == target_angle_X3[3]);
    input_X3.is_done_m5 = (current_angle_X3[4] == target_angle_X3[4]);
    input_X3.is_done_m6 = (current_angle_X3[5] == target_angle_X3[5]);
    input_X3.want_cargo_on_out = cargo_on_Z3;

    // Execute the controller
        auto out_X3 = control3.go_step(input_X3);

    
    // Mutex release

    if (out_X1.want_to_release && mutex.acquired == 1){
        // Serial.println(analogRead(RELEASE_X1_PIN));
        MESSAGE("X1 released");
        digitalWrite(OUT_LAMP_PIN, LOW);
        input_X1.is_acquired = false;
        mutex.release();
    }

    if (out_X2.want_to_release && mutex.acquired == 2){
        MESSAGE("X2 released");
        digitalWrite(OUT_LAMP_PIN, LOW);
        input_X2.is_acquired = false;
        mutex.release();
    }

    if (out_X3.want_to_release && mutex.acquired == 3){
        MESSAGE("X3 released");
        digitalWrite(OUT_LAMP_PIN, LOW);
        input_X3.is_acquired = false;
        mutex.release();
    }

    
    // Try acquire

    mutex.try_acquire(out_X1.want_to_acquire, out_X2.want_to_acquire, out_X3.want_to_acquire);

    if (mutex.acquired == 1){
        input_X1.is_acquired = true;
        digitalWrite(OUT_LAMP_PIN, HIGH);
    } else if (mutex.acquired == 2){
        input_X2.is_acquired = true;
        digitalWrite(OUT_LAMP_PIN, HIGH); 
    } else if (mutex.acquired == 3){
        input_X3.is_acquired = true;
        digitalWrite(OUT_LAMP_PIN, HIGH);
    }

    // Send controls to the plant
    target_angle_X1[0] = (int)out_X1.go_base;
    target_angle_X1[1] = (int)out_X1.go_shoulder;
    target_angle_X1[2] = (int)out_X1.go_elbow;
    target_angle_X1[3] = (int)out_X1.go_wrist_ver;
    target_angle_X1[4] = (int)out_X1.go_wrist_rot;
    target_angle_X1[5] = (int)out_X1.go_gripper;
    send_target_angles(I2C_ADDR_SLAVE_X1, target_angle_X1);

    target_angle_X2[0] = (int)out_X2.go_base;
    target_angle_X2[1] = (int)out_X2.go_shoulder;
    target_angle_X2[2] = (int)out_X2.go_elbow;
    target_angle_X2[3] = (int)out_X2.go_wrist_ver;
    target_angle_X2[4] = (int)out_X2.go_wrist_rot;
    target_angle_X2[5] = (int)out_X2.go_gripper;
    send_target_angles(I2C_ADDR_SLAVE_X2, target_angle_X2);

    target_angle_X3[0] = (int)out_X3.go_base;
    target_angle_X3[1] = (int)out_X3.go_shoulder;
    target_angle_X3[2] = (int)out_X3.go_elbow;
    target_angle_X3[3] = (int)out_X3.go_wrist_ver;
    target_angle_X3[4] = (int)out_X3.go_wrist_rot;
    target_angle_X3[5] = (int)out_X3.go_gripper;
    send_target_angles(I2C_ADDR_SLAVE_X3, target_angle_X3);
}
