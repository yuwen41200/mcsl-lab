.data
	ans:   .word 1
.text
	.global main
	.equ N,20

fibb:
	cmp R0,#100
	bgt outdone
	cmp R0,#1
	blt outdone
	mov R5,#2
	cmp R0,#3
	blt done


fib:
	mov R1,R2
	mov R2,R4
	add R4,R1,R2
	add R5,#1
	cmp R5,R0
	bne fib

done:
	bx lr
outdone:
	ldr R4,=0xFFFFFFFF
	bx lr

main:
	movs R0,#N
	movs R1,#1      @ R1=1
	movs R2,#1	@ R2=1
	movs R4,#1      @ R4=1

	bl fibb
	ldr R7,=ans
	str R4,[R7]

L: b L
