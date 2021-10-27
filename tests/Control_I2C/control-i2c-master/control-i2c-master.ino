#include <Wire.h>

#define I2C_SLAVE_ADDRESS 11


void setup() {
    Wire.begin(); // begin as master
    Serial.begin(9600);
}

void read_angles() {
    // Read the state (current angles) of the plant
    int current_angle[6];
    Wire.requestFrom(I2C_SLAVE_ADDRESS, 6);
    for (int i=0; i<6; i++){
        current_angle[i] = Wire.read();
    }

    Serial.println("recieved state: ");
    for (int i=0; i<6; i++){
        Serial.print(current_angle[i]);
        Serial.print(" ");
    }
    Serial.println();
}

// Send controls (desired angles) to the plant
void send_angles(uint8_t angles[6]) {
    Wire.beginTransmission(I2C_SLAVE_ADDRESS);
    Wire.write(angles, 6);
    Wire.endTransmission();
}

void loop() {
    uint8_t positions[][6] = {
        {0,140,0,10,0,10},
        {0,65,180,160,90,10},
        {0,80,180,170,90,10},
        {90,65,180,160,90,73},
    };

    for(auto item : positions){
        
        send_angles(item);
        delay(500);
        read_angles();
        delay(500);
        read_angles();
        delay(500);
        read_angles();
        delay(500);
        read_angles();
        delay(500);
        read_angles();
    
    }
}
