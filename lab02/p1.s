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
	eor  r0, r1
	cmp  r0, #0x0
	beq  done

count_ones:
	add  r3, #0x1
	sub  r1, r0, #0x1
	ands r0, r1
	bne  count_ones

done:
	bx   lr

main:
	@ http://www.davespace.co.uk/arm/introduction-to-arm/immediates.html
	ldr  r0, =X
	ldr  r1, =Y
	ldr  r2, =result
	ldr  r3, [r2]
	@ https://en.wikipedia.org/wiki/Hamming_distance
	bl   hamming_distance
	str  r3, [r2]

forever:
	b    forever
