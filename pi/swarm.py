import radio
import robot
import yarn
from OSC import OSCServer
import time
import threading

global _swarm 

def sync_callback(path, tags, args, source):
    _swarm.sync(args[0],args[1])
    print(args)

def osc_loop(swarm):
    print(swarm)
    global _swarm
    _swarm = swarm
    while True:
        #while not swarm.osc_server.timed_out:
        swarm.osc_server.handle_request()

class swarm:
    def __init__(self):
        self.radio = radio.radio([0xa7, 0xa7, 0xa7, 0xa7, 0xaa])
        self.swarm = [robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x01]),
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x02]),
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x03]),
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x04]),
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x05]),
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x06]),
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x07]),
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x08]),
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x09])
        ]
        
        self.compiler = yarn.compiler()
        #self.osc_server = OSCServer(("localhost", 8000))
        #self.osc_server.timeout = 0
        #self.osc_server.addMsgHandler("/sync", sync_callback)
        self.sync_pos=0

        # load code here
        #self.swarm[0].load_asm("../asm/comptest.asm",self.compiler,self.radio)
        #self.swarm[0].load_asm("../asm/slow_led.asm",self.compiler,self.radio)
        self.swarm[0].load_asm("../asm/back_forward2.asm",self.compiler,self.radio)

        # start sync osc server
        #t = threading.Thread(target=osc_loop, args=(self,))
        #t.start()

    def sync(self,beat,bpm):
        self.swarm[self.sync_pos].sync(self.radio,beat,bpm)
        self.sync_pos=(self.sync_pos+1)%len(self.swarm)

    def start(self):
        for robot in self.swarm:
            robot.start(self.compiler,self.radio)
        
    def upload_code(self,robot_id,fn):
        self.swarm[robot_id].upload_asm(fn,self.compiler,self.radio)

    def update_regs(self,regs):
        for i,robot in enumerate(self.swarm):
            robot.update_regs(regs[i],self.compiler)

    def update(self):
        for robot in self.swarm:
            robot.update(self.radio)
        self.radio.update()

if __name__ == "__main__":
    s = swarm()
    while True:
        s.update()
        time.sleep(0.5)
