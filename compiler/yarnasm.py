import array

code_start = 32

# change robot.c and yarnc.scm registers to match here
registers = {
    "PC": 0,
    "STACK": 1,
    "ROBOT_ID": 2,
    "LED": 3,
    "COMP_ANGLE": 4,
    "COMP_DELTA_RESET": 5,
    "COMP_DELTA": 6,
    "STEP_COUNT": 7,
    "STEP_COUNT_RESET": 8,
    "NEXT_PATTERN": 9,
    "A": 10,
    "B": 11,
    "C": 12,
    "D": 13,
    "SERVO_MS_PER_STEP": 14, # arg step not=step counter type of step
    "SERVO_1_AMP": 15,
    "SERVO_2_AMP": 16,
    "SERVO_3_AMP": 17,
    "SERVO_1_BIAS": 18,
    "SERVO_2_BIAS": 19,
    "SERVO_3_BIAS": 20,
    "SERVO_1_SMOOTH": 21,
    "SERVO_2_SMOOTH": 22,
    "SERVO_3_SMOOTH": 23,
    
    "NULL": 0,
    "ALL_STOP": 1,
    "WALK_FORWARD": 2,
    "WALK_BACKWARD": 3,
}

instructions = {
    "nop":  0,
    "jmp":  1,
    "jpz":  2,
    "jsr":  3, 
    "rts":  4,
    "ldl":  5,
    "ldl16":  6,
    "ld":   7,
    "ldi":  8,
    "ldsi": 9,
    "st":   10,
    "sti":  11,
    "sts":  12,
    "stsi": 13,
    "dup":  14,
    "drop": 15,
    "swap": 16,
    "equ":  17, 
    "lt":   18, 
    "gt":   19, 
    "lte":  20, 
    "gte":  21,
    "slt":  22,
    "sgt":  23,
    "slte": 24,
    "sgte": 25,
    "add":  26,
    "sub":  27,
    "addl": 28,
    "sub;": 29,
    "mul":  30,
    "div":  31,
    "mod":  32,
    "and":  33,
    "or":   34,
    "xor":  35,
    "not":  36,
    "notl": 37,
    "rr":   38,
    "rl":   39,
    "sin":  40,
    "cos":  41,
    "tan":  42
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
            #print("label: "+l[0][:-1],addr+code_start)
            l=l[1:] # strip out label

    def assemble_line(self,i,l):
        l = l.split()
        if len(l)<1: return []
        if l[0].endswith(":"):
            l=l[1:] # strip out label
            if len(l)==0:
                return []
        if not l[0] in self.instr:
            print("yarnasm: instruction "+l[0]+" not understood on line "+str(i))
            return []
        else:
            if len(l)>1:
                # deal with operand
                operand = l[1]
                if self.is_int(operand):
                    if int(operand)>=0 and int(operand)<1024:
                        return [(self.instr[l[0]]<<10)|int(operand)]
                    else: # add 16bit value
                        if self.instr[l[0]]!=instructions["ldl16"]:
                            print("yarnasm: trying to add 16bit data for op: "+str(self.instr[l[0]]))
                        return [(self.instr[l[0]]<<10),int(operand)]
                if operand in self.regs:
                    return [(self.instr[l[0]]<<10)|self.regs[operand]]
                if operand in self.labels.keys():
                    return [(self.instr[l[0]]<<10)|self.labels[operand]]
                else:
                    print(l)
                    print("yarnasm: i do not understand "+operand+" on line "+str(i))
                    return []

            return [self.instr[l[0]]<<10]

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
        #print(ls)
        if len(ls)==0: return 0
        # strip out label
        if ls[0].endswith(":"):
            ls=ls[1:]
        
        if len(ls)>0:
            if ls[0]=="ldl16":
                #print(2)
                return 2
            else:
                #print(1)
                return 1
        else:
            #print(0)
            return 0

    def assemble(self,s):
        print("yarnasm 2.0")
        s = s.split("\n")
        print("yarnasm: "+str(len(s))+" lines in asm")
        s = self.remove_comments(s)
        print("yarnasm: "+str(len(s))+" with comments removed")
        s = self.optimise(s)
        print("yarnasm: "+str(len(s))+" optimised instructions + labels")
        addr=0
        for i,line in enumerate(s):
            sl = line.strip()
            self.collect_label(addr,sl)
            if len(sl)>0: 
                addr+=self.line_size(sl)
        print("yarnasm: "+str(len(self.labels))+ " labels")
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
