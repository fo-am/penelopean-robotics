import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
from lib_nrf24 import NRF24
import time
import spidev
import struct
import math
import yarnasm

class radio:
    def __init__(self,address):
        self.address = address
        self.destination_address = [0xa7, 0xa7, 0xa7, 0xa7, 0x01]
        self.max_retries = 1 # can be a bit cleverer than this
        self.time_between_sends=0
        self.startup()
        self.debug()
        self.stop_osc=False
        self.telemetry=[]
        
    def debug(self):
        self.device.printDetails()        

    def startup(self):
	spi=spidev.SpiDev()
        self.device = NRF24(GPIO, spi)
        self.device.begin(0, 17)
        time.sleep(0.1)
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
        self.telemetry=[]


    def set_destination(self,addr):
        self.device.flush_tx();  
        if self.destination_address!=addr:
            self.destination_address=addr
            self.device.openWritingPipe(addr)
            
    def request_telemetry(self,addr,start):
        self.set_destination(addr)
        return self.send(self.build_telemetry(start))
    
    def send_set(self,addr,address,val):
        self.set_destination(addr)
        self.send(self.build_write(address,struct.pack("H",val)))
    
    def send_calibrate(self,addr,do_cali,samples,sense):
        self.set_destination(addr)
        return self.send(self.build_calibrate(do_cali,samples,sense))

    def send_save_eeprom(self,addr):
        self.set_destination(addr)
        self.send(self.build_eewrite())
    
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
                self.send(self.build_write(yarnasm.code_start+pos16,buf)) 
                pos16+=max_code_size/2
                # make time for receipt?
        else:
            self.send(self.build_write(yarnasm.code_start,code))
        self.send(self.build_reset())                
        #self.send(self.build_eewrite())
        
    def send_sync(self,addr,beat,ms_per_step,reg_sets):
        self.set_destination(addr)
        #if a_reg_set==1: print("starting: "+str(addr[4]))
        return self.send(self.build_sync(beat,ms_per_step,reg_sets))

    def send_pattern(self,addr,p,ms_per_step):
        self.set_destination(addr)
        self.send(self.build_pattern(p,ms_per_step))
    
    def send(self,b):
        #print("sending "+b[0]+" to "+str(self.destination_address))
        if len(b)!=32:
            print("wrong number of bytes in message: "+str(len(b)))
        else:
            status = self.device.write(b)
            if status!=32:
                print("send failed to "+str(self.destination_address[4])+" "+str(status))
                
            time.sleep(0.05)
            self.update()
            return self.telemetry

    def update(self):
        if self.device.isAckPayloadAvailable():
            data=[]
            self.telemetry=[]
            size = self.device.getDynamicPayloadSize()
            self.device.read(data, size)
            if size==32:
                self.telemetry = struct.unpack_from("hhhhhhhhhhhhhhhh","".join(map(chr,data)))
            elif size==28: # calibration data
                self.telemetry = struct.unpack_from("fffffff","".join(map(chr,data)))                
            elif size==12: # sense data
                self.telemetry = struct.unpack_from("fff","".join(map(chr,data)))                
            else:
                print("unexpected ack payload size of "+str(size))

    def build_ping(self):
        return struct.pack("cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx","P")

    def build_halt(self):
        return struct.pack("cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx","H")

    def build_reset(self):
        return struct.pack("cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx","R")

    def build_pattern(self,p,ms_per_step):
        return struct.pack("cxHH", 'M', ms_per_step, 4)+p+"00000000000000"
    
    def build_eewrite(self):
        return struct.pack("cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx","E")

    def build_sync(self,beat,ms_per_step,reg_sets):
        regs=[[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0]]
        
        for i in range(0,len(reg_sets)):
            regs[i]=reg_sets[i]

        # just sending two regs for now because arg
        return struct.pack("cxHHHHHHxxxxxxxxxxxxxxxxxx","S",beat,ms_per_step,regs[0][0],regs[0][1],regs[1][0],regs[1][1])

    def build_calibrate(self,do_cali,samples,return_sense):
        return struct.pack("cbHbxxxxxxxxxxxxxxxxxxxxxxxxxxx","C",do_cali,samples,return_sense)

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
