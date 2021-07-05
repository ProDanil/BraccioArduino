/* Движение манипулятора (перенос груза из 0 на указанный угол) по нажатой кнопке
 * с использованием класса. Переписано к виду КА inter-pose-wise

  * go(digitalRead(), deg) - выполнение переноса груза с 0 град на указанный deg
  * go_wait - переход в позицию ожидания
  */

#include <MyBraccio.h>
#include <Servo.h>

#define BUTTON_PIN 2
#define CONTROL_PIN 12

/*Step Delay: a milliseconds delay between the movement of each servo.  Allowed values from 10 to 30 msec.
  M1 = base degrees from 0 to 180
  M2 = shoulder degrees from 15 to 165
  M3 = elbow degrees from 0 to 180
  M4 = vertical wrist degrees from 0 to 180
  M5 = rotatory wrist degrees from 0 to 180
  M6 = gripper degrees from 10 to 73 degrees*/

/*begin:
  base.write(0);
  shoulder.write(40);
  elbow.write(180);
  wrist_ver.write(170);
  wrist_rot.write(0);
  gripper.write(73);*/

Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_ver;
Servo wrist_rot;
Servo gripper;

enum State {IDLE_, GO_WAIT, WAIT, GO_UP, UP, GO_LOW, LOW_, GO_PICKUP, PICKUP, GO_UP_PICKUP, UP_PICKUP,
            GO_UP_OUT_PICKUP, UP_OUT_PICKUP, GO_LOW_OUT_PICKUP, LOW_OUT_PICKUP, GO_DROP, DROP, GO_UP_OUT, UP_OUT, DONE
           };
State state = IDLE_;
unsigned iter = 0;
String strState[] = {IDLE_, GO_WAIT, WAIT, GO_UP, UP, GO_LOW, LOW_, GO_PICKUP, PICKUP, GO_UP_PICKUP, UP_PICKUP,
            GO_UP_OUT_PICKUP, UP_OUT_PICKUP, GO_LOW_OUT_PICKUP, LOW_OUT_PICKUP, GO_DROP, DROP, GO_UP_OUT, UP_OUT, DONE
           };

//Вывод состояния серво (вызов из библиотеки MyBraccio), текущее положение = конечное TRUE/FALSE
void callback(
    int final_base, int final_shoulder, int final_elbow,
    int final_wrist_ver, int final_wrist_rot, int final_gripper,
    int current_base, int current_shoulder, int current_elbow,
    int current_wrist_ver, int current_wrist_rot, int current_gripper
) {
    Serial.print("Desired values: (");
    Serial.print(final_base);
    Serial.print(", ");
    Serial.print(final_shoulder);
    Serial.print(", ");
    Serial.print(final_elbow);
    Serial.print(", ");
    Serial.print(final_wrist_ver);
    Serial.print(", ");
    Serial.print(final_wrist_rot);
    Serial.print(", ");
    Serial.print(final_gripper);
    Serial.println(")");
    Serial.print("Current values: (");
    Serial.print(current_base);
    Serial.print(", ");
    Serial.print(current_shoulder);
    Serial.print(", ");
    Serial.print(current_elbow);
    Serial.print(", ");
    Serial.print(current_wrist_ver);
    Serial.print(", ");
    Serial.print(current_wrist_rot);
    Serial.print(", ");
    Serial.print(current_gripper);
    Serial.println(")");

    //Serial.print("[#iter] [M1.done = FALSE/TRUE]");
    Serial.print("["+String(iter)+"] ")
    if (current_base == final_base){
        Serial.print("[M1.done] "+"TRUE");
    } else {Serial.print("[M1.done] "+"false");}

    if (current_shoulder == final_shoulder){
        Serial.print("[M2.done] "+"TRUE");
    } else {Serial.print("[M2.done] "+"false");}

    if (current_elbow == final_elbow){
        Serial.print("[M3.done] "+"TRUE");
    } else {Serial.print("[M3.done] "+"false");}

    if (current_wrist_ver == final_wrist_ver){
        Serial.print("[M4.done] "+"TRUE");
    } else {Serial.print("[M4.done] "+"false");}

    if (current_wrist_rot == final_wrist_rot){
        Serial.print("[M5.done] "+"TRUE");
    } else {Serial.print("[M5.done] "+"false");}

    if (current_gripper == final_gripper){
        Serial.println("[M6.done] "+"TRUE");
    } else {Serial.println("[M6.done] "+"false");}

}

/*std::string state2method(State state) {
    //
}*/

class Manipulator {
public:
    Manipulator() {};

    void go_step(bool input, int deg) {
        //Serial.print("[#iter] [input = from BUTTON_PIN]");
        Serial.println("["+String(iter)+"] "+"[input "+String(input)+"]"); //номер итерации, состояние кнопки

        if (state == WAIT || state == UP || state == LOW_ || state == PICKUP || state == UP_PICKUP ||
                state == UP_OUT_PICKUP || state == LOW_OUT_PICKUP || state == DROP || state == UP_OUT) {
            digitalWrite(CONTROL_PIN, HIGH);
            if (state == WAIT && input == 1) {
                state = state + 1;
                    //Serial.print("[#iter] [out = go_wait]");
                    //Serial.println("["+String(iter)+"] "+"[out = "+strState[state]+"]");
            } else if (state != WAIT) {
                if (state == DONE){
                    Serial.println("["+String(iter)+"] "+"[out = DONE!]");
                    state = GO_WAIT;
                } else {state = state + 1;}

                // if (state == GO_WAIT) {
                //     Serial.print("[#iter] [out = go_wait]");
                // } else if ...
            }

        } else if (state == GO_WAIT) {
            go_wait();

        } else if (state == GO_UP) {
            _go_up();

        } else if (state == GO_LOW) {
            _go_low();

        } else if (state == GO_PICKUP) {
            _go_pickup();

        } else if (state == GO_UP_PICKUP) {
            _go_up_pickup();

        } else if (state == GO_UP_OUT_PICKUP) {
            _go_up_out_pickup(deg);

        } else if (state == GO_LOW_OUT_PICKUP) {
            _go_low_out_pickup(deg);

        } else if (state == GO_DROP) {
            _go_drop(deg);

        } else if (state == GO_UP_OUT){
            _go_up_out(deg);
        }

    }
    // переход в позицию ожидания вход сигнала
    void go_wait() {
        Serial.println("["+String(iter)+"] "+"[out = "+strState[state]+"]");
        Braccio.ServoMovement(20,  0, 140, 0, 10, 0, 73, callback);
        state = state+1;
        delay(100);
    }

    // перенос груза с позиции 0 на указанный
    void go(bool input, int deg) {

        if (state == WAIT && input == 1) {
            state = state + 1;
            Serial.println("going to up...");
            _go_up();
            Serial.println("reached and position up");
            delay(100);

        } else if (state == UP) {
            state = state + 1;
            Serial.println("going to low...");
            _go_low();
            Serial.println("reached and position low");
            delay(100);

        } else if (state == LOW_) {
            state = state + 1;
            Serial.println("going to pickup...");
            _go_pickup();
            Serial.println("reached and position pickup");
            delay(100);

        } else if (state == PICKUP) {
            state = state + 1;
            Serial.println("going to up_pickup...");
            _go_up_pickup();
            Serial.println("reached and position up_pickup");
            delay(100);

        } else if (state == UP_PICKUP) {
            state = state + 1;
            Serial.println("going to up_out_pickup...");
            _go_up_out_pickup(deg);
            Serial.println("reached and position up_out_pickup");
            delay(100);

        } else if (state == UP_OUT_PICKUP) {
            state = state + 1;
            Serial.println("going to low_out_pickup...");
            _go_low_out_pickup(deg);
            Serial.println("reached and position low_out_pickup");
            delay(100);

        } else if (state == LOW_OUT_PICKUP) {
            state = state + 1;
            Serial.println("going to drop...");
            _go_drop(deg);
            Serial.println("reached and position drop");
            delay(100);

        } else if (state == DROP) {
            state = state + 1;
            Serial.println("DONE!");

        } else if (state == DONE) {
            state = WAIT;
            Serial.println("going to wait...");
            go_wait();
            Serial.println("reached wait state");
        }

    }

private:
    // Верхняя позиция на 0 град
    void _go_up() {
        Serial.println("["+String(iter)+"] "+"[out = "+strState[state]+"]");
        Braccio.ServoMovement(20, 0, 115,  0, 15, 90, 73, callback);
        state = state+1;
    }

    // Нижняя позиция на 0 град
    void _go_low() {
        Serial.println("["+String(iter)+"] "+"[out = "+strState[state]+"]");
        Braccio.ServoMovement(30, 0, 100,  0, 10, 90, 73, callback);
        state = state+1;
    }

    // Захват груза
    void _go_pickup() {
        Serial.println("["+String(iter)+"] "+"[out = "+strState[state]+"]");
        Braccio.ServoMovement(30, 0, 100,  0, 10, 90, 10, callback);
        state = state+1;
    }

    // Верхняя позиция с грузом на 0
    void _go_up_pickup() {
        Serial.println("["+String(iter)+"] "+"[out = "+strState[state]+"]");
        Braccio.ServoMovement(30, 0, 115,  0, 15, 90, 10, callback);
        state = state+1;
    }
    //////////////////////////////////////////////////
    // Переход на указанный угол с грузом (верхняя позиция)
    void _go_up_out_pickup(int deg) {
        Serial.println("["+String(iter)+"] "+"[out = "+strState[state]+"]");
        if (deg <= 180) {
            Braccio.ServoMovement(20, deg, 115,  0, 15, 90, 10, callback);
        } else {
            Braccio.ServoMovement(20, deg - 180, 65, 180, 165, 90, 10, callback);
        }
        state = state+1;
    }

    // Нижняя позиция с грузом на указанном угле
    void _go_low_out_pickup(int deg) {
        Serial.println("["+String(iter)+"] "+"[out = "+strState[state]+"]");
        if (deg <= 180) {
            Braccio.ServoMovement(30, deg, 100,  0, 10, 90, 10, callback);
        } else {
            Braccio.ServoMovement(30, deg - 180, 80, 180, 170, 90, 10, callback);
        }
        state = state+1;
    }

    // Бросание груза на указанном угле
    void _go_drop(int deg) {
        Serial.println("["+String(iter)+"] "+"[out = "+strState[state]+"]");
        if (deg <= 180) {
            Braccio.ServoMovement(30, deg, 100,  0, 10, 90, 73, callback);
        } else {
            Braccio.ServoMovement(30,  deg - 180, 80, 180, 170, 90, 73, callback);
        }
        state = state+1;
    }

    void _go_up_out(int deg) {
        Serial.println("["+String(iter)+"] "+"[out = "+strState[state]+"]");
        if (deg <= 180) {
            Braccio.ServoMovement(20, deg, 115,  0, 15, 90, 73, callback);
        } else {
            Braccio.ServoMovement(20, deg - 180, 65, 180, 165, 90, 73, callback);
        }
        state = state+1;
    }
};

Manipulator man;

void setup() {
    Serial.begin(9600);
    pinMode(BUTTON_PIN, INPUT);
    Braccio.begin();
    state = GO_WAIT;
}

void loop() {
    iter++;
    //man.go(digitalRead(BUTTON_PIN),180); //Перекладывание груза с 0 град на указанный угол
    man.go_step((digitalRead(BUTTON_PIN), 180);
}
