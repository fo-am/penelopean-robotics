import sys
sys.path.append("../pi")

import radio
import robot
import yarnasm
import time
import threading
import sys 
import math
import subprocess
import os

class transmit:
    def __init__(self):
        self.radio = radio.radio([0xa7, 0xa7, 0xa7, 0xa7, 0xaa])
        self.swarm = [robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x01]),
                      robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x02]),
                      robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x03]),
                      robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x04]),
                      robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x05]),
                      robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x06]),
                      robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x07]),
                      robot.robot([0xa7, 0xa7, 0xa7, 0xa7, 0x08])
        ]
    
        self.compiler = yarnasm.compiler()
        self.lisp_fifo_name = "../fifo/lisp_fifo"
        self.asm_fifo_name = "../fifo/asm_fifo"
        os.remove(self.lisp_fifo_name)
        os.remove(self.asm_fifo_name)
        os.mkfifo(self.lisp_fifo_name)
        os.mkfifo(self.asm_fifo_name)
        # start compiler
        subprocess.Popen(['racket', '../compiler/yarnc.scm'])

        
    def program(self,robot_id,lisp_file):
        fifo=open(self.lisp_fifo_name,"w")
        fifo.write(lisp_file)
        fifo.close()

        asm = ""
        fifo=open(self.asm_fifo_name,"r")
        for line in fifo.readlines():
            asm+=line
        fifo.close()

        print(asm)
        
        if self.swarm[robot_id].send_asm(asm,self.compiler,self.radio) == False:
            return False
        return True
    


    
