#ifndef _GUI_DRIVER__H
#define _GUI_DRIVER__H

#include "TFT_eSPI.h"

void GUI_setup();

void GUI_logPrint(std::string logStr);

void GUI_sysPrint(int32_t x, int32_t y, const char* str, ...);

extern TFT_eSPI tft;

#endif // !_GUI_DRIVER__H