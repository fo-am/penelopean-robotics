(
 (defconst middle 64)
 (defconst threshold 5)
 (defconst num-lost-retries 5)
 
 ;; set up servo parameters for this robot
 (set! servo-2-bias -10)
 (set! servo-ms-per-step 100)
 (set! servo-1-amp 500)
 (set! servo-2-amp 1000)
 (set! servo-3-amp 500)

 ;; init the line follower
 (set-lf linefollower-mode linefollower-white-line)
 ;; filter out features outside of this range 
 (set-lf linefollower-min-width 20)
 (set-lf linefollower-max-width 40)

 ;; calibrate the camera exposure
 (set-lf linefollower-do-autoexposure 1)
 ;; wait a bit...
 (while (< usr-a 50)
   (set! led 1)
   (set! led 0)
   (set! usr-a (+ usr-a 1)))
 (set-lf linefollower-do-autoexposure 0)
   
 (defvar lost-tries 0)
 (defvar midpos 0)
 (defvar features-detected 0)
 (defvar state 0)

 (defconst state-line-middle 0) 
 (defconst state-line-left 1) 
 (defconst state-line-right 2) 
 (defconst state-line-lost 3) 
 
 (forever
  ;; load midpos from camera and debug info
  (set! midpos (get-lf linefollower-line-midpos))
  (set! features-detected (get-lf linefollower-num-features))
  ;; save to user registers so visible in pi debugger
  (set! usr-a midpos)
  (set! usr-b features-detected)
  ;; width of first feature
  (set! usr-c (get-lf 34))
  
  (set! led 0)

  ;; a little state machine
  (cond
   ((eq? features-detected 1) ;; have we found any features? 
    (cond
     ((< midpos (- middle threshold)) ;; right
      (set! lost-tries 0)
      (set! state state-line-right) 
      (set! led 1) ;; flash twice
      (set! led 0)
      (set! led 1)
      (set! led 0))
     ((> midpos (+ middle threshold)) ;; left
      (set! lost-tries 0)
      (set! state state-line-left) 
      (set! led 1) ;; flash once
      (set! led 0))
     (else ;; straight ahead!
      (set! lost-tries 0)
      (set! state state-line-middle) 
      (set! led 1) ;; keep led on
      )))
   (else ;; no line found
    ;; keep trying the same direction for a bit longer
    (set! lost-tries (+ lost-tries 1))
    (when (> lost-tries num-lost-retries)
      ;; nope, now try reversing
      (set! state state-line-lost) 
      (set! led 0)))) ;; turn led off

  ;; dispatch based on state
  (cond
   ((eq? state state-line-middle) ;; straight ahead    
    (set! servo-2-bias -10)
    (move walk-backward 1)) ;; we are set up in reverse formation!
   ((eq? state state-line-left) ;; turn to the left
    (set! servo-2-bias -30)
    (move walk-backward 2))
   ((eq? state state-line-right) ;; turn to the right
    (set! servo-2-bias 10)
    (move walk-backward 1))   
   (else ;; line lost, reverse
    (set! servo-2-bias -10)
    (reverse walk-forward 1)))) 

 ;;-------------------------------------------------------
 
 (defun (move direction steps)
     (set! next-pattern direction)
     (set! step-count-reset 1)
     (while (< step-count steps) 0)
     (set! servo-2-bias -10)
     (set! next-pattern walk-stop))

 ;; access the line follower over i2c
 (defun (set-lf addr value)
     (set! i2c-device (<< linefollower-i2c 1))
     (set! i2c-addr addr)
     (set! i2c-data value)
     (set! i2c-ctrl i2c-write))

 (defun (get-lf addr)
     (set! i2c-device (<< linefollower-i2c 1))
     (set! i2c-addr addr)
     (set! i2c-ctrl i2c-read)
     i2c-data)
 
 
)
