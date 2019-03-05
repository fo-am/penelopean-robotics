
#!/usr/bin/python
# -*- coding: utf-8 -*-

# This script is modified based on 
# https://github.com/d-zenju/mpu9250/blob/master/MPU9250.py
# from d-zenju


import smbus, time

class MPU9250():
    def __init__(self):
        self.bus = smbus.SMBus(1) 
        self.addr = 0x68 
        # set bypass mode for magnetometer
        self.bus.write_byte_data(self.addr, 0x37, 0x02)        
    def readLine(self, address, high, low):
        hline = self.bus.read_byte_data(address, high)
        lline = self.bus.read_byte_data(address, low)
        value = (hline << 8) + lline        
        if (value >= 0x8000):
            return -((65535 - value) + 1)
        else:
            return value
    def readAccel(self):
        # register address(MPU9250)
        ACCEL_XOUT_H = 0x3B
        ACCEL_XOUT_L = 0x3C
        ACCEL_YOUT_H = 0x3D
        ACCEL_YOUT_L = 0x3E
        ACCEL_ZOUT_H = 0x3F
        ACCEL_ZOUT_L = 0x40
        # calculate
        xout = self.readLine(self.addr, ACCEL_XOUT_H, ACCEL_XOUT_L)
        yout = self.readLine(self.addr, ACCEL_YOUT_H, ACCEL_YOUT_L)
        zout = self.readLine(self.addr, ACCEL_ZOUT_H, ACCEL_ZOUT_L)
        x = 2.0 * xout / 32768.0
        y = 2.0 * yout / 32768.0
        z = 2.0 * zout / 32768.0
        return [x, y, z]
    def readGyro(self):
        # register address(MPU9250)
        GYRO_XOUT_H = 0x43
        GYRO_XOUT_L = 0x44
        GYRO_YOUT_H = 0x45
        GYRO_YOUT_L = 0x46
        GYRO_ZOUT_H = 0x47
        GYRO_ZOUT_L = 0x48
        # calculate
        xout = self.readLine(self.addr, GYRO_XOUT_H, GYRO_XOUT_L)
        yout = self.readLine(self.addr, GYRO_YOUT_H, GYRO_YOUT_L)
        zout = self.readLine(self.addr, GYRO_ZOUT_H, GYRO_ZOUT_L)
        x = 250.0 * xout / 32768.0
        y = 250.0 * yout / 32768.0
        z = 250.0 * zout / 32768.0
        return [x, y, z]
    def readTemp(self):
        # register address(MPU9250)
        TEMP_OUT_H = 0x41
        TEMP_OUT_L = 0x42
        temp_out = self.readLine(self.addr, TEMP_OUT_H, TEMP_OUT_L)
        temp = temp_out / 340.0 + 36.53
        return temp
    def readMagnet(self):    
        # register address(AK8963)
        MAGNET_XOUT_L = 0x03
        MAGNET_XOUT_H = 0x04
        MAGNET_YOUT_L = 0x05
        MAGNET_YOUT_H = 0x06
        MAGNET_ZOUT_L = 0x07
        MAGNET_ZOUT_H = 0x08
        # MPU9250 datsheet page 24: slave address for the AK8963 is 0X0C
        AK8963_ADDRESS = 0x0C
        self.bus.write_byte_data(AK8963_ADDRESS, 0x0A, 0x12)
        time.sleep(0.15)
        # calculate
        xout = self.readLine(AK8963_ADDRESS, MAGNET_XOUT_H, MAGNET_XOUT_L)
        yout = self.readLine(AK8963_ADDRESS, MAGNET_YOUT_H, MAGNET_YOUT_L)
        zout = self.readLine(AK8963_ADDRESS, MAGNET_ZOUT_H, MAGNET_ZOUT_L)
        x = 1200.0 * xout / 4096.0
        y = 1200.0 * yout / 4096.0
        z = 1200.0 * zout / 4096.0
        return [x, y, z]



mpu9250 = MPU9250()

mpu9250.readAccel()
mpu9250.readGyro()
mpu9250.readTemp()
mpu9250.readMagnet()


