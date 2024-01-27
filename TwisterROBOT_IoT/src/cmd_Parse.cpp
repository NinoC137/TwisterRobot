#include "cmd_Parse.h"

void cmd1()
{
    WiFi.disconnect();
    WiFi.scanNetworks(3); // 扫描3个WiFi

    cJSON *tx_root = cJSON_CreateObject();
    cJSON *tx_wifi_array = cJSON_CreateArray();
    // 开始生成json串
    if (WiFi.encryptionType(1) != 0)
    {
        cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
    }
    else
    {
        cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(1));
    }
    cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(1));
    cJSON_AddItemToObject(tx_root, "wifi", tx_wifi_array);
    for (int i = 0; i < 3; i++)
    {
        cJSON *tx_wifi_objcet = cJSON_CreateObject();
        cJSON_AddItemToArray(tx_wifi_array, tx_wifi_objcet);
        cJSON_AddItemToObject(tx_wifi_objcet, "ssid", cJSON_CreateString(WiFi.SSID(i).c_str()));
        cJSON_AddItemToObject(tx_wifi_objcet, "enc", cJSON_CreateNumber(WiFi.encryptionType(i)));
        cJSON_AddItemToObject(tx_wifi_objcet, "RSSI", cJSON_CreateNumber(WiFi.RSSI(i)));
    }
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();

#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
    WiFi.begin(WiFi_Data.WiFi_store[0].SSID, WiFi_Data.WiFi_store[0].PassWord);
}

void cmd2(cJSON *root)
{
    cJSON *cmd_idx = cJSON_GetObjectItem(root, "idx");
    cJSON *cmd_ssid = cJSON_GetObjectItem(root, "ssid");
    cJSON *cmd_pws = cJSON_GetObjectItem(root, "pws");
    if (cmd_idx == NULL || cmd_ssid == NULL || cmd_pws == NULL)
    {
        TX_Characteristics.setValue("json string error!!");
        TX_Characteristics.notify();
        return;
    }
    // 将数据存储至全局变量
    WiFi_Data.WiFi_store[cmd_idx->valueint].SSID = cmd_ssid->valuestring;
    WiFi_Data.WiFi_store[cmd_idx->valueint].PassWord = cmd_pws->valuestring;

    cJSON *tx_root = cJSON_CreateObject();
    cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(2));
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd3(cJSON *root) // 读取设备保存的WiFi(一个)
{
    cJSON *tx_root = cJSON_CreateObject();
    cJSON *tx_wifi_array = cJSON_CreateArray();
    // 开始生成json串

    cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(3));
    cJSON_AddItemToObject(tx_root, "wifi", tx_wifi_array);

    cJSON *tx_wifi_objcet = cJSON_CreateObject();
    cJSON_AddItemToArray(tx_wifi_array, tx_wifi_objcet);
    cJSON_AddItemToObject(tx_wifi_objcet, "idx", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(tx_wifi_objcet, "ssid", cJSON_CreateString(WiFi_Data.WiFi_store[0].SSID));
    cJSON_AddItemToObject(tx_wifi_objcet, "psw", cJSON_CreateString(WiFi_Data.WiFi_store[0].PassWord));

    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd4(cJSON *root) // 启/停设备
{
    cJSON *tx_root = cJSON_CreateObject();
    cJSON *cmd_switch = cJSON_GetObjectItem(root, "switch");
    cJSON *cmd_worktime = cJSON_GetObjectItem(root, "worktime");
    /**************************************/
    /*************新增CRC******************/
    cJSON *cmd_newCRC = cJSON_GetObjectItem(root, "new_crc");
    /**************************************/
    if (cmd_switch == NULL || cmd_worktime == NULL)
    {
        TX_Characteristics.setValue("json string error!!");
        TX_Characteristics.notify();
        return;
    }

    ProjectData.switchStatus = cmd_switch->valueint;
    ProjectData.worktime = cmd_worktime->valueint;
    ProjectData.runTime = 0;
    /**************************************/
    /*************新增CRC******************/
    if (std::string(cmd_newCRC->valuestring) == ProjectData.old_CRC)
    {
        CRC_CHECKED = 1; // CRC验证通过
        cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
        cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(4));
    }
    else
    {
        CRC_CHECKED = 0;
        cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(-1));
        cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(4));
    }
    /**************************************/

    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd5(cJSON *root) // 设置时间
{
    cJSON *cmd_year = cJSON_GetObjectItem(root, "year");
    cJSON *cmd_month = cJSON_GetObjectItem(root, "month");
    cJSON *cmd_day = cJSON_GetObjectItem(root, "day");
    cJSON *cmd_hour = cJSON_GetObjectItem(root, "hour");
    cJSON *cmd_minute = cJSON_GetObjectItem(root, "minute");
    cJSON *cmd_second = cJSON_GetObjectItem(root, "second");

    if (cmd_year == NULL || cmd_month == NULL || cmd_day == NULL || cmd_hour == NULL || cmd_minute == NULL || cmd_second == NULL)
    {
        TX_Characteristics.setValue("json string error!!");
        TX_Characteristics.notify();
        return;
    }

    std::stringstream stringstream;
    stringstream << cmd_year->valueint << "-"
                 << cmd_month->valueint << "-"
                 << cmd_day->valueint << " "
                 << cmd_hour->valueint << ":"
                 << cmd_minute->valueint << ":"
                 << cmd_second->valueint;

    ProjectData.time = stringstream.str();

    cJSON *tx_root = cJSON_CreateObject();
    cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(5));
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd6(cJSON *root) // 设置时区
{
    cJSON *cmd_timezone = cJSON_GetObjectItem(root, "timezone");
    if (cmd_timezone == NULL)
    {
        TX_Characteristics.setValue("json string error!!");
        TX_Characteristics.notify();
        return;
    }
    my_timezone = cmd_timezone->valueint;
    gmtOffset_sec = my_timezone * 3600;
    updateLocalTime();

    cJSON *tx_root = cJSON_CreateObject();
    cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(6));
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd7(cJSON *root) // 查询时区
{
    cJSON *tx_root = cJSON_CreateObject();
    cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(7));
    cJSON_AddItemToObject(tx_root, "timezone", cJSON_CreateNumber(my_timezone));
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd8(cJSON *root) // 查询设备信息
{
    cJSON *tx_root = cJSON_CreateObject();
    cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(8));
    cJSON_AddItemToObject(tx_root, "staMAC", cJSON_CreateString(WiFi_Data.WiFi_store[0].MacAddress.c_str()));
    cJSON_AddItemToObject(tx_root, "staIP", cJSON_CreateString(WiFi_Data.WiFi_store[0].ipv4.toString().c_str()));
    cJSON_AddItemToObject(tx_root, "devID", cJSON_CreateString(WiFi_Data.WiFi_store[0].devID.c_str()));
    cJSON_AddItemToObject(tx_root, "devType", cJSON_CreateString(WiFi_Data.WiFi_store[0].devType));
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd9(cJSON *root) // 设置设备工作时长
{
    cJSON *cmd_worktime = cJSON_GetObjectItem(root, "worktime");
    cJSON *cmd_speed = cJSON_GetObjectItem(root, "speed");
    cJSON *cmd_temp = cJSON_GetObjectItem(root, "temp");
    if (cmd_worktime == NULL || cmd_speed == NULL || cmd_temp == NULL)
    {
        TX_Characteristics.setValue("json string error!!");
        TX_Characteristics.notify();
        return;
    }
    // 存入设备数据包
    ProjectData.worktime = cmd_worktime->valueint;
    ProjectData.speed = cmd_speed->valueint;
    ProjectData.temp = cmd_temp->valueint;
    ProjectData.runTime = 0;

    cJSON *tx_root = cJSON_CreateObject();
    cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(9));
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd10(cJSON *root) // 查询设备剩余时长
{
    cJSON *tx_root = cJSON_CreateObject();
    cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(10));
    cJSON_AddItemToObject(tx_root, "worktime", cJSON_CreateNumber(ProjectData.worktime));
    cJSON_AddItemToObject(tx_root, "runtime", cJSON_CreateNumber(ProjectData.runTime));
    cJSON_AddItemToObject(tx_root, "speed", cJSON_CreateNumber(ProjectData.speed));
    cJSON_AddItemToObject(tx_root, "temp", cJSON_CreateNumber(ProjectData.temp));
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd12(cJSON *root) // 服务器设置
{
    cJSON *cmd_serverip = cJSON_GetObjectItem(root, "serverip");
    cJSON *cmd_serverport = cJSON_GetObjectItem(root, "serverport");

    if (cmd_serverip == NULL || cmd_serverport == NULL)
    {
        TX_Characteristics.setValue("json string error!!");
        TX_Characteristics.notify();
        return;
    }

    WiFi_Data.serverip = cmd_serverip->valuestring;
    WiFi_Data.serverport = cmd_serverport->valueint;

    std::stringstream urlStream;
    urlStream << "http://" << WiFi_Data.serverip << ":" << WiFi_Data.serverport;

    http.begin(urlStream.str().c_str()); // 连接服务器对应域名

    cJSON *tx_root = cJSON_CreateObject();
    cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(12));
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd13(cJSON *root) // 读取服务器设置
{
    cJSON *tx_root = cJSON_CreateObject();
    cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(13));
    cJSON_AddItemToObject(tx_root, "serverip", cJSON_CreateString(WiFi_Data.serverip.c_str()));
    cJSON_AddItemToObject(tx_root, "serverport", cJSON_CreateNumber(WiFi_Data.serverport));
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd14(cJSON *root) // 心跳包设置
{
    cJSON *cmd_keepalivetime = cJSON_GetObjectItem(root, "keepalivetime");
    cJSON *cmd_keepalivecnt = cJSON_GetObjectItem(root, "keepalivecnt");

    if (cmd_keepalivetime == NULL || cmd_keepalivecnt == NULL)
    {
        TX_Characteristics.setValue("json string error!!");
        TX_Characteristics.notify();
        return;
    }

    HeartBeat.keepAliveTime = cmd_keepalivetime->valueint;
    HeartBeat.keepLiveCnt = cmd_keepalivecnt->valueint;

    cJSON *tx_root = cJSON_CreateObject();
    cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(14));
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd15(cJSON *root) // 读取心跳包设置
{
    cJSON *tx_root = cJSON_CreateObject();
    cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(15));
    cJSON_AddItemToObject(tx_root, "keepalivetime", cJSON_CreateNumber(HeartBeat.keepAliveTime));
    cJSON_AddItemToObject(tx_root, "keepalivecnt", cJSON_CreateNumber(HeartBeat.keepLiveCnt));
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd16() // 通过WiFi向服务器发送事件日志
{
    cJSON *tx_root = cJSON_CreateObject();
    // cJSON_AddItemToObject(tx_root, "device_id", cJSON_CreateString(ProjectData.device_ID.c_str()));
    cJSON_AddItemToObject(tx_root, "device_id", cJSON_CreateString(WiFi_Data.WiFi_store[0].devID.c_str()));
    cJSON_AddItemToObject(tx_root, "blestatus", cJSON_CreateNumber(ProjectData.blestatus));
    cJSON_AddItemToObject(tx_root, "wifistatus", cJSON_CreateNumber(ProjectData.wifistatus));
    cJSON_AddItemToObject(tx_root, "switch", cJSON_CreateNumber(ProjectData.switchStatus));
    cJSON_AddItemToObject(tx_root, "speed", cJSON_CreateNumber(ProjectData.speed));
    cJSON_AddItemToObject(tx_root, "temp", cJSON_CreateNumber(ProjectData.temp));
    cJSON_AddItemToObject(tx_root, "time", cJSON_CreateString(ProjectData.time.c_str()));
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    int httpCode = http.connected();
    if (httpCode == true)
    {
        if (httpCode == HTTP_CODE_OK) // HTTP请求无异常
        {
            http.POST(json_string);
        }
    }
    else
    {
        TX_Characteristics.setValue("http disconnected!");
        TX_Characteristics.notify();
    }
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd17() // 通过蓝牙向宿主机发送事件日志
{
    cJSON *tx_root = cJSON_CreateObject();
    // cJSON_AddItemToObject(tx_root, "device_id", cJSON_CreateString(ProjectData.device_ID.c_str()));
    cJSON_AddItemToObject(tx_root, "device_id", cJSON_CreateString(WiFi_Data.WiFi_store[0].devID.c_str()));
    cJSON_AddItemToObject(tx_root, "blestatus", cJSON_CreateNumber(ProjectData.blestatus));
    cJSON_AddItemToObject(tx_root, "wifistatus", cJSON_CreateNumber(ProjectData.wifistatus));
    cJSON_AddItemToObject(tx_root, "switch", cJSON_CreateNumber(ProjectData.switchStatus));
    cJSON_AddItemToObject(tx_root, "speed", cJSON_CreateNumber(ProjectData.speed));
    cJSON_AddItemToObject(tx_root, "temp", cJSON_CreateNumber(ProjectData.temp));
    cJSON_AddItemToObject(tx_root, "time", cJSON_CreateString(ProjectData.time.c_str()));
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd18(cJSON *root)
{
    cJSON *cmd_oldCRC = cJSON_GetObjectItem(root, "old_crc");
    cJSON *cmd_newCRC = cJSON_GetObjectItem(root, "new_crc");

    cJSON *tx_root = cJSON_CreateObject();
    if (cmd_oldCRC == NULL || cmd_newCRC == NULL)
    {
        TX_Characteristics.setValue("json string error!!");
        TX_Characteristics.notify();
        return;
    }

    if (std::string(cmd_oldCRC->valuestring) == ProjectData.old_CRC) // 传入的old_CRC是正确的, 匹配原本的CRC值
    {
        ProjectData.old_CRC = std::string(cmd_newCRC->valuestring); // 替换新的CRC
        CRC_CHECKED = 1;                                            // CRC验证通过

        cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
        cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(18));
    }
    else
    {
        CRC_CHECKED = 0;
        cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(-1));
        cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(18));
    }

    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
    Serial.printf("old crc:%s", ProjectData.old_CRC.c_str());
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}

void cmd19(cJSON *root) // 初始化设备信息
{
    cJSON *cmd_devName = cJSON_GetObjectItem(root, "devName");
    if (cmd_devName == NULL)
    {
        TX_Characteristics.setValue("json string error!!");
        TX_Characteristics.notify();
        return;
    }
    ProjectData.device_ID = cmd_devName->valuestring;

    cJSON *tx_root = cJSON_CreateObject();
    cJSON_AddItemToObject(tx_root, "res", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(tx_root, "cmd", cJSON_CreateNumber(19));
    char *json_string = cJSON_Print(tx_root);
    // 生成完毕, 准备发送
    TX_Characteristics.setValue(json_string);
    TX_Characteristics.notify();
#if DEBUG
    Serial.printf("%s\r\n", json_string);
#endif // DEBUG
    cJSON_Delete(tx_root);
    free(json_string);
}