
import serial
import time
import numpy as np
import matplotlib.pyplot as plt

ser = serial.Serial("/dev/ttyUSB0",115200)
plt.axis([0, 10, 0, 1])
plt.show(False)
    
while True:
    plt.clf()
    d = []
    for i in range(0,128):
        b = ser.readline()         # read a byte string
        string_n = b.decode()  # decode byte string into Unicode    
        string = string_n.rstrip() # remove \n and \r    
        try:
            flt = float(string)        # convert string to float
            #print(flt)
            d.append(flt)
        except:
            pass
    plt.ylim([0,1024])
    plt.plot(d)
    plt.pause(0.05)
    plt.draw()
