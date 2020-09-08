(
 (forever
  (set! step-count-reset 1)
  (while (< step-count 2)
      	 (set! next-pattern walk-forward))
  (set! step-count-reset 1)
  (while (< step-count 2)
      	 (set! next-pattern walk-stop))
  (set! step-count-reset 1)
  (while (< step-count 2)
      	 (set! next-pattern walk-backward))
  )
 )
