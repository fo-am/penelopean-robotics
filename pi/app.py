import curses
import time
import swarm

from curses import wrapper
from curses.textpad import Textbox, rectangle

class win:
    def __init__(self,stdscr):
        self.swarm = swarm.swarm()
        self.stdscr = stdscr
        self.stdscr.clear()
        curses.noecho()
        curses.cbreak() 
        curses.curs_set(0)
        self.stdscr.keypad(True)
        self.stdscr.nodelay(True)
        curses.init_pair(1, curses.COLOR_YELLOW, curses.COLOR_BLACK)
        curses.init_pair(2, curses.COLOR_WHITE, curses.COLOR_BLACK)
        curses.init_pair(3, curses.COLOR_BLACK, curses.COLOR_WHITE)
 
        self.codelines=[]
        self.load_file("../asm/weft.asm")

        self.line_offset=0
        self.regs_x = 60
        self.regs_y = 0
        self.regs = {"robot": 0,
                     "state": "disconnected",
                     "ping": 0,
                     "pc": 2,
                     "stack": 0,
                     "led": 0,
                     "comp_angle": 360,
                     "comp_dr": 0,
                     "comp_d": 0,
                     "step_count": 0,
                     "step_reset": 0}

        self.reg_order = ["robot",
                          "state",
                          "ping",
                          "pc",
                          "stack",
                          "led",
                          "comp_angle",
                          "comp_dr",
                          "comp_d",
                          "step_count",
                          "step_reset"]

        self.reg_win = curses.newwin(len(self.regs)-1,19,1,61)
        self.render()

    def render(self):
        self.stdscr.clear()

        for l in range(0,20):
            pc = self.line_offset+l
            col = 2
            if self.regs["pc"]==pc: col=3
            if pc>0 and pc<len(self.codelines):
                self.stdscr.addstr(l+1,1, self.codelines[pc], curses.color_pair(col))

        for i,r in enumerate(self.reg_order):
            sp = " "
            for j in range(0,11-len(r)): sp+=" "
            self.stdscr.addstr(i+1,61, 
                               (r+sp+str(self.regs[r]))[0:18],
                               curses.color_pair(1))
        rectangle(self.stdscr,0,60,len(self.regs)+1,79)
        rectangle(self.stdscr,0,0,20,79)

        self.reg_win.clear()
        self.reg_win.refresh()
        self.stdscr.addstr(0,0, "Penelopian Swarm Robot Lab 1.0", curses.color_pair(col))

        self.stdscr.refresh()

    def load_file(self,fn):
        with open(fn, 'r') as f:
            self.codelines=f.readlines();
            print(self.codelines)

    def update(self):
        self.swarm.update()        

        self.swarm.update_regs(self.regs["robot"],self.regs)

        self.stdscr.refresh()
        cmd=self.stdscr.getch()
        if  cmd == curses.KEY_DOWN:
            self.line_offset+=1
        if  cmd == curses.KEY_UP:
            if self.line_offset>0: self.line_offset-=1
        if  cmd == curses.KEY_RIGHT:
            self.regs["robot"]+=1
            if self.regs["robot"]>7: self.regs["robot"]=0
        if  cmd == curses.KEY_LEFT:
            self.regs["robot"]-=1
            if self.regs["robot"]<0: self.regs["robot"]=7
        self.render()
        time.sleep(0.1)


    def exit(self):
        curses.nocbreak()
        self.stdscr.keypad(False)
        curses.echo()
        curses.endwin()


def main(stdscr):
    w = win(stdscr)
    while True:
        w.update()

wrapper(main)
