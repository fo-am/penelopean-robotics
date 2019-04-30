start:	ldl	1
	st	STEP_COUNT_RESET
	ldl	WALK_FORWARD
	st	NEXT_PATTERN
xxx:	ld   	COMP_ANGLE  
	ldl	180
	lte
	jpz	xxx
	ldl	1
	st	STEP_COUNT_RESET
	ldl	ALL_STOP
	st	NEXT_PATTERN
yyy:	ld   	STEP_COUNT  
	ldl	2
	lte
	jpz	yyy
	jmp  	start
