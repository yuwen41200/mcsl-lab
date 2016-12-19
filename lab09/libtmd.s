	.syntax unified
	.cpu cortex-m4
	.thumb

.text
	.global delay_ms

delay_ms:
	push {r0, lr}
	muls r0, 4000

delay_ms_loop:
	beq  delay_ms_end
	subs r0, 4
	b    delay_ms_loop

delay_ms_end:
	pop  {r0, pc}
