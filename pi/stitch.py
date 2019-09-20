import smbus
import time

bus = smbus.SMBus(1)

addrs = [#0x0a,0x0b,0x0c,0x0d,
         0x0e,0x0f,0x10,0x11,
         0x12,0x13,0x14,0x15,
         0x16,0x17,0x18,0x19
]

while True:
    for addr in addrs:
        try:
            if addr<0x0e:
                print(str(addr)+" "+str(bus.read_byte_data(addr,0x01))+":"+\
                      str(bus.read_byte_data(addr,0x02))+":"+\
                      str(bus.read_byte_data(addr,0x03))+":"+\
                      str(bus.read_byte_data(addr,0x04)))
            else:
                print(str(addr)+" "+str(bus.read_byte_data(addr,0x00))+":"+\
                      str(bus.read_byte_data(addr,0x01))+":"+\
                      str(bus.read_byte_data(addr,0x02))+":"+\
                      str(bus.read_byte_data(addr,0x03)))
        except:
            print("err: "+str(addr))
        time.sleep(0.3)




