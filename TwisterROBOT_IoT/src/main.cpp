#include <Arduino.h>
#include "WiFi_BLE.h"
#include "my_MPU6050.h"
#include "GUI_Driver.h"
#include "ServoControl.h"

#include "FreeRTOS.h"

SemaphoreHandle_t GUILog_Mutex;

void IoTTaskThread(void *argument);
TaskHandle_t IoTTaskHandle;

void SensorTaskThread(void *argument);
TaskHandle_t SensorTaskHandle;

void GUITaskThread(void *argument);
TaskHandle_t GUITaskHandle;

void SerialThread(void *argument);
TaskHandle_t SerialHandle;

static void SystemData_GUI();

void setup()
{
  Serial.begin(115200);

  Serial1.begin(115200);
  Serial1.setPins(15, 16);

  GUILog_Mutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(IoTTaskThread, "IoTTask", 4096, NULL, 2, &IoTTaskHandle, 1);

  xTaskCreatePinnedToCore(SensorTaskThread, "SensorTask", 4096, NULL, 2, &IoTTaskHandle, 0);

  xTaskCreatePinnedToCore(GUITaskThread, "GUITask", 4096, NULL, 1, &IoTTaskHandle, 0);

  xTaskCreatePinnedToCore(SerialThread, "Serial", 1024*8, NULL, 1, &SerialHandle, 0);
  // std::stringstream urlStream;
  // urlStream << "http://" << WiFi_Data.serverip << ":" << WiFi_Data.serverport;
  // Serial.printf("Try to connect %s\r\n",urlStream.str().c_str());

  // http.begin(urlStream.str().c_str()); //连接服务器对应域名
}

void SensorTaskThread(void *argument)
{
  MPU6050_setup();
  for (;;)
  {
    MPU6050_getData();
    MPU6050_SendJSONPack();
    vTaskDelay(200);
  }
}

void GUITaskThread(void *argument)
{
  GUI_setup(); 
  for (;;)
  {
    SystemData_GUI();
    MPU6050_GUILog();
    vTaskDelay(500);
  }
}

void IoTTaskThread(void *argument)
{
  WiFi_BLE_setUp();
  configTime(gmtOffset_sec, 0, "pool.ntp.org");

  for (;;)
  {
    BLEHandler();
    // WiFiHandler();
    ProjectDataUpdate();
    vTaskDelay(5);
  }
}

void SerialThread(void *argument)
{
  for (;;)
  {
    if (value.length() > 0)
    {
      Serial1.print(value.c_str());
    }

    if(Serial1.available())
    {
      String *received = new String(150);
      *received = Serial1.readString();
      size_t pos = std::string(received->c_str()).find('}');
      // GUI_logPrint(received_chars);
      TX_Characteristics.setValue(std::string(received->c_str()).substr(0,pos+1));
      TX_Characteristics.notify();
      delete(received);
    }
    vTaskDelay(50);
  }
}

void SystemData_GUI()
{
  GUI_sysPrint(0, 16, "                                                      ");
  GUI_sysPrint(0, 16, "WiFi Status: %s", WiFiStatus_str[ProjectData.wifistatus]);
  GUI_sysPrint(0, 32, "                                                      ");
  GUI_sysPrint(0, 32, "ipv4 address: %s", WiFi.localIP().toString().c_str());

  GUI_sysPrint(0, 192, "%04d-%02d-%02d %02d:%02d:%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
               timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  GUI_sysPrint(0, 208, "last update date: %s", __DATE__);
}

void loop()
{
  delay(500);
}