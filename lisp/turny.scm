(
 (forever
  (cond
   ((< comp-angle 90)
    (set! led 1)
    (set! next-pattern walk-forward))
   ((and (> comp-angle 180) (< comp-angle 270))
    (set! led 1)
    (set! next-pattern walk-backward))
   (else
    (set! led 0)
    (set! next-pattern walk-stop))))
 )
