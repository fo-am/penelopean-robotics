wait: ld   A                            ;; wait for start flag
      jpz  wait                         ;; if zero, loop
      ld   B                            ;; start walking
      st   REG_SERVO_NEXT_PATTERN_ID    ;; send to servo seq
      ld   B                            ;; compare current direction
      ldl  MOTION_SEQ_PATTERN_ID_FORWARD ;; to forward
      equ                               ;; are they the same?
      jpz  xx                           ;; we are going backward (or null at start)
      ldl  MOTION_SEQ_PATTERN_ID_BACKWARD ;; we are going forward so load backward
      st   B                              ;; store in direction
      jmp  stch                             ;; skip next part
xx:   ldl  MOTION_SEQ_PATTERN_ID_FORWARD  ;; set to forward again
      st   B
stch: ld   REG_SERVO_STEP_COUNT         ;; now check step counter     
      ld   5                            ;;  numsteps
      gte                               ;; num_steps >= stepcount 
      jpz  stch                         ;; not yet, step check again
      ldl  MOTION_SEQ_PATTERN_ID_STOPPED ;; stop walking
      st   REG_SERVO_NEXT_PATTERN_ID
      ldl  1                            ;; reset the step counter
      st   REG_SERVO_STEP_COUNT_RESET
      ldl  0
      st   A                            ;; clear the running flag
      jmp  wait                         ;; wait for running flag to be set
