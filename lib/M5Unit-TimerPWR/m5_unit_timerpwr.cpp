/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "m5_unit_timerpwr.hpp"

void M5UnitTimerPWR::write_bytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length)
{
    _wire->beginTransmission(addr);
    _wire->write(reg);
    for (int i = 0; i < length; i++) {
        _wire->write(*(buffer + i));
    }
    _wire->endTransmission();
}

void M5UnitTimerPWR::read_bytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length)
{
    uint8_t index = 0;
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(addr, length);
    for (int i = 0; i < length; i++) {
        buffer[index++] = _wire->read();
    }
}

bool M5UnitTimerPWR::begin(TwoWire *wire, uint8_t addr, uint8_t sda, uint8_t scl, uint32_t speed)
{
    _wire  = wire;
    _addr  = addr;
    _sda   = sda;
    _scl   = scl;
    _speed = speed;
    _wire->begin(_sda, _scl);
    _wire->setClock(speed);
    delay(10);
    _wire->beginTransmission(_addr);
    uint8_t error = _wire->endTransmission();
    if (error == 0) {
        return true;
    } else {
        return false;
    }
}

void M5UnitTimerPWR::set_grove_5v_output(bool en)
{
    write_bytes(_addr, TIMERPWR_5V_OUTPUT_CTRL_REG, (uint8_t *)&en, 1);
}

uint8_t M5UnitTimerPWR::get_grove_5v_output(void)
{
    uint8_t res;

    read_bytes(_addr, TIMERPWR_5V_OUTPUT_CTRL_REG, (uint8_t *)&res, 1);

    return res;
}

void M5UnitTimerPWR::get_button_value(uint8_t *btnA, uint8_t *btnB)
{
    uint8_t res[2];

    read_bytes(_addr, TIMERPWR_BUTTON_REG, (uint8_t *)&res, 2);

    *btnA = res[0];
    *btnB = res[1];
}

void M5UnitTimerPWR::set_oled_backlight(bool en)
{
    write_bytes(_addr, TIMERPWR_OLED_BACKLIGHT_CTRL_REG, (uint8_t *)&en, 1);
}

uint8_t M5UnitTimerPWR::get_oled_backlight(void)
{
    uint8_t res;

    read_bytes(_addr, TIMERPWR_OLED_BACKLIGHT_CTRL_REG, (uint8_t *)&res, 1);

    return res;
}

void M5UnitTimerPWR::set_button_wakeup_trigger(bool en)
{
    uint8_t reg_data;

    read_bytes(_addr, TIMERPWR_WAKEUP_TRIGGER_REG, (uint8_t *)&reg_data, 1);

    if (en)
        reg_data |= (1 << 1);
    else
        reg_data &= ~(1 << 1);

    write_bytes(_addr, TIMERPWR_WAKEUP_TRIGGER_REG, (uint8_t *)&reg_data, 1);
}

bool M5UnitTimerPWR::get_button_wakeup_trigger(void)
{
    uint8_t res;

    read_bytes(_addr, TIMERPWR_WAKEUP_TRIGGER_REG, (uint8_t *)&res, 1);

    return !!(res & (1 << 1));
}

void M5UnitTimerPWR::set_timer_wakeup_trigger(bool en)
{
    uint8_t reg_data;

    read_bytes(_addr, TIMERPWR_WAKEUP_TRIGGER_REG, (uint8_t *)&reg_data, 1);

    if (en)
        reg_data |= (1 << 0);
    else
        reg_data &= ~(1 << 0);

    write_bytes(_addr, TIMERPWR_WAKEUP_TRIGGER_REG, (uint8_t *)&reg_data, 1);
}

bool M5UnitTimerPWR::get_timer_wakeup_trigger(void)
{
    uint8_t res;

    read_bytes(_addr, TIMERPWR_WAKEUP_TRIGGER_REG, (uint8_t *)&res, 1);

    return !!(res & (1 << 0));
}

void M5UnitTimerPWR::set_timer_sleep_trigger(bool en)
{
    uint8_t reg_data;

    read_bytes(_addr, TIMERPWR_SLEEP_TRIGGER_REG, (uint8_t *)&reg_data, 1);

    if (en)
        reg_data |= (1 << 0);
    else
        reg_data &= ~(1 << 0);

    write_bytes(_addr, TIMERPWR_SLEEP_TRIGGER_REG, (uint8_t *)&reg_data, 1);
}

bool M5UnitTimerPWR::get_timer_sleep_trigger(void)
{
    uint8_t res;

    read_bytes(_addr, TIMERPWR_SLEEP_TRIGGER_REG, (uint8_t *)&res, 1);

    return !!(res & (1 << 0));
}

void M5UnitTimerPWR::set_button_sleep_trigger(bool en)
{
    uint8_t reg_data;

    read_bytes(_addr, TIMERPWR_SLEEP_TRIGGER_REG, (uint8_t *)&reg_data, 1);

    if (en)
        reg_data |= (1 << 1);
    else
        reg_data &= ~(1 << 1);

    write_bytes(_addr, TIMERPWR_SLEEP_TRIGGER_REG, (uint8_t *)&reg_data, 1);
}

bool M5UnitTimerPWR::get_button_sleep_trigger(void)
{
    uint8_t res;

    read_bytes(_addr, TIMERPWR_SLEEP_TRIGGER_REG, (uint8_t *)&res, 1);

    return !!(res & (1 << 1));
}

void M5UnitTimerPWR::set_i2c_sleep_trigger(bool en)
{
    uint8_t reg_data;

    read_bytes(_addr, TIMERPWR_SLEEP_TRIGGER_REG, (uint8_t *)&reg_data, 1);

    if (en)
        reg_data |= (1 << 2);
    else
        reg_data &= ~(1 << 2);

    write_bytes(_addr, TIMERPWR_SLEEP_TRIGGER_REG, (uint8_t *)&reg_data, 1);
}

bool M5UnitTimerPWR::get_i2c_sleep_trigger(void)
{
    uint8_t res;

    read_bytes(_addr, TIMERPWR_SLEEP_TRIGGER_REG, (uint8_t *)&res, 1);

    return !!(res & (1 << 2));
}

void M5UnitTimerPWR::set_cycle_sleep(bool en)
{
    write_bytes(_addr, TIMERPWR_CYCLE_SLEEP_COMMAND_REG, (uint8_t *)&en, 1);
}

uint8_t M5UnitTimerPWR::get_cycle_sleep(void)
{
    uint8_t res;

    read_bytes(_addr, TIMERPWR_CYCLE_SLEEP_COMMAND_REG, (uint8_t *)&res, 1);

    return res;
}

void M5UnitTimerPWR::set_sleep(void)
{
    uint8_t data = 1;
    write_bytes(_addr, TIMERPWR_SLEEP_COMMAND_REG, (uint8_t *)&data, 1);
}

void M5UnitTimerPWR::set_power_on_time(uint8_t hour, uint8_t min, uint8_t sec)
{
    uint8_t reg_data[3];

    reg_data[0] = hour;
    reg_data[1] = min;
    reg_data[2] = sec;
    write_bytes(_addr, TIMERPWR_POWER_ON_TIME_REG, reg_data, 3);
}

void M5UnitTimerPWR::get_power_on_time(uint8_t *hour, uint8_t *min, uint8_t *sec)
{
    uint8_t reg_data[3];

    read_bytes(_addr, TIMERPWR_POWER_ON_TIME_REG, reg_data, 3);

    *hour = reg_data[0];
    *min  = reg_data[1];
    *sec  = reg_data[2];
}

void M5UnitTimerPWR::set_power_off_time(uint8_t hour, uint8_t min, uint8_t sec)
{
    uint8_t reg_data[3];

    reg_data[0] = hour;
    reg_data[1] = min;
    reg_data[2] = sec;
    write_bytes(_addr, TIMERPWR_POWER_OFF_TIME_REG, reg_data, 3);
}

void M5UnitTimerPWR::get_power_off_time(uint8_t *hour, uint8_t *min, uint8_t *sec)
{
    uint8_t reg_data[3];

    read_bytes(_addr, TIMERPWR_POWER_OFF_TIME_REG, reg_data, 3);

    *hour = reg_data[0];
    *min  = reg_data[1];
    *sec  = reg_data[2];
}

int32_t M5UnitTimerPWR::get_usb_voltage(void)
{
    int32_t res;

    read_bytes(_addr, TIMERPWR_VOLTAGE_AND_CURRENT_REG, (uint8_t *)&res, 4);

    return res;
}

int32_t M5UnitTimerPWR::get_usb_current(void)
{
    int32_t res;

    read_bytes(_addr, TIMERPWR_VOLTAGE_AND_CURRENT_REG + 4, (uint8_t *)&res, 4);

    return res;
}

int32_t M5UnitTimerPWR::get_grove_voltage(void)
{
    int32_t res;

    read_bytes(_addr, TIMERPWR_VOLTAGE_AND_CURRENT_REG + 8, (uint8_t *)&res, 4);

    return res;
}

int32_t M5UnitTimerPWR::get_grove_current(void)
{
    int32_t res;

    read_bytes(_addr, TIMERPWR_VOLTAGE_AND_CURRENT_REG + 12, (uint8_t *)&res, 4);

    return res;
}

int32_t M5UnitTimerPWR::get_battery_voltage(void)
{
    int32_t res;

    read_bytes(_addr, TIMERPWR_VOLTAGE_AND_CURRENT_REG + 16, (uint8_t *)&res, 4);

    return res;
}

int32_t M5UnitTimerPWR::get_battery_current(void)
{
    int32_t res;

    read_bytes(_addr, TIMERPWR_VOLTAGE_AND_CURRENT_REG + 20, (uint8_t *)&res, 4);

    return res;
}

uint8_t M5UnitTimerPWR::get_battery_charge_status(void)
{
    uint8_t res;

    read_bytes(_addr, TIMERPWR_BATTERY_CHARGE_STATUS_REG, (uint8_t *)&res, 1);

    return res;
}

void M5UnitTimerPWR::save_flash(void)
{
    uint8_t data = 1;
    write_bytes(_addr, TIMERPWR_SAVE_FLASH_REG, (uint8_t *)&data, 1);
}

uint8_t M5UnitTimerPWR::get_bootloader_version(void)
{
    _wire->beginTransmission(_addr);
    _wire->write(TIMERPWR_BOOTLOADER_VERSION_REG);
    _wire->endTransmission(false);

    uint8_t reg_value;

    _wire->requestFrom(_addr, 1);
    reg_value = Wire.read();
    return reg_value;
}

uint8_t M5UnitTimerPWR::get_i2c_address(void)
{
    _wire->beginTransmission(_addr);
    _wire->write(TIMERPWR_I2C_ADDRESS_REG);
    _wire->endTransmission(false);

    uint8_t reg_value;

    _wire->requestFrom(_addr, 1);
    reg_value = Wire.read();
    return reg_value;
}

uint8_t M5UnitTimerPWR::set_i2c_address(uint8_t addr)
{
    _wire->beginTransmission(_addr);
    _wire->write(TIMERPWR_I2C_ADDRESS_REG);
    _wire->write(addr);
    _addr = addr;
    if (_wire->endTransmission() == 0)
        return true;
    else
        return false;
}

uint8_t M5UnitTimerPWR::get_firmware_version(void)
{
    _wire->beginTransmission(_addr);
    _wire->write(TIMERPWR_FIRMWARE_VERSION_REG);
    _wire->endTransmission(false);

    uint8_t reg_value;

    _wire->requestFrom(_addr, 1);
    reg_value = Wire.read();
    return reg_value;
}
