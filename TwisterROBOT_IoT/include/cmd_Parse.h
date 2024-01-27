#ifndef _CMD_PARSE_H
#define _CMD_PARSE_H

#include "WiFi_BLE.h"

void cmd1(); // 扫描3个WiFi

void cmd2(cJSON *root); // 设置WiFi

void cmd3(cJSON *root); // 读取设备保存的WiFi(一个)

void cmd4(cJSON *root); // 启/停设备

void cmd5(cJSON *root); // 设置时间

void cmd6(cJSON *root); // 设置时区

void cmd7(cJSON *root); // 查询时区

void cmd8(cJSON *root); // 查询设备信息

void cmd9(cJSON *root); // 设置设备工作时长

void cmd10(cJSON *root); // 查询设备剩余时长

void cmd12(cJSON *root); // 服务器设置

void cmd13(cJSON *root); // 读取服务器设置

void cmd14(cJSON *root); // 心跳包设置

void cmd15(cJSON *root); // 读取心跳包设置

void cmd16(); // 通过WiFi向服务器发送事件日志

void cmd17(); // 通过蓝牙向宿主机发送事件日志

void cmd18(cJSON *root); // 设定CRC校验

void cmd19(cJSON *root); // 初始化设备信息

#endif // !_CMD_PARSE_H