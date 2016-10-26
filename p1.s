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

	mov  R0, R1
	bl   atoi

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
	add  R0, 0x1
	mov  R1, 0x0
	b    atoi_inner

atoi_neg:
	add  R0, 0x1
	mov  R1, 0x0
	mov  R2, 0x1
	b    atoi_inner

atoi_inner:
	ldrb R3, [R0]
	cmp  R3, 0x0
	beq  atoi_end
	cmp  R3, 0x3A @ character after 9
	bge  error
	cmp  R3, 0x2F @ character before 0
	ble  error
	sub  R3, 0x30 @ '0'
	mul  R1, 0xA
	add  R1, R3
	add  R0, 0x1
	b    atoi_inner

atoi_end:
	add  R0, 0x1
	cmp  R2, 0x0
	bne  additive_inverse
	bx   lr

additive_inverse:
	sub  R1, 0, R1
	bx   lr

error:
	ldr  R0, =expr_result
	ldr  R1, =0xFFFFFFFF
	str  R1, [R0]
	b    program_end

program_end:
	b    program_end
