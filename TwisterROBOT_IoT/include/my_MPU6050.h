#ifndef _MY_MPU6050__H
#define _MY_MPU6050__H

#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"

#include "WiFi_BLE.h"

void MPU6050_setup(void);

void MPU6050_getData();

void MPU6050_GUILog();

void MPU6050_SendJSONPack();

// ypr[0] = yaw(2pi) ; ypr[1] = pitch(2pi) ;  ypr[2] = roll(2pi)
extern float ypr[3];

#endif // !_MY_MPU6050__H