/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
float targetAngle_left = 60;
float targetAngle_right = 0;
float targetMotorSpeed_Left;
float targetMotorSpeed_Right;

float offset_parameters = -20.0f;   //重心偏移
float yaw_pitch_roll[3];
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern float angularDeviation;
/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void FOCTask(void const *argument) {
    Pid_Value_Init();
    FOC_Vbus(12.3f);    //3s 电池

    HAL_GPIO_WritePin(Driver1_EN_GPIO_Port, Driver1_EN_Pin, 1);
    HAL_GPIO_WritePin(Driver2_EN_GPIO_Port, Driver2_EN_Pin, 1);
    FOC_alignSensor(&FOCMotor_Right, 7, 1);
    FOC_alignSensor(&FOCMotor_Left, 7, -1);

    for (;;) {
        Speed_PID.Target = Position_Pid_Calculate(&Balance_PID);
        Speed_PID.Actual = (FOCMotor_Left.speed + FOCMotor_Right.speed) / 2;
        Speed_PID.Error = Speed_PID.Target - Speed_PID.Actual;
        float targetSpeed_BySpeedPID = Position_Pid_Calculate(&Speed_PID);

        FOC_setVelocity_currentControl(&FOCMotor_Left, targetMotorSpeed_Left, FOCMotor_Left.currentLimited);
        FOC_setVelocity_currentControl(&FOCMotor_Right, targetMotorSpeed_Left, FOCMotor_Right.currentLimited);
        osDelay(3);
    }
}

void ServoTask(void const *argument) {
    Servo_init();

    setAngle_270(&Servo_LeftLeg, 60);
    setAngle_270(&Servo_RightLeg, 5);
    osDelay(500);

    for (;;) {
//        offset_parameters = -20.0f + (targetAngle_right - 25) * 0.1f;
        offset_parameters = -20.0f;
        setAngle_270(&Servo_LeftLeg, targetAngle_left);
        setAngle_270(&Servo_RightLeg, targetAngle_right);
        osDelay(300);
    }
}

void LCDTask(void const *argument) {
    uart3_printf("LCD Task Start\n");
    ST7735_Init();
    gui_draw_init("NinoC137", 1);
    for (;;) {
        osDelay(10);
    }
}

void UARTTask(void const *argument) {
    HAL_UART_Receive_IT(&huart3, (uint8_t*)&uart3Buffer, 1);

    osEvent JsonQueueEvt;
    t_JsonPackage *JsonBuffer = NULL;

    for (;;) {
        JsonQueueEvt = osMessageGet(JsonQueueHandle, osWaitForever);

        if(JsonQueueEvt.status == osEventMessage){
            JsonBuffer = JsonQueueEvt.value.p;
//            uart3_printf("buffer:\r\n %s\r\n", JsonBuffer->JsonString);
            cmd_startParse(JsonBuffer->JsonString);
            osPoolFree(JsonQ_Mem, JsonBuffer);
        }

        Balance_PID.Target = offset_parameters;
        Balance_PID.Actual = yaw_pitch_roll[1];
        Balance_PID.Error = Balance_PID.Target - Balance_PID.Actual;
    }
}

void CANTask(void const *argument) {
    for (;;) {
        osDelay(1000);
    }
}

void ButtonTask(void const *argument) {
    button_init(&KEY1, read_KEY1_GPIO, 0);
    button_init(&KEY2, read_KEY2_GPIO, 0);
    button_init(&KEY3, read_KEY3_GPIO, 0);
    button_init(&KEY4, read_KEY4_GPIO, 0);
    button_init(&KEY5, read_KEY5_GPIO, 0);
    button_init(&KEY6, read_KEY6_GPIO, 0);

    button_attach(&KEY1, PRESS_DOWN, KEY1_PRESS_DOWN_Handler);
    button_attach(&KEY2, PRESS_DOWN, KEY2_PRESS_DOWN_Handler);
    button_attach(&KEY3, PRESS_DOWN, KEY3_PRESS_DOWN_Handler);
    button_attach(&KEY4, PRESS_DOWN, KEY4_PRESS_DOWN_Handler);
    button_attach(&KEY5, PRESS_DOWN, KEY5_PRESS_DOWN_Handler);
    button_attach(&KEY6, PRESS_DOWN, KEY6_PRESS_DOWN_Handler);

    button_start(&KEY1);
    button_start(&KEY2);
    button_start(&KEY3);
    button_start(&KEY4);
    button_start(&KEY5);
    button_start(&KEY6);

    for (;;) {
        button_ticks();
        osDelay(5);
    }
}
/* USER CODE END Application */

