#!/usr/bin/python
# -*- coding: utf-8 -*-

# This script is modified based on 
# https://github.com/victorchinn/bmp280/blob/master/bmp280.py
# from S. Dame

import smbus, ctypes     

def getShort(data, index):
  # return two bytes from data as a signed 16-bit value
  return ctypes.c_short((data[index+1] << 8) + data[index]).value

def getUShort(data, index):
  # return two bytes from data as an unsigned 16-bit value
  return (data[index+1] << 8) + data[index]


class BMP280(object):
    # the address of BMP280 module on I2C bus is: 0x76
    def __init__(self):
        self.bus = smbus.SMBus(1)
        self.addr = 0x76
    def readAll(self):
      # Register Addresses
      REG_DATA    = 0xF7
      REG_CONTROL = 0xF4
      REG_CONFIG  = 0xF5
      # Oversample setting - page 27
      OVERSAMPLE_TEMP = 2
      OVERSAMPLE_PRES = 2
      MODE = 1
      # write oversample configuration and mode
      control = OVERSAMPLE_TEMP<<5 | OVERSAMPLE_PRES<<2 | MODE
      self.bus.write_byte_data(self.addr, REG_CONTROL, control)
      # Read blocks of calibration data from EEPROM
      # See Page 22 data sheet
      cal1 = self.bus.read_i2c_block_data(self.addr, 0x88, 24)
      cal2 = self.bus.read_i2c_block_data(self.addr, 0xA1, 1)
      cal3 = self.bus.read_i2c_block_data(self.addr, 0xE1, 7)
      # Convert byte data to word values
      dig_T1 = getUShort(cal1, 0)
      dig_T2 = getShort(cal1, 2)
      dig_T3 = getShort(cal1, 4)
      dig_P1 = getUShort(cal1, 6)
      dig_P2 = getShort(cal1, 8)
      dig_P3 = getShort(cal1, 10)
      dig_P4 = getShort(cal1, 12)
      dig_P5 = getShort(cal1, 14)
      dig_P6 = getShort(cal1, 16)
      dig_P7 = getShort(cal1, 18)
      dig_P8 = getShort(cal1, 20)
      dig_P9 = getShort(cal1, 22)
      data = self.bus.read_i2c_block_data(self.addr, REG_DATA, 6)
      pres_raw = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4)
      temp_raw = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4)
      # Refine temperature based on calibration per spec
      var1 = ((((temp_raw>>3)-(dig_T1<<1)))*(dig_T2)) >> 11
      var2 = (((((temp_raw>>4) - (dig_T1)) * ((temp_raw>>4) - (dig_T1))) >> 12) * (dig_T3)) >> 14
      t_fine = var1+var2
      temperature = float(((t_fine * 5) + 128) >> 8);
      # Refine pressure and adjust for temperature
      var1 = t_fine / 2.0 - 64000.0
      var2 = var1 * var1 * dig_P6 / 32768.0
      var2 = var2 + var1 * dig_P5 * 2.0
      var2 = var2 / 4.0 + dig_P4 * 65536.0
      var1 = (dig_P3 * var1 * var1 / 524288.0 + dig_P2 * var1) / 524288.0
      var1 = (1.0 + var1 / 32768.0) * dig_P1
      if var1 == 0:
        pressure=0
      else:
        pressure = 1048576.0 - pres_raw
        pressure = ((pressure - var2 / 4096.0) * 6250.0) / var1
        var1 = dig_P9 * pressure * pressure / 2147483648.0
        var2 = pressure * dig_P8 / 32768.0
        pressure = pressure + (var1 + var2 + dig_P7) / 16.0
      return temperature/100.0,pressure/100.0


        
bmp280 = BMP280()

bmp280.readAll()


