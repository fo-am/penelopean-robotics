(
 (forever
  (set! comp-delta-reset 1)
  (set! next-pattern turn-left1)
  (while (< comp-delta 90)
	 (set! led 1))
  (set! step-count-reset 1)
  (set! next-pattern walk-forward)
  (while (< step-count 4)
	 (set! led 0)))
 )
