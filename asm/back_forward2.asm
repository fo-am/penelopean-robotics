start:	ldl	1
	st	STEP_COUNT_RESET
	ldl	WALK_FORWARD
	st	NEXT_PATTERN
xxx:	ld   	STEP_COUNT  
	ldl	10
	lte
	jpz	xxx
	ldl	1
	st	STEP_COUNT_RESET
	ldl	WALK_BACKWARD
	st	NEXT_PATTERN
yyy:	ld   	STEP_COUNT  
	ldl	10
	lte
	jpz	yyy
	jmp  	start
