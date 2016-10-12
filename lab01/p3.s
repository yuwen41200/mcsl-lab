	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	X: .word 5
	Y: .word 10
	Z: .word

.text
	.global main

main:
	ldr  r0, =X     @ r0 = &X
	ldr  r1, [r0]   @ r1 = *r0
	movs r2, #10    @ r2 = 10
	muls r2, r1, r2
	ldr  r3, =Y     @ r3 = &Y
	ldr  r1, [r3]   @ r1 = *r3
	adds r2, r1, r2
	str  r2, [r0]   @ *r0 = r2
	subs r2, r1, r2
	ldr  r0, =Z     @ r0 = &Z
	str  r2, [r0]   @ *r0 = r2

L:
	B L
