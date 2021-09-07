#include <Wire.h>

#include "Controller1.hpp"
#include "Controller2.hpp"
// #include "Controller3.hpp"

#define PIN_BUTTON_Z1 0
#define PIN_BUTTON_Z2 0
#define PIN_INPUT_Z1 0
#define PIN_INPUT_Z2 0

// X1
#define PIN_REQ_X1 0
#define PIN_ACK_X1 0
#define PIN_OUT_X1 0
#define PIN_IN_X1 0
#define I2C_SLAVE1_ADDRESS 11
// X2
#define PIN_REQ_X2 0
#define PIN_ACK_X2 0
#define PIN_OUT_X2 0
#define PIN_IN_X2 0
#define I2C_SLAVE2_ADDRESS 12
// X3
// TODO

/*#define SEND_X(X, value) \
    master_send(value, PIN_OUT_X##X, PIN_REQ_X##X, PIN_ACK_X##X)
#define RECV_X(X) \
    master_recv(PIN_IN_X##X, PIN_REQ_X##X, PIN_ACK_X##X)
#define SEND_INT_X(X, value) \
    master_send_uint8(value, PIN_OUT_X##X, PIN_REQ_X##X, PIN_ACK_X##X)
#define RECV_INT_X(X) \
    master_recv_uint8(PIN_IN_X##X, PIN_REQ_X##X, PIN_ACK_X##X)
#define BEGIN_SEND_X(X) \
    SEND_X(X, HIGH)
#define BEGIN_RECV_X(X) \
    SEND_X(X, LOW)*/

ControllerX1 control1;
ControllerX2 control2;
// ControllerX3 control3;

int array_state_X1[6] = {0, 0, 0, 0, 0, 0};
int array_state_X2[6] = {0, 0, 0, 0, 0, 0};
uint8_t send_state_X1[6] = {0, 0, 0, 0, 0, 0};
uint8_t send_state_X2[6] = {0, 0, 0, 0, 0, 0};

void setup() {
    Wire.begin(); // begin as master
    Serial.begin(9600);
    // setup pins
}

void loop() {
    // TODO: setup input buffers
    // TODO: recheck and refactor
    bool button_Z1 = !digitalRead(PIN_BUTTON_Z1);
    bool button_Z2 = !digitalRead(PIN_BUTTON_Z2);
    bool input_Z1 = !digitalRead(PIN_INPUT_Z1);
    bool input_Z2 = !digitalRead(PIN_INPUT_Z2);

    // Contract of the communication with the plant:
    // - If master wants to send, it should first send REQ[1]:
    //    - master_send(1)
    //    - master_send(...)
    // - If master wants to recv, it should first send REQ[0]:
    //    - master_send(0)
    //    - master_recv(...)

    // X1
    {
        // Read the state of the plant
        Wire.requestFrom(I2C_SLAVE1_ADDRESS, 6);
        Serial.println("recieved state X1: ");

        for (int i=0; i<6; i++){
            array_state_X1[i] = Wire.read();
            Serial.print(array_state_X1[i]);
            Serial.print(" ");
        }

        Serial.println();

        /*BEGIN_RECV_X(1);
        uint8_t angle_base_X1 = RECV_INT_X(1);
        uint8_t angle_shoulder_X1 = RECV_INT_X(1);
        uint8_t angle_elbow_X1 = RECV_INT_X(1);
        uint8_t angle_wrist_ver_X1 = RECV_INT_X(1);
        uint8_t angle_wrist_rot_X1 = RECV_INT_X(1);
        uint8_t angle_gripper_X1 = RECV_INT_X(1);*/
        // TODO: FIN?

        // Calculate inputs for the controller
        bool is_done_m1 = (array_state_X1[0] == send_state_X1[0]);
        bool is_done_m2 = (array_state_X1[1] == send_state_X1[1]);
        bool is_done_m3 = (array_state_X1[2] == send_state_X1[2]);
        bool is_done_m4 = (array_state_X1[3] == send_state_X1[3]);
        bool is_done_m5 = (array_state_X1[4] == send_state_X1[4]);
        bool is_done_m6 = (array_state_X1[5] == send_state_X1[5]);

        // Execute the controller
        auto out = control1.go_step(
            button_Z1, button_Z2,
            input_Z1, input_Z2,
            is_done_m1, is_done_m2,
            is_done_m3, is_done_m4,
            is_done_m5, is_done_m6);

        // Send controls to the plant
        Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
        
        send_state_X1[0] = (int)out.go_base;
        send_state_X1[1] = (int)out.go_shoulder;
        send_state_X1[2] = (int)out.go_elbow;
        send_state_X1[3] = (int)out.go_wrist_ver;
        send_state_X1[4] = (int)out.go_wrist_rot;
        send_state_X1[5] = (int)out.go_gripper;

        Wire.write(send_state_X1, 6);

        Wire.endTransmission();

        /*BEGIN_SEND_X(1);
        SEND_INT_X(1, (int)out.go_base);
        SEND_INT_X(1, (int)out.go_shoulder);
        SEND_INT_X(1, (int)out.go_elbow);
        SEND_INT_X(1, (int)out.go_wrist_ver);
        SEND_INT_X(1, (int)out.go_wrist_rot);
        SEND_INT_X(1, (int)out.go_gripper);*/
        // TODO: FIN?
    }

    // X2
    {
        // Read the state of the plant
        Wire.requestFrom(I2C_SLAVE2_ADDRESS, 6);
        Serial.println("recieved state X2: ");

        for (int i=0; i<6; i++){
            array_state_X2[i] = Wire.read();
            Serial.print(array_state_X2[i]);
            Serial.print(" ");
        }

        Serial.println();

        /*BEGIN_RECV_X(1);
        uint8_t angle_base_X1 = RECV_INT_X(1);
        uint8_t angle_shoulder_X1 = RECV_INT_X(1);
        uint8_t angle_elbow_X1 = RECV_INT_X(1);
        uint8_t angle_wrist_ver_X1 = RECV_INT_X(1);
        uint8_t angle_wrist_rot_X1 = RECV_INT_X(1);
        uint8_t angle_gripper_X1 = RECV_INT_X(1);*/
        // TODO: FIN?

        // Calculate inputs for the controller
        bool is_done_m1 = (array_state_X2[0] == send_state_X2[0]);
        bool is_done_m2 = (array_state_X2[1] == send_state_X2[1]);
        bool is_done_m3 = (array_state_X2[2] == send_state_X2[2]);
        bool is_done_m4 = (array_state_X2[3] == send_state_X2[3]);
        bool is_done_m5 = (array_state_X2[4] == send_state_X2[4]);
        bool is_done_m6 = (array_state_X2[5] == send_state_X2[5]);

        // Execute the controller
        auto out = control2.go_step(
//            button_Z1, button_Z2,
            input_Z1, input_Z2,
            is_done_m1, is_done_m2,
            is_done_m3, is_done_m4,
            is_done_m5, is_done_m6);

        // Send controls to the plant
        Wire.beginTransmission(I2C_SLAVE2_ADDRESS);
        
        send_state_X2[0] = (int)out.go_base;
        send_state_X2[1] = (int)out.go_shoulder;
        send_state_X2[2] = (int)out.go_elbow;
        send_state_X2[3] = (int)out.go_wrist_ver;
        send_state_X2[4] = (int)out.go_wrist_rot;
        send_state_X2[5] = (int)out.go_gripper;

        Wire.write(send_state_X2, 6);

        Wire.endTransmission();
    }
}
