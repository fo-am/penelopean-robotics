import radio
import robot
import yarn
from OSC import OSCServer
import time
import threading

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
                      #robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x08])
        ]
    
        self.bpm=120
        self.beat=0
        self.beats_per_cycle = 4
        self.ms_per_beat = self.bpm_to_mspb(self.bpm)    
        self.last_sync = time.time()

        self.weft_swarm=[0]
        self.warp_swarm=[0]
        self.state="weft-walking"
        
        self.compiler = yarn.compiler()
        self.osc_server = OSCServer(("192.168.0.1", 8000))
        #self.osc_server.timeout = 0
        self.osc_server.addMsgHandler("/sync", sync_callback)
        self.sync_pos=0

        # load code here
        #self.swarm[0].load_asm("../asm/comptest.asm",self.compiler,self.radio)

        #for id in self.weft_swarm:
        #    self.swarm[id].load_asm("../asm/warp.asm",self.compiler,self.radio)
        #for id in self.warp_swarm:
        #    self.swarm[id].load_asm("../asm/warp.asm",self.compiler,self.radio)

        #self.swarm[0].load_asm("../asm/slow_led.asm",self.compiler,self.radio)
        #self.swarm[0].load_asm("../asm/back_forward2.asm",self.compiler,self.radio)

        # start sync osc server
        t = threading.Thread(target=osc_loop, args=(self,))
        t.start()

    def update(self):
        #print(self.state)
        #for r in self.swarm:
        #    r.pretty_print(self.compiler)


        #self.leds_on()
        #self.leds_off()
        self.weave_pattern()
        
        #if time.time()>self.last_sync+self.ms_per_beat/1000.0:
        #    self.sync(self.beat,self.bpm)
        #    self.beat+=1

        for robot in self.swarm:
            robot.update(self.radio)

        self.radio.update()

        
    def bpm_to_mspb(self,bpm):
        beats_per_sec = bpm/60.0
        beats_per_ms = beats_per_sec/1000.0
        return 1/beats_per_ms

    def sync(self,beat,bpm):
        print(beat,bpm)
        self.beat = int(beat)
        if self.beat%self.beats_per_cycle !=0: return

        # check beat number? 
        # clamp bpm??
        self.bpm=bpm
        self.last_sync = self.last_sync+1/(bpm/60.0)
        self.ms_per_beat = self.bpm_to_mspb(bpm)
        # update all robots in swarm at once (will cause small delays)
        #for robot in self.swarm:
            # send sync here, which updates telemetry
            #robot.sync(self.radio,beat,self.ms_per_beat)
            #robot.update(self.radio)

        # alternatively update one at a time at the right sync time
        # (will cause slower telemetry update - do we care?)
        self.swarm[self.sync_pos].sync(self.radio,beat,self.ms_per_beat)
        self.sync_pos=(self.sync_pos+1)%len(self.swarm)

        
    def upload_code(self,robot_id,fn):
        self.swarm[robot_id].upload_asm(fn,self.compiler,self.radio)

    def update_regs(self,regs):
        for i,robot in enumerate(self.swarm):
            robot.update_regs(regs[i],self.compiler)

    def trigger(self):
        if self.state=="weft-wait":
            self.state="weft-ready"
        if self.state=="warp-wait":
            self.state="warp-ready"

    def leds_on(self):
        for r in self.swarm:
            r.led_set(True)

    def leds_off(self):
        for r in self.swarm:
            r.led_set(False)

    def weave_pattern(self):
        if self.state=="weft-walking":
            # start warp swarm
            all_ready=True
            for id in self.weft_swarm:
                if self.swarm[id].is_walking(self.compiler):
                    all_ready=False
            # try not changing state till we have all stopped
            if all_ready:
                self.state="warp-start"
        
        if self.state=="warp-start":
            all_walking=True
            for id in self.warp_swarm:
                self.swarm[id].start_walking_set()
                if not self.swarm[id].is_walking(self.compiler):
                    all_walking=False
            # try not changing state till we are all walking
            if all_walking:
                self.state="warp-walking"

        if self.state=="warp-walking":
            all_ready=True
            for id in self.warp_swarm:
                if self.swarm[id].is_walking(self.compiler):
                    all_ready=False
            # try not changing state till we have all stopped
            if all_ready:
                self.state="weft-start"

        if self.state=="weft-start":
            all_walking=True
            for id in self.weft_swarm:
                self.swarm[id].start_walking_set()
                if not self.swarm[id].is_walking(self.compiler):
                    all_walking=False
            # try not changing state till we are all walking
            if all_walking:
                self.state="weft-walking"
            


if __name__ == "__main__":
    s = swarm()
    while True:
        s.update()
        time.sleep(1)
