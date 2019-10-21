(
 (set! servo-ms-per-step 200)
 (set! servo-1-smooth 1000)
 (set! servo-2-smooth 1000)
 (set! servo-3-smooth 1000)
 
 (forever
  (walk-turn)
  )

 (defun (walk-turn)
   (set! next-pattern turn-left))
 
 
 )
