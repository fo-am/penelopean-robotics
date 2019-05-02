wait1:	ld   	A 		;; wait for A to be set to 1
	jpz  	wait1
	ldl  	1               
	st   	STEP_COUNT_RESET
	ldl  	WALK_FORWARD
	st   	NEXT_PATTERN
xx:  	ld 	STEP_COUNT   
	ld   	5            	        
	gte                       	
	jpz  	xx
	ldl  	WALK_STOP
	st   	NEXT_PATTERN	
	ldl  	0		;; signal we are ready
	st   	A       
wait2:	ld   	A		;; wait for A to be set to 1 
	jpz  	wait2
	ldl  	WALK_BACKWARD
	st   	NEXT_PATTERN
	ldl  	1
	st  	STEP_COUNT_RESET
yy:  	ld   	STEP_COUNT   
	ld   	5	        
	gte                       	
	jpz  	yy
	ldl  	WALK_STOP
	st   	NEXT_PATTERN
	ldl  	0
	st   	A       	;; signal we are ready
	jmp  	wait1   
