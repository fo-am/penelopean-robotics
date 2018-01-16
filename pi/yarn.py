import array

class compiler:
    code_start=32
    def __init__(self):
        self.labels={}
        self.instr = {
            "nop": 0,
            "jmp": 1,
            "jpr":   2,
            "jpz":   3,
            "jprz":  4,
            "ldl":   5,
            "ld":    6,
            "ldi":   7,
            "st":    8,
            "sti":   9,
            "dup":   10,
            "equ":  11, 
            "lt":   12, 
            "gt":   13, 
            "lte":  14, 
            "gte":  15,
            "slt":  16,
            "sgt":  17,
            "slte": 18,
            "sgte": 19,
            "add":  20,
            "sub":  21,
            "inc":  22,
            "dec":  23,
            "and":  24,
            "or":   25,
            "xor":  26,
            "not":  27,
            "rr":   28,
            "rl":   29,
            "sin":  30,
            "cos":  31,
            "tan":  32,
            "rnd":  33,
            "inci": 34,
            "deci": 35,
            "incp": 36,
            "decp": 37,
        }

        # change robot.c registers to match here

        self.regs = {
            "ROBOT_ID": 0,
            "PC_MIRROR": 1,
            "LED": 2,
            "COMP_ANGLE": 3,
            "COMP_DELTA_RESET": 4,
            "COMP_DELTA": 5,
            "STEP_COUNT": 6,
            "STEP_COUNT_RESET": 7,
            "NEXT_PATTERN": 8,
            "A": 9,
            "B": 10,
            "C": 11,
            "D": 12,
            "SERVO_MS_PER_STEP": 13, # arg step not=step counter type of step
            "SERVO_1_AMP": 14,
            "SERVO_2_AMP": 15,
            "SERVO_3_AMP": 16,
            "SERVO_1_BIAS": 17,
            "SERVO_2_BIAS": 18,
            "SERVO_3_BIAS": 19,
            "SERVO_1_SMOOTH": 20,
            "SERVO_2_SMOOTH": 21,
            "SERVO_3_SMOOTH": 22,

            "NULL": 0,
            "ALL_STOP": 1,
            "WALK_FORWARD": 2,
            "WALK_BACKWARD": 3,

        }

    def is_int(self,s):
        try:
            int(s)
            return True
        except ValueError:
            return False

    def collect_label(self,addr,l):
        l = l.split()
        if len(l)<1: return []
        if l[0].endswith(":"):
            # add new label
            self.labels[l[0][:-1]]=addr+self.code_start
            l=l[1:] # strip out label

    def assemble_line(self,i,l):
        l = l.split()
        if len(l)<1: return []
        if l[0].endswith(":"):
            l=l[1:] # strip out label
        if not l[0] in self.instr:
            print("instruction "+l[0]+" not understood on line "+str(i))
            return []
        else:
            if len(l)>1:
                # deal with operand
                operand = l[1]
                if self.is_int(operand):
                    return [self.instr[l[0]],int(operand)]
                if operand in self.regs:
                    return [self.instr[l[0]],self.regs[operand]]
                if operand in self.labels.keys():
                    return [self.instr[l[0]],self.labels[operand]]
                if operand == ";;":
                    return []
                else:
                    print(l)
                    print("i do not understand "+operand+" on line "+str(i))
                    return []

            return [self.instr[l[0]]]




    def line_size(self,l):
        ls=l.split()
        if len(ls)==0: return 0
        # strip out label
        if ls[0].endswith(":"):
            ls=ls[1:]
        pos=0
        word=""
        while word!=";;" and pos<len(ls):
            word=ls[pos]
            if word!=";;": pos+=1
        return pos

    def assemble(self,s):
        s=s.split("\n")
        addr=0
        for i,line in enumerate(s):
            sl = line.strip()
            self.collect_label(addr,sl)
            if len(sl)>0:
                addr+=self.line_size(sl)
        code = []
        for i,line in enumerate(s):
            sl=line.strip()
            if len(sl)>0 and sl[0]!=";":
                code+=self.assemble_line(i,sl)
        print code
        print(len(code))
        return code

    # returns array of bytes (from 16bit values)
    def assemble_bytes(self,s):
        code = self.assemble(s)
        split = []
        for i in code:
            split.append(i & 0xff)
            split.append((i>>8) & 0xff)
        return array.array("B",split).tostring()

    def assemble_file(self,fn):
        with open(fn, 'r') as f:
            return self.assemble_bytes(f.read())

def unit_test():
    c = compiler()

    assert(c.line_size("xor \n")==1)
    assert(c.line_size("ld 0 \n")==2)
    assert(c.line_size("ld 0 ;; one two\n")==2)
    assert(c.line_size("start: ld 0 ;; one two three\n")==2)
    assert(c.line_size("start: xor ;; one two\n")==1)
    
    assert(c.assemble("ld 0 \n\
                       st 100")==[6,0,8,100])

    assert(c.assemble("ld LED \n\
                       st 100")==[6,2,8,100])

    assert(c.assemble("label: ld LED \n\
                       jmp label")==[6,2,1,32])

    assert(c.assemble("ld 1 \n\
                       label: ld LED \n\
                       jmp label")==[6,1,6,2,1,34])
    
    #c.assemble_file("../asm/led_flash.asm")
    #c.assemble_file("../asm/back_forward.asm")
    #c.assemble_file("../asm/warp.asm")

    # wait - forward - wait - back (1st version running)
    #c.assemble_file("../asm/warp.asm")

    # wait - forward (simple weft version)
    #c.assemble_file("../asm/weft-simple.asm")


unit_test()

