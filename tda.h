#ifndef TDA_H
#define TDA_H

#include "TDA7439.h"
#include "header_file.h"

void tda_init()
{
  cur_mode = SET_VOLUME;
  tda.begin();
  tda.mute();

  cur_volume = read_eeprom_8(EEPROM_INDEX_FOR_VOLUME);
  if (cur_volume > 48 || cur_volume == 0)
  {
    cur_volume = 20;
  }

  switchingInput(readCurInput(), true);
}

TDA7439_input readCurInput()
{
  uint8_t inp = read_eeprom_8(EEPROM_INDEX_FOR_INPUT);
  if (inp > 3)
  {
    inp = 1;
  }

  return (TDA7439_input)inp;
}

void setInputData(TDA7439_input _input)
{
  tda.mute();

  tda.setInput(_input);
  cur_input = _input;
  next_input = cur_input;
  write_eeprom_8(EEPROM_INDEX_FOR_INPUT, (uint8_t)cur_input);

  // считать данные канала из EEPROM
  readInputData(cur_data, cur_input);

  // передать данные в TDA7439
  tda.setSnd(cur_data.bass, BASS);
  tda.setSnd(cur_data.middle, MIDDLE);
  tda.setSnd(cur_data.trebble, TREBBLE);
  tda.inputGain(cur_data.input_gain);
  setBalance(cur_data.balance);

  tda.setVolume(cur_volume);
}

void setBalance(int8_t _balance)
{
  if (_balance > 7)
  {
    _balance = 7;
  }
  else if (_balance < -7)
  {
    _balance = -7;
  }
  uint8_t right = (_balance < 0) ? -4 * _balance : 0;
  uint8_t left = (_balance > 0) ? 4 * _balance : 0;
  tda.spkAtt(right, left);
}

void switchingInput(TDA7439_input _input, bool _init)
{
  new_input = false;
  if (_input != cur_input || _init)
  {
    if (!_init)
    {
      saveSettingsInEeprom();
    }

#if BT_MODULE_IS_USED
    // питание Bt-модуля включаем при первом переходе на вход, к
    // которому он подключен; при переходе на другой вход питание 
    // Bt-модуля не выключаем, чтобы иметь возможность переключать 
    // входы TDA7439 без потери сигнала
    if (_input == (TDA7439_input)(4 - USE_BT_MODULE_ON_INPUT))
    {
      digitalWrite(BT_POWER_PIN, BT_CONTROL_LEVEL);
    }
#endif
    setInputData(_input);
    cur_mode = SET_VOLUME;
  }
  printCurScreen();
}

#endif // TDA_H