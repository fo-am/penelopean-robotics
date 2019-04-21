	;; stuf....
strt: ld   18                           ;; start walking
      st   REG_SERVO_NEXT_PATTERN_ID    ;; send to servo seq
      ld   18                           ;; compare current direction
      ldl  MOTION_SEQ_PATTERN_ID_FORWARD ;; to forward
      equ                               ;; are they the same?
      jpz  xx                           ;; we are going backward (or null at start)
      ldl  MOTION_SEQ_PATTERN_ID_BACKWARD ;; we are going forward so load backward
      st   18                            ;; store in direction
      jmp  stch                             ;; skip next part
xx:   ldl  MOTION_SEQ_PATTERN_ID_FORWARD  ;; set to forward again
      st   18
stch: ld   REG_SERVO_STEP_COUNT         ;; now check step counter      
      ld   5                            ;; 5=num_steps 
      gte                               ;; num_steps >= stepcount 
      jpz  stch                         ;; not yet, step check again
      ldl  1                            ;; reset the step counter
      st   REG_SERVO_STEP_COUNT_RESET
      jmp  strt                         ;; wait for running flag to be set
