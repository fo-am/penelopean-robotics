strt: ldl  10
      st   A
loop: decp A
      jprz 2 
      jmp loop
      ld REG_LED
      not
      st REG_LED
      jmp strt
