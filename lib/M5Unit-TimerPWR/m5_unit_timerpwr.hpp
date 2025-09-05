/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef _M5_UNIT_TIMERPWR_HPP_
#define _M5_UNIT_TIMERPWR_HPP_

#include "Arduino.h"
#include "Wire.h"

#define TIMERPWR_ADDR                      0x56
#define TIMERPWR_5V_OUTPUT_CTRL_REG        0x00
#define TIMERPWR_OLED_BACKLIGHT_CTRL_REG   0x01
#define TIMERPWR_SLEEP_COMMAND_REG         0x40
#define TIMERPWR_CYCLE_SLEEP_COMMAND_REG   0x41
#define TIMERPWR_BUTTON_REG                0x10
#define TIMERPWR_WAKEUP_TRIGGER_REG        0x20
#define TIMERPWR_SLEEP_TRIGGER_REG         0x21
#define TIMERPWR_POWER_ON_TIME_REG         0x30
#define TIMERPWR_POWER_OFF_TIME_REG        0x80
#define TIMERPWR_VOLTAGE_AND_CURRENT_REG   0x60
#define TIMERPWR_BATTERY_CHARGE_STATUS_REG 0x90
#define TIMERPWR_SAVE_FLASH_REG            0xF0
#define TIMERPWR_FIRMWARE_VERSION_REG      0xFE
#define TIMERPWR_BOOTLOADER_VERSION_REG    0xFC
#define TIMERPWR_I2C_ADDRESS_REG           0xFF

/**
 * @brief Unit TimerPWR control API
 */
class M5UnitTimerPWR {
public:
    /**
     * @brief Unit TimerPWR init
     * @param wire I2C Wire number
     * @param addr I2C address
     * @param sda I2C SDA Pin
     * @param scl I2C SCL Pin
     * @param speed I2C clock
     * @return 1 success, 0 false
     */
    bool begin(TwoWire *wire = &Wire, uint8_t addr = TIMERPWR_ADDR, uint8_t sda = 21, uint8_t scl = 22,
               uint32_t speed = 400000UL);

    /**
     * @brief set Unit TimerPWR I2C address
     * @param addr I2C address
     * @return 1 success, 0 false
     */
    uint8_t set_i2c_address(uint8_t addr);

    /**
     * @brief get Unit TimerPWR I2C address
     * @return I2C address
     */
    uint8_t get_i2c_address(void);

    /**
     * @brief get Unit TimerPWR firmware version
     * @return firmware version
     */
    uint8_t get_firmware_version(void);

    /**
     * @brief get Unit TimerPWR bootloader version
     * @return bootloader version
     */
    uint8_t get_bootloader_version(void);

    /**
     * @brief set Unit TimerPWR 5V output
     * @param en 1:5V output enable, 0:5V output disable
     */
    void set_grove_5v_output(bool en);

    /**
     * @brief get Unit TimerPWR 5V output status
     * @return 1:5V output enable, 0:5V output disable
     */
    uint8_t get_grove_5v_output(void);

    /**
     * @brief get Unit TimerPWR button value
     * @param btnA pointer of the value of btnA
     * @param btnB pointer of the value of btnB
     */
    void get_button_value(uint8_t *btnA, uint8_t *btnB);

    /**
     * @brief set Unit TimerPWR oled backlight
     * @param en 1:backlight on, 0:backlight off
     */
    void set_oled_backlight(bool en);

    /**
     * @brief get Unit TimerPWR oled backlight status
     * @return 1:backlight on, 0:backlight off
     */
    uint8_t get_oled_backlight(void);

    /**
     * @brief set Unit TimerPWR button wakeup trigger
     * @param en 1:button wakeup enable, 0:button wakeup disable
     */
    void set_button_wakeup_trigger(bool en);

    /**
     * @brief get Unit TimerPWR button wakeup trigger
     * @return 1:button wakeup enable, 0:button wakeup disable
     */
    bool get_button_wakeup_trigger(void);

    /**
     * @brief set Unit TimerPWR timer wakeup trigger
     * @param en 1:timer wakeup enable, 0:timer wakeup disable
     */
    void set_timer_wakeup_trigger(bool en);

    /**
     * @brief get Unit TimerPWR timer wakeup trigger
     * @return 1:timer wakeup enable, 0:timer wakeup disable
     */
    bool get_timer_wakeup_trigger(void);

    /**
     * @brief set Unit TimerPWR timer sleep trigger
     * @param en 1:timer sleep enable, 0:timer sleep disable
     */
    void set_timer_sleep_trigger(bool en);

    /**
     * @brief get Unit TimerPWR timer sleep trigger
     * @return 1:timer sleep enable, 0:timer sleep disable
     */
    bool get_timer_sleep_trigger(void);

    /**
     * @brief set Unit TimerPWR button sleep trigger
     * @param en 1:button sleep enable, 0:button sleep disable
     */
    void set_button_sleep_trigger(bool en);

    /**
     * @brief get Unit TimerPWR button sleep trigger
     * @return 1:button sleep enable, 0:button sleep disable
     */
    bool get_button_sleep_trigger(void);

    /**
     * @brief set Unit TimerPWR i2c sleep trigger
     * @param en 1:i2c sleep enable, 0:i2c sleep disable
     */
    void set_i2c_sleep_trigger(bool en);

    /**
     * @brief get Unit TimerPWR i2c sleep trigger
     * @return 1:i2c sleep enable, 0:i2c sleep disable
     */
    bool get_i2c_sleep_trigger(void);

    /**
     * @brief set Unit TimerPWR cycle sleep
     * @param en 1:cycle sleep enable, 0:cycle sleep disable
     */
    void set_cycle_sleep(bool en);

    /**
     * @brief get Unit TimerPWR cycle sleep status
     * @return 1:cycle sleep enable, 0:cycle sleep disable
     */
    uint8_t get_cycle_sleep(void);

    /**
     * @brief set Unit TimerPWR sleep start
     */
    void set_sleep(void);

    /**
     * @brief set Unit TimerPWR power on time
     * @param hour power on time on hour(0-255)
     * @param min power on time on min(0-59)
     * @param sec power on time on sec(0-59)
     */
    void set_power_on_time(uint8_t hour, uint8_t min, uint8_t sec);

    /**
     * @brief get Unit TimerPWR power on time
     * @param hour power on time on hour(0-255)
     * @param min power on time on min(0-59)
     * @param sec power on time on sec(0-59)
     */
    void get_power_on_time(uint8_t *hour, uint8_t *min, uint8_t *sec);

    /**
     * @brief set Unit TimerPWR power off time
     * @param hour power off time on hour(0-255)
     * @param min power off time on min(0-59)
     * @param sec power off time on sec(0-59)
     */
    void set_power_off_time(uint8_t hour, uint8_t min, uint8_t sec);

    /**
     * @brief get Unit TimerPWR power off time
     * @param hour power off time on hour(0-255)
     * @param min power off time on min(0-59)
     * @param sec power off time on sec(0-59)
     */
    void get_power_off_time(uint8_t *hour, uint8_t *min, uint8_t *sec);

    /**
     * @brief get Unit TimerPWR USB voltage
     * @return USB voltage V (result need to divide by 100)
     */
    int32_t get_usb_voltage(void);

    /**
     * @brief get Unit TimerPWR USB current
     * @return USB current mA (result need to divide by 100)
     */
    int32_t get_usb_current(void);

    /**
     * @brief get Unit TimerPWR grove voltage
     * @return grove voltage V (result need to divide by 100)
     */
    int32_t get_grove_voltage(void);

    /**
     * @brief get Unit TimerPWR grove current
     * @return grove current mA (result need to divide by 100)
     */
    int32_t get_grove_current(void);

    /**
     * @brief get Unit TimerPWR battery voltage
     * @return battery voltage V (result need to divide by 100)
     */
    int32_t get_battery_voltage(void);

    /**
     * @brief get Unit TimerPWR battery current
     * @return battery current mA (result need to divide by 100)
     */
    int32_t get_battery_current(void);

    /**
     * @brief get Unit TimerPWR battery charge status
     * @return 1: charging, 0: no charging
     */
    uint8_t get_battery_charge_status(void);

    /**
     * @brief set Unit TimerPWR save flash
     */
    void save_flash(void);

private:
    /**
     * @brief I2C write
     * @param addr I2C device address
     * @param reg I2C reg address
     * @param buffer pointer of write buffer
     * @param length write length
     */
    void write_bytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length);

    /**
     * @brief I2C read
     * @param addr I2C device address
     * @param reg I2C reg address
     * @param buffer pointer of read buffer
     * @param length read length
     */
    void read_bytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length);
    uint8_t _addr;
    TwoWire *_wire;
    uint8_t _scl;
    uint8_t _sda;
    uint32_t _speed;
};

#endif
