import yarn
import radio
import time

# things we want to be able to do:
# * calibrate
# * write code to eeprom
# * tweak servo defaults
# * queue of messages to send?

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
        self.set_led=False
        self.led_state=False
        

    def pretty_print(self):
        out = "robot: "+str(self.telemetry[yarn.registers["ROBOT_ID"]])+"\n"
        out+= "pc: "+str(self.telemetry[yarn.registers["PC_MIRROR"]])+"\n"
        out+= "a: "+str(self.telemetry[yarn.registers["A"]])+"\n"
        out+= "step: "+str(self.telemetry[yarn.registers["STEP_COUNT"]])+"\n"

    def telemetry_callback(self,data):
        if self.state=="disconnected" or self.state=="waiting":        
            self.state="connected"
        self.telemetry=data
        #print("telemetry: "+str(self.address[4])+" "+str(data[0])+" "+str(data[9]))
        self.ping_time=time.time()
                
    def sync(self,radio,beat,ms_per_beat):
        reg_sets = []
        # update A register here, based on if the start flag has been set
        if self.start_walking:
            reg_sets+=[[yarn.registers["A"],1]]
            self.start_walking=False
        if self.set_led:
            reg_sets+=[[yarn.registers["LED"],self.led_state]]

        telemetry = radio.send_sync(self.address,beat,ms_per_beat,reg_sets)
        if telemetry!=[]:
            self.telemetry = telemetry
            print("telemetry: "+str(self.address[4])+" "+str(self.telemetry[0])+" "+str(self.telemetry[9]))
            # stop update requesting telemetry for a bit
            self.ping_time=time.time()
            
    def load_asm(self,fn,compiler,radio):
        with open(fn, 'r') as f:
            self.source=f.read()
        self.code = compiler.assemble_file(fn)
        radio.send_code(self.address,self.code)

    def write(self,addr,val,radio):
        radio.send_set(self.address,addr,val)
        
    # A register is cleared when the robot reaches it's end position
    # and set by the Pi when we are ready to start again
    def start_walking_set(self):
        self.start_walking=True

    def led_set(self,state):
        self.set_led=True
        self.led_state=state

    # has been set above, and returned in a telemetry packet...
    def is_walking(self):
        return self.telemetry[yarn.registers["A"]]==1

    def update(self,radio):
        pass

    def update_regs(self,regs):
        regs["state"]=self.state
        regs["ping"]=time.time()-self.ping_time
        regs["pc"]=self.telemetry[yarn.registers["PC_MIRROR"]]
        regs["a"]=self.telemetry[yarn.registers["A"]]
        regs["b"]=self.telemetry[yarn.registers["B"]]
        regs["comp_angle"]=self.telemetry[yarn.registers["COMP_ANGLE"]]
        regs["comp_dr"]=self.telemetry[yarn.registers["COMP_DELTA_RESET"]]
        regs["comp_d"]=self.telemetry[yarn.registers["COMP_DELTA"]]
        regs["step_count"]=self.telemetry[yarn.registers["STEP_COUNT"]]
        regs["step_reset"]=self.telemetry[yarn.registers["STEP_COUNT_RESET"]]
        regs["robot"]=self.telemetry[yarn.registers["ROBOT_ID"]]
