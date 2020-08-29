(
 (forever
  (set! usr-d vcc)
  (set! led 0)
  (set! led 0)
  (set! usr-b (+ usr-b 1))

  (set! usr-c temperature)
  
  ;; device 0x76 addr 0xd0 value 0x58
  (set! i2c-device (<< #x76 1))
  (set! i2c-addr #xd0)
  (set! i2c-ctrl 1)
  (set! usr-a i2c-data)

  (set! sleep 1)
  )
)
