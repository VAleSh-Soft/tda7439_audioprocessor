#ifndef _HEADER_FILE_
#define _HEADER_FILE_

#include <Rotary.h>        // https://github.com/brianlow/Rotary
#include <shButton.h>      // https://github.com/VAleSh-Soft/shButton
#include <shTaskManager.h> // https://github.com/VAleSh-Soft/shTaskManager
#include "TDA7439.h"

// ===================================================

constexpr uint32_t TIMEOUT_OF_RETURN_TO_DEFMODE = 6; // таймаут автовозврата в режим по умолчанию, секунд
constexpr uint32_t TIMEOUT_OF_AUTOSAVE_DATA = 5;     // таймаут задержки автосохранения настроек, секунд

constexpr uint8_t BT_CONTROL_LEVEL = HIGH; // управляющий уровень для включения модуля Bt

constexpr uint8_t ENC_A_PIN = 3;    // пин A энкодера (CLK)
constexpr uint8_t ENC_B_PIN = 2;    // пин B энкодера (DT)
constexpr uint8_t BUTTON_PIN = 4;   // пин кнопки энкодера (SW)
constexpr uint8_t BT_POWER_PIN = 5; // пин для управления питанием Bt-модуля

constexpr uint16_t EEPROM_INDEX_FOR_VOLUME = 10; // индекс в EEPROM для сохранения текущей громкости (1 байт)
constexpr uint16_t EEPROM_INDEX_FOR_INPUT = 11;  // индекс в EEPROM для сохранения текущего входа (1 байт)
constexpr uint16_t EEPROM_INDEX_FOR_DATA_1 = 12; // индекс в EEPROM для сохранения данных первого канала (5 байт)
constexpr uint16_t EEPROM_INDEX_FOR_DATA_2 = 17; // индекс в EEPROM для сохранения данных второго канала (5 байт)
constexpr uint16_t EEPROM_INDEX_FOR_DATA_3 = 22; // индекс в EEPROM для сохранения данных третьего канала (5 байт)
constexpr uint16_t EEPROM_INDEX_FOR_DATA_4 = 27; // индекс в EEPROM для сохранения данных четвертого канала (5 байт)

// ===================================================

struct TDA_DATA // структура с данными настройки для каждого входа
{
  int8_t bass;
  int8_t middle;
  int8_t trebble;
  int8_t balance;
  uint8_t input_gain;
};

// ===================================================

enum TDA_CUR_MODE : uint8_t // режим работы модуля
{
  SET_VOLUME,     // установка громкости
  SET_BASS,       // установка низких частот
  SET_MIDDLE,     // установка средних частот
  SET_TREBBLE,    // установка высоких частот
  SET_BALANCE,    // установка баланса
  SET_INPUT_GAIN, // установка предусиления
  SET_INPUT       // установка входа
};

static TDA_CUR_MODE getNextMode(const TDA_CUR_MODE obj)
{
  switch (obj)
  {
  case SET_VOLUME:
    return (SET_BASS);
  case SET_BASS:
    return (SET_MIDDLE);
  case SET_MIDDLE:
    return (SET_TREBBLE);
  case SET_TREBBLE:
    return (SET_BALANCE);
  case SET_BALANCE:
    return (SET_VOLUME);
  default:
    return (SET_VOLUME);
  }
}

TDA_CUR_MODE &operator++(TDA_CUR_MODE &obj)
{
  obj = getNextMode(obj);
  return (obj);
}

TDA_CUR_MODE operator++(TDA_CUR_MODE &obj, const int)
{
  const TDA_CUR_MODE copy = obj;
  obj = getNextMode(obj);
  return (copy);
}

// ==== tda7439_audioprocessor.ino ===================

void checkRotary();           // опрос энкодера и его кнопки
void changeCurData(bool _up); // изменение текущего параметра
void returnToDefMode();       // возврат в режим по умолчанию
void saveSettingsInEeprom();  // сохранение настроек в EEPROM

// ==== _eeprom.h ====================================

uint8_t read_eeprom_8(uint16_t _index);
void write_eeprom_8(uint16_t _index, uint8_t _data);
void readInputData(TDA_DATA &_data, TDA7439_input _input);  // чтение данных настройки входа из EEPROM
void writeInputData(TDA_DATA &_data, TDA7439_input _input); // запись данных настройки входа в EEPROM

// ==== display_LCD_I2C.h ============================

void display_init();
void printNumData(int8_t number);    // вывод цифрового значения текущего параметра в правом верхнем углу экрана
void printInData(bool bt = true);    // вывод номера текущего входа в левом верхнем углу экрана
void printProgressBar(int8_t _data); //  вывод прогресс-бара
void printCurScreen();               // отрисовка текущего экрана
void setBacklight(bool flag);        // управление подсветкой экрана

// ==== tda.h =========================================

void tda_init();
void readCurInput();                     // получение сохраненного значения входа из EEPROM
void setInputData(TDA7439_input _input); // первоначальная настройка TDA7439 для текущего входа
void setBalance(int8_t _balance);        // установка баланса

// ===================================================

shHandle return_to_default_mode;
shHandle save_settings_in_eeprom;

shTaskManager tasks(2);

// ===================================================

class tdaButton : public shButton
{
private:
public:
  tdaButton(uint8_t button_pin) : shButton(button_pin)
  {
    shButton::setTimeoutOfLongClick(1000);
    shButton::setVirtualClickOn(true);
    shButton::setLongClickMode(LCM_ONLYONCE);
  }

  uint8_t getButtonState()
  {
    uint8_t _state = shButton::getButtonState();
    switch (_state)
    {
    case BTN_DOWN:
    case BTN_DBLCLICK:
      setBacklight(true);
      tasks.startTask(return_to_default_mode);
      tasks.startTask(save_settings_in_eeprom);
      break;
    }
    return (_state);
  }
};

tdaButton btn(BUTTON_PIN);

// ===================================================

class tdaRotary : public Rotary
{
public:
  tdaRotary() : Rotary(ENC_A_PIN, ENC_B_PIN) {}
  unsigned char process()
  {
    unsigned char _state = Rotary::process();
    if (_state)
    {
      setBacklight(true);
      tasks.startTask(return_to_default_mode);
      tasks.startTask(save_settings_in_eeprom);
      if (btn.isButtonClosed())
      {
        btn.resetButtonState();
      }
    }
    return (_state);
  }
};

tdaRotary enc;

// ===================================================

TDA_CUR_MODE cur_mode = SET_VOLUME; // текущий режим работы модуля
TDA7439_input cur_input = INPUT_1;  // текущий вход
TDA7439_input next_input = INPUT_1; // вход для переключения
bool new_input = false;             // флаг необходимости переключения входа

uint8_t cur_volume = 20; // текущая громкость
bool mute = false;       // флаг отключения звука
TDA_DATA cur_data;       // данные для настройки текущего канала

int8_t dir = 0;

// ===================================================

#endif // _HEADER_FILE_