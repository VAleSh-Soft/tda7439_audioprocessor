#ifndef DISPLAY_LCD_I2C_H
#define DISPLAY_LCD_I2C_H

#include <LiquidCrystal_PCF8574.h> // https://github.com/mathertel/LiquidCrystal_PCF8574
#include "header_file.h"

// ===================================================

LiquidCrystal_PCF8574 display(0x27);

// ===================================================

void display_init()
{
  display.begin(16, 2);
}

void printNumData(int8_t number)
{
  display.setCursor(13, 0);
  if (number >= 0)
  {
    display.print(" ");
  }

  if (number < 10 && number > -10)
  {
    display.print(" ");
  }
  display.print(number);
}

void printInData(bool bt)
{
  display.setCursor(0, 0);
  if (bt && cur_input == INPUT_4)
  {
    display.print("Bt");
  }
  else
  {
    display.print((uint8_t)cur_input);
    display.print(" ");
  }
}

void printProgressBar(int8_t _data)
{
  display.setCursor(0, 1);
  uint8_t x;
  bool flat = true;

  switch (cur_mode)
  {
  case SET_VOLUME:
    x = (_data % 3 < 1) ? _data / 3 : _data / 3 + 1;
    break;
  case SET_INPUT_GAIN:
    x = _data;
    break;
  case SET_BASS:
  case SET_MIDDLE:
  case SET_TREBBLE:
    x = _data + 7;
    flat = false;
    break;
  case SET_BALANCE:
    x = _data / 3 + 7;
    flat = false;
Serial.println(x);
    break;
  default:
    break;
  }

  if (flat)
  {
    for (uint8_t i = 0; i < 16; i++)
    {
      (i < x) ? display.write(255) : display.print(" ");
    }
  }
  else
  {
    for (uint8_t i = 0; i < 15; i++)
    {
      (i != x) ? display.write(255) : display.print("X");
    }
  }
}

void printCurScreen()
{
  display.clear();
  printInData(cur_input);

  int8_t _data;

  if (cur_mode != SET_INPUT)
  {
    display.setCursor(3, 0);
    switch (cur_mode)
    {
    case SET_VOLUME:
      _data = cur_volume;
      display.print(F("  Volume"));
      break;
    case SET_INPUT_GAIN:
      _data = cur_data.input_gain;
      display.print(F("InputGane"));
      break;
    case SET_BASS:
      _data = cur_data.bass;
      display.print(F("  Bass"));
      break;
    case SET_MIDDLE:
      _data = cur_data.middle;
      display.print(F("  Middle"));
      break;
    case SET_TREBBLE:
      _data = cur_data.trebble;
      display.print(F("  Trebble"));
      break;
    case SET_BALANCE:
      _data = cur_data.balance;
      display.print(F(" Balance"));
      break;
    default:
      break;
    }
    printNumData(_data);
    printProgressBar(_data);
  }
  else
  {
  }
}

void setBacklight(bool flag)
{
  (flag) ? display.setBacklight(255) : display.setBacklight(0);
}

#endif // DISPLAY_LCD_I2C_H