#ifndef TDA_H
#define TDA_H

#include "TDA7439.h"
#include "header_file.h"

void tda_init()
{
  cur_mode = SET_VOLUME;
  tda.begin();

  cur_volume = read_eeprom_8(EEPROM_INDEX_FOR_VOLUME);
  if (cur_volume > 48 || cur_volume == 0)
  {
    cur_volume = 20;
  }

  readCurInput();
  setInputData(cur_input);
  printCurScreen();
}

void readCurInput()
{
  uint8_t inp = read_eeprom_8(EEPROM_INDEX_FOR_INPUT);
  if (inp > 3)
  {
    inp = 1;
  }

  cur_input = (TDA7439_input)inp;
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
  if (_balance > 21)
  {
    _balance = 21;
  }
  else if (_balance < -21)
  {
    _balance = -21;
  }
  uint8_t right = (_balance < 0) ? 21 + _balance : 21;
  uint8_t left = (_balance > 0) ? 21 - _balance : 21;
  tda.spkAtt(right, left);
}

#endif // TDA_H