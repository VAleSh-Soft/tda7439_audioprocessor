#ifndef _HEADER_FILE_
#define _HEADER_FILE_

#include <Rotary.h>        // https://github.com/brianlow/Rotary
#include <shButton.h>      // https://github.com/VAleSh-Soft/shButton
#include <shTaskManager.h> // https://github.com/VAleSh-Soft/shTaskManager
#include "TDA7439.h"

// ===================================================

#define NUMBER_OF_INPUT_IS_USED 4   // количество используемых входов TDA7439; 1..4
#define USE_BT_MODULE_ON_INPUT 4    // использовать Bluetooth трансмиттер на указанном входе; 0 - не использовать
#define USE_DEBUG_OUT 0             // включить вывод отладочной информации в сериал
#define DEBUG_BAUD_COUNT 115200     // скорость передачи данных в сериал
#define USE_MUTE_BUTTON 0           // использовать кнопку для быстрого отключения звука
#define USE_MODE_BUTTON 0           // использовать кнопку для переключения режима управления
#define USE_INPUT_BUTTON 0          // использовать кнопку для переключения входа
#define TURN_OFF_SCREEN_BACKLIGHT 1 // отключать экран при возврате в режим по умолчанию

constexpr uint32_t TIMEOUT_OF_RETURN_TO_DEFMODE = 10; // таймаут автовозврата в режим по умолчанию, секунд
constexpr uint32_t TIMEOUT_OF_AUTOSAVE_DATA = 5;      // таймаут задержки автосохранения настроек, секунд

constexpr bool INT_PULLUP_OF_ROTARY_PINS = true; // используется внутренняя подтяжка пинов к VCC, установите false, если ваш модуль энкодера использует внешнюю подтяжку

#if USE_BT_MODULE_ON_INPUT > 0 && USE_BT_MODULE_ON_INPUT <= NUMBER_OF_INPUT_IS_USED
constexpr uint8_t BT_CONTROL_LEVEL = HIGH; // управляющий уровень для включения модуля Bt
#endif

constexpr uint8_t ENC_A_PIN = 4;      // пин A энкодера (DT)
constexpr uint8_t ENC_B_PIN = 5;      // пин B энкодера (CLK)
constexpr uint8_t ENC_BUTTON_PIN = 3; // пин кнопки энкодера (SW)
#if USE_BT_MODULE_ON_INPUT > 0 && USE_BT_MODULE_ON_INPUT <= NUMBER_OF_INPUT_IS_USED
constexpr uint8_t BT_POWER_PIN = 6; // пин для управления питанием Bt-модуля
constexpr uint8_t BT_LED_PIN = 8;   // пин светодиода - индикатора включения Bt-модуля
#endif
constexpr uint8_t MUTE_LED_PIN = 9;        // пин светодиода mute
constexpr uint8_t VOLTAGE_CONTROL_PIN = 2; // пин контроля пропадания напряжения
#if USE_MUTE_BUTTON
constexpr uint8_t MUTE_BUTTON_PIN = 7; // пин кнопки для отключения звука
#endif
#if USE_MODE_BUTTON
constexpr uint8_t MODE_BUTTON_PIN = 10; // пин кнопки для переключения режима управления
#endif
#if USE_INPUT_BUTTON
constexpr uint8_t INPUT_BUTTON_PIN = 11; // пин кнопки для переключения входа
#endif

constexpr uint16_t EEPROM_INDEX_FOR_VOLUME = 10; // индекс в EEPROM для сохранения текущей громкости (1 байт)
constexpr uint16_t EEPROM_INDEX_FOR_INPUT = 11;  // индекс в EEPROM для сохранения текущего входа (1 байт)
constexpr uint16_t EEPROM_INDEX_FOR_DATA_1 = 12; // индекс в EEPROM для сохранения данных первого канала (5 байт)
constexpr uint16_t EEPROM_INDEX_FOR_DATA_2 = 17; // индекс в EEPROM для сохранения данных второго канала (5 байт)
constexpr uint16_t EEPROM_INDEX_FOR_DATA_3 = 22; // индекс в EEPROM для сохранения данных третьего канала (5 байт)
constexpr uint16_t EEPROM_INDEX_FOR_DATA_4 = 27; // индекс в EEPROM для сохранения данных четвертого канала (5 байт)

// ===================================================

#if NUMBER_OF_INPUT_IS_USED < 1
#error "Set the correct value of NUMBER_OF_INPUT_IS_USED - 1..4"
#endif

#if USE_BT_MODULE_ON_INPUT > 0 && USE_BT_MODULE_ON_INPUT <= NUMBER_OF_INPUT_IS_USED
#define BT_MODULE_IS_USED 1
#else
#define BT_MODULE_IS_USED 0
#endif

#if USE_DEBUG_OUT
#define TDA_PRINTLN(x) Serial.println(x)
#define TDA_PRINT(x) Serial.print(x)
#else
#define TDA_PRINTLN(x)
#define TDA_PRINT(x)
#endif

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

void setMute();                          // управление режимом mute
void setNewMode(bool gain_mode = false); // переход к следующему режиму работы модуля
void checkRotary();                      // опрос энкодера, его кнопки и прочих кнопок, при их наличии
void changeCurData(bool _up);            // изменение текущего параметра
void returnToDefMode();                  // возврат в режим по умолчанию
void saveSettingsInEeprom();             // сохранение настроек в EEPROM
void ledGuard();                         // управление светодиодом
void powerShutdownGuard();               // контроль за пропаданием напряжения питания

// ==== _eeprom.h ====================================

uint8_t read_eeprom_8(uint16_t _index);
void write_eeprom_8(uint16_t _index, uint8_t _data);
void readInputData(TDA_DATA &_data, TDA7439_input _input);  // чтение данных настройки входа из EEPROM
void writeInputData(TDA_DATA &_data, TDA7439_input _input); // запись данных настройки входа в EEPROM

// ==== display_LCD_I2C.h ============================

void display_init();
void printBigChar(uint8_t x);        // вывод больших символов
void printNumData(int8_t number);    // вывод цифрового значения текущего параметра в правом верхнем углу экрана
void printInData();                  // вывод номера текущего входа в левом верхнем углу экрана
void printProgressBar(int8_t _data); //  вывод прогресс-бара
void printCurScreen();               // отрисовка текущего экрана
void setBacklight(bool flag);        // управление подсветкой экрана

// ==== tda.h =========================================

void tda_init();
TDA7439_input readCurInput();            // получение сохраненного значения входа из EEPROM
void setInputData(TDA7439_input _input); // первоначальная настройка TDA7439 для текущего входа
void setBalance(int8_t _balance);        // установка баланса
void switchingInput(TDA7439_input _input,
                    bool _init = false); // переключение входа

// ===================================================

shHandle return_to_default_mode;
shHandle save_settings_in_eeprom;
shHandle led_guard;

shTaskManager tasks(3);

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
      if (!tasks.getTaskState(return_to_default_mode))
      {
        // если экран погашен, щелчок кнопкой просто включает его, не выполняя заложенное на щелчок действие; исключение - кнопки mute_btn и input_btn, они срабатывают в любом случае
        resetButtonState();
      }
      tasks.startTask(return_to_default_mode);
      tasks.startTask(save_settings_in_eeprom);
      break;
    }
    return (_state);
  }
};

tdaButton enc_btn(ENC_BUTTON_PIN);
#if USE_MUTE_BUTTON
tdaButton mute_btn(MUTE_BUTTON_PIN);
#endif
#if USE_MODE_BUTTON
tdaButton mode_btn(MODE_BUTTON_PIN);
#endif
#if USE_INPUT_BUTTON
tdaButton input_btn(INPUT_BUTTON_PIN); // пин кнопки для переключения входа
#endif

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

uint8_t cur_volume = 20;            // текущая громкость
bool mute_flag = false;             // флаг отключения звука
bool no_mute = false;               // флаг запрета отключения звука
volatile bool no_save_flag = false; // флаг запрета сохранения настроек; если поднят, значит сработал монитор пропадания напряжения питания
TDA_DATA cur_data;                  // данные для настройки текущего канала

int8_t dir = 0;

// ===================================================

#if NUMBER_OF_INPUT_IS_USED > 1
static TDA7439_input getNextInput(const TDA7439_input obj)
{
  switch (obj)
  {
  case INPUT_1:
    return (INPUT_2);
#if NUMBER_OF_INPUT_IS_USED > 2
  case INPUT_2:
    return (INPUT_3);
#endif
#if NUMBER_OF_INPUT_IS_USED > 3
  case INPUT_3:
    return (INPUT_4);
#endif
  default:
    return (INPUT_1);
  }
}

static TDA7439_input getPrevInput(const TDA7439_input obj)
{
  switch (obj)
  {
  case INPUT_2:
    return (INPUT_1);
#if NUMBER_OF_INPUT_IS_USED > 2
  case INPUT_3:
    return (INPUT_2);
#endif
#if NUMBER_OF_INPUT_IS_USED > 3
  case INPUT_4:
    return (INPUT_3);
#endif
  case INPUT_1:
#if NUMBER_OF_INPUT_IS_USED == 2
    return (INPUT_2);
#elif NUMBER_OF_INPUT_IS_USED == 3
    return (INPUT_3);
#else
    return (INPUT_4);
#endif
  }
}

TDA7439_input &operator++(TDA7439_input &obj)
{
  obj = getNextInput(obj);
  return (obj);
}

TDA7439_input operator++(TDA7439_input &obj, const int)
{
  const TDA7439_input copy = obj;
  obj = getNextInput(obj);
  return (copy);
}

TDA7439_input &operator--(TDA7439_input &obj)
{
  obj = getPrevInput(obj);
  return (obj);
}

TDA7439_input operator--(TDA7439_input &obj, const int)
{
  const TDA7439_input copy = obj;
  obj = getPrevInput(obj);
  return (copy);
}
#endif

// ===================================================

#endif // _HEADER_FILE_