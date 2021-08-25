/* Движение манипулятора (перенос груза из 0 на указанный угол) по нажатой кнопке
 * с использованием класса. Переписано к виду КА inter-wise
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

unsigned iter = 0;

void go_wait() {
    Braccio.ServoMovement(20,  0, 140, 0, 10, 0, 10);
}

// Верхняя позиция на 0 град
void go_up() {
    Braccio.ServoMovement(20, 0, 115,  0, 15, 90, 10);
}

// Нижняя позиция на 0 град
void go_low() {
    Braccio.ServoMovement(30, 0, 100,  0, 10, 90, 10);
}

// Захват груза
void go_pickup() {
    Braccio.ServoMovement(30, 0, 100,  0, 10, 90, 73);
}

// Верхняя позиция с грузом на 0
void go_up_pickup() {
    Braccio.ServoMovement(30, 0, 115,  0, 15, 90, 73);
}
//////////////////////////////////////////////////
// Переход на указанный угол с грузом (верхняя позиция)
void go_up_out_pickup() {
    Braccio.ServoMovement(20, 180, 115,  0, 15, 90, 73);
}

// Нижняя позиция с грузом на указанном угле
void go_low_out_pickup() {
    Braccio.ServoMovement(30, 180, 100,  0, 10, 90, 73);
}

// Бросание груза на указанном угле
void go_drop() {
    Braccio.ServoMovement(30, 180, 100,  0, 10, 90, 10);
}

void go_up_out() {
    Braccio.ServoMovement(20, 180, 115,  0, 15, 90, 10);
}

void go_done() {
    Braccio.ServoMovement(20,  180, 140, 0, 10, 0, 10);
}

struct Controller {
public:

    struct Outputs {
        /// enum Action { Do_Nothing, Go_Wait,... } action;
        bool go_wait, go_up, go_low, go_pickup, go_up_pickup,
             go_up_out_pickup, go_low_out_pickup, go_drop, go_up_out, go_done;
        bool done;
    };

    Outputs out;

    enum State {
        BEGIN, GO_WAIT, WAIT, GO_UP, GO_LOW, GO_PICKUP, GO_UP_PICKUP,
        GO_UP_OUT_PICKUP, GO_LOW_OUT_PICKUP, GO_DROP, GO_UP_OUT, GO_DONE, DONE
    };

    State state = BEGIN;

    void go_step(bool input) {
        if (state == BEGIN) {
            state = GO_WAIT;
            make_all_out_false();
            out.go_wait = true;
        } else if (state == GO_WAIT) {
            state = WAIT;
            make_all_out_false();
            //out.go_wait = true;
        } else if (state == WAIT && input) {
            state = GO_UP;
            make_all_out_false();
            out.go_up = true;
        } else if (state == GO_UP) {
            state = GO_LOW;
            make_all_out_false();
            out.go_low = true;
        } else if (state == GO_LOW) {
            state = GO_PICKUP;
            make_all_out_false();
            out.go_pickup = true;
        } else if (state == GO_PICKUP) {
            state = GO_UP_PICKUP;
            make_all_out_false();
            out.go_up_pickup = true;
        } else if (state == GO_UP_PICKUP) {
            state = GO_UP_OUT_PICKUP;
            make_all_out_false();
            out.go_up_out_pickup = true;
        } else if (state == GO_UP_OUT_PICKUP) {
            state = GO_LOW_OUT_PICKUP;
            make_all_out_false();
            out.go_low_out_pickup = true;
        } else if (state == GO_LOW_OUT_PICKUP) {
            state = GO_DROP;
            make_all_out_false();
            out.go_drop = true;
        } else if (state == GO_DROP) {
            state = GO_UP_OUT;
            make_all_out_false();
            out.go_up_out = true;
        } else if (state == GO_UP_OUT) {
            state = GO_DONE;
            make_all_out_false();
            out.go_done = true;
        } else if (state == GO_DONE) {
            state = DONE;
            make_all_out_false();
            out.done = true;
        } else if (state == DONE) {
            state = GO_WAIT;
            make_all_out_false();
            out.go_wait = true;
        }
    }

private:
    void make_all_out_false() {
        out = Outputs();
    }

};

Controller control;

void setup() {
    Serial.begin(9600);
    pinMode(BUTTON_PIN, INPUT);
    Braccio.begin();
}

void loop() {
    iter++;

    bool but = digitalRead(BUTTON_PIN);
    control.go_step(but);
    // Вывод номера итерации, состояния кнопки и состояний Outputs
    Serial.print("[" + String(iter) + "] " + "[but = " + String(but) + "] ");
    Serial.print(" [go_wait = " + String(control.out.go_wait) + "]");
    Serial.print(" [go_up = " + String(control.out.go_up) + "]");
    Serial.print(" [go_low = " + String(control.out.go_low) + "]");
    Serial.print(" [go_pickup = " + String(control.out.go_pickup) + "]");
    Serial.print(" [go_up_pickup = " + String(control.out.go_up_pickup) + "]");
    Serial.print(" [go_up_out_pickup = " + String(control.out.go_up_out_pickup) + "]");
    Serial.print(" [go_low_out_pickup = " + String(control.out.go_low_out_pickup) + "]");
    Serial.print(" [go_drop = " + String(control.out.go_drop) + "]");
    Serial.print(" [go_up_out = " + String(control.out.go_up_out) + "]");
    Serial.print(" [go_done = " + String(control.out.go_done) + "]");
    Serial.print(" [done = " + String(control.out.done) + "]");
    Serial.println();

    // В зависимости от Outputs выполнить действие
    if (control.out.go_wait) {
        go_wait();
    } else if (control.out.go_up) {
        go_up();
    } else if (control.out.go_low) {
        go_low();
    } else if (control.out.go_pickup) {
        go_pickup();
    } else if (control.out.go_up_pickup) {
        go_up_pickup();
    } else if (control.out.go_up_out_pickup) {
        go_up_out_pickup();
    } else if (control.out.go_low_out_pickup) {
        go_low_out_pickup();
    } else if (control.out.go_drop) {
        go_drop();
    } else if (control.out.go_up_out) {
        go_up_out();
    } else if (control.out.go_done) {
        go_done();
    }
}
