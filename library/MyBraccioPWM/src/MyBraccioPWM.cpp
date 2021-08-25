/*
 Braccio.cpp - board library Version 2.0
 Written by Andrea Martino and Angelo Ferrante

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "MyBraccioPWM.h"

#define NUM_STEP 2
#define NUM_STEP_BASE 2
#define NUM_DELAY 1
#define DELAY1 100
#define DELAY2 400
#define T (DELAY1+DELAY2)/1000 // period millisec
#define braccio_speed 0.3 // deg/millisec

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
	/*base.write(0);
	shoulder.write(40);
	elbow.write(180);
	wrist_ver.write(170);
	wrist_rot.write(0);
	gripper.write(180-73);*/
	base.write(0);
	shoulder.write(140);
	elbow.write(0);
	wrist_ver.write(10);
	wrist_rot.write(0);
	gripper.write(180-73);

	//Previous step motor position
	/*step_base = 0;
	step_shoulder = 40;
	step_elbow = 180;
	step_wrist_ver = 170;
	step_wrist_rot = 0;
	step_gripper = 73;*/
	step_base = 0;
	step_shoulder = 140;
	step_elbow = 0;
	step_wrist_ver = 10;
	step_wrist_rot = 0;
	step_gripper = 73;

	if(soft_start_level!=SOFT_START_DISABLED)
    		_softStart(soft_start_level);
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
int _Braccio::ServoMovement(int stepDelay, int vBase, int vShoulder, int vElbow,int vWrist_ver, int vWrist_rot, int vgripper, void (*callback)(int,int,int,int,int,int,int,int,int,int,int,int)) {

	/*Serial.print("step_base = "); Serial.println(step_base);
	Serial.print("step_shoulder = "); Serial.println(step_shoulder);
	Serial.print("step_elbow = "); Serial.println(step_elbow);
	Serial.print("step_wrist_rot = "); Serial.println(step_wrist_rot);
	Serial.print("step_wrist_ver = "); Serial.println(step_wrist_ver);
	Serial.print("step_gripper = "); Serial.println(step_gripper);*/

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
	//vgripper = 180 - vgripper;


	int step = 0;
	int numT = 0;

	/*bool is_m1_done = false, is_m2_done = false, is_m3_done = false, is_m4_done = false, is_m5_done = false,
	is_m6_done = false;*/

	int max1 = max(abs(vBase-step_base),abs(vShoulder-step_shoulder));
	int max2 = max(abs(vElbow-step_elbow),abs(vWrist_ver-step_wrist_ver));
	int max3 = max(abs(vWrist_rot-step_wrist_rot),abs(vgripper-step_gripper));

	int max12 = max(max1, max2);
	int max123 = max(max12, max3); // максимальная разница между текущими и финальными углами

	// Serial.println(String(max1)+" "+String(max2)+" "+String(max3));
	if (max123>0){Serial.println("max "+String(max123));}

	if (max123>0){Serial.println(String(vBase)+" "+String(vShoulder)+" "+String(vElbow)+" "+String(vWrist_ver)+" "+String(vWrist_rot)+" "+String(vgripper));
	Serial.println(String(step_base)+" "+String(step_shoulder)+" "+String(step_elbow)+" "+String(step_wrist_ver)+" "+String(step_wrist_rot)+" "+String(step_gripper));}

/*	int numT = floor(max(max(max(abs(vBase-step_base),abs(vShoulder-step_shoulder)),max(abs(vElbow-step_elbow),abs(vWrist_ver-step_wrist_ver))),max(abs(vWrist_rot-step_wrist_rot),abs(vgripper-step_gripper)))/(400*500));
*/
	numT = max123/(braccio_speed*T); // количество периодов ШИМ

	if (max123>0){Serial.println(String(numT));}

	digitalWrite(SOFT_START_CONTROL_PIN,LOW);
	base.write(vBase);
	shoulder.write(vShoulder);
	elbow.write(vElbow);
	wrist_ver.write(vWrist_ver);
	wrist_rot.write(vWrist_rot);
	gripper.write(180-vgripper);

	for (int i = 1; i <= numT; i++){
		digitalWrite(SOFT_START_CONTROL_PIN,HIGH);
		delayMicroseconds(DELAY1);
		digitalWrite(SOFT_START_CONTROL_PIN,LOW);
		delayMicroseconds(DELAY2);
	}
	digitalWrite(SOFT_START_CONTROL_PIN,HIGH);

	/*base.write(vBase);
	shoulder.write(vShoulder);
	elbow.write(vElbow);
	wrist_ver.write(vWrist_ver);
	wrist_rot.write(vWrist_rot);
	gripper.write(180-vgripper);*/
	int it = 0;

	step_base = vBase;
	step_shoulder = vShoulder;
	step_elbow = vElbow;
	step_wrist_ver = vWrist_ver;
	step_wrist_rot = vWrist_rot;
	step_gripper = vgripper;

	//delay(200);

		/*if (callback != 0 && need_cb){
			 callback(vBase , vShoulder , vElbow , vWrist_ver , vWrist_rot , vgripper , step_base, step_shoulder, step_elbow, step_wrist_ver, step_wrist_rot, step_gripper);
		}*/
		
	//It checks if all the servo motors are in the desired position
	/*if ((vBase == step_base) && (vShoulder == step_shoulder)
			&& (vElbow == step_elbow) && (vWrist_ver == step_wrist_ver)
			&& (vWrist_rot == step_wrist_rot) && (vgripper == step_gripper)) {
		step_base = vBase;
		step_shoulder = vShoulder;
		step_elbow = vElbow;
		step_wrist_ver = vWrist_ver;
		step_wrist_rot = vWrist_rot;
		step_gripper = vgripper;
	}*/
}
