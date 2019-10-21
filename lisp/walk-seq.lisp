(
 (set! servo-ms-per-step 200)
 (set! servo-1-smooth 1000)
 (set! servo-2-smooth 1000)
 (set! servo-3-smooth 1000)
 
 (forever
  (walk-turn)
  (while (< comp-angle 90)
    (set! led 0))
  (walk-turn)
  (while (< comp-angle 180)
    (set! led 0))
  (walk-turn)
  (while (< comp-angle 270)
    (set! led 0))
  (walk-turn)
  (while (> comp-angle 270)
    (set! led 0))
  )

 (defun (walk-turn)
   (set! step-count-reset 1)
   (set! next-pattern walk-forward)
   (while (< step-count 4)
     (set! led 0))
   (set! step-count-reset 1)
   (set! next-pattern turn-left))


     )
