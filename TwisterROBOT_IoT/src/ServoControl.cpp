#include "ServoControl.h"

// Servo LegServo_Left("left");

// Servo LegServo_Right("right");

static uint16_t TimerFreq = 50000;
static uint16_t resolutionBit = 10;

Servo::Servo(std::string name, uint8_t GPIO, uint8_t channel){
    this->name = name;
    this->GPIO = GPIO;
    this->channel = channel;

    ledcSetup(channel, TimerFreq, resolutionBit);
    ledcAttachPin(GPIO, channel);
}

void Servo::setAngle_180(uint16_t angle){
    uint32_t tim_Temp = 0.5*1000 * TimerFreq;    // offset 0.5 ms

    angle += this->offset;

    tim_Temp += (angle / 180) * (2*1000 * TimerFreq);

    api_writePWM(*this, tim_Temp);
}

void Servo::setAngle_270(uint16_t angle){
    uint32_t tim_Temp = 0.5*1000 * TimerFreq;    // offset 0.5 ms

    angle += this->offset;

    tim_Temp += (angle / 270) * (2*1000 * TimerFreq);

    api_writePWM(*this, tim_Temp);
}

void Servo_setup(){

}