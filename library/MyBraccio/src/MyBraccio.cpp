/*
 Исправленная библиотека Braccio.h
 */

#include "MyBraccio.h"

#define NUM_STEP 2
#define NUM_STEP_BASE 2
#define NUM_DELAY 1

extern Servo base;           /// M1
extern Servo shoulder;       /// M2
extern Servo elbow;          /// M3
extern Servo wrist_ver;      /// M4
extern Servo wrist_rot;      /// M5
extern Servo gripper;        /// M6

/////////////////////////////////////////////////
/*extern int step_base = 0;
extern int step_shoulder = 45;
extern int step_elbow = 180;
extern int step_wrist_ver = 180;
extern int step_wrist_rot = 90;
extern int step_gripper = 10;*/
extern int step_base = 0;
extern int step_shoulder = 140;
extern int step_elbow = 0;
extern int step_wrist_ver = 10;
extern int step_wrist_rot = 0;
extern int step_gripper = 10;
//////////////////////////////////////////////////


_Braccio Braccio;

//Initialize Braccio object
_Braccio::_Braccio() {
}

/**
 * Braccio initialization and set intial position
 * Modifing this function you can set up the initial position of all the
 * servo motors of Braccio
 * @param soft_start_level: default value is 0 (SOFT_START_DEFAULT)
 * You should set begin(SOFT_START_DISABLED) if you are using the Arm Robot shield V1.6
 * SOFT_START_DISABLED disable the Braccio movements
 */
unsigned int _Braccio::begin(int soft_start_level) {
	//Calling Braccio.begin(SOFT_START_DISABLED) the Softstart is disabled and you can use the pin 12
	if(soft_start_level!=SOFT_START_DISABLED){
		pinMode(SOFT_START_CONTROL_PIN,OUTPUT);
		digitalWrite(SOFT_START_CONTROL_PIN,LOW);
	}

////////////////////////////////////////////////////////////
	// initialization pin Servo motors
	base.attach(11);    //M1
	shoulder.attach(10);//M2
	elbow.attach(9);    //M3
	wrist_ver.attach(6);//M4
	wrist_rot.attach(5);//M5
	gripper.attach(3);  //M6

/////////////////////////////////////////////////////////////
        
	//For each step motor this set up the initial degree

	base.write(0);
	shoulder.write(140);
	elbow.write(0);
	wrist_ver.write(10);
	wrist_rot.write(0);
	gripper.write(10);


	//Previous step motor position
	step_base = 0;
	step_shoulder = 140;
	step_elbow = 0;
	step_wrist_ver = 10;
	step_wrist_rot = 0;
	step_gripper = 10;

	if(soft_start_level!=SOFT_START_DISABLED){
		_softStart(soft_start_level);
	}

	return 1;

}

/*
Software implementation of the PWM for the SOFT_START_CONTROL_PIN,HIGH
@param high_time: the time in the logic level high
@param low_time: the time in the logic level low
*/
void _Braccio::_softwarePWM(int high_time, int low_time){
	digitalWrite(SOFT_START_CONTROL_PIN,HIGH);
	delayMicroseconds(high_time);
	digitalWrite(SOFT_START_CONTROL_PIN,LOW);
	delayMicroseconds(low_time); 
}

/*
* This function, used only with the Braccio Shield V4 and greater,
* turn ON the Braccio softly and save it from brokes.
* The SOFT_START_CONTROL_PIN is used as a software PWM
* @param soft_start_level: the minimum value is -70, default value is 0 (SOFT_START_DEFAULT)
*/
void _Braccio::_softStart(int soft_start_level){      
	long int tmp=millis();
	while(millis()-tmp < LOW_LIMIT_TIMEOUT)
		_softwarePWM(80+soft_start_level, 450 - soft_start_level);   //the sum should be 530usec	

	while(millis()-tmp < HIGH_LIMIT_TIMEOUT)
		_softwarePWM(75 + soft_start_level, 430 - soft_start_level); //the sum should be 505usec

	digitalWrite(SOFT_START_CONTROL_PIN,HIGH);
}

/**
 * This functions allow you to control all the servo motors
 * 
 * @param stepDelay The delay between each servo movement
 * @param vBase next base servo motor degree
 * @param vShoulder next shoulder servo motor degree
 * @param vElbow next elbow servo motor degree
 * @param vWrist_ver next wrist rotation servo motor degree
 * @param vWrist_rot next wrist vertical servo motor degree
 * @param vgripper next gripper servo motor degree
 */
int _Braccio::ServoMovement(int stepDelay, int vBase, int vShoulder, int vElbow,int vWrist_ver, 
			    int vWrist_rot, int vgripper, void (*callback)(int,int,int,int,int,int,int,int,int,int,int,int)) {

	// Check values, to avoid dangerous positions for the Braccio
    	if (stepDelay > 30) stepDelay = 30;
	if (stepDelay < 10) stepDelay = 10;
	if (vBase < 0) vBase=0;
	if (vBase > 180) vBase=180;
	if (vShoulder < 15) vShoulder=15;
	if (vShoulder > 165) vShoulder=165;
	if (vElbow < 0) vElbow=0;
	if (vElbow > 180) vElbow=180;
	if (vWrist_ver < 0) vWrist_ver=0;
	if (vWrist_ver > 180) vWrist_ver=180;
	if (vWrist_rot > 180) vWrist_rot=180;
	if (vWrist_rot < 0) vWrist_rot=0;
    	if (vgripper < 10) vgripper = 10;
	if (vgripper > 73) vgripper = 73;

	int exit = 1;
	int step = 0;
	bool is_m1_done = false, is_m2_done = false, is_m3_done = false, is_m4_done = false, is_m5_done = false,
	is_m6_done = false;

	//Until the all motors are in the desired position
	while (exit) 
	{			
		step++;
		bool need_cb = (step%10)==0;
		//For each servo motor if next degree is not the same of the previuos than do the movement
		if (vBase != step_base) 
		{			
			base.write(step_base);
			//One step ahead
			if (vBase > step_base) {
				step_base = step_base + NUM_STEP_BASE;
				//step_base++;
			}
			//One step beyond
			if (vBase < step_base) {
				step_base = step_base - NUM_STEP_BASE;
				//step_base--;
			}
			if (abs(vBase-step_base) < NUM_STEP_BASE){
				step_base = vBase;
			}

		} else if (!is_m1_done){
			is_m1_done = true;
			need_cb = true;
		}

		if (vShoulder != step_shoulder)  
		{
			shoulder.write(step_shoulder);
			//One step ahead
			if (vShoulder > step_shoulder) {
				//step_shoulder++;
				step_shoulder = step_shoulder + NUM_STEP;
			}
			//One step beyond
			if (vShoulder < step_shoulder) {
				//step_shoulder--;
				step_shoulder = step_shoulder - NUM_STEP;
			}
			if (abs(vShoulder-step_shoulder) < NUM_STEP){
				step_shoulder = vShoulder;
			}

		} else if (!is_m2_done){
			is_m2_done = true;
			need_cb = true;
		}

		if (vElbow != step_elbow)  
		{
			elbow.write(step_elbow);
			//One step ahead
			if (vElbow > step_elbow) {
				//step_elbow++;
				step_elbow = step_elbow + NUM_STEP;
			}
			//One step beyond
			if (vElbow < step_elbow) {
				//step_elbow--;
				step_elbow = step_elbow - NUM_STEP;
			}
			if (abs(vElbow-step_elbow) < NUM_STEP){
				step_elbow = vElbow;
			}

		} else if (!is_m3_done){
			is_m3_done = true;
			need_cb = true;
		}

////////////////////////////////////////////////////////////////
		if (vWrist_ver != step_wrist_ver)
		{
			wrist_ver.write(step_wrist_ver);
			//One step ahead
			if (vWrist_ver > step_wrist_ver) {
				//step_wrist_ver++;
				step_wrist_ver = step_wrist_ver + NUM_STEP;
			}
			//One step beyond
			if (vWrist_ver < step_wrist_ver) {
				//step_wrist_ver--;
				step_wrist_ver = step_wrist_ver - NUM_STEP;
			}
			if (abs(vWrist_ver-step_wrist_ver) < NUM_STEP){
				step_wrist_ver = vWrist_ver;
			}

		} else if (!is_m4_done){
			is_m4_done = true;
			need_cb = true;
		}

		if (vWrist_rot != step_wrist_rot)
		{
			wrist_rot.write(step_wrist_rot);
			//One step ahead
			if (vWrist_rot > step_wrist_rot) {
				//step_wrist_rot++;
				step_wrist_rot = step_wrist_rot + NUM_STEP;
			}
			//One step beyond
			if (vWrist_rot < step_wrist_rot) {
				//step_wrist_rot--;
				step_wrist_rot = step_wrist_rot - NUM_STEP;
			}
			if (abs(vWrist_rot-step_wrist_rot) < NUM_STEP){
				step_wrist_rot = vWrist_rot;
			}
		} else if (!is_m5_done){
			is_m5_done = true;
			need_cb = true;
		}

////////////////////////////////////////////////////////////////


		if (vgripper != step_gripper)
		{
			gripper.write(step_gripper);
			// gripper.write(180-step_gripper);
			if (vgripper > step_gripper) {
				//step_gripper++;
				step_gripper = step_gripper + NUM_STEP;
			}
			//One step beyond
			if (vgripper < step_gripper) {
				//step_gripper--;
				step_gripper = step_gripper - NUM_STEP;
			}
			if (abs(vgripper-step_gripper) < NUM_STEP){
				step_gripper = vgripper;
			}
		} else if (!is_m6_done){
			is_m6_done = true;
			need_cb = true;
		}
		
		//delay between each movement
		delay(stepDelay*NUM_DELAY);
		if (callback != 0 && need_cb){
			 callback(vBase , vShoulder , vElbow , vWrist_ver , vWrist_rot , vgripper , step_base, step_shoulder, step_elbow, step_wrist_ver, step_wrist_rot, step_gripper);
		}
		
		//It checks if all the servo motors are in the desired position 
		if ((vBase == step_base) && (vShoulder == step_shoulder)
				&& (vElbow == step_elbow) && (vWrist_ver == step_wrist_ver)///////////////////////////
				&& (vWrist_rot == step_wrist_rot) && (vgripper == step_gripper)) {/////////////////////////
			step_base = vBase;
			step_shoulder = vShoulder;
			step_elbow = vElbow;
			step_wrist_ver = vWrist_ver; /////////////////////////////////////////
			step_wrist_rot = vWrist_rot; /////////////////////////////////////////
			step_gripper = vgripper;

			base.write(step_base);
			shoulder.write(step_shoulder);
			elbow.write(step_elbow);
			wrist_ver.write(step_wrist_ver);
			wrist_rot.write(step_wrist_rot);
			gripper.write(step_gripper);
			// gripper.write(180-step_gripper);

			exit = 0;
		} else {
			exit = 1;
		}
	}
}
