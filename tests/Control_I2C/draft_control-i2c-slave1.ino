#include <BraccioV2.h>
#include <Wire.h>
// #include <MyCommonUtils.hpp>

// #define PIN_REQ 0
// #define PIN_ACK 0
// #define PIN_IN 0
// #define PIN_OUT 0

#define I2C_SLAVE1_ADDRESS 11

/*#define SEND(value) \
    slave_send(value, PIN_OUT, PIN_REQ, PIN_ACK)
#define RECV() \
    slave_recv(PIN_IN, PIN_REQ, PIN_ACK)
#define SEND_INT(value) \
    slave_send_uint8(value, PIN_OUT, PIN_REQ, PIN_ACK)
#define RECV_INT() \
    slave_recv_uint8(PIN_IN, PIN_REQ, PIN_ACK)*/

Braccio arm;

int array_state_X1[6] = {0, 0, 0, 0, 0, 0};

// Setup was copied from the 'Basic Movement' example of BraccioV2 lib.
void setup() {
    Wire.begin(I2C_SLAVE1_ADDRESS);
    Serial.begin(9600);
    Serial.println("Initializing... Please Wait");  //Start of initialization, see note below regarding begin method.

    // TODO: setup pins

    //Update these lines with the calibration code outputted by the calibration program.
    arm.setJointCenter(WRIST_ROT, 90);
    arm.setJointCenter(WRIST, 90);
    arm.setJointCenter(ELBOW, 90);
    arm.setJointCenter(SHOULDER, 90);
    arm.setJointCenter(BASE_ROT, 90);
    arm.setJointCenter(GRIPPER, 50);  //Rough center of gripper, default opening position

    //Set max/min values for joints as needed. Default is min: 0, max: 180
    //The only two joints that should need this set are gripper and shoulder.
    arm.setJointMax(GRIPPER, 100);  //Gripper closed, can go further, but risks damage to servos
    arm.setJointMin(GRIPPER, 15);   //Gripper open, can't open further

    //There are two ways to start the arm:
    //1. Start to default position.
    arm.begin(true);  // Start to default vertical position.
    //This method moves the arm to the values specified by setJointCenter
    //and by default will make the arm be roughly straight up.

    //2. Start to custom position.
    //arm.begin(false);
    //arm.setAllNow(base_rot_val, shoulder_val, elbow_val, wrist_val, wrist_rot_val, gripper_val);
    //This method allows a custom start position to be set, but the setAllNow method MUST be run
    //immediately after the begin method and before any other movement commands are issued.

    //NOTE: The begin method takes approximately 8 seconds to start, due to the time required
    //to initialize the power circuitry.
    Serial.println("Initialization Complete");

    // There are start position
    array_state_X1[0] = ;
    array_state_X1[1] = ;
    array_state_X1[2] = ;
    array_state_X1[3] = ;
    array_state_X1[4] = ;
    array_state_X1[5] = ;

    Wire.onRequest(slave_on_request);
    Wire.onReceive(slave_on_reсeive);
}

void slave_on_reсeive(){

    Serial.println("recieved state from master: ");

    for (int i=0; i<6; i++){
        array_state_X1[i] = Wire.read();
        Serial.print(array_state_X1[i]);
        Serial.print(" ");
    }

    Serial.println();

    arm.setAllAbsolute(array_state_X1[0], array_state_X1[1], array_state_X1[2], array_state_X1[3], array_state_X1[4],
        array_state_X1[5]);

    /*b = array_state_X1[0];
    s = array_state_X1[1];
    e = array_state_X1[2];
    w = array_state_X1[3];
    r = array_state_X1[4];
    g = array_state_X1[5];*/

}

void slave_on_request(){
    Wire.write(array_state_X1, 6);
}

void loop() {
    arm.safeDelay(500);



    /*if (digitalRead(PIN_REQ)) {
        bool is_master_wants_to_send = RECV();

        if (is_master_wants_to_send) {
            uint8_t b = RECV_INT();  // base
            uint8_t s = RECV_INT();  // shoulder
            uint8_t e = RECV_INT();  // elbow
            uint8_t w = RECV_INT();  // wrist
            uint8_t r = RECV_INT();  // wrist-rot
            uint8_t g = RECV_INT();  // gripper

            arm.setAllAbsolute(b, s, e, w, r, g);

            // TODO: FIN?
        } else {
            // TODO: extract current joint positions
            uint8_t b = 0;  // base
            uint8_t s = 0;  // shoulder
            uint8_t e = 0;  // elbow
            uint8_t w = 0;  // wrist
            uint8_t r = 0;  // wrist-rot
            uint8_t g = 0;  // gripper

            SEND_INT(b);
            SEND_INT(s);
            SEND_INT(e);
            SEND_INT(w);
            SEND_INT(r);
            SEND_INT(g);

            // TODO: FIN?
        }
    }*/
}
