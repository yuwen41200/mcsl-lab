.data
	ans:   .word 1
.text
	.global main
	.equ N,20

fibb:
	mov R5,#2
	cmp R0,#3
	blt done
fib:
	mov R1,R2
	mov R2,R3
	add R3,R1,R2
	add R5,#1
	cmp R5,R0
	bne fib
done:
	bx lr


main:
	movs R0,#N
	movs R1,#1      @ R1=1
	movs R2,#1		@ R2=1
	movs R3,#1      @ R3=1

	bl fibb
	ldr R7,=ans
	str R3,[R7]

L: b L
