import yarn
import radio

class robot:
    def __init__(self):
        self.address=[0xa7, 0xa7, 0xa7, 0xa7, 0x01]
        self.telemetry=[]
        self.code=[]
        self.source=""

    def telemetry_callback(self,data):
        print(data)

    def load_asm(self,fn,compiler,radio):
        with open(fn, 'r') as f:
            self.source=f.read()
        self.code = compiler.assemble_file(fn)
        radio.send_code(self.address,self.code)

    def update(self,radio):
        radio.request_telemetry(self.address,self.telemetry_callback(),self)
        
