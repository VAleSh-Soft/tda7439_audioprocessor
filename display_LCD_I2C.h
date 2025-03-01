#ifndef DISPLAY_LCD_I2C_H
#define DISPLAY_LCD_I2C_H

#include <LiquidCrystal_PCF8574.h> // https://github.com/mathertel/LiquidCrystal_PCF8574
#if defined(ARDUINO_ARCH_ESP32)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif
#include "header_file.h"

// ===================================================

LiquidCrystal_PCF8574 display(0x27);

// ==== отрисовка больших цифр =======================

// массивы для отрисовки сегментов цифр
static uint8_t const LT[8] PROGMEM =
    {
        0b00111,
        0b01111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111};

static uint8_t const UB[8] PROGMEM =
    {
        0b11111,
        0b11111,
        0b11111,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000};

static uint8_t const RT[8] PROGMEM =
    {
        0b11100,
        0b11110,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111};

static uint8_t const LL[8] PROGMEM =
#if USE_BT_MODULE_ON_INPUT == 4 && NUMBER_OF_INPUT_IS_USED >= 4
    {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b11000,
        0b11100,
        0b11110};
#else
    {
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B01111,
        B00111};
#endif

static uint8_t const LB[8] PROGMEM =
    {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b11111,
        0b11111,
        0b11111};

static uint8_t const LR[8] PROGMEM =
    {
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11111,
        0b11110,
        0b11100};

static uint8_t const MB[8] PROGMEM =
#if USE_BT_MODULE_ON_INPUT == 3 && NUMBER_OF_INPUT_IS_USED >= 3
    {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b11000,
        0b11100,
        0b11110};
#else
    {
      0b11111,
      0b11111,
      0b11111,
      0b00000,
      0b00000,
      0b00000,
      0b11111,
      0b11111};
#endif

static uint8_t const BM[8] PROGMEM =
#if USE_BT_MODULE_ON_INPUT == 2 && NUMBER_OF_INPUT_IS_USED >= 2
    {
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b00000,
        0b11000,
        0b11100,
        0b11110};
#else
    {
      0b11111,
      0b11111,
      0b00000,
      0b00000,
      0b00000,
      0b11111,
      0b11111,
      0b11111};
#endif

// массивы сегментов для отрисовки цифр
uint8_t const PROGMEM nums[]{
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // space
    0x01, 0x02, 0x20, 0x04, 0xFF, 0x04, // 1
    0x01, 0x01, 0x02, 0x00, 0x07, 0x07, // 2
    0x01, 0x06, 0x02, 0x04, 0x04, 0x05, // 3
    0x03, 0x04, 0xFF, 0x20, 0x20, 0xFF  // 4
#if BT_MODULE_IS_USED
#if USE_BT_MODULE_ON_INPUT == 4
    ,
    0xFF, 0x04, 0x03, 0xFF, 0x04, 0x05 // b
#elif USE_BT_MODULE_ON_INPUT == 3
    ,
    0xFF, 0x04, 0x06, 0xFF, 0x04, 0x05 // b
#elif USE_BT_MODULE_ON_INPUT == 2
    ,
    0xFF, 0x04, 0x07, 0xFF, 0x04, 0x05 // b
#else
    ,
    0xFF, 0x04, 0x04, 0xFF, 0x04, 0x05 // b
#endif
    ,
    0x01, 0xFF, 0x01, 0x20, 0xFF, 0x20 // T
#endif
};

// ===================================================
void display_init()
{
  display.begin(16, 2);

  display.createChar(0, LT);
  display.createChar(1, UB);
  display.createChar(2, RT);
  display.createChar(3, LL);
  display.createChar(4, LB);
  display.createChar(5, LR);
  display.createChar(6, MB);
  display.createChar(7, BM);

  setBacklight(true);
}

static void _print_bc(uint8_t offset, uint8_t x)
{
  display.setCursor(offset, 0);
  for (uint8_t i = 0; i < 3; i++)
  {
    display.write(pgm_read_byte(&nums[x * 6 + i]));
  }
  display.setCursor(offset, 1);
  for (uint8_t i = 3; i < 6; i++)
  {
    display.write(pgm_read_byte(&nums[x * 6 + i]));
  }
}

void printBigChar(uint8_t x)
{
  const uint8_t offset = 10;
  if (x > 0 && x <= 4)
  {
#if BT_MODULE_IS_USED
    if (x == USE_BT_MODULE_ON_INPUT)
    {
      x = 5;
    }
#endif
    _print_bc(offset, x);
#if BT_MODULE_IS_USED
    if (x == 5)
    {
      _print_bc(offset + 3, 6);
    }
    else
    {
      _print_bc(offset + 3, 0);
    }
#endif
  }
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

void printInData()
{
  display.setCursor(0, 0);
#if BT_MODULE_IS_USED
  if (cur_input == (TDA7439_input)(4 - USE_BT_MODULE_ON_INPUT))
  {
    display.print("Bt");
  }
  else
#endif
  {
    display.print(4 - (uint8_t)cur_input);
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
  case SET_BALANCE:
    x = _data + 7;
    flat = false;
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
  printInData();

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
    display.setCursor(3, 0);
    display.print(F("Next"));
    display.setCursor(3, 1);
    display.print(F("input"));
    printBigChar(4 - (uint8_t)next_input);
  }
}

void setBacklight(bool flag)
{
  (flag) ? display.setBacklight(255) : display.setBacklight(0);
}

#endif // DISPLAY_LCD_I2C_H