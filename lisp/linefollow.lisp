
(
 (set! servo-ms-per-step 200)
 (set! next-pattern walk-forward)
 (set! step-count-reset 1)
 (set! i2c-device (<< linefollower-i2c 1))
 (while (< step-count 1) 0)

 (forever 
  (set! i2c-addr linefollower-line-midpos)
  (set! i2c-ctrl i2c-read)
  (cond
   ((> i2c-data (- 127 32))
    (set! next-pattern turn-left))
   ((< i2c-data 32)
    (set! next-pattern turn-right))
   (else
    (set! next-pattern walk-forward)))
  (set! step-count-reset 1)
  (while (< step-count 1) 0))
 )

