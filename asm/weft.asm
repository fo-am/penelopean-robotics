wait1:	ld   	A 		;; wait for A to be set to 1
	jpz  	wait1
	ldl  	1               
	st   	COMP_DELTA_RESET
	ldl  	WALK_FORWARD
	st   	NEXT_PATTERN
xx:  	ld 	COMP_DELTA    	;; check the compass
	ld   	90   	        
	gte     		;; rotated more than 90 degrees?
	jpz  	xx
	ldl  	ALL_STOP	;; stop!
	st   	NEXT_PATTERN	
	ldl  	0		;; signal we are ready
	st   	A
	jmp 	wait1
