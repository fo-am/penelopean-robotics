wait1:	ld   	A 		;; wait for A to be set to 1
	jpz  	wait1
	ldl  	1               
	st   	STEP_COUNT_RESET
	ldl  	WALK_FORWARD
	st   	NEXT_PATTERN
xx:  	ld 	STEP_COUNT   
	ldl   	8		
	lte                       	
	jpz  	xx
	ldl  	ALL_STOP
	st   	NEXT_PATTERN	
	ldl  	0
	st   	A       
	nop
wait2:	ld   	A
	jpz  	wait2
	ldl  	WALK_BACKWARD
	st   	NEXT_PATTERN
	ldl  	1
	st  	STEP_COUNT_RESET
yy:  	ld   	STEP_COUNT   
	ldl   	8	        
	lte                       	
	jpz  	yy
	ldl  	ALL_STOP
	st   	NEXT_PATTERN
	ldl  	0
	st   	A
	jmp  	wait1   
