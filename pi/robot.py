import yarn
import radio
import time

class robot:
    def __init__(self,address):
        self.address=address
        self.telemetry=[0 for i in range(256)]
        self.code=[]
        self.source=""
        self.state="disconnected"
        self.ping_time=time.time()
        self.watchdog_timeout=10
        self.ping_duration=1
        
    def telemetry_callback(self,data):
        if self.state=="disconnected" or self.state=="waiting":        
            self.state="connected"
        self.telemetry=data
        self.ping_time=time.time()

    def sync(self,radio,beat,bpm):
        radio.send_sync(self.address,beat,bpm)

    def upload_asm(self,fn,compiler,radio):
        if self.state!="disconnected":
            with open(fn, 'r') as f:
                self.source=f.read()
            self.code = compiler.assemble_file(fn)
            radio.send_code(self.address,self.code)
            self.state="running"

    def update(self,radio):
        if self.state=="disconnected":
            self.state="waiting"        
            self.ping_time=time.time()
            radio.request_telemetry(self.address,self.telemetry_callback,self)
        else:
            if time.time()-self.ping_time>self.watchdog_timeout:
                self.state="disconnected"
            if self.state!="waiting" and time.time()-self.ping_time>self.ping_duration:
                self.ping_time=time.time()
                radio.request_telemetry(self.address,self.telemetry_callback,self)

    def update_regs(self,regs,c):
        regs["state"]=self.state
        regs["ping"]=time.time()-self.ping_time
        regs["pc"]=self.telemetry[c.regs["PC_MIRROR"]]
        regs["a"]=self.telemetry[c.regs["A"]]
        regs["b"]=self.telemetry[c.regs["B"]]
        regs["comp_angle"]=self.telemetry[c.regs["COMP_ANGLE"]]
        regs["comp_dr"]=self.telemetry[c.regs["COMP_DELTA_RESET"]]
        regs["comp_d"]=self.telemetry[c.regs["COMP_DELTA"]]
        regs["step_count"]=self.telemetry[c.regs["STEP_COUNT"]]
        regs["step_reset"]=self.telemetry[c.regs["STEP_COUNT_RESET"]]
