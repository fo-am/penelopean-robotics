strt: ldl  10
      st   A
loop: decp A
      jpz  fls 
      jmp  loop
fls:  ld   LED
      not
      st   LED
      jmp  strt
