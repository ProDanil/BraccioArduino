// КРЕСТИКИ-НОЛИКИ //
// COM vs HUMAN

// TODO:
//       FIX ENDS

/*    
    1 | 2 | 3
   ---|---|---
    4 | 5 | 6
   ---|---|---
    7 | 8 | 9
*/

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

#include <MyBraccioGrip.h>
#include <Servo.h>

#define COM_PIN 7
#define HUMAN_PIN 8

bool inputArr[9] = {false, false, false, false, false, false, false, false, false}; //входной массив для манипулятора
bool comArr[9] = {false, false, false, false, false, false, false, false, false}; // клетки, занятые роботом
bool humanArr[9] = {false, false, false, false, false, false, false, false, false}; // клетки, занятые человеком
bool play_field[9] = {false, false, false, false, false, false, false, false, false}; // все занятые клетки
bool control_done = false;

int number;

Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_ver;
Servo wrist_rot;
Servo gripper;


void setup() {
    Serial.begin(115200);
    pinMode(A0, INPUT); // кнопки полей 1, 2, 3
    pinMode(A1, INPUT); // кнопки полей 4, 5, 6
    pinMode(A2, INPUT); // кнопки полей 7, 8, 9
    pinMode(COM_PIN, OUTPUT);
    pinMode(HUMAN_PIN, OUTPUT);
    Braccio.begin(20);
    randomSeed(analogRead(A3));
    
}

class Game {
public:
    Game(){};

    bool first_com;

    enum Step {
        BEGIN,
        WAIT,
        FIRST_COM,
        FIRST_HUMAN,
        CHECK,
        STEP_HUMAN,
        STEP_COM,
        DRAW,
        WIN,
        LOSE
    };
    Step step_game;

    void go_gamer(){

        if (step_game == BEGIN){ // рандомный выбор кто ходит первым
            first_com = false;
            digitalWrite(COM_PIN, LOW);
            digitalWrite(HUMAN_PIN, LOW);
            number = random(1, 3);
            if (number == 1){
                step_game = FIRST_HUMAN;
                Serial.println("FIRST HUMAN");
            } else {
                step_game = FIRST_COM;
                first_com = true;
                Serial.println("FIRST COM");
            }

        } else if (step_game == WAIT){ // ожидает, пока манипулятор закончит работу
            if (control_done){
                step_game = CHECK;
                control_done = false;
            }

        } else if (step_game == FIRST_HUMAN){
            digitalWrite(HUMAN_PIN, HIGH);
            go_human();

        } else if (step_game == FIRST_COM){
            digitalWrite(COM_PIN, HIGH);
            step_game = WAIT;
            number = random(0, 9); // выбор рандомной клетки
            comArr[number] = true;

        } else if (step_game == CHECK){
            check_game();

        } else if (step_game == STEP_HUMAN){
            go_human();

        } else if (step_game == STEP_COM){
            step_game = WAIT;
            digitalWrite(COM_PIN, LOW);
            // проверка по вертикали и горизонтали, сможет ли робот выйграть следующим ходом, если да - выйграть 
            for (int i = 0; i < 3; i++){ 
                if ((comArr[0+i*3] && comArr[1+i*3] && !humanArr[2+i*3]) || (comArr[0+i*3] && comArr[2+i*3] && !humanArr[1+i*3]) || (comArr[1+i*3] && comArr[2+i*3] && !humanArr[0+i*3])){
                    comArr[0+i*3] = true;
                    comArr[1+i*3] = true;
                    comArr[2+i*3] = true;
                    return;

                } else if ((comArr[0+i] && comArr[3+i] && !humanArr[6+i]) || (comArr[0+i] && comArr[6+i] && !humanArr[3+i]) || (comArr[3+i] && comArr[6+i] && !humanArr[0+i])){
                    comArr[0+i] = true;
                    comArr[3+i] = true;
                    comArr[6+i] = true;
                    return;
                }
            }
            // проверка диагоналей для третьего победного хода
            for (int i = 0; i < 2; i++){
                if ((comArr[0+i*2] && comArr[4] && !humanArr[8-i*2]) || (comArr[0+i*2] && comArr[8-i*2] && !humanArr[4]) || (comArr[4] && comArr[8-i*2] && !humanArr[0+i*2])){
                    comArr[0+i*2] = true;
                    comArr[4] = true;
                    comArr[8-i*2] = true;
                    return;
                }
            }
            // проверка по вертикали и горизонтали, сможет ли человек выйграть следующим ходом, если да - помешать
            for (int i = 0; i < 3; i++){
                if ((humanArr[0+i*3] && humanArr[1+i*3] && !comArr[2+i*3]) || (humanArr[0+i*3] && humanArr[2+i*3] && !comArr[1+i*3]) || (humanArr[1+i*3] && humanArr[2+i*3] && !comArr[0+i*3])){
                    comArr[0+i*3] = !humanArr[0+i*3];
                    comArr[1+i*3] = !humanArr[1+i*3];
                    comArr[2+i*3] = !humanArr[2+i*3];
                    return;

                } else if ((humanArr[0+i] && humanArr[3+i] && !comArr[6+i]) || (humanArr[0+i] && humanArr[6+i] && !comArr[3+i]) || (humanArr[3+i] && humanArr[6+i] && !comArr[0+i])){
                    comArr[0+i] = !humanArr[0+i];
                    comArr[3+i] = !humanArr[3+i];
                    comArr[6+i] = !humanArr[6+i];
                    return;
                }
            }
            // проверка диагоналей, не дать выйграть человеку
            for (int i = 0; i < 2; i++){
                if ((humanArr[0+i*2] && humanArr[4] && !comArr[8-i*2]) || (humanArr[0+i*2] && humanArr[8-i*2] && !comArr[4]) || (humanArr[4] && humanArr[8-i*2] && !comArr[0+i*2])){
                    comArr[0+i*2] = !humanArr[0+i*2];
                    comArr[4] = !humanArr[4];
                    comArr[8-i*2] = !humanArr[8-i*2];
                    return;
                }
            }
           // выбор рандомной свободной клетки
            while(play_field[number]){
                number = random(0, 9);
            }
            comArr[number] = true;

        } else if (step_game == WIN){ // победа робота
            digitalWrite(COM_PIN, HIGH);
            for (int i; i < 9; i++){
                inputArr[i] = false;
                comArr[i] = false;
                humanArr[i] = false;
                play_field[i] = false;
            }
            step_game = BEGIN;
            Serial.println("COM WIN");
            delay(5000);

        } else if (step_game == LOSE){ // проигрышь робота
            digitalWrite(HUMAN_PIN, HIGH);
            for (int i; i < 9; i++){
                inputArr[i] = false;
                comArr[i] = false;
                humanArr[i] = false;
                play_field[i] = false;
            }
            step_game = BEGIN;
            Serial.println("HUMAN WIN");
            delay(5000);

        } else if (step_game == DRAW){ // ничья
            digitalWrite(COM_PIN, HIGH);
            digitalWrite(HUMAN_PIN, HIGH);
            for (int i; i < 9; i++){
                inputArr[i] = false;
                comArr[i] = false;
                humanArr[i] = false;
                play_field[i] = false;
            }
            step_game = BEGIN;
            Serial.println("DRAW");
            delay(5000);
        }
    }

private:
    void check_game(){ // проверка на исход игры (WIN, LOSE, DRAW)
        // проверка по горизонтал и вертикали
        for (int i = 0; i < 3; i++){  
            if ((humanArr[0+i*3] && humanArr[1+i*3] && humanArr[2+i*3]) || (humanArr[0+i*3] && humanArr[2+i*3] && humanArr[1+i*3]) || (humanArr[1+i*3] && humanArr[2+i*3] && humanArr[0+i*3])){
                step_game = LOSE;
                return;

            } else if ((comArr[0+i*3] && comArr[1+i*3] && comArr[2+i*3]) || (comArr[0+i*3] && comArr[2+i*3] && comArr[1+i*3]) || (comArr[1+i*3] && comArr[2+i*3] && comArr[0+i*3])){
                step_game = WIN;
                return;

            } else if ((humanArr[0+i] && humanArr[3+i] && humanArr[6+i]) || (humanArr[0+i] && humanArr[6+i] && humanArr[3+i]) || (humanArr[3+i] && humanArr[6+i] && humanArr[0+i])){
                step_game = LOSE;
                return;

            } else if ((comArr[0+i] && comArr[3+i] && comArr[6+i]) || (comArr[0+i] && comArr[6+i] && comArr[3+i]) || (comArr[3+i] && comArr[6+i] && comArr[0+i])){
                step_game = WIN;
                return;
            }
        }

        // проверка диагоналей
        for (int i = 0; i < 2; i++){
            if ((humanArr[0+i*2] && humanArr[4] && humanArr[8-i*2]) || (humanArr[0+i*2] && humanArr[8-i*2] && humanArr[4]) || (humanArr[4] && humanArr[8-i*2] && humanArr[0+i*2])){
                step_game = LOSE;
                return;

            } else if ((comArr[0+i*2] && comArr[4] && comArr[8-i*2]) || (comArr[0+i*2] && comArr[8-i*2] && comArr[4]) || (comArr[4] && comArr[8-i*2] && comArr[0+i*2])){
                step_game = WIN;
                return;
            }
        }

        int full_play_field = 0;
        int full_com = 0;
        int full_human = 0;
        // проверка ничьи
        for (int i = 0; i < 9; i++){
            if (play_field[i]){
                full_play_field += 1;
            }

            if (comArr[i]){
                full_com += 1;
            }

            if (humanArr[i]){
                full_human += 1;
            }
        }

        if (full_play_field == 9){
            step_game = DRAW;
            return;
        }
        // чей следующий ход
        if ((full_human == full_com && first_com) || (full_human != full_com && !first_com)){
            step_game = STEP_COM;
        } else {
            step_game = STEP_HUMAN;
        }

    }

    void go_human(){ // ход человека

        int value1 = analogRead(A0);
        int value2 = analogRead(A1);
        int value3 = analogRead(A2);

        byte input = 0; // номер клетки на поле

        if (value1 >= 400){
            if (value1 >= 400 && value1 <= 420){
                input = 1;
            } else if (value1 >= 680 && value1 <= 700){
                input = 2;
            } else if (value1 >= 1000){
                input = 3;
            }
        }

        if (value2 >= 400){
            if (value2 >= 400 && value2 <= 420){
                input = 4;
            } else if (value2 >= 680 && value2 <= 700){
                input = 5;
            } else if (value2 >= 1000){
                input = 6;
            }
        }

        if (value3 >= 400){
            if (value3 >= 400 && value3 <= 420){
                input = 7;
            } else if (value3 >= 680 && value3 <= 700){
                input = 8;
            } else if (value3 >= 1000){
                input = 9;
            }
        }

        if (input && !play_field[input-1]){
            humanArr[input-1] = true;
            play_field[input-1] = true;
            Serial.println("HUMAN: "+String(input));
            step_game = CHECK;
            digitalWrite(HUMAN_PIN, LOW);
            Serial.print("humanArr: ");
            Serial.print(humanArr[0]);
            Serial.print(" ");
            Serial.print(humanArr[1]);
            Serial.print(" ");
            Serial.print(humanArr[2]);
            Serial.print(" ");
            Serial.print(humanArr[3]);
            Serial.print(" ");
            Serial.print(humanArr[4]);
            Serial.print(" ");
            Serial.print(humanArr[5]);
            Serial.print(" ");
            Serial.print(humanArr[6]);
            Serial.print(" ");
            Serial.print(humanArr[7]);
            Serial.print(" ");
            Serial.print(humanArr[8]);
            Serial.println(" ");

            Serial.print("play_field: ");
            Serial.print(play_field[0]);
            Serial.print(" ");
            Serial.print(play_field[1]);
            Serial.print(" ");
            Serial.print(play_field[2]);
            Serial.print(" ");
            Serial.print(play_field[3]);
            Serial.print(" ");
            Serial.print(play_field[4]);
            Serial.print(" ");
            Serial.print(play_field[5]);
            Serial.print(" ");
            Serial.print(play_field[6]);
            Serial.print(" ");
            Serial.print(play_field[7]);
            Serial.print(" ");
            Serial.print(play_field[8]);
            Serial.println(" ");
            
        }
    }
};

Game game;

struct Controller { // управление манипулятором
public:

    enum struct M1Angle {
        A0 = 0,
        A65 = 65,
        A73 = 73,
        A77 = 77,
        A90 = 90,
        A103 = 103,
        A107 = 107,
        A115 = 115,
        A180 = 180
    };
    enum struct M2Angle {
        A60 = 60,
        A80 = 80,
        A90 = 90,
        A110 = 110,
        A115 = 115,
        A140 = 140
    };
    enum struct M3Angle {
        A0 = 0,
        A20 = 20,
        A40 = 40
    };
    enum struct M4Angle {
        A0 = 0,
        A10 = 10,
        A15 = 15,
        A20 = 20
    };
    enum struct M5Angle {
        A0 = 0,
        A90 = 90
    };
    enum struct M6Angle {
        A10 = 10,
        A73 = 73
    };
    
    M1Angle m1_angle;
    M2Angle m2_angle;
    M3Angle m3_angle;
    M4Angle m4_angle;
    M5Angle m5_angle;
    M6Angle m6_angle;

    enum State {
        BEGIN, GO_WAIT, WAIT, GO_UP, GO_LOW, GO_PICKUP, GO_UP_PICKUP,
        GO_UP_PICK_1, GO_UP_PICK_2, GO_UP_PICK_3, GO_UP_PICK_4, GO_UP_PICK_5,
        GO_UP_PICK_6, GO_UP_PICK_7, GO_UP_PICK_8, GO_UP_PICK_9,
        GO_LOW_PICK_1, GO_LOW_PICK_2, GO_LOW_PICK_3, GO_LOW_PICK_4, GO_LOW_PICK_5,
        GO_LOW_PICK_6, GO_LOW_PICK_7, GO_LOW_PICK_8, GO_LOW_PICK_9,
        GO_DROP_1, GO_DROP_2, GO_DROP_3, GO_DROP_4, GO_DROP_5,
        GO_DROP_6, GO_DROP_7, GO_DROP_8, GO_DROP_9,
        GO_UP_1, GO_UP_2, GO_UP_3, GO_UP_4, GO_UP_5,
        GO_UP_6, GO_UP_7, GO_UP_8, GO_UP_9
    };
    State state;

    void go_step(bool input_1, bool input_2, bool input_3, bool input_4, bool input_5, bool input_6, bool input_7, 
                  bool input_8, bool input_9, bool m1_is_done, bool m2_is_done, bool m3_is_done, bool m4_is_done, 
                  bool m5_is_done, bool m6_is_done) {

        bool m_is_done = m1_is_done && m2_is_done && m3_is_done && m4_is_done && m5_is_done && m6_is_done;
        bool input = input_1 || input_2 || input_3 || input_4 || input_5 || input_6 || input_7 || input_8 || input_9; 

        if (state == BEGIN) {
            state = GO_WAIT;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A140;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A0;
            m6_angle = M6Angle::A10;

        } else if (state == GO_WAIT && m_is_done) {
            state = WAIT;

        } else if (state == WAIT && input) {
            state = GO_UP;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A15;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP && m_is_done) {
            state = GO_LOW;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A90;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_LOW && m_is_done) {
            state = GO_PICKUP;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A90;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_PICKUP && m_is_done) {
            state = GO_UP_PICKUP;
            m1_angle = M1Angle::A0;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A15;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_PICKUP && m_is_done && input_1) {
            Serial.println("GO_1");
            play_field[0] = true;
            state = GO_UP_PICK_1;
            m1_angle = M1Angle::A77;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_PICK_1 && m_is_done) {
            state = GO_LOW_PICK_1;
            m1_angle = M1Angle::A77;
            m2_angle = M2Angle::A60;
            m3_angle = M3Angle::A40;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_LOW_PICK_1 && m_is_done) {
            state = GO_DROP_1;
            m1_angle = M1Angle::A77;
            m2_angle = M2Angle::A60;
            m3_angle = M3Angle::A40;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_DROP_1 && m_is_done) {
            state = GO_UP_1;
            m1_angle = M1Angle::A77;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_PICKUP && m_is_done && input_2) {
            Serial.println("GO_2");
            play_field[1] = true;
            state = GO_UP_PICK_2;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_PICK_2 && m_is_done) {
            state = GO_LOW_PICK_2;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A60;
            m3_angle = M3Angle::A40;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_LOW_PICK_2 && m_is_done) {
            state = GO_DROP_2;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A60;
            m3_angle = M3Angle::A40;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_DROP_2 && m_is_done) {
            state = GO_UP_2;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_PICKUP && m_is_done && input_3) {
            Serial.println("GO_3");
            play_field[2] = true;
            state = GO_UP_PICK_3;
            m1_angle = M1Angle::A103;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_PICK_3 && m_is_done) {
            state = GO_LOW_PICK_3;
            m1_angle = M1Angle::A103;
            m2_angle = M2Angle::A60;
            m3_angle = M3Angle::A40;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_LOW_PICK_3 && m_is_done) {
            state = GO_DROP_3;
            m1_angle = M1Angle::A103;
            m2_angle = M2Angle::A60;
            m3_angle = M3Angle::A40;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_DROP_3 && m_is_done) {
            state = GO_UP_3;
            m1_angle = M1Angle::A103;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_PICKUP && m_is_done && input_4) {
            Serial.println("GO_4");
            play_field[3] = true;
            state = GO_UP_PICK_4;
            m1_angle = M1Angle::A73;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_PICK_4 && m_is_done) {
            state = GO_LOW_PICK_4;
            m1_angle = M1Angle::A73;
            m2_angle = M2Angle::A80;
            m3_angle = M3Angle::A20;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_LOW_PICK_4 && m_is_done) {
            state = GO_DROP_4;
            m1_angle = M1Angle::A73;
            m2_angle = M2Angle::A80;
            m3_angle = M3Angle::A20;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_DROP_4 && m_is_done) {
            state = GO_UP_4;
            m1_angle = M1Angle::A73;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_PICKUP && m_is_done && input_5) {
            Serial.println("GO_5");
            play_field[4] = true;
            state = GO_UP_PICK_5;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_PICK_5 && m_is_done) {
            state = GO_LOW_PICK_5;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A80;
            m3_angle = M3Angle::A20;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_LOW_PICK_5 && m_is_done) {
            state = GO_DROP_5;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A80;
            m3_angle = M3Angle::A20;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_DROP_5 && m_is_done) {
            state = GO_UP_5;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_PICKUP && m_is_done && input_6) {
            Serial.println("GO_6");
            play_field[5] = true;
            state = GO_UP_PICK_6;
            m1_angle = M1Angle::A107;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_PICK_6 && m_is_done) {
            state = GO_LOW_PICK_6;
            m1_angle = M1Angle::A107;
            m2_angle = M2Angle::A80;
            m3_angle = M3Angle::A20;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_LOW_PICK_6 && m_is_done) {
            state = GO_DROP_6;
            m1_angle = M1Angle::A107;
            m2_angle = M2Angle::A80;
            m3_angle = M3Angle::A20;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_DROP_6 && m_is_done) {
            state = GO_UP_6;
            m1_angle = M1Angle::A107;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_PICKUP && m_is_done && input_7) {
            Serial.println("GO_7");
            play_field[6] = true;
            state = GO_UP_PICK_7;
            m1_angle = M1Angle::A65;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_PICK_7 && m_is_done) {
            state = GO_LOW_PICK_7;
            m1_angle = M1Angle::A65;
            m2_angle = M2Angle::A110;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A0;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_LOW_PICK_7 && m_is_done) {
            state = GO_DROP_7;
            m1_angle = M1Angle::A65;
            m2_angle = M2Angle::A110;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A0;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_DROP_7 && m_is_done) {
            state = GO_UP_7;
            m1_angle = M1Angle::A65;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_PICKUP && m_is_done && input_8) {
            Serial.println("GO_8");
            play_field[7] = true;
            state = GO_UP_PICK_8;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_PICK_8 && m_is_done) {
            state = GO_LOW_PICK_8;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A110;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A0;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_LOW_PICK_8 && m_is_done) {
            state = GO_DROP_8;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A110;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A0;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_DROP_8 && m_is_done) {
            state = GO_UP_8;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_UP_PICKUP && m_is_done && input_9) {
            Serial.println("GO_9");
            play_field[8] = true;
            state = GO_UP_PICK_9;
            m1_angle = M1Angle::A115;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_UP_PICK_9 && m_is_done) {
            state = GO_LOW_PICK_9;
            m1_angle = M1Angle::A115;
            m2_angle = M2Angle::A110;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A0;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A73;

        } else if (state == GO_LOW_PICK_9 && m_is_done) {
            state = GO_DROP_9;
            m1_angle = M1Angle::A115;
            m2_angle = M2Angle::A110;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A0;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if (state == GO_DROP_9 && m_is_done) {
            state = GO_UP_9;
            m1_angle = M1Angle::A115;
            m2_angle = M2Angle::A115;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A20;
            m5_angle = M5Angle::A90;
            m6_angle = M6Angle::A10;

        } else if ((state == GO_UP_1 || state == GO_UP_2 || state == GO_UP_3 || state == GO_UP_4 || state == GO_UP_5 || state == GO_UP_6 || state == GO_UP_7 || state == GO_UP_8 || state == GO_UP_9) && m_is_done){
            state = GO_WAIT;
            control_done = true;
            m1_angle = M1Angle::A90;
            m2_angle = M2Angle::A140;
            m3_angle = M3Angle::A0;
            m4_angle = M4Angle::A10;
            m5_angle = M5Angle::A0;
            m6_angle = M6Angle::A10;

        }
        
    }

private:

};

Controller control;

void callback(
    int final_base, int final_shoulder, int final_elbow,
    int final_wrist_ver, int final_wrist_rot, int final_gripper,
    int current_base, int current_shoulder, int current_elbow,
    int current_wrist_ver, int current_wrist_rot, int current_gripper
) {
    Serial.print("Desired values: (");
    Serial.print(String(final_base) +", "+ String(final_shoulder) +", "+ String(final_elbow) +", "+
    String(final_wrist_ver) +", "+ String(final_wrist_rot) +", "+ String(final_gripper));
    Serial.println(")");
    Serial.print("Current values: (");
    Serial.print(String(current_base) +", "+ String(current_shoulder) +", "+ String(current_elbow) +", "+
    String(current_wrist_ver) +", "+ String(current_wrist_rot) +", "+ String(current_gripper));
    Serial.println(")");

    Serial.print("["+String(iter)+"] ");
    if (current_base == final_base){
        Serial.print("[M1.done] TRUE ");
    } else {Serial.print("[M1.done] false ");}

    if (current_shoulder == final_shoulder){
        Serial.print("[M2.done] TRUE ");
    } else {Serial.print("[M2.done] false ");}

    if (current_elbow == final_elbow){
        Serial.print("[M3.done] TRUE ");
    } else {Serial.print("[M3.done] false ");}

    if (current_wrist_ver == final_wrist_ver){
        Serial.print("[M4.done] TRUE ");
    } else {Serial.print("[M4.done] false ");}

    if (current_wrist_rot == final_wrist_rot){
        Serial.print("[M5.done] TRUE ");
    } else {Serial.print("[M5.done] false ");}

    if (current_gripper == final_gripper){
        Serial.println("[M6.done] TRUE ");
    } else {Serial.println("[M6.done] false ");}

}


void loop() {
    game.go_gamer();
    // проверка, сделал ли ход робот
    for (int i = 0; i < 9; i++){
        inputArr[i] = (!play_field[i]&&humanArr[i])||(play_field[i]&&!humanArr[i]);
        inputArr[i] = (!inputArr[i]&&comArr[i])||(inputArr[i]&&!comArr[i]);
    }

    if (inputArr[0] || inputArr[1] || inputArr[2] || inputArr[3] || inputArr[4] || inputArr[5] || inputArr[6] || inputArr[7] || inputArr[8]){
        Serial.print("inputArr: ");
        Serial.print(inputArr[0]);
        Serial.print(" ");
        Serial.print(inputArr[1]);
        Serial.print(" ");
        Serial.print(inputArr[2]);
        Serial.print(" ");
        Serial.print(inputArr[3]);
        Serial.print(" ");
        Serial.print(inputArr[4]);
        Serial.print(" ");
        Serial.print(inputArr[5]);
        Serial.print(" ");
        Serial.print(inputArr[6]);
        Serial.print(" ");
        Serial.print(inputArr[7]);
        Serial.print(" ");
        Serial.print(inputArr[8]);
        Serial.println(" ");

        Serial.print("comArr: ");
        Serial.print(comArr[0]);
        Serial.print(" ");
        Serial.print(comArr[1]);
        Serial.print(" ");
        Serial.print(comArr[2]);
        Serial.print(" ");
        Serial.print(comArr[3]);
        Serial.print(" ");
        Serial.print(comArr[4]);
        Serial.print(" ");
        Serial.print(comArr[5]);
        Serial.print(" ");
        Serial.print(comArr[6]);
        Serial.print(" ");
        Serial.print(comArr[7]);
        Serial.print(" ");
        Serial.print(comArr[8]);
        Serial.println(" ");

        Serial.print("play_field: ");
        Serial.print(play_field[0]);
        Serial.print(" ");
        Serial.print(play_field[1]);
        Serial.print(" ");
        Serial.print(play_field[2]);
        Serial.print(" ");
        Serial.print(play_field[3]);
        Serial.print(" ");
        Serial.print(play_field[4]);
        Serial.print(" ");
        Serial.print(play_field[5]);
        Serial.print(" ");
        Serial.print(play_field[6]);
        Serial.print(" ");
        Serial.print(play_field[7]);
        Serial.print(" ");
        Serial.print(play_field[8]);
        Serial.println(" ");

    }

     control.go_step(inputArr[0], inputArr[1], inputArr[2], inputArr[3], inputArr[4], 
        inputArr[5], inputArr[6], inputArr[7], inputArr[8], 
        true, true, true, true, true, true);

    Braccio.ServoMovement(20, static_cast<int>(control.m1_angle), static_cast<int>(control.m2_angle), static_cast<int>(control.m3_angle),
     static_cast<int>(control.m4_angle), static_cast<int>(control.m5_angle), static_cast<int>(control.m6_angle));

}
