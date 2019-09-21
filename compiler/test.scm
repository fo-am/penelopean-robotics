(
 (forever
  (defvar i 99)
  (assert-eq i 99 1)
  (set! i 20)
  (assert-eq i 20 2)
  ;; basic maths
  (set! i (+ i 1))
  (assert-eq i 21 3)
  (set! i (- i 1))
  (assert-eq i 20 4)

  ;; if
  (if 0 (set! i 99) (set! i 30))
  (assert-eq i 30 5)
  (if 1 (set! i 99) (set! i 20))
  (assert-eq i 99 6)
  ;; when
  (when 0 (set! i 102))
  (assert-eq i 99 7)
  (when 1 (set! i 102))
  (assert-eq i 102 8)
  ;; comparisons
  (assert (> i 101) 9) 
  (assert (< i 103) 10) 
  ;; ;; while
  (while (< i 105)
	 (set! i (+ i 1)))
  (assert i 105 11)
  ;; more comparisons
  (assert (<= i 105) 12) 
  (assert (>= i 105) 13) 
  ;; signed numbers
  (set! i -203)
  (assert-eq i -203 14)
  ;; signed comparisons
  (assert (s< i -202) 15)
  (assert (s> i -2000) 16)
  (set! i (+ i -1))
  (assert (s<= i -204) 17)
  (assert (s>= i -204) 18)
  ;; bitwise
  (set! i #xffff)
  (set! i (and i #xffff))
  (assert-eq i #xffff 19)
  (set! i (or i #x0ff0))
  (assert-eq i #xffff 20)
  (set! i (bitwise-not i))
  (assert-eq i #x0000 21)
  (set! i #x0ff0)
  (set! i (or i #x0f0f))
  (assert-eq i #x0fff 22)

  ;; check return values
  (assert-eq (identity 99) 99 23)
  (assert-eq 10 (recursive 10 0) 24)

  ;; check arguments
  (assert-eq 3 (argscheck1 1 2 3) 25)
  (assert-eq 2 (argscheck2 1 2 3) 26)
  (assert-eq 5 (argscheck3 1 2 3) 27)
  (assert-eq 100 (subcall1 200 -100 888) 28)
  (assert-eq 10 (subcall3 -100 888) 29)
  
  (set! led 9999)
  )
 
 (defun (led-on state) (set! led state))

 (defun (identity x) x)
 
 (defun (recursive x y)
   (cond
    ((eq? x 0) y)
    (else (recursive (- x 1) (+ y 1)))))

 (defun (argscheck1 a b c) c)
 (defun (argscheck2 a b c) b)
 (defun (argscheck3 a b c) (+ b c))

 (defun (subcall1 a b c)
   (subcall2 c b a))

 (defun (subcall2 a b c)
   (+ c b))

 (defun (subcall3 a b)
   (set! a 10)
   (subcall1 1 2 b)
   a)

 
 (defun (assert-eq a b c)
   (when (not (eq? a b)) (error c)))

 (defun (assert e c)
   (when (not e) (error c)))
 
 (defun (error c)
   (set! led c)
   (forever))
 
 
 )       



