	.syntax unified
	.cpu cortex-m4
	.thumb

.text
	.global main
	.equ input_one, -5
	.equ input_two, 3

main:
	ldr  r0, =input_one
	ldr  r1, =input_two

	cmp  r0, -128
	blt  error
	cmp  r0, 128
	bge  error

	cmp  r1, 0
	blt  error
	cmp  r1, 255
	bgt  error

	and  r9, r1, 0b00000000000000000000000000000001

	cmp  r0, 0
	blt  special_1
	beq  special_2
	mov  r2, 1
	b    loop

special_1:
	mov  r2, -1
	b    loop

special_2:
	b    error

loop:
	cmp  r1, 0
	ble  program_end

	mov  r3, r0
	cmp  r3, 0
	it   lt
	bllt abs
	mov  r4, 0
	bl   my_mul
	mov  r2, r4

	sub  r1, 1
	b    loop

abs:
	mov  r10, 0
	sub  r3, r10, r3
	bx   lr

my_mul:
	cmp  r3, 0
	ble  my_mul_end

	adds r4, r2
	bvs  error

	sub  r3, 1
	b    my_mul

my_mul_end:
	bx   lr

error:
	mov  r2, 0
	b    program_end

program_end:
	cmp  r9, 0
	beq  abs_2
	b    program_end_real

abs_2:
	cmp  r2, 0
	blt  abs_2_inner
	b    program_end_real

abs_2_inner:
	sub  r2, r10, r2
	b    program_end_real

program_end_real:
	b    program_end_real
