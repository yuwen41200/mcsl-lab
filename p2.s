	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	result:   .word 0
	max_size: .word 0

.text
	.global main
	m: .word 24
	n: .word 60

main:
	ldr  r2, =m
	ldr  r0, [r2]
	ldr  r3, =n
	ldr  r1, [r3]

	mov  r10, 0x0      @ r10: max stack size

	push {r0, r1, lr}
	bl   gcd
	pop  {r0, r1, r9}

	ldr  r3, =result
	str  r2, [r3]

	ldr  r3, =max_size
	str  r10, [r3]

	b    forever

gcd:
	ldr  r0, [sp]      @ param a
	ldr  r1, [sp, 0x4] @ param b

	mrs  r8, msp
	mov  r7, sp
	sub  r8, r7
	cmp  r8, r10
	bgt  update_r10
	b    gcd_final

update_r10:
	mov  r10, r8

gcd_final:
	cmp  r0, 0x0
	beq  return_b

	cmp  r1, 0x0
	beq  return_a

	mov  r2, 0x1
	and  r3, r0, r2    @ r3: bool, a is even
	and  r4, r1, r2    @ r4: bool, b is even

	ands r2, r3, r4
	bne  case_1

	cmp  r3, 0x0
	bne  case_2

	cmp  r4, 0x0
	bne  case_3

	b    case_4

return_a:
	mov  r2, r0
	bx   lr

return_b:
	mov  r2, r1
	bx   lr

case_1:
	asr  r0, r0, 0x1
	asr  r1, r1, 0x1
	push {r0, r1, lr}
	bl   gcd
	pop  {r0, r1, r9}
	mov  r8, 0x2
	mul  r2, r8
	mov  pc, r9

case_2:
	asr  r0, r0, 0x1
	push {r0, r1, lr}
	bl   gcd
	pop  {r0, r1, r9}
	mov  pc, r9

case_3:
	asr  r1, r1, 0x1
	push {r0, r1, lr}
	bl   gcd
	pop  {r0, r1, r9}
	mov  pc, r9

case_4:
	cmp r0, r1
	bgt a_is_bigger
	sub r2, r1, r0     @ b - a
	mov r1, r0
	mov r0, r2
	b   case_4_final

a_is_bigger:
	sub r0, r1         @ a - b

case_4_final:
	push {r0, r1, lr}
	bl   gcd
	pop  {r0, r1, r9}
	mov  pc, r9

forever:
	b    forever
