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
        self.state="running"
        self.ping_time=time.time()
        self.watchdog_timeout=10
        self.ping_duration=2
        self.start_walking=False
        
    def telemetry_callback(self,data):
        if self.state=="disconnected" or self.state=="waiting":        
            self.state="connected"
        self.telemetry=data
        print("telemetry: "+str(self.address[4])+" "+str(data[9]))
        self.ping_time=time.time()

    def pretty_print(self,c):
        out = "robot: "+str(self.telemetry[c.regs["ROBOT_ID"]])+"\n"
        out+= "pc: "+str(self.telemetry[c.regs["PC_MIRROR"]])+"\n"
        out+= "a: "+str(self.telemetry[c.regs["A"]])+"\n"
        out+= "step: "+str(self.telemetry[c.regs["STEP_COUNT"]])+"\n"
        print(out)
        
        
    def sync(self,radio,beat,ms_per_beat):
        a_reg_val=0
        a_reg_set=0
        # update A register here, based on if the start flag has been set
        if self.start_walking:
            a_reg_val=1
            a_reg_set=1            
            self.start_walking=False
        radio.send_sync(self.address,beat,ms_per_beat,a_reg_set,a_reg_val,telemetry_callback,self)
        radio.update()
        # stop update requesting telemetry for a bit
        self.ping_time=time.time()

        
    def load_asm(self,fn,compiler,radio):
        with open(fn, 'r') as f:
            self.source=f.read()
        self.code = compiler.assemble_file(fn)

    # A register is cleared when the robot reaches it's end position
    # and set by the Pi when we are ready to start again
    def send_start_walking(self,compiler):
        #radio.send_set(self.address,compiler.regs["A"],1)
        self.start_walking=True

    # has been set above, and returned in a telemetry packet...
    def is_walking(self,compiler):
        return self.telemetry[compiler.regs["A"]]==1

    def update(self,radio):
        # if self.state=="disconnected":
        #     self.state="waiting"        
        #     self.ping_time=time.time()
        #     radio.request_telemetry(self.address,telemetry_callback,self)
        #     radio.update()

        # if self.state=="waiting":
        #     if time.time()-self.ping_time>self.watchdog_timeout:
        #         self.state="disconnected"
            
        # if self.state=="connected":
        #     if time.time()-self.ping_time>self.watchdog_timeout:
        #         self.state="disconnected"
        #     if time.time()-self.ping_time>self.ping_duration:
        #         self.ping_time=time.time()
        #         radio.request_telemetry(self.address,telemetry_callback,self)
        #         radio.update()
        #     if self.code!=[]:
        #         radio.send_code(self.address,self.code)
        #         self.state="running"
                
        # if self.state=="running":
        #     if time.time()-self.ping_time>self.watchdog_timeout:
        #         self.state="disconnected"
        #     if time.time()-self.ping_time>self.ping_duration:
        #         self.ping_time=time.time()
        #         radio.request_telemetry(self.address,telemetry_callback,self)
        #         radio.update()
        pass
        
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

