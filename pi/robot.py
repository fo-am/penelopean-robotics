import yarn
import radio
import time

def telemetry_callback(robot,data):
    robot.telemetry_callback(data)

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
        print(data[0],data[3])
        self.ping_time=time.time()

    def sync(self,radio,beat,bpm):
        radio.send_sync(self.address,beat,bpm)

    def load_asm(self,fn,compiler,radio):
        with open(fn, 'r') as f:
            self.source=f.read()
        self.code = compiler.assemble_file(fn)

    def update(self,radio):
        if self.state=="disconnected":
            self.state="waiting"        
            self.ping_time=time.time()
            radio.request_telemetry(self.address,telemetry_callback,self)

        if self.state=="waiting":
            if time.time()-self.ping_time>self.watchdog_timeout:
                self.state="disconnected"
            
        if self.state=="connected":
            if time.time()-self.ping_time>self.watchdog_timeout:
                self.state="disconnected"
            if time.time()-self.ping_time>self.ping_duration:
                self.ping_time=time.time()
                radio.request_telemetry(self.address,telemetry_callback,self)
            if self.code!=[]:
                radio.send_code(self.address,self.code)
                self.state="running"
                
        if self.state=="running":
            if time.time()-self.ping_time>self.watchdog_timeout:
                self.state="disconnected"
            if time.time()-self.ping_time>self.ping_duration:
                self.ping_time=time.time()
                radio.request_telemetry(self.address,telemetry_callback,self)
 
