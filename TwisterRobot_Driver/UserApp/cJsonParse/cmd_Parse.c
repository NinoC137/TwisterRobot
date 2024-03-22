//
// Created by Yoshi on 2024/4/10.
//

#include "cmd_Parse.h"

extern float targetAngle_left;
extern float targetAngle_right;

extern float yaw_pitch_roll[3];

void cmd_startParse(char* JsonString){
    cJSON *root = cJSON_Parse(JsonString);
    if(root == NULL){
        JSON_response("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }
    cJSON *cmd = cJSON_GetObjectItem(root, "cmd");
    switch (cmd->valueint) {
        case 1:
            cmd_setAngularDeviation(root);
            break;
        case 2:
            cmd_setMotorAngle(root);
            break;
        case 3:
            cmd_setServoAngle(root);
            break;
        case 4:
            cmd_getMotorSpeed(root);
            break;
        case 5:
            cmd_getMotorAngle(root);
            break;
        case 6:
            cmd_getMotorCurrent(root);
            break;
        case 7:
            cmd_getSystemMode(root);
            break;
        case 8:
            cmd_setHeartBeat(root);
            break;
        case 9:
            cmd_setFOCMode(root);
            break;
        case 10:
            cmd_setIMUValue(root);
            break;
        default:
            break;
    }
    cJSON_Delete(root);
}

void res_sendHeartBeat(){
    cJSON *response_root = cJSON_CreateObject();
    cJSON_AddItemToObject(response_root, "name", cJSON_CreateString(sysLog.name));
    cJSON_AddItemToObject(response_root, "lastUpgrade", cJSON_CreateString(sysLog.lastUpgradeTime));
    cJSON_AddItemToObject(response_root, "sysRunTime_ms", cJSON_CreateNumber(sysLog.sysRunTime));
    cJSON_AddItemToObject(response_root, "beatTime_ms", cJSON_CreateNumber(sysLog.beatTime_ms));

    char* responseText = cJSON_Print(response_root);

    JSON_response("%s", responseText);

    cJSON_Delete(response_root);
    free(responseText);
}

float angularDeviation;
void cmd_setAngularDeviation(cJSON* root){
    cJSON *cmd_Deviation = cJSON_GetObjectItem(root, "Deviation");
    if(cmd_Deviation == NULL){
        JSON_response("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }

    angularDeviation = (float)cmd_Deviation->valuedouble;

    cJSON *response_root = cJSON_CreateObject();
    cJSON_AddItemToObject(response_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(response_root, "cmd", cJSON_CreateNumber(1));

    char* responseText = cJSON_Print(response_root);

    JSON_response("%s", responseText);

    cJSON_Delete(response_root);
    free(responseText);
}

void cmd_setMotorAngle(cJSON *root){
    cJSON *cmd_AngleLeft = cJSON_GetObjectItem(root, "MotorAngle_Left");
    cJSON *cmd_AngleRight = cJSON_GetObjectItem(root, "MotorAngle_Right");
    if(cmd_AngleLeft == NULL || cmd_AngleRight == NULL){
        JSON_response("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }

    //TODO:在这里添加消息队列, 使得FOC线程及时获取最新的目标角度

    cJSON *response_root = cJSON_CreateObject();
    cJSON_AddItemToObject(response_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(response_root, "cmd", cJSON_CreateNumber(3));
    cJSON_AddItemToObject(response_root, "NewMotorAngle_Left", cJSON_CreateNumber(FOCMotor_Left.angle_pi));
    cJSON_AddItemToObject(response_root, "NewMotorAngle_Right", cJSON_CreateNumber(FOCMotor_Right.angle_pi));

    char* responseText = cJSON_Print(response_root);

    JSON_response("%s", responseText);

    cJSON_Delete(response_root);
    free(responseText);
}

void cmd_setServoAngle(cJSON* root){
    cJSON *cmd_AngleLeft = cJSON_GetObjectItem(root, "ServoAngle_Left");
    cJSON *cmd_AngleRight = cJSON_GetObjectItem(root, "ServoAngle_Right");
    if(cmd_AngleLeft == NULL || cmd_AngleRight == NULL){
        JSON_response("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }

    targetAngle_left = 60.0f - (float)cmd_AngleLeft->valueint;
    targetAngle_right = (float)cmd_AngleRight->valueint;

    cJSON *response_root = cJSON_CreateObject();
    cJSON_AddItemToObject(response_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(response_root, "cmd", cJSON_CreateNumber(3));
    cJSON_AddItemToObject(response_root, "currentAngle_Left", cJSON_CreateNumber(60.0f - targetAngle_left));
    cJSON_AddItemToObject(response_root, "currentAngle_Right", cJSON_CreateNumber(targetAngle_right));

    char* responseText = cJSON_Print(response_root);

    JSON_response("%s", responseText);

    cJSON_Delete(response_root);
    free(responseText);
}

void cmd_getMotorSpeed(cJSON* root){
    cJSON *response_root = cJSON_CreateObject();
    cJSON_AddItemToObject(response_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(response_root, "cmd", cJSON_CreateNumber(4));
    cJSON_AddItemToObject(response_root, "speedLeft", cJSON_CreateNumber(FOCMotor_Left.speed));
    cJSON_AddItemToObject(response_root, "speedRight", cJSON_CreateNumber(FOCMotor_Right.speed));

    char* responseText = cJSON_Print(response_root);

    JSON_response("%s", responseText);

    cJSON_Delete(response_root);
    free(responseText);
}

void cmd_getMotorAngle(cJSON* root){
    cJSON *response_root = cJSON_CreateObject();
    cJSON_AddItemToObject(response_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(response_root, "cmd", cJSON_CreateNumber(5));
    cJSON_AddItemToObject(response_root, "angleLeft", cJSON_CreateNumber(FOCMotor_Left.angle_pi));
    cJSON_AddItemToObject(response_root, "angleRight", cJSON_CreateNumber(FOCMotor_Right.angle_pi));

    char* responseText = cJSON_Print(response_root);

    JSON_response("%s", responseText);

    cJSON_Delete(response_root);
    free(responseText);
}

void cmd_getMotorCurrent(cJSON* root){
    cJSON *response_root = cJSON_CreateObject();
    cJSON_AddItemToObject(response_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(response_root, "cmd", cJSON_CreateNumber(6));
    cJSON_AddItemToObject(response_root, "currentLeft", cJSON_CreateNumber(FOCMotor_Left.Iq));
    cJSON_AddItemToObject(response_root, "currentRight", cJSON_CreateNumber(FOCMotor_Right.Id));

    char* responseText = cJSON_Print(response_root);

    JSON_response("%s", responseText);

    cJSON_Delete(response_root);
    free(responseText);
}

void cmd_getSystemMode(cJSON* root){
    cJSON *response_root = cJSON_CreateObject();
    cJSON_AddItemToObject(response_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(response_root, "cmd", cJSON_CreateNumber(7));

    char* responseText = cJSON_Print(response_root);

    JSON_response("%s", responseText);

    cJSON_Delete(response_root);
    free(responseText);
}

void cmd_setHeartBeat(cJSON* root){
    cJSON *cmd_beatTime_ms = cJSON_GetObjectItem(root, "beatTime_ms");
    if(cmd_beatTime_ms == NULL){
        JSON_response("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }
    sysLog.beatTime_ms = cmd_beatTime_ms->valueint;

    cJSON *response_root = cJSON_CreateObject();
    cJSON_AddItemToObject(response_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(response_root, "cmd", cJSON_CreateNumber(8));

    char* responseText = cJSON_Print(response_root);

    JSON_response("%s", responseText);

    res_sendHeartBeat();

    cJSON_Delete(response_root);
    free(responseText);
}

void cmd_setFOCMode(cJSON *root){
    cJSON *cmd_currentLimited = cJSON_GetObjectItem(root, "currentLimited");

    if(cmd_currentLimited == NULL){
        JSON_response("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }

    if(cmd_currentLimited->valuedouble > 2.0f){
        JSON_response("out of max current limited!\n");
        return;
    }

    FOCMotor_Left.currentLimited = (float)cmd_currentLimited->valuedouble;
    FOCMotor_Right.currentLimited = (float)cmd_currentLimited->valuedouble;

    cJSON *response_root = cJSON_CreateObject();
    cJSON_AddItemToObject(response_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(response_root, "cmd", cJSON_CreateNumber(9));
    cJSON_AddItemToObject(response_root, "NewCurrent", cJSON_CreateNumber(FOCMotor_Left.currentLimited));

    char* responseText = cJSON_Print(response_root);

    JSON_response("%s", responseText);

    cJSON_Delete(response_root);
    free(responseText);
}

void cmd_setIMUValue(cJSON *root){
    cJSON *cmd_yaw = cJSON_GetObjectItem(root, "yaw");
    cJSON *cmd_pitch = cJSON_GetObjectItem(root, "pitch");
    cJSON *cmd_roll = cJSON_GetObjectItem(root, "roll");

    if(cmd_yaw == NULL || cmd_pitch == NULL || cmd_roll == NULL){
        JSON_response("Error before: [%s]\n", cJSON_GetErrorPtr());
        return;
    }

    yaw_pitch_roll[0] = (float)cmd_yaw->valuedouble;
    yaw_pitch_roll[1] = (float)cmd_pitch->valuedouble;
    yaw_pitch_roll[2] = (float)cmd_roll->valuedouble;

//    cJSON *response_root = cJSON_CreateObject();
//    cJSON_AddItemToObject(response_root, "res", cJSON_CreateNumber(0));
//    cJSON_AddItemToObject(response_root, "cmd", cJSON_CreateNumber(10));
//
//    char* responseText = cJSON_Print(response_root);
//
//    JSON_response("%s", responseText);
//
//    cJSON_Delete(response_root);
//    free(responseText);
}