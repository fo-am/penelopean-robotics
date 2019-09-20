import radio
import robot
import yarn
from OSC import OSCServer
import time
import threading

import tangible
import smbus
bus = smbus.SMBus(1)

i2c_addrs = [0x0a, 0x0b, 0x0c, 0x0d,
             0x0e, 0x0f, 0x10, 0x11,  
             0x12, 0x13, 0x14, 0x15,
             0x16, 0x17, 0x18, 0x19]

dn = 0
up = 1
lr = 2
rl = 3

layout = [[0x0a,0,dn], [0x0b,1,dn], [0x0c,2,dn], [0x0d,3,dn],  
          [0x0e,0,dn], [0x0f,1,dn], [0x10,2,dn], [0x11,3,dn], 
          [0x12,0,dn], [0x13,1,dn], [0x14,2,dn], [0x15,3,dn],
          [0x16,0,dn], [0x17,1,dn], [0x18,2,dn], [0x19,3,dn]]
  
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
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x02]),
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x03]),
                      robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x04]),
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x05]),
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

        # load code here
        self.swarm[0].load_asm("../asm/pm.asm",self.compiler,self.radio)

        # start sync osc server
        #t = threading.Thread(target=osc_loop, args=(self,))
        #t.start()

    def update(self):
        #for r in self.swarm:
        #    r.pretty_print(self.compiler)

        #self.some_leds_on([0])
        #self.some_leds_on([4,5,6,7])
        #self.some_leds_on([0,1,2,3])
        #self.leds_on()
        #self.leds_off()

        for address in i2c_addrs:
            grid.update(frequency,address,
                        mcp23017.read_inputs_a(bus,address),
                        mcp23017.read_inputs_b(bus,address))
            pat = build_pattern(grid.data(5),symbols)
            cc = pat[0]+pat[1]+pat[2]+pat[3]+pat[4]
            if cc!=self.last:
                self.last=cc    
                print("   "+pat[0]+"\n   "+pat[1]+"\n   "+pat[2]+"\n   "+pat[3]+"\n   "+pat[4]+"\n")


        
        # read pm and update robots directly
        self.swarm[0].write(32+1,1,self.radio)
        
        for robot in self.swarm:
            if robot:
                robot.update(self.radio)

        self.radio.update()

        
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
        time.sleep(0.1)
