import array

code_start = 32

# change robot.c registers to match here
registers = {
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

instructions = {
    "nop":  0,
    "jmp":  1,
    "jpr":  2,
    "jpz":  3,
    "jprz": 4,
    "jsr":  5, 
    "rts":  6,
    "ldl":  7,
    "ld":   8,
    "ldi":  9,
    "ldsi": 10,
    "st":   11,
    "sti":  12,
    "sts":  13,
    "stsi": 14,
    "dup":  15,
    "drop": 16,
    "swap": 17,
    "equ":  18, 
    "lt":   19, 
    "gt":   20, 
    "lte":  21, 
    "gte":  22,
    "slt":  23,
    "sgt":  24,
    "slte": 25,
    "sgte": 26,
    "add":  27,
    "sub":  28,
    "addl": 29,
    "sub;": 30,
    "inc":  31,
    "dec":  32,
    "and":  33,
    "or":   34,
    "xor":  35,
    "not":  36,
    "notl": 37,
    "rr":   38,
    "rl":   39,
    "sin":  40,
    "cos":  41,
    "tan":  42,
    "rnd":  43,
}


class compiler:
    def __init__(self):
        self.labels={}
        self.instr = instructions
        self.regs = registers


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
            self.labels[l[0][:-1]]=addr+code_start
            #print(l[0][:-1],addr+code_start)
            l=l[1:] # strip out label

    def assemble_line(self,i,l):
        l = l.split()
        if len(l)<1: return []
        if l[0].endswith(":"):
            l=l[1:] # strip out label
            if len(l)==0:
                return []
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

    def remove_comments(self,s):
        ret = []
        for l in s:
            ls = l.split()
            if len(ls)>0:
                pos = 0
                word = ls[pos]
                line= ""
                while word!=";;" and pos<len(ls)-1:
                    line+=word+" "
                    pos+=1
                    word=ls[pos]
                if word!=";;": line+=word+" "
                if line!="": ret.append(line)
        return ret

    def optimise(self,s):
        start=len(s)
        ret=[s[0]]
        for p in range(1,len(s)):
            if (s[p].startswith("ld") and s[p+1].startswith("drop 1")) or\
               (s[p-1].startswith("ld") and s[p].startswith("drop 1")):
                pass
            else:
                ret.append(s[p])
        print("yarnasm: removed "+str(start-len(ret))+" consecutive load/drop instructions")
        return ret
    
    def line_size(self,l):
        ls=l.split()
        if len(ls)==0: return 0
        # strip out label
        if ls[0].endswith(":"):
            ls=ls[1:]
        return len(ls)

    def assemble(self,s):
        s = s.split("\n")
        print("yarnasm: "+str(len(s))+" lines in asm")
        s = self.remove_comments(s)
        print("yarnasm: "+str(len(s))+" with comments removed")
        s = self.optimise(s)
        print("yarnasm: "+str(len(s))+" optimised instructions")
        
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
        #print code
        print("yarnasm: "+str(len(code))+" double bytes total")
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


    def test(self):
        s=["st 19","ldl 0","drop 1","ldl 2","jsr assert"]
        s=self.optimise(s)
        print(s)
        
if __name__ == "__main__":
    import sys
    #compiler().test()
    f=open(sys.argv[1]+".bin","wb")
    f.write(compiler().assemble_file(sys.argv[1]))
    f.close()