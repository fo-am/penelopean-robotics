
class compiler:
    def __init__(self):
        self.labels={}
        self.code_start=32
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

        self.regs = {
            "PC_MIRROR": 0,
            "STACK_MIRROR": 1,
            "LED": 2,
            "COMP_ANGLE": 3,
            "COMP_DELTA_RESET": 4,
            "COMP_DELTA": 5,
            "STEP_COUNT": 6,
            "STEP_COUNT_RESET": 7,
            "NEXT_PATTERN": 8,
            "SERVO_SPEED": 9,
            "SERVO_1_AMP": 10,
            "SERVO_2_AMP": 11,
            "SERVO_3_AMP": 12,
            "SERVO_1_BIAS": 13,
            "SERVO_2_BIAS": 14,
            "SERVO_3_BIAS": 15,
            "SERVO_1_SMOOTH": 16,
            "SERVO_2_SMOOTH": 17,
            "SERVO_3_SMOOTH": 18,

            "NULL": 0,
            "ALL_STOP": 1,
            "WALK_FORWARD": 2,
            "WALK_BACKWARD": 3,

            "A": 19,
            "B": 20,
            "C": 21,
            "D": 22
            
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




    def assemble(self,s):
        s=s.split("\n")
        addr=0
        for i,line in enumerate(s):
            sl = line.strip()
            self.collect_label(addr,sl)
            if len(sl)>0 and sl[0]!=";":
                addr+=1
        code = []
        for i,line in enumerate(s):
            sl=line.strip()
            if len(sl)>0 and sl[0]!=";":
                code+=self.assemble_line(i,sl)
        print code
        return code

    def assemble_file(self,fn):
        with open(fn, 'r') as f:
            return self.assemble(f.read())

    def assemble_to_bin(self,fn):
        d = self.assemble_file(fn)




def unit_test():
    c = compiler()

    assert(c.assemble("ld 0 \n\
                       st 100")==[6,0,8,100])

    assert(c.assemble("ld LED \n\
                       st 100")==[6,2,8,100])

    assert(c.assemble("label: ld LED \n\
                       jmp label")==[6,2,1,32])
    
    c.assemble_file("../asm/led_flash.asm")
    c.assemble_file("../asm/back_forward.asm")
    c.assemble_file("../asm/warp.asm")
    c.assemble_file("../asm/slow_led.asm")

unit_test()

