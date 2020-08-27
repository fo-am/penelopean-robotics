import radio
import robot
import yarn
from OSC import OSCServer
import time
import threading
import tangible

import tangible
import smbus
bus = smbus.SMBus(1)

i2c_addrs = [0x0a, 0x0b, 0x0c, 0x0d,
             0x0e, 0x0f, 0x10, 0x11,
             0x12, 0x13, 0x14, 0x15,
             0x16, 0x17, 0x18, 0x19]

frequency=0.01

def read_sensor_byte(bus,address,loc):
    r=0
    try:
        r = bus.read_byte_data(address,loc)
        if r!=0 and r!=1: r=0
    except:
        print("err: "+str(address))
    return r

def read_sensor(bus,addr):
    return [read_sensor_byte(bus,addr,0x00),
            read_sensor_byte(bus,addr,0x01),
            read_sensor_byte(bus,addr,0x02),
            read_sensor_byte(bus,addr,0x03)]
    
dn = 0
up = 1
lr = 2
rl = 3
st = 4

layout = [[0x0a,0,st], [0x0b,0,st], [0x0c,0,st], [0x0d,0,st],
          [0x0e,0,st], [0x0f,0,st], [0x10,0,st], [0x11,0,st],
          [0x12,0,st], [0x13,0,st], [0x14,0,st], [0x15,0,st],
          [0x16,0,st], [0x17,0,st], [0x18,0,st], [0x19,0,st]]

tokens = {"circle":    [[0,0,0,0],[1,1,1,1]],

          "rectangle": [[0,1,
                         1,0],
                        [1,0,
                         0,1]],

          "triangle":  [[1,1,
                         0,0],
                        [0,1,
                         0,1],
                        [0,0,
                         1,1],
                        [1,0,
                         1,0]],
          
          "square":    [[0,0,0,1],[0,0,1,0],[0,1,0,0],[1,0,0,0],
                        [1,1,1,0],[1,1,0,1],[1,0,1,1],[0,1,1,1]]}

symbols = [[".",[1,1,1,1]],["#",[0,0,0,0]],
           ["-",[1,0,1,0]],["!",[0,1,0,1]],
           ["u",[1,1,0,0]],["f",[0,1,1,0]],["d",[0,0,1,1]],["b",[1,0,0,1]],
           ["1",[1,0,0,0]],["2",[0,1,0,0]],["3",[0,0,1,0]],["4",[0,0,0,1]],
           ["5",[0,1,1,1]],["6",[1,0,1,1]],["7",[1,1,0,1]],["8",[1,1,1,0]]]

def convert_symbols(s):
    return {tangible.convert_4bit_twist(v):k for k, v in s}

symbols = convert_symbols(symbols)

def build_pattern(data,symbols):
    pat=[]
    for i in range(0,4):
        s=""
        for v in data[i][:4]:
            s+=symbols[v]+" "
        pat.append(s)
    return pat

global _swarm 

def sync_callback(path, tags, args, source):
    print("osc sync "+str(args[0])+" "+str(args[1]))
    _swarm.sync(args[0],args[1])
    
def osc_loop(swarm):
    global _swarm
    _swarm = swarm
    print("running osc")
    while not swarm.radio.stop_osc:
        #while not swarm.osc_server.timed_out:
        swarm.osc_server.handle_request()

class pmswarm:
    def __init__(self):
        self.radio = radio.radio([0xa7, 0xa7, 0xa7, 0xa7, 0xaa])
        self.swarm = [#robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x01]),
                      robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x02]),
                      robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x03]),
                      robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x04]),
                      robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x05]),
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x06]),
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x07]),
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x08])
        ]
    
        self.bpm=150
        self.beat=0
        self.beats_per_cycle = 1
        self.ms_per_beat = self.bpm_to_mspb(self.bpm)    
        self.last_sync = time.time()
        self.last=""
        
        self.compiler = yarn.compiler()
        self.osc_server = OSCServer(("0.0.0.0", 8000))
        self.osc_server.timeout = 0
        self.osc_server.addMsgHandler("/sync", sync_callback)
        self.sync_pos=0

        
        self.ms_per_step = (frequency*len(i2c_addrs))*1000
        self.ms_per_step/=8
        print(self.ms_per_step)

        # load code here
        #for r in self.swarm:
        #    r.load_asm("../asm/pm.asm",self.compiler,self.radio)
        #    r.write(13,self.ms_per_step,self.radio)
            
        self.grid=tangible.sensor_grid(16,layout,tokens)
        self.last=""
        self.next_address=0
        self.seq=0
        # start sync osc server
        #t = threading.Thread(target=osc_loop, args=(self,))
        #t.start()

    def interpret_token(self,robot,token):
        #print(robot,self.seq,symbols[token])
        if symbols[token]=='f':
            print("forward")
            self.swarm[robot].write(32+1,2,self.radio)
        elif symbols[token]=='b':
            print("back")
            self.swarm[robot].write(32+1,3,self.radio)
        else:
            self.swarm[robot].write(32+1,1,self.radio)

        
    def update(self):
        #for r in self.swarm:
        #    r.pretty_print(self.compiler)

        #self.some_leds_on([0])
        #self.some_leds_on([4,5,6,7])
        #self.some_leds_on([0,1,2,3])
        #self.leds_on()
        #self.leds_off()

        self.next_address+=1
        if self.next_address>=len(i2c_addrs):
            self.next_address=0
        
        address=i2c_addrs[self.next_address]
        sensor_data=read_sensor(bus,address)
        #print(address,sensor_data)
        self.grid.update(frequency*len(i2c_addrs),address,sensor_data)
        data = self.grid.data(4)
        pat = build_pattern(data,symbols)
        cc = pat[0]+pat[1]+pat[2]+pat[3]
        #print("   "+pat[0]+"\n   "+pat[1]+"\n   "+pat[2]+"\n   "+pat[3])
        if cc!=self.last:
            self.last=cc
            print("   "+pat[0]+"\n   "+pat[1]+"\n   "+pat[2]+"\n   "+pat[3])
            print("")


        if self.next_address==0:
            self.seq+=1
            if self.seq>=4: self.seq=0

            #self.swarm[self.seq].sync2(self.radio,0,self.ms_per_step)
                        
            #self.interpret_token(0,data[2][self.seq])
            #self.interpret_token(1,data[2][self.seq])
            #self.interpret_token(2,data[3][self.seq])
            #self.interpret_token(3,data[3][self.seq])
                
        # read pm and update robots directly
        #self.swarm[0].write(32+1,2,self.radio)
        #self.swarm[1].write(32+1,2,self.radio)
        
        #for robot in self.swarm:
        #    if robot:
        #        robot.update(self.radio)

        #self.radio.update()

        
    def bpm_to_mspb(self,bpm):
        beats_per_sec = bpm/60.0
        beats_per_ms = beats_per_sec/1000.0
        return 1/beats_per_ms
        
    def upload_code(self,robot_id,fn):
        self.swarm[robot_id].upload_asm(fn,self.compiler,self.radio)

    def update_regs(self,regs):
        for i,robot in enumerate(self.swarm):
            robot.update_regs(regs[i],self.compiler)

    def leds_on(self):
        for r in self.swarm:
            r.led_set(True)
            
    def some_leds_on(self,sel):
        for i,r in enumerate(self.swarm):
            if i in sel: r.led_set(True)
            else: r.led_set(False)

    def leds_off(self):
        for r in self.swarm:
            r.led_set(False)
            


if __name__ == "__main__":
    s = pmswarm()
    while True:
        s.update()
        time.sleep(frequency)
