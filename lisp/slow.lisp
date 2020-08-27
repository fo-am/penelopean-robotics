(
 ;;(set! servo-2-bias 20)
 (set! servo-1-smooth 1000)
 (set! servo-2-smooth 1000)
 (set! servo-3-smooth 1000)
 (set! servo-ms-per-step 600)
 
 
 (forever
     (set! next-pattern walk-backward)
     (set! step-count-reset 1)
     (while (< step-count 8) 0)

     (set! next-pattern walk-forward)
     (set! step-count-reset 1)
     (while (< step-count 8) 0))

    
   )
