import pygame, sys
from pygame.locals import *
import serial

ser = serial.Serial("/dev/ttyUSB0",115200)

def main():
    pygame.init()

    DISPLAY=pygame.display.set_mode((500,400),0,32)

    WHITE=(255,255,255)
    BLUE=(0,0,255)

    DISPLAY.fill(WHITE)


    while True:
        for event in pygame.event.get():
            if event.type==QUIT:
                pygame.quit()
                sys.exit()

        for i in range(0,128):
            b = ser.readline()         # read a byte string
            string_n = b.decode()  # decode byte string into Unicode    
            string = string_n.rstrip() # remove \n and \r    
            c=0
            try:
                flt = float(string)        # convert string to float
                #print(flt)
                c = int((flt/1024) * 255)
            except:
                pass
            pygame.draw.rect(DISPLAY,(c,c,c),(i*4,0,4,400))


        pygame.display.update()

main()
