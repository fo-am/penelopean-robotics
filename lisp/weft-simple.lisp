(
(forever
 (while (not ready-semaphore)
   (set! led 1))
 (set! led 0)
 (set! step-count-reset 1)
 (set! next-pattern walk-forward)
 (while (< step-count 8))
 (set! next-pattern walk-stop)
 (set! ready-semaphore 0))
)
