#lang racket
;; co2 Copyright (C) 2016 Dave Griffiths
;;
;; This program is free software: you can redistribute it and/or modify
;; it under the terms of the GNU Affero General Public License as
;; published by the Free Software Foundation, either version 3 of the
;; License, or (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU Affero General Public License for more details.
;;
;; You should have received a copy of the GNU Affero General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

(require racket/cmdline)

(define reg-table
  '((pc                     "0000")
    (stack                  "0001")
    (robot-id               "0002")
    (led                    "0003")
    (comp-angle             "0004")
    (comp-delta-reset       "0005")
    (comp-delta             "0006")
    (step-count             "0007")
    (step-count-reset       "0008")
    (next-pattern           "0009")
    (ready-semaphore        "0010")
    (usr-a                  "0010")
    (usr-a                  "0010")
    (usr-b                  "0011")
    (usr-c                  "0012")
    (usr-d                  "0013")
    
    ;; robot regs
    (servo-ms-per-step      "0014")
    (servo-1-amp            "0015")
    (servo-2-amp            "0016")
    (servo-3-amp            "0017")
    (servo-1-bias           "0018")
    (servo-2-bias           "0019")
    (servo-3-bias           "0020")
    (servo-1-smooth         "0021")
    (servo-2-smooth         "0022")
    (servo-3-smooth         "0023")

    (temperature            "0024")
    (sleep                  "0025")
    (i2c-device             "0026")
    (i2c-addr               "0027")
    (i2c-data               "0028")
    (i2c-ctrl               "0029")
    (vcc                    "0030")

    (accel-x                "0031")
    (accel-y                "0032")
    (accel-z                "0033")
    (gyro-x                 "0034")
    (gyro-y                 "0035")
    (gyro-z                 "0036")
    (comp-x                 "0037")
    (comp-y                 "0038")
    (comp-z                 "0039")

    
    (reg-usr-start              "0056") 
    (reg-usr-end                "0064")
    (reg-code-start             "0064")
    
;;    (swarm-pulse "0009") ??
    
    ))

;; internal compiler register on zero page
(define working-reg "53")
(define stack-frame "54")
(define var-start 55)

;; constants lookup
(define constants 
  '((walk-null "0")
    (walk-stop "1")
    (walk-forward "2")
    (walk-backward "3")
    (turn-left "4")
    (turn-right "5")
    (turn-left2 "6")
    (turn-right2 "7")
    (walk-silly "8")

    (i2c-read "1")
    (i2c-write "2")

    ;; see also linefollower.c
    (linefollower-i2c "10")
    (linefollower-id "0")
    (linefollower-i2c "1") 
    (linefollower-alive "2")
    (linefollower-exposure-lo "3")
    (linefollower-exposure-hi "4")
    (linefollower-courseness "5")
    (linefollower-min-width "6")
    (linefollower-max-width "7")
    (linefollower-average "8")
    (linefollower-line-midpos "9")
    (linefollower-barcode-bits "10")
    (linefollower-barcode-skip "11")
    (linefollower-barcode-data "12")
    (linefollower-line-error "13")
    (linefollower-barcode-error "14")
    (linefollower-do-autoexposure "15")
    (linefollower-num-features "31")
    (linefollower-features-start "322")
    (linefollower-raw-image-start "127")

    ))



(define (reg-table-lookup x)
  (let ((lu (assoc x reg-table)))
    (if lu (cadr lu) #f)))

;;-------------------------------------------------------------
;; a label generator

(define label-id 99)

(define (generate-label name)
  (set! label-id (+ label-id 1))
  (string-append name "_" (number->string label-id)))

;;----------------------------------------------------------------
;; variables are just an address lookup table to var-start

(define variables '())

(define (make-variable! name)
  (when (not (memq name variables))
        (set! variables (append variables (list name)))))

(define (variable-lookup name)
  (define (_ l c)
    (cond
     ((null? l) (display "yarnc ERROR: cant find variable ")(display name)(newline) #f)
     ((equal? name (car l)) (number->string (+ c var-start)))
     (else (_ (cdr l) (+ c 1)))))
  (_ variables 0))

;;----------------------------------------------------------------

(define (make-constant! name value)
  (when (not (string? value))
	(display "yarnc ERROR: constant ")(display name)(display " is not a string: ")
	(display value)(newline))
  (set! constants (cons (list name value) constants)))

(define (constant-lookup name)
  (let ((t (assoc name constants)))
    (if t (cadr t) #f)))

;;------------------------------------------------------------------
;; store function args here - this is not a proper call stack
;; (considered far too bloaty!), so these get clobbered with function
;; calls within function calls - beware...

(define fnargs-start #x1)

(define (fnarg index)
  (string-append (number->string (+ fnargs-start index))))

(define fnarg-mapping '())

(define (set-fnarg-mapping! args)
  (set! fnarg-mapping
	(foldl
	 (lambda (arg r)
	   ;; map the argument name to the address
	   (cons (list arg (fnarg (- (length r) 1))) r))
	 '()
	 args)))

(define (clear-fnarg-mapping!)
  (set! fnarg-mapping '()))

(define (fnarg-lookup name)
  (let ((t (assoc name fnarg-mapping)))
    (if t (cadr t) #f)))

;; -----------------------

(define (is-fnarg? name)
  (let ((t (assoc name fnarg-mapping)))
    (if t #t #f)))

(define (emit-load-fnarg name)
  (append
   (emit "ld" stack-frame "\t ;; loading fnarg " (symbol->string name))
   (emit "addl" (fnarg-lookup name))
   (emit "ldsi"))) ;; load from top of stack indirect
  
;;----------------------------------------------------------------

;; lookup a symbol everywhere, in order...
(define (lookup name)
  ;; check registers first
  (let ((reg (reg-table-lookup name)))
    (if reg reg
        ;; then constants
        (let ((const (constant-lookup name)))
          (if const const
	      ;; finally variables
	      (variable-lookup name))))))

;;----------------------------------------------------------------

(define (dash->underscore s)
  (foldl
   (lambda (c r)
     (if (eq? c #\-)
         (string-append r "_")
         (string-append r (string c))))
   ""
   (string->list s)))

;;---------------------------------------------------------------

(define (immediate-value x)
  (if (number? x)
      (number->string x)
      (let ((lu (lookup x)))	
        (if lu
	    (string-append lu "\t\t;; " (symbol->string x))
	    (symbol->string x)))))

;; is this an immediate value
(define (immediate? x)
  (or (number? x) (symbol? x)))

;; is this a primitive call?
(define (primcall? x)
  (and (list? x) (not (null? x)) (symbol? (car x))))

;--------------------------------------------------------------
;; code generation
;;
;; general rules:
;; * don't use registers across emit-expr as they can be clobbered
;; * use the stack (pha/pla) to store data in this case
;; * currently using internal "working-reg" as a 4th register on zero page
;;   for arithmetic stuff
;; * don't use shorthand branch labels ("-") across emit-expr either
;; * use (generate-label) in this case to use a unique one
;; * working register and returns all stored in a
;; * x/y are used for local optimisation
;; * results of the relevant subexpression should be left in a register as
;;   implicit return (e.g. loops, if, when etc)

(define (emit . args)
  (list
   (foldl
    (lambda (arg r)
      (if (equal? r "")
          (string-append r arg)
          (string-append r " " arg)))
    "\t"
    args)))

;; append a bunch of expressions
(define (emit-expr-list l)
  (cond
    ((null? l) '())
    (else     
     (append
      (emit-expr (car l))
      (if (null? (cdr l)) '()
	  (append
	   ;; insert drops to ignore returned data
	   ;; from expressions in the list we're not
	   ;; using (must have side effects)
	   ;; - keep last push
	   (emit "drop" "1")
	   (emit-expr-list (cdr l))))))))

(define (emit-expr-list-no-value l)
  (cond
    ((null? l) '())
    (else     
     (append
      (emit-expr (car l))
      (emit "drop" "1")
      (emit-expr-list-no-value (cdr l))))))

(define (emit-asm x)
  (let ((r
         (foldl
          (lambda (str r)
            (if (equal? r "")
                str
                (string-append r "\n" str)))
          ""
          (cdr x))))
    (list r)))

(define (emit-label label)
  (list (string-append label ":")))

(define (emit-load-variable x)
  (if (is-fnarg? x)
      (emit-load-fnarg x)
      (emit "ld" (immediate-value x))))

(define (emit-load-immediate x)
  (cond
   ((number? x)
    (if (or (< x 0) (> x 1023))
	(emit "ldl16" (number->string x))
	(emit "ldl" (number->string x))))
   ((and (symbol? x) (constant-lookup x))
    (emit "ldl" (constant-lookup x) (string-append "\t;; const " (symbol->string x))))
   ((symbol? x) (emit-load-variable x))
   (else
    (display "yarnc ERROR: unknown immediate: ")
    (display x)
    (newline))))

(define (emit-defvar x)
  (make-variable! (cadr x))
  (append
   (emit-expr (caddr x))
   (emit "st" (immediate-value (cadr x)))
   (emit "ldl" "0")))

;; arguments are mapped to arg-n...
(define (emit-defun x)
  (set-fnarg-mapping! (cdr (cadr x)))
  (let ((r (append
            (emit-label (dash->underscore
			 (symbol->string (car (cadr x)))))
            (emit-expr-list (cddr x))
	    ;; need to always push return value
	    ;; and swap em here to get the pc return value
	    ;; back to the top
	    (emit "swap")
            (emit "rts"))))
    (clear-fnarg-mapping!) 
    r)) 

;; function stack frame looks like:
;; 
;; previous stack location
;; arg n
;; ...
;; arg 1
;; return pc

(define (emit-fncall x)
  (append
   (emit "ld" stack-frame "\t\t;; fncall start") ;; store previous stack location
   ;; push the arguments on the stack
   (foldl
    (lambda (arg r)
      (append r (emit-expr arg)))
    '()
    (reverse (cdr x))) ;; in reverse order
   (emit "sts" stack-frame) ;; to find arguments later
   ;; call the function
   (emit "jsr" (dash->underscore (symbol->string (car x))))
   (emit "st" working-reg) ;; temp store return value
   ;; remove arguments from the stack
   (emit "drop" (number->string (length (cdr x))))
   (emit "st" stack-frame) ;; reinstate previous stack frame
   (emit "ld" working-reg "\t\t;; fncall end") ;; load return value
   ))

(define (emit-set! x)
  (if (is-fnarg? (cadr x))
      (append
       (emit-expr (caddr x))
       (emit "ld" stack-frame)
       (emit "addl" (fnarg-lookup (cadr x)))
       (emit "stsi")
       (emit "ldl" "0"))
      (append
       (emit-expr (caddr x))
       (emit "st" (immediate-value (cadr x)))
       (emit "ldl" "0"))))

;; (define (emit-write! x)
;;   (append
;;    (emit-expr (list-ref x 3))
;;    (emit "ld" (immediate-value (list-ref x 2)))
;;    (emit "sta" (immediate-value (list-ref x 1)) ",y")))

;; (define (emit-poke! x)
;;   (append
;;    (emit-expr (list-ref x 2)) ;; value
;;    (emit "st" (immediate-value (list-ref x 1)))))

;; (define (emit-peek x)
;;   (append
;;    (emit "ld" (immediate-value (list-ref x 1)))))


(define (emit-forever x)
  (let ((label-start (generate-label "forever_start")))
    (append
     (emit-label label-start)
     (emit-expr-list-no-value (cdr x))
     (emit "jmp" label-start)))) 

;; (loop var from to expr)
;; todo: fix branch limit
;; (define (emit-loop x)
;;   (let ((label-start (generate-label "loop_start"))
;; 	(label-end (generate-label "loop_end")))
;;     (append
;;      (emit-expr (list-ref x 2))
;;      (emit "sta" (immediate-value (list-ref x 1)))
;;      (emit-label label-start)
;;      (emit-expr-list (cddddr x))
;;      (emit "sta" working-reg) ;; store return in case we need it
;;      (emit "inc" (immediate-value (list-ref x 1)))
;;      (emit-expr (list-ref x 3))
;;      (emit "cmp" (immediate-value (list-ref x 1)))
;;      (emit "bcs" label-start)
;;      (emit "lda" working-reg)))) ;; retrieve return

;; ;; (if pred then else)
(define (emit-if x)
  (let ((false-label (generate-label "if_false"))
        (end-label (generate-label "if_end")))
    (append
     (emit-expr (list-ref x 1))
     (emit "jpz" false-label)
     (emit-expr (list-ref x 2)) ;; true block
     (emit "jmp" end-label)
     (emit-label false-label)
     (emit-expr (list-ref x 3)) ;; false block
     (emit-label end-label))))

;; ;; (when pred then)
(define (emit-when x)
  (let ((end-label (generate-label "when_end"))
	(false-label (generate-label "when_false")))
    (append
     (emit-expr (list-ref x 1))
     (emit "jpz" false-label)
     (emit-expr-list (cddr x)) ;; true block
     (emit "jmp" end-label)
     (emit-label false-label)
     (emit "ldl" "0")
     (emit-label end-label))))

;; (while pred then)
(define (emit-while x)
  (let ((loop-label (generate-label "while_loop"))
	(pred-label (generate-label "while_loop_pred")))
    (append
     (emit "jmp" pred-label) ;; check first
     (emit-label loop-label)
     (emit-expr-list (cddr x)) ;; loop block
     (emit "drop" "1")         ;; discard the last expression value
     (emit-label pred-label)
     (emit-expr (list-ref x 1)) ;; predicate
     (emit "notl")
     (emit "jpz" loop-label)
     (emit "ldl" "0"))))
  

(define (emit-left-shift x)
  (append
   (emit-expr (list-ref x 1))
   (emit "rl" (immediate-value (list-ref x 2)))))

(define (emit-right-shift x)
  (append
   (emit-expr (list-ref x 1))
   (emit "rr" (immediate-value (list-ref x 2)))))

;; (define (emit-mul x)
;;   (let ((label (generate-label "mul")))
;;     (append
;;      (emit-expr (cadr x))
;;      (emit-expr (caddr x))
;;      (emit "st" working-reg)
;;      (emit "dec" working-reg)
;;      (emit-label label)
;;      (emit "add" working-reg)
;;      (emit "dex")
;;      (emit "bne" label))))


(define (unary-procedure proc x)
  (append
   (emit-expr (cadr x))
   (emit proc)))

(define (binary-procedure proc x)
  (append
   (emit-expr (caddr x))
   (emit-expr (cadr x))
   (emit proc)))

(define (emit-procedure x)
  (cond
   ((eq? (car x) 'init)
    (append
     (emit "")
     ))
   ((eq? (car x) 'asm) (emit-asm x))
   ((eq? (car x) 'byte) (list (string-append ".byte " (cadr x) "\n")))
   ((eq? (car x) 'text) (list (string-append ".byte \"" (cadr x) "\"\n")))
   ((eq? (car x) 'defvar) (emit-defvar x))
   ((eq? (car x) 'defun) (emit-defun x))
   ((eq? (car x) 'defconst) (make-constant! (cadr x) (caddr x)) '())
   ((eq? (car x) 'set!) (emit-set! x))
   ((eq? (car x) 'if) (emit-if x))
   ((eq? (car x) 'when) (emit-when x))
   ((eq? (car x) 'while) (emit-while x))
   ;; ((eq? (car x) 'loop) (emit-loop x))
   ((eq? (car x) 'do) (emit-expr-list (cdr x)))
   ((eq? (car x) 'forever) (emit-forever x))
   ((eq? (car x) 'eq?) (binary-procedure "equ" x))
   ((eq? (car x) '<) (binary-procedure "lt" x))
   ((eq? (car x) '<=) (binary-procedure "lte" x))
   ((eq? (car x) '>) (binary-procedure "gt" x))
   ((eq? (car x) '>=) (binary-procedure "gte" x))
   ((eq? (car x) 's<) (binary-procedure "slt" x))
   ((eq? (car x) 's<=) (binary-procedure "slte" x))
   ((eq? (car x) 's>) (binary-procedure "sgt" x))
   ((eq? (car x) 's>=) (binary-procedure "sgte" x))
   ((eq? (car x) '+) (binary-procedure "add" x))
   ((eq? (car x) '-) (binary-procedure "sub" x))
   ((eq? (car x) '*) (binary-procedure "mul" x))
   ((eq? (car x) '/) (binary-procedure "div" x))
   ((eq? (car x) 'mod) (binary-procedure "mod" x))
   ((eq? (car x) 'and) (binary-procedure "and" x))
   ((eq? (car x) 'or) (binary-procedure "or" x))
   ((eq? (car x) 'xor) (binary-procedure "xor" x))
   ((eq? (car x) 'bitwise-not) (unary-procedure "not" x))
   ((eq? (car x) 'not) (unary-procedure "notl" x))
   ((eq? (car x) '<<) (emit-left-shift x))
   ((eq? (car x) '>>) (emit-right-shift x))
   ;; ((eq? (car x) 'poke!) (emit-poke! x))
   ;; ((eq? (car x) 'peek) (emit-peek x))
   ;; ((eq? (car x) 'memset) (emit-memset x))
   (else
    (emit-fncall x)
    )))


(define debug #f)

(define histogram '())

(define (add-histogram name count hist)
  (cond
   ((null? hist) (list (list name count)))
   ((eq? (car (car hist)) name)
    (cons (list name (+ (cadr (car hist)) count)) (cdr hist)))
   (else
    (cons (car hist) (add-histogram name count (cdr hist))))))

(define (histogram-print histogram)
  (for-each
   (lambda (h)
     (display (car h))(display ": ")(display (cadr h))(newline))
   histogram))

(define (emit-expr x)
  (cond
   ((immediate? x)
    (emit-load-immediate x))
   ((primcall? x)
    (let ((r (emit-procedure x)))
     ;; (display x)(newline)
      (set! histogram (add-histogram (car x) (length r) histogram))
      (append
       (emit ";; starting " (symbol->string (car x)))
       r
       (emit ";; ending " (symbol->string (car x)))
       )))
   (else
    (display "yarnc: I don't understand ")(display x)(newline) '())))

;----------------------------------------------------------------

(define (preprocess-cond-to-if x)
  (define (_ l)
    (cond
      ((null? l) 0)
      ((eq? (pre-process (caar l)) 'else)
       (cons 'do (pre-process (cdr (car l)))))
      (else (list 'if (pre-process (caar l))
		  (cons 'do (pre-process (cdr (car l))))
                  (_ (cdr l))))))
  (_ (cdr x)))

(define (preprocess-zero? x)
  (display (cadr x)) (newline)
  (list 'eq? 0 (pre-process (cadr x))))

(define (preprocess-led x) (list 'set! 'reg-led (cadr x)))
(define (preprocess-comp-delta-reset x) (list 'set! 'reg-comp-delta-reset 1))
(define (preprocess-step-count-reset x) (list 'set! 'step-count-reset 1))
(define (preprocess-set-next-move x) (list 'set! 'reg-servo-next-pattern-id (cadr x)))

;; basically diy-macro from the main tinyscheme stuff
(define (pre-process s)
  (cond
    ((null? s) s)
    ((list? s)
     (map
      (lambda (i)
        (if (and (list? i) (not (null? i)))
            ;; dispatch to macro processors
	    (cond
	     ((eq? (car i) 'cond) (preprocess-cond-to-if i))
	     ;;((eq? (car i) 'zero?) (preprocess-zero? i))	     
	     (else (pre-process i)))
	    (pre-process i)))
      s))
    (else s)))

(define (compile-program x)
  (set! variables '())
;;  (set! constants '())
  (let ((done
	 (foldl
	  (lambda (x r)
	    (append r (emit-expr (pre-process x))))
	  '() x)))
    (display "yarnc: compiled size: ")(display (length done))(newline)
    ;;(histogram-print (sort histogram (lambda (a b) (> (cadr a) (cadr b)))))
    done))

(define (output fn x)
  (let ((f (open-output-file fn #:exists 'update)))
    (for-each
     (lambda (line)
       (display line f)(newline f))
     (compile-program x))
    (close-output-port f)
    ;;(msg (string-append "server: closing " fn))
    ;;(histogram-print histogram)
    ))

(define (assert fn x)
  (when (not x)
    (display "assert failed: ")(display fn)(newline)))

(define (dbg x)
  (display x)(newline) x)

(define (msg x)
  (display x)(newline))


(define input-file "../fifo/lisp_fifo")
(define output-file "../fifo/asm_fifo")

(define (loop)
  ;;(msg "compiler: opening lisp_fifo")
  (let ((f (open-input-file input-file)))
    (msg "compiler: waiting for lisp to compile")
    (let ((data (read f)))
      (msg data)
      (cond
       ((list? data) (output output-file data))
       (else
	(msg "data seems corrupted, ignoring")
	(output output-file '())))
      (msg "compiler: sent asm"))
    (close-input-port f))
  (loop))
(loop)
