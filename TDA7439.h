#ifndef TDA7439_H
#define TDA7439_H

#include <Wire.h>
#include <Arduino.h>

#define I2C_PORT Wire

// 7-битный адрес микросхемы на шине I2C
#define TDA7439_address 0x44

// Sub addresses
#define TDA7439_INPUT_SEL 0x00
#define TDA7439_INPUT_GAIN 0x01
#define TDA7439_VOLUME 0x02
#define TDA7439_BASS 0x03
#define TDA7439_MIDDLE 0x04
#define TDA7439_TREBBLE 0x05

#define TDA7439_RATT 0x06
#define TDA7439_LATT 0x07

// выбор входного канала
enum TDA7439_input : uint8_t
{
	INPUT_4, // вход 4
	INPUT_3, // вход 3
	INPUT_2, // вход 2
	INPUT_1	 // вход 1
};

static TDA7439_input getNextInput(const TDA7439_input obj)
{
	switch (obj)
	{
	case INPUT_4:
		return (INPUT_3);
	case INPUT_3:
		return (INPUT_2);
	case INPUT_2:
		return (INPUT_1);
	default:
		return (INPUT_4);
	}
}

static TDA7439_input getPrevInput(const TDA7439_input obj)
{
	switch (obj)
	{
	case INPUT_3:
		return (INPUT_4);
	case INPUT_2:
		return (INPUT_3);
	case INPUT_1:
		return (INPUT_2);
	default:
		return (INPUT_1);
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

// диапазон регулировки тембра
enum TDA7439_bands : uint8_t
{
	BASS = 0x03,	 // низкие частоты
	MIDDLE = 0x04, // средние частоты
	TREBBLE = 0x05 // высокие частоты
};

// TDA7439 диапазон предусиления на входе, db (с шагом 2db)
// 0x00 .. 0x0F

// TDA7439 диапазон громкости (-db)
// 0x00 .. 0x2F
#define TDA7439_MUTE 0x38 // отключение звука (mute_flag)

class TDA7439
{
public:
	/**
	 * @brief Construct a new TDA7439 object
	 *
	 */
	TDA7439();

	/**
	 * @brief инициализация TDA7439
	 *
	 * @param wire I2C-интерфейс для работы с TDA7439
	 */
	void begin();

	/**
	 * @brief установка входного канала
	 *
	 * @param input выбранный канал - INPUT_4, INPUT_3, INPUT_2, INPUT_1
	 */
	void setInput(TDA7439_input input);

	/**
	 * @brief установка усиления на входе
	 *
	 * @param input_gain уровень усиления; 0..15
	 */
	void inputGain(uint8_t input_gain);

	/**
	 * @brief установка громкости
	 *
	 * @param volume уровень громкости; 0..47
	 */
	void setVolume(uint8_t volume);

	/**
	 * @brief установка тембра
	 *
	 * @param val устанавливаемое значение; -7..7
	 * @param range диапазон - BASS, MIDDLE, TREBBLE
	 */
	void setSnd(int8_t val, TDA7439_bands range);

	/**
	 * @brief отключение звука
	 *
	 */
	void mute_flag();

	/**
	 * @brief установка баланса
	 *
	 * @param att_r правый канал; 0..79 (db)
	 * @param att_l левый канал; 0..79 (db)
	 */
	void spkAtt(uint8_t att_r, uint8_t att_l);

private:
	void writeWire(uint8_t reg, uint8_t data);
};

// ===================================================

TDA7439::TDA7439() {}

void TDA7439::begin()
{
  I2C_PORT.begin();
}

void TDA7439::setInput(TDA7439_input input)
{
  writeWire(TDA7439_INPUT_SEL, (uint8_t)input);
}

void TDA7439::inputGain(uint8_t input_gain)
{
  if (input_gain > 15)
  {
    input_gain = 15;
  }
  writeWire(TDA7439_INPUT_GAIN, input_gain);
}

void TDA7439::setVolume(uint8_t volume)
{
  volume = (volume) ? ((volume <= 47) ? 47 - volume : 0) : TDA7439_MUTE;
  writeWire(TDA7439_VOLUME, volume);
}

void TDA7439::setSnd(int8_t val, TDA7439_bands range)
{
  val = (val < -7) ? -7 : ((val > 7) ? 7 : val);
  val = (val > 0) ? 15 - val : val + 7;
  writeWire((uint8_t)range, val);
}

void TDA7439::mute_flag()
{
  writeWire(TDA7439_VOLUME, TDA7439_MUTE);
}

void TDA7439::spkAtt(uint8_t att_r, uint8_t att_l)
{
  // Mainly used to override the default attenuation of mute_flag at power up
  // can be used for balance with some simple code changes here.
  if (att_l > 79)
  {
    att_l = 79;
  }
  if (att_r > 79)
  {
    att_r = 79;
  }

  writeWire(TDA7439_RATT, att_r);
  writeWire(TDA7439_LATT, att_l);
}

void TDA7439::writeWire(uint8_t reg, uint8_t data)
{
  I2C_PORT.beginTransmission(TDA7439_address);
  I2C_PORT.write(reg);
  I2C_PORT.write(data);
  I2C_PORT.endTransmission();
}

// ===================================================

TDA7439 tda;

// ===================================================

#endif // TDA7439_H
