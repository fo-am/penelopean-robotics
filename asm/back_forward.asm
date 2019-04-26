      ldl  1                  ;; reset the step counter
      st   STEP_COUNT_RESET
      ldl  FORWARD            ;; to forward
      st   18
strt: ld   18                 ;; start walking
      st   NEXT_PATTERN_ID    ;; send to servo seq
      ld   18                 ;; compare current direction
      ldl  FORWARD            ;; to forward
      equ                     ;; are they the same?
      jpz  xx                 ;; we are going backward (or null at start)
      ldl  BACKWARD           ;; we are going forward so load backward
      st   18                 ;; store in direction
      jmp  stch               ;; skip next part
xx:   ldl  FORWARD            ;; set to forward again
      st   18
stch: ld   STEP_COUNT         ;; now check step counter      
      ld   5                  ;; 5=num_steps 
      gte                     ;; num_steps >= stepcount 
      jpz  stch               ;; not yet, step check again
      ldl  1                  ;; reset the step counter
      st   STEP_COUNT_RESET
      jmp  strt               ;; wait for running flag to be set
