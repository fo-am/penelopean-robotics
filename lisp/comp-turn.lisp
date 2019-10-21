(
 (set! servo-2-bias 20)
 (set! servo-1-smooth 1000)
 (set! servo-2-smooth 1000)
 (set! servo-3-smooth 1000)
 (set! servo-ms-per-step 200)
 (defvar turning 1)
 
 (forever
  (walk-turn 4 90 0)
  (walk-turn 4 180 0)
  (walk-turn 4 270 0)
  (walk-turn 4 270 1))
 
   (defun (walk-turn steps angle less-than)      
     (set! servo-ms-per-step 200)
     (set! next-pattern walk-forward)
     (set! step-count-reset 1)
     (while (< step-count steps) 0)
     (set! led 0)
     (set! turning 1)
     (while turning
       ;; turn one step
       (set! servo-ms-per-step 100)
       (set! next-pattern turn-left)
       (set! step-count-reset 1)
       (while (< step-count 1) 0)
       ;; stop
       (set! next-pattern walk-stop)
       (set! step-count-reset 1)
       (while (< step-count 1) 0)
       ;; check angle
       (if less-than
	   (when (< comp-angle angle)
	     (set! led 1)
	     (set! turning 0))
	   (when (> comp-angle angle)
	     (set! led 1)
	     (set! turning 0)))))
	        

   
 )
