import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
from lib_nrf24 import NRF24
import time
import spidev
import struct
import math
import yarn

class radio:
    def __init__(self,address):
        self.address = address
        self.destination_address = [0xa7, 0xa7, 0xa7, 0xa7, 0x01]
        self.startup()
        self.debug()
        
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
        self.time_between_sends=0.5
        self.telemetry=[]
        self.update_callback = False
        self.update_context = False

    def set_destination(self,addr):
        if self.destination_address!=addr:
            self.destination_address=addr
            self.device.openWritingPipe(addr)

    def request_telemetry(self,addr,callback,context):
        self.set_destination(addr)
        self.update_callback=callback
        self.update_context=context
        self.send(self.build_telemetry(0))
        
    def send_code(self,addr,code):
        self.set_destination(addr)
        l = len(code)
        self.send(self.build_halt())
        time.sleep(self.time_between_sends)
        max_code_size = 32-6

        if l>max_code_size:
            # chop into packets
            pos=0
            pos16=0
            while pos<l:
                buf=""
                for i in range(0,max_code_size):
                    if pos<l:
                        buf+=code[pos]
                    else:
                        buf+='\0'
                    pos+=1
                self.send(self.build_write(yarn.compiler.code_start+pos16,buf)) 
                pos16+=max_code_size/2
                # make time for receipt?
                time.sleep(self.time_between_sends)
        else:
            self.send(self.build_write(yarn.compiler.code_start,code))
        time.sleep(self.time_between_sends)
        self.send(self.build_reset())                

    def send_sync(self,addr,beat,bpm):
        self.set_destination(addr)
        self.send(self.build_sync(beat,bpm))

    def update(self):
        if self.device.isAckPayloadAvailable():
            data=[]
            size = self.device.getDynamicPayloadSize()
            self.device.read(data, size)
            if size==32:
                i = struct.unpack_from("hhhhhhhhhhhhhhhh","".join(map(chr,data)))
                if self.update_context and self.update_callback:
                    self.update_callback(self.update_context,i)
                    self.update_callback=False
                    self.update_context=False
            else:
                print("ack payload size other than 32 bytes need supporting")
                
    def send(self,b):
        print("sending "+b[0]+" to "+str(self.destination_address))
        if len(b)!=32:
            print("wrong number of bytes in message: "+str(len(b)))
        else:
            status = self.device.write(b)
            print status
            if status!=32: # tx_ok bit set??
                print("send failed...")
            return status
            
    def build_ping(self):
        return struct.pack("cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx","P")

    def build_halt(self):
        return struct.pack("cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx","H")

    def build_reset(self):
        return struct.pack("cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx","R")

    def build_sync(self,beat,bpm):
        return struct.pack("cxHfxxxxxxxxxxxxxxxxxxxxxxxx","S",beat,bpm)

    def build_calibrate(self,samples):
        return struct.pack("cxHxxxxxxxxxxxxxxxxxxxxxxxxxxx","C",samples)

    def build_telemetry(self,addr):
        return struct.pack("cxHxxxxxxxxxxxxxxxxxxxxxxxxxxxx","T",addr)  
        
    def build_write(self,addr,data):
        print("sending "+str(len(data))+" bytes of data to "+str(addr))
        if len(data)>32-6:
            print("build_write: data is too large ("+str(len(data))+" bytes)")
        else:
            padding = 32-6-len(data)
            pad=""
            for i in range(0,padding):
                pad+="\0"
            return struct.pack("cxHH","W",addr,len(data))+data+pad    

