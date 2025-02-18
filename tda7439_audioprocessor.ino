#include "header_file.h"
#include "_eeprom.h"
#include "display_LCD_I2C.h"
#include "tda.h"

// ===================================================

void checkRotary()
{
  unsigned char enc_res = enc.process();
  if (enc_res)
  {
    if (btn.isButtonClosed())
    {
      // переключение текущего входа
      cur_mode = SET_INPUT;
      new_input = true;
      switch (enc_res)
      {
      case DIR_CW:
        next_input++;
        break;
      case DIR_CCW:
        next_input--;
        break;
      }
    }
    else
    {
      changeCurData(enc_res == DIR_CW);
    }
  }

  switch (btn.getButtonState())
  {
  case BTN_ONECLICK:
    // переключение текущего режима
    cur_mode++;
    printCurScreen();
    break;
  case BTN_UP:
    // переключение текущего входа, если нужно
    if (new_input)
    {
      new_input = false;
      if (next_input != cur_input)
      {
        // для четвертого канала включим питание Bt-модуля
        uint8_t bt_pwr = cur_input == INPUT_4;
        if (!BT_CONTROL_LEVEL)
        {
          bt_pwr = !bt_pwr;
        }
        digitalWrite(BT_POWER_PIN, bt_pwr);

        setInputData(next_input);
        cur_mode = SET_VOLUME;
        printCurScreen();
      }
    }
    break;
  case BTN_DBLCLICK:
    // вход в режим установки предусиления
    cur_mode = SET_INPUT_GAIN;
    printCurScreen();
    break;
  case BTN_LONGCLICK:
    mute = !mute;
    (mute) ? tda.mute() : tda.setVolume(cur_volume);
    break;
  }
}

static void _change_data(int8_t &_data, int8_t _min, int8_t _max, bool _up)
{
  _data = (_up) ? _data + 1 : _data - 1;
  if (_data > _max)
  {
    _data = _max;
  }
  else if (_data < _min)
  {
    _data = _min;
  }
}

void changeCurData(bool _up)
{
  int8_t x;
  switch (cur_mode)
  {
  case SET_VOLUME:
    x = cur_volume;
    _change_data(x, 0, 47, _up);
    cur_volume = x;
    mute = false;
    tda.setVolume(cur_volume);
    printNumData(cur_volume);
    printProgressBar(cur_volume);
    break;
  case SET_BASS:
    _change_data(cur_data.bass, -7, 7, _up);
    tda.setSnd(cur_data.bass, BASS);
    printNumData(cur_data.bass);
    printProgressBar(cur_data.bass);
    break;
  case SET_MIDDLE:
    _change_data(cur_data.middle, -7, 7, _up);
    tda.setSnd(cur_data.bass, MIDDLE);
    printNumData(cur_data.middle);
    printProgressBar(cur_data.middle);
    break;
  case SET_TREBBLE:
    _change_data(cur_data.trebble, -7, 7, _up);
    tda.setSnd(cur_data.bass, TREBBLE);
    printNumData(cur_data.trebble);
    printProgressBar(cur_data.trebble);
    break;
  case SET_INPUT_GAIN:
    x = cur_data.input_gain;
    _change_data(x, 0, 15, _up);
    cur_data.input_gain = x;
    tda.inputGain(cur_data.input_gain);
    printNumData(cur_data.input_gain);
    printProgressBar(cur_data.input_gain);
    break;
  case SET_BALANCE:
    _change_data(cur_data.balance, -21, 21, _up);
    setBalance(cur_data.balance);
    printNumData(cur_data.balance);
    printProgressBar(cur_data.balance);
    break;
  default:
    break;
  }
}

// ==== tasks ========================================

void returnToDefMode()
{
  cur_mode = SET_VOLUME;
  printCurScreen();
  tasks.stopTask(return_to_default_mode);
  setBacklight(false);
}

void saveSettingsInEeprom()
{
  write_eeprom_8(EEPROM_INDEX_FOR_VOLUME, cur_volume);
  write_eeprom_8(EEPROM_INDEX_FOR_INPUT, cur_input);
  writeInputData(cur_data, cur_input);
  tasks.stopTask(save_settings_in_eeprom);
}

// ===================================================

void setup()
{
  // Serial.begin(115200);

  digitalWrite(BT_POWER_PIN, !BT_CONTROL_LEVEL);
  pinMode(BT_POWER_PIN, OUTPUT);

  return_to_default_mode = tasks.addTask(TIMEOUT_OF_RETURN_TO_DEFMODE * 1000, returnToDefMode, false);
  save_settings_in_eeprom = tasks.addTask(TIMEOUT_OF_AUTOSAVE_DATA * 1000, saveSettingsInEeprom, false);

  display_init();
  enc.begin(true);
  tda_init();
}

void loop()
{
  tasks.tick();
  checkRotary();
}
