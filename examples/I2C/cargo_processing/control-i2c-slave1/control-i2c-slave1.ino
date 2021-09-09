#include <BraccioV2.h>
#include <Wire.h>

#define I2C_SLAVE1_ADDRESS 11

Braccio arm;

int array_angles_X1[6] = {0, 0, 0, 0, 0, 0};

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
    array_angles_X1[0] = ;
    array_angles_X1[1] = ;
    array_angles_X1[2] = ;
    array_angles_X1[3] = ;
    array_angles_X1[4] = ;
    array_angles_X1[5] = ;

    Wire.onRequest(slave_on_request);
    Wire.onReceive(slave_on_reсeive);
}

void slave_on_reсeive(){

    Serial.println("recieved state from master: ");

    for (int i=0; i<6, i++){
        array_angles_X1[i] = Wire.read();
        Serial.print(array_angles_X1[i]);
        Serial.print(" ");
    }

    Serial.println();

    arm.setAllAbsolute(array_angles_X1[0], array_angles_X1[1],
                    array_angles_X1[2], array_angles_X1[3],
                    array_angles_X1[4], array_angles_X1[5]);

}

void slave_on_request(){
    Wire.write(array_angles_X1, 6);
}

void loop() {
    arm.safeDelay(500);

}
