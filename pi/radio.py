import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
from lib_nrf24 import NRF24
import time
import spidev
import struct
import math
import yarn

class radio:
    def __init__(self):
        self.address = [0xa7, 0xa7, 0xa7, 0xa7, 0xaa]
        self.destination_address = [0xa7, 0xa7, 0xa7, 0xa7, 0x01]
        self.startup()

    def debug(self):
        self.device.printDetails()

    def startup(self):
        self.device = NRF24(GPIO, spidev.SpiDev())
        self.device.begin(0, 17)
        time.sleep(1)
        self.device.setRetries(15,15)
        self.device.setPayloadSize(32)
        self.device.setChannel(100)
        self.device.write_register(NRF24.FEATURE, 0)
        self.device.setPALevel(NRF24.PA_MIN)
        self.device.setAutoAck(True)
        self.device.enableDynamicPayloads()
        self.device.enableAckPayload()
        self.device.openReadingPipe(1, self.address)
        self.device.openWritingPipe(self.destination_address)
        self.time_between_sends=0.1
        self.telemetry=[]
        self.update_callback = False
        self.update_context = False

    def set_destination(addr):
        self.destination_address=addr
        self.device.openWritingPipe(addr)

    def request_telemetry(addr,callback,context):
        if self.destination_address!=addr:
            self.destination_address=addr
            self.device.openWritingPipe(addr)
        self.update_callback=callback
        self.update_context=context
        self.device.send(self.build_telemetry(0))

    def send_code(addr,code):
        if self.destination_address!=addr:
            self.destination_address=addr
            self.device.openWritingPipe(addr)
        l = len(code)
        self.device.send(self.build_stop())                
        max_code_size = 32-6
        if l>max_code_size:
            # chop into packets
            pos=0
            while pos<l:
                buf=[]
                for i in range(0,max_code_size):
                    if pos<l:
                        buf.append(code[pos])
                    else:
                        buf.append(0)
                    pos+=1
            self.device.send(self.build_write(yarn.code_start+pos,buf))                
            # make time for receipt?
            time.sleep(self.time_between_sends)
        else:
            self.device.send(self.build_write(yarn.code_start,code))

    def update(self):
        if self.device.isAckPayloadAvailable():
            data=[]
            radio.read(data, radio.getDynamicPayloadSize())
            if self.update_context and self.update_callback:
                self.update_callback(self.update_context,data)
                self.update_callback=False
                self.update_context=False
                
    def send(self,b):
        print("sending "+str(b))
        if len(b!=32):
            print("wrong number of bytes in message: "+b)
        else:
            self.device.write(b)

    def build_ping(self):
        return struct.pack("cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx","P")

    def build_halt(self):
        return struct.pack("cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx","H")

    def build_reset(self):
        return struct.pack("cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx","R")

    def build_calibrate(self,samples):
        return struct.pack("cxHxxxxxxxxxxxxxxxxxxxxxxxxxxx","C",samples)

    def build_telemetry(self,addr):
        return struct.pack("cxHxxxxxxxxxxxxxxxxxxxxxxxxxxxx","T",addr)  
        
    def build_write(self,addr,data):
        print("sending "+str(len(data))+" bytes of data to "+str(addr))
        if len(data)>32-6:
            print("build_write: data is too large ("+str(len(data))+" bytes)"
        else:
            padding = 32-len(data)
            pad = []
            return struct.pack("cxHH","w",addr,len(data))+data    

