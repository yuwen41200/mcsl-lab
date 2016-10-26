	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	copy_expr:   .zero 256
	user_stack:  .zero 128
	expr_result: .word 0

.text
	.global main
	postfix_expr: .asciz "-100 10 20 + - 10 +"

main:
	ldr  R0, =postfix_expr
	mov  R1, 0x0
	bl   strlen

	ldr  R0, =postfix_expr
	ldr  R1, =copy_expr
	bl   memcpy

	ldr  R0, =user_stack
	msr  msp, R0
	ldr  R2, =postfix_expr
	mov  R3, 0x0
	ldr  R4, =copy_expr
	b    arithmetic

arithmetic:
	@ arithmetic-related
	ldrb R1, [R2, R3]
	cmp  R1, 0x0
	beq  arithmetic_end
	cmp  R1, 0x20   @ '[space]'
	beq  duplicate_spaces
	ldrh R1, [R2, R3]
	cmp  R1, 0x2B00 @ '+\0'
	beq  addition
	cmp  R1, 0x2D00 @ '-\0'
	beq  substraction
	add  R0, R4, R3
	mov  R5, R0
	bl   atoi
	push {R1}
	sub  R1, R0, R5
	add  R3, R1
	b    arithmetic

addition:
	@ arithmetic-related
	pop  {R5}
	pop  {R6}
	add  R5, R6
	push {R5}
	add  R3, 0x2
	b    arithmetic

substraction:
	@ arithmetic-related
	pop  {R5}
	pop  {R6}
	sub  R5, R6
	push {R5}
	add  R3, 0x2
	b    arithmetic

duplicate_spaces:
	@ arithmetic-related
	add  R3, 0x1
	b    arithmetic

arithmetic_end:
	@ arithmetic-related
	pop  {R2}
	mrs  R0, msp
	ldr  R1, =user_stack
	cmp  R0, R1
	bne  error
	ldr  R0, =expr_result
	str  R2, [R0]
	b    program_end

memcpy:
	@ R0: source address
	@ R1: destination address
	push {lr}
	ldrb R2, [R0]
	cmp  R2, 0x0
	beq  go_back
	cmp  R2, 0x20 @ '[space]'
	it   eq
	bleq space_to_zero
	strb R2, [R1]
	add  R0, 0x1
	add  R1, 0x1
	b    memcpy

space_to_zero:
	@ called by memcpy
	mov  R2, 0x0
	bx   lr

go_back:
	@ called by memcpy
	pop  {pc}

strlen:
	@ R0: start address of the string
	@ R1: string length (return)
	ldrb R2, [R0]
	cmp  R2, 0x0
	it   eq
	bxeq lr
	add  R0, 0x1
	add  R1, 0x1
	b    strlen

atoi:
	@ R0: start address of the string
	@     start address of the next token (return)
	@ R1: integer value (return)
	ldrb R1, [R0]
	mov  R2, 0x0  @ is_negative flag
	cmp  R1, 0x2B @ '+'
	beq  atoi_pos
	cmp  R1, 0x2D @ '-'
	beq  atoi_neg
	mov  R1, 0x0
	b    atoi_inner

atoi_pos:
	@ called by atoi
	add  R0, 0x1
	mov  R1, 0x0
	b    atoi_inner

atoi_neg:
	@ called by atoi
	add  R0, 0x1
	mov  R1, 0x0
	mov  R2, 0x1
	b    atoi_inner

atoi_inner:
	@ called by atoi
	ldrb R3, [R0]
	cmp  R3, 0x0
	beq  atoi_end
	cmp  R3, 0x3A @ character after '9'
	bge  error
	cmp  R3, 0x2F @ character before '0'
	ble  error
	sub  R3, 0x30 @ '0'
	mov  R9, 0xA
	mul  R1, R9
	add  R1, R3
	add  R0, 0x1
	b    atoi_inner

atoi_end:
	@ called by atoi
	add  R0, 0x1
	cmp  R2, 0x0
	bne  additive_inverse
	bx   lr

additive_inverse:
	@ called by atoi
	mov  R9, 0x0
	sub  R1, R9, R1
	bx   lr

error:
	@ error handling
	ldr  R0, =expr_result
	ldr  R1, =0xFFFFFFFF
	str  R1, [R0]
	b    program_end

program_end:
	@ infinate loop
	b    program_end
