#ifndef _EEPROM_H
#define _EEPROM_H

#include <EEPROM.h>
#include "header_file.h"

// ===================================================

#if (__USE_ARDUINO_ESP__ || defined(ARDUINO_ARCH_RP2040)) && \
    (__USE_OTHER_SETTING__ || __USE_ON_OFF_DATA__)
#define __USE_EEPROM_IN_FLASH__ 1
#ifndef EEPROM_SIZE
#define EEPROM_SIZE 256
#endif
#else
#define __USE_EEPROM_IN_FLASH__ 0
#endif

// ===================================================

#if __USE_EEPROM_IN_FLASH__
void eeprom_update(uint16_t _index, uint8_t _data)
{
  if (EEPROM.read(_index) != _data)
  {
    EEPROM.write(_index, _data);
    EEPROM.commit();
  }
}

void eeprom_init(uint16_t _eeprom_size)
{
  EEPROM.end();
  if (_eeprom_size > 4096u || _eeprom_size == 0)
  {
    _eeprom_size = 4096;
  }
  EEPROM.begin(_eeprom_size);
}
#endif

uint8_t read_eeprom_8(uint16_t _index)
{
  uint8_t result = EEPROM.read(_index);
  return result;
}

void write_eeprom_8(uint16_t _index, uint8_t _data)
{
  if (!no_save_flag)
  {
#if __USE_EEPROM_IN_FLASH__
    eeprom_update(_index, _data);
#else
    EEPROM.update(_index, _data);
#endif
  }
}

static uint16_t _get_index(TDA7439_input _input)
{
  switch (_input)
  {
  case INPUT_1:
    return (EEPROM_INDEX_FOR_DATA_1);
  case INPUT_2:
    return (EEPROM_INDEX_FOR_DATA_2);
  case INPUT_3:
    return (EEPROM_INDEX_FOR_DATA_3);
  case INPUT_4:
    return (EEPROM_INDEX_FOR_DATA_4);
  }
}

static int8_t _check_data(int8_t _data, int8_t _min, int8_t _max)
{
  return ((_data < _min || _data > _max) ? 0 : _data);
}

void readInputData(TDA_DATA &_data, TDA7439_input _input)
{
  uint16_t index = _get_index(_input);

  _data.bass = _check_data(read_eeprom_8(index), -7, 7);
  _data.middle = _check_data(read_eeprom_8(index + 1), -7, 7);
  _data.trebble = _check_data(read_eeprom_8(index + 2), -7, 7);
  _data.balance = _check_data(read_eeprom_8(index + 3), -14, 14);
  _data.input_gain = (read_eeprom_8(index + 4) > 15) ? 5 : read_eeprom_8(index + 4);
}

void writeInputData(TDA_DATA &_data, TDA7439_input _input)
{
  uint16_t index = _get_index(_input);

  write_eeprom_8(index, _data.bass);
  write_eeprom_8(index + 1, _data.middle);
  write_eeprom_8(index + 2, _data.trebble);
  write_eeprom_8(index + 3, _data.balance);
  write_eeprom_8(index + 4, _data.input_gain);
}

#endif // _EEPROM_H