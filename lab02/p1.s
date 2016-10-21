@ ARM Cortex-M4 Documents:
@ http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/DUI0553A_cortex_m4_dgug.pdf

	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	result: .byte 0

.text
	.global main
	.equ X, 0x55AA
	.equ Y, 0xAA55

hamming_distance:
	@ TODO
	bx lr

main:
	movs r0, #X           @ error expected
	movs r1, #Y
	ldr  r2, =result
	bl   hamming_distance

forever:
	b    forever
