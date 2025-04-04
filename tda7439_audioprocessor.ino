#include "header_file.h"
#include "_eeprom.h"
#include "display_LCD_I2C.h"
#include "tda.h"

// ===================================================

void setMute()
{
  if (!no_mute)
  {
    mute_flag = !mute_flag;
    if (mute_flag)
    {
      tda.mute();
    }
    else
    {
      tda.setVolume(cur_volume);
    }
  }
  printCurScreen();
}

void setNewMode(bool gain_mode)
{
  if (gain_mode)
  {
    cur_mode = (cur_mode == SET_INPUT_GAIN) ? SET_SPEAKER_ATT : SET_INPUT_GAIN;
  }
  else
  {
    cur_mode++;
  }
  printCurScreen();
}

void checkRotary()
{
  unsigned char enc_res = enc.process();
  if (enc_res)
  {
    // обработка поворота энкодера
#if NUMBER_OF_INPUT_IS_USED > 1
    if (enc_btn.isButtonClosed())
    {
      new_input = true;
      no_mute = true;
      enc_btn.resetButtonState();
      // переключение текущего входа
      cur_mode = SET_INPUT;
      switch (enc_res)
      {
      case DIR_CW:
        next_input++;
        break;
      case DIR_CCW:
        next_input--;
        break;
      }

      printCurScreen();
    }
    else
#endif
    {
      if (mute_flag) // если поднят флаг mute_flag, то первый щелчок энкодера просто его сбрасывает и только последующие щелчки регулируют текущий параметр
      {
        mute_flag = false;
        tda.setVolume(cur_volume);
        printCurScreen();
      }
      else
      {
        changeCurData(enc_res == DIR_CW);
      }
    }
  }

  // обработка событий кнопки энкодера
  switch (enc_btn.getButtonState())
  {
  case BTN_ONECLICK:
    // переключение текущего режима
    setNewMode();
    break;
  case BTN_UP:
    // переключение текущего входа, если нужно
    if (new_input)
    {
      switchingInput(next_input);
    }
    no_mute = false;
    break;
  case BTN_DBLCLICK:
    // вход в режим установки предусиления
    setNewMode(true);
    break;
  case BTN_LONGCLICK:
    // отключение звука
    setMute();
    break;
  }

#if USE_MUTE_BUTTON
  if (mute_btn.getButtonState() == BTN_DOWN)
  {
    // отключение звука
    setMute();
  }
#endif

#if USE_MODE_BUTTON
  switch (mode_btn.getButtonState())
  {
  case BTN_ONECLICK:
    // переключение текущего режима
    setNewMode();
    break;
  case BTN_DBLCLICK:
    // вход в режим установки предусиления
    setNewMode(true);
    break;
  }
#endif

#if USE_INPUT_BUTTON
  if (input_btn.getButtonState() == BTN_DOWN)
  {
    // переключение текущего входа
    switchingInput(++next_input);
  }
#endif
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
    mute_flag = false;
    tda.setVolume(cur_volume);
    printNumData(cur_volume);
    printProgressBar(cur_volume);
    TDA_PRINT(F("New volume set: "));
    TDA_PRINTLN(cur_volume);
    break;
  case SET_BASS:
    _change_data(cur_data.bass, -7, 7, _up);
    tda.setEqRange(cur_data.bass, BASS);
    printNumData(cur_data.bass);
    printProgressBar(cur_data.bass);
    TDA_PRINT(F("New bass set: "));
    TDA_PRINTLN(cur_data.bass);
    break;
  case SET_MIDDLE:
    _change_data(cur_data.middle, -7, 7, _up);
    tda.setEqRange(cur_data.middle, MIDDLE);
    printNumData(cur_data.middle);
    printProgressBar(cur_data.middle);
    TDA_PRINT(F("New middle set: "));
    TDA_PRINTLN(cur_data.middle);
    break;
  case SET_TREBBLE:
    _change_data(cur_data.trebble, -7, 7, _up);
    tda.setEqRange(cur_data.trebble, TREBBLE);
    printNumData(cur_data.trebble);
    printProgressBar(cur_data.trebble);
    TDA_PRINT(F("New trebble set: "));
    TDA_PRINTLN(cur_data.trebble);
    break;
  case SET_INPUT_GAIN:
    x = cur_data.input_gain;
    _change_data(x, 0, 15, _up);
    cur_data.input_gain = x;
    tda.setInputGain(cur_data.input_gain);
    printNumData(cur_data.input_gain);
    printProgressBar(cur_data.input_gain);
    TDA_PRINT(F("New input gain set: "));
    TDA_PRINTLN(cur_data.input_gain);
    break;
  case SET_SPEAKER_ATT:
    x = cur_data.spk_att;
    _change_data(x, 0, 15, _up);
    cur_data.spk_att = x;
    tda.setSpeakerAtt(cur_data.spk_att);
    printNumData(cur_data.spk_att);
    printProgressBar(cur_data.spk_att);
    TDA_PRINT(F("New speaker attenuation set: "));
    TDA_PRINTLN(cur_data.spk_att);
    break;
  case SET_BALANCE:
    _change_data(cur_data.balance, -14, 14, _up);
    setBalance(cur_data.balance);
    printNumData(cur_data.balance);
    printProgressBar(cur_data.balance);
    TDA_PRINT(F("New balance set: "));
    TDA_PRINTLN(cur_data.balance);
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
#if TURN_OFF_SCREEN_BACKLIGHT
  setBacklight(false);
#endif
}

void saveSettingsInEeprom()
{
  tasks.stopTask(save_settings_in_eeprom);
  write_eeprom_8(EEPROM_INDEX_FOR_VOLUME, cur_volume);
  writeInputData(cur_data, cur_input);
}

void ledGuard()
{
  // светодиод mute мигает только если поднят флаг mute_flag
  static int8_t flag = 0;
  if (!mute_flag)
  {
    digitalWrite(MUTE_LED_PIN, LOW);
    flag = 0;
  }
  else
  {
    digitalWrite(MUTE_LED_PIN, flag < 10);
    if (++flag >= 20)
    {
      flag = 0;
    }
  }

#if BT_MODULE_IS_USED
  // светодиод Bt-модуля горит только если выбран его вход, и питание на Bt-модуль подано
  digitalWrite(BT_LED_PIN, (cur_input == getInput(USE_BT_MODULE_ON_INPUT) &&
                            digitalRead(BT_POWER_PIN) == BT_CONTROL_LEVEL));
#endif
}

void powerShutdownGuard()
{
  tda.mute(); // главное, что делает монитор напряжения питания - отключает звук при выключении, чтобы избежать щелчка в колонках
#if BT_MODULE_IS_USED
  digitalWrite(BT_POWER_PIN, !BT_CONTROL_LEVEL);
#endif
  // если питание отключено, то запрещаем сохранение данных, т.к. есть риск, что питание пропадет в момент записи в EEPROM, и данные будут потеряны
  no_save_flag = true;
  TDA_PRINTLN(F("The power of the device is shutdown"));
}

// ===================================================

void setup()
{
#if USE_DEBUG_OUT
  Serial.begin(DEBUG_BAUD_COUNT);
#endif

  // ---------------------------------------------------

#if BT_MODULE_IS_USED
  digitalWrite(BT_POWER_PIN, !BT_CONTROL_LEVEL);
  pinMode(BT_POWER_PIN, OUTPUT);
  pinMode(BT_LED_PIN, OUTPUT);
#endif
  pinMode(MUTE_LED_PIN, OUTPUT);

  // ---------------------------------------------------

#if __USE_EEPROM_IN_FLASH__
  // инициализация EEPROM
  eeprom_init(EEPROM_SIZE);
#endif
  display_init();
  tda_init();
  enc.begin(INT_PULLUP_OF_ROTARY_PINS);

  // ---------------------------------------------------

  return_to_default_mode = tasks.addTask(TIMEOUT_OF_RETURN_TO_DEFMODE * 1000, returnToDefMode);
  save_settings_in_eeprom = tasks.addTask(TIMEOUT_OF_AUTOSAVE_DATA * 1000, saveSettingsInEeprom, false);
  led_guard = tasks.addTask(50ul, ledGuard);

  // ---------------------------------------------------

  attachInterrupt(0, powerShutdownGuard, FALLING);
  no_save_flag = false;

  // ---------------------------------------------------

  TDA_PRINTLN(F("Start device"));
  TDA_PRINTLN();
}

void loop()
{
  tasks.tick();
  checkRotary();
}
