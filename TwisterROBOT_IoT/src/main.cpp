#include <Arduino.h>
#include "WiFi_BLE.h"
#include "my_MPU6050.h"
#include "GUI_Driver.h"

static void SystemData_GUI();

void setup()
{
  Serial.begin(115200);

  GUI_setup();

  WiFi_BLE_setUp();

  if (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    WiFi.begin(WiFi_Data.WiFi_store[0].SSID, WiFi_Data.WiFi_store[0].PassWord);
  }

  MPU6050_setup();

  SystemData_GUI();

  // std::stringstream urlStream;
  // urlStream << "http://" << WiFi_Data.serverip << ":" << WiFi_Data.serverport;
  // Serial.printf("Try to connect %s\r\n",urlStream.str().c_str());

  // http.begin(urlStream.str().c_str()); //连接服务器对应域名
}

void loop()
{
  // BLEHandler();
  // WiFiHandler();
  // ProjectDataUpdate();

  MPU6050_getData();
  MPU6050_GUILog();

  delay(500);
}

void SystemData_GUI(){
  GUI_sysPrint(0, 16, "WiFi Status: %s", WiFiStatus_str[ProjectData.wifistatus]);
  GUI_sysPrint(0, 32, "ipv4 address: %s", WiFi.localIP().toString().c_str());

  GUI_sysPrint(0, 208, "last update date: %s", __DATE__);
}