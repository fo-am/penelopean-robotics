wait: ld   A             ;; wait for start flag
      jpz  wait          ;; if zero, loop
      ldl  WALK_FORWARD  ;; start walking
      st   NEXT_PATTERN  ;; send to servo seq
      ld   B             ;; compare current direction
      ldl  WALK_FORWARD  ;; to forward
      equ                ;; are they the same?
      jpz  xx            ;; we are going backward 
      ldl  WALK_BACKWARD ;; we are going forward 
      st   B             ;; store in direction
      jmp  stch          ;; skip next part
xx:   ldl  WALK_FORWARD  ;; set to forward again
      st   B
stch: ld   STEP_COUNT    ;; now check step counter     
      ld   5             ;; numsteps
      gte                ;; num_steps >= stepcount 
      jpz  stch          ;; not yet, step check again
      ldl  WALK_STOP	 ;; stop walking
      st   NEXT_PATTERN
      ldl  1             ;; reset the step counter
      st   STEP_COUNT_RESET
      ldl  0
      st   A             ;; clear the running flag
      jmp  wait          ;; wait for running flag 
