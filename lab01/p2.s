	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	str: .asciz "Hello World!"
	str2: .word 0x6c6c6548
	str3: .word 0x6f57206f
	str4: .word 0x21646c72
	str5: .byte 0x00

.text
	.global main
	.equ AA, 0x55
	X: .word 100

main:
	ldr  r1, =X     @ address of X
	ldr  r0, [r1]   @ value of r1
	movs r2, #AA
	adds r2, r2, r0
	str  r2, [r1]   @ store r2 to value of r1
	ldr  r1, =str
	ldr  r2, [r1]   @ "Hell" only

L: B L
