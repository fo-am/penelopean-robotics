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
        self.max_retries = 1 # can be a bit cleverer than this
        self.time_between_sends=0
        self.startup()
        self.debug()
        self.stop_osc=False
        
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
        self.device.setPALevel(NRF24.PA_MAX)
        self.device.setAutoAck(True)
        self.device.enableDynamicPayloads()
        self.device.enableAckPayload()
        self.device.openReadingPipe(1, self.address)
        self.device.openWritingPipe(self.destination_address)


    def set_destination(self,addr):
        if self.destination_address!=addr:
            self.destination_address=addr
            self.device.openWritingPipe(addr)
            
    def request_telemetry(self,addr):
        self.set_destination(addr)
        return self.send(self.build_telemetry(0))
        
    def send_code(self,addr,code):
        self.set_destination(addr)
        l = len(code)
        self.send(self.build_halt())
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
                self.send(self.build_write(yarn.code_start+pos16,buf)) 
                pos16+=max_code_size/2
                # make time for receipt?
        else:
            self.send(self.build_write(yarn.code_start,code))
        self.send(self.build_reset())                

    def send_sync(self,addr,beat,ms_per_step,reg_sets,callback,context):
        self.set_destination(addr)
        self.update_callback=callback
        self.update_context=context
        if a_reg_set==1: print("starting: "+str(addr[4]))
        self.send(self.build_sync(beat,ms_per_step,reg_sets))

    def send(self,b):
        #print("sending "+b[0]+" to "+str(self.destination_address))
        if len(b)!=32:
            print("wrong number of bytes in message: "+str(len(b)))
        else:
            status = 0
            while status!=32 and retries<self.max_retries: 
                status = self.device.write(b)
                if status!=32:
                    print("send failed to "+str(self.destination_address[4])+" retries: "+str(retries)+" "+str(status))

                if self.device.isAckPayloadAvailable():
                    data=[]
                    size = self.device.getDynamicPayloadSize()
                    self.device.read(data, size)
                    if size==32:
                        i = struct.unpack_from("hhhhhhhhhhhhhhhh","".join(map(chr,data)))
                        return i
                    else:
                        print("ack payload size other than 32 bytes need supporting")
                        return []

    def build_seq(self,ms_per_step,length,pattern):
        return struct.pack("cxIIs","M",ms_per_step,length,pattern[:26])
            
    def build_ping(self):
        return struct.pack("cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx","P")

    def build_halt(self):
        return struct.pack("cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx","H")

    def build_reset(self):
        return struct.pack("cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx","R")

    def build_sync(self,beat,ms_per_step,reg_sets):
        ret = struct.pack("cxHH","S",beat,ms_per_step)
        for i in range(0,7):
            if i<len(reg_sets):
                ret+=struct.pack("HH",reg[0],reg[1])
            else:
                ret+=struct.pack("HH",0,0)        
        return ret

    def build_calibrate(self,samples):
        return struct.pack("cxHxxxxxxxxxxxxxxxxxxxxxxxxxxx","C",samples)

    def build_save_eeprom(self):
        return struct.pack("cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx","E",samples)

    def build_telemetry(self,addr):
        return struct.pack("cxHxxxxxxxxxxxxxxxxxxxxxxxxxxxx","T",addr)  
        
    def build_write(self,addr,data):
        #print("sending "+str(len(data))+" bytes of data to "+str(addr))
        if len(data)>32-6:
            print("build_write: data is too large ("+str(len(data))+" bytes)")
        else:
            padding = 32-6-len(data)
            pad=""
            for i in range(0,padding):
                pad+="\0"
            return struct.pack("cxHH","W",addr,len(data))+data+pad    
