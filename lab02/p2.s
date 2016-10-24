	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	ans: .word 1

.text
	.global main
	.equ N,20

fibb:
	cmp R0,#100
	bgt outdone
	cmp R0,#1
	blt outdone
	mov R5,#2 @count from 2 index
	cmp R0,#3
	blt done

fib:
	mov R1,R2
	mov R2,R4
	adds R4,R1,R2
	bvs overflow
	add R5,#1
	cmp R5,R0
	bne fib

done:
	bx lr

outdone:
	ldr R4,=0xFFFFFFFF
	bx lr

overflow:
	ldr R4,=0xFFFFFFFE
	bx lr

main:
	mov R0,#N
	mov R1,#1      @ R1=1
	mov R2,#1	   @ R2=1
	mov R4,#1      @ R4=1

	bl fibb
	ldr R7,=ans
	str R4,[R7]

L: b L
