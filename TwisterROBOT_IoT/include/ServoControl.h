#ifndef _SERVOCONTROL__H
#define _SERVOCONTROL__H

#include "Arduino.h"

class Servo{
public:
    Servo(std::string name, uint8_t GPIO, uint8_t channel);

    uint16_t offset;
    uint16_t currentAngle;

    void setAngle_180(uint16_t angle);
    void setAngle_270(uint16_t angle);
private:
    std::string name;

    uint8_t GPIO;
    uint8_t channel;

    static inline void api_writePWM(Servo target, uint32_t value){
        ledcWrite(target.channel, value);
    }
};

#endif // !_SERVOCONTROL__H