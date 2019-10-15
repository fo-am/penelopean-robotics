(
 (forever
  (if (< comp-angle 180)
      (if (< comp-angle 90)
	  (set! led 0)
	  (set! led 1))
      (if (< comp-angle 270)
	  (set! led 0)
	  (set! led 1))))
 )
