def convert_4bit_st(arr):
    return arr[0]|arr[1]<<1|arr[2]<<2|arr[3]<<3

def convert_4bit(arr):
    # clockwise from top left
    return arr[0]|arr[1]<<1|arr[3]<<2|arr[2]<<3

def convert_4bit_upsidedown(arr):
    return arr[2]|arr[3]<<1|arr[1]<<2|arr[0]<<3

def convert_4bit_lr(arr):
    # clockwise from top left
    return arr[1]|arr[0]<<1|arr[2]<<2|arr[3]<<3

def convert_4bit_rl(arr):
    # clockwise from top left
    return arr[3]|arr[2]<<1|arr[0]<<2|arr[1]<<3

def convert_arr(num):
    # clockwise from top left
    return [(num>>0)&1,(num>>1)&1,(num>>2)&1,(num>>3)&1]


# 
def convert_4bit_twist(arr):
    # clockwise from top left
    return arr[0]|arr[1]<<1|arr[2]<<2|arr[3]<<3


## this classifier is simply a filter for biasing against readings
## for different tokens in favour of orientations of existing ones
## noise is considered to be caused primarily by 'in between' sensor
## readings of the tangible tokens

class sensor_filter:
    def __init__(self,tokens):
        self.token_current = "?"
        self.token_theory = "?"
        self.value_current = 0
        self.value_theory = 0
        self.confidence_time = 0
        self.token_change_time = 1.0
        self.token_orient_time = 0.1
        # convert to integer representation
        self.tokens = self.convert_tokens(tokens)

    def convert_token(self,patterns):
        return map(convert_4bit,patterns)

    def convert_tokens(self,tokens):
        return {k: self.convert_token(v) for k, v in tokens.items()}

    def value_to_token(self,val):
        for k,v in self.tokens.items():
            if val in v: return k
        return "?"

    def observation(self,dt,value):
        # have we witnessed a change?
        if value != self.value_theory:
            self.value_theory = value
            self.token_theory = self.value_to_token(value)
            if self.token_current in self.tokens and \
               value in self.tokens[self.token_current]:
                # permutation of the current token is more likely
                self.confidence_time = self.token_orient_time
            else:
                # than a new token (could be rotation noise etc)
                self.confidence_time = self.token_change_time
        if self.confidence_time==0:
            self.token_current=self.token_theory
            self.value_current=self.value_theory
            self.confidence_time=-1
            #print(self.token_current+" "+str(convert_arr(self.value_current))+" "+str(self.value_current))
            return convert_arr(self.value_current)
        else:
            if self.confidence_time>0:
                self.confidence_time=max(0,self.confidence_time-dt)
        return False

class sensor_grid:
    def __init__(self, num_sensors, layout, tokens):
        self.state = []
        for i in range(0,num_sensors):
            self.state.append(sensor_filter(tokens))
        self.layout = layout
        self.sensor_orientation_dn = 0
        self.sensor_orientation_up = 1
        self.sensor_orientation_lr = 2
        self.sensor_orientation_rl = 3
        self.sensor_orientation_st = 4
        self.last_debug=[0,0,0,0]

    # version for single sensor
    def update(self,dt,address,sensor):
        sensor_data=[sensor]
        # search layout and update filters
        for i,p in enumerate(self.layout):
            if p[0]==address:
                converted = 0
                if p[2]==self.sensor_orientation_st:
                    converted=convert_4bit_st(sensor_data[p[1]])
                if p[2]==self.sensor_orientation_dn:
                    converted=convert_4bit(sensor_data[p[1]])
                if p[2]==self.sensor_orientation_up:
                    converted=convert_4bit_upsidedown(sensor_data[p[1]])
                if p[2]==self.sensor_orientation_lr:
                    converted=convert_4bit_lr(sensor_data[p[1]])
                if p[2]==self.sensor_orientation_rl:
                    converted=convert_4bit_rl(sensor_data[p[1]])
                r = self.state[i].observation(dt,converted)
                if r: self.last_debug=r

    def data(self,width):
        d=[]
        r=[]
        row=1
        for i,p in enumerate(self.layout):
            r.append(self.state[i].value_current)
            if row==width:
                d.append(r)
                row=0
                r=[]
            row+=1
        return d
            
    def pprint(self,width):
        s=""
        row=1
        for i,p in enumerate(self.layout):
            data=convert_arr(self.state[i].value_current)
            s+=str(data[0])
            s+=str(data[1])
            s+=str(data[2])
            s+=str(data[3])
            s+=" "
            if row==width:
                row=0
                s+="\n"
            row+=1
        print(s)

    def big_matrix(self,width):
        s=""
        for y in range(0,width):
            for x in range(0,width):
                data=convert_arr(self.state[y*width+x].value_current)
                s+=str(data[2])+" "+str(data[3])+" "
            s+="\n"
            for x in range(0,width):
                data=convert_arr(self.state[y*width+x].value_current)
                s+=str(data[1])+" "+str(data[0])+" "
            s+="\n"
        return s
