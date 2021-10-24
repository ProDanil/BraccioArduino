#include <MyBraccioV2.h>
#include <Wire.h>

#define I2C_SLAVE_ADDRESS 11 

Braccio arm;

uint8_t target_angle[6];
uint8_t current_angle[6];

// Setup was copied from the 'Basic Movement' example of BraccioV2 lib.
void setup() {
    Wire.begin(I2C_SLAVE_ADDRESS);
    Serial.begin(9600);
    Serial.println("Initializing... Please Wait");  //Start of initialization, see note below regarding begin method.

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

     //Start position
    for (size_t i = 0; i < 6; i++) {
        current_angle[i] = arm._currentJointPositions[i];
        target_angle[i] = arm._currentJointPositions[i];
    }

    Wire.onRequest(slave_on_request);
    Wire.onReceive(slave_on_receive);
}

void print_array(uint8_t (&data)[6]) {
    Serial.print("[");
    for (size_t i = 0; i < 6; i++) {
        Serial.print(data[i]);
        Serial.print(", ");
    }
    Serial.println("]");
}

void slave_on_receive() {
    for (size_t i = 0; i < 6; i++) {
        target_angle[i] = Wire.read();
    }

    Serial.println("recieved target angles from master: ");
    print_array(target_angle);
}

void slave_on_request() {
    Serial.println("sending current angles to master: ");
    print_array(current_angle);

    Wire.write(current_angle, 6);
}

void loop() {
    arm.setAllAbsolute(target_angle[0], target_angle[1],
                       target_angle[2], target_angle[3],
                       target_angle[4], target_angle[5]);

    arm.safeDelay(500);

    for (size_t i = 0; i < 6; i++) {
        current_angle[i] = arm._currentJointPositions[i];
    }
}
