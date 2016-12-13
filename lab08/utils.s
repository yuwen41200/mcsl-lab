	.syntax unified
	.cpu cortex-m4
	.thumb

.text
	.global gpio_init
	.global max7219_init
	.global max7219_send
	.global delay_1s
	.global fpu_enable

	.equ RCC_AHB2ENR,  0x4002104C
	.equ RCC_APB2ENR,  0x40021060

	.equ DECODE_MODE,  0x09
	.equ DISPLAY_TEST, 0x0F
	.equ SCAN_LIMIT,   0x0B
	.equ INTENSITY,    0x0A
	.equ SHUTDOWN,     0x0C

	.equ MAX7219_DIN,  0x20 @ PA5
	.equ MAX7219_CS,   0x40 @ PA6
	.equ MAX7219_CLK,  0x80 @ PA7

	.equ GPIOA_BASE,   0x48000000
	.equ BSRR_OFFSET,  0x18 @ set bit
	.equ BRR_OFFSET,   0x28 @ clear bit

	.equ GPIOB_BASE,   0x48000400
	.equ AFRL_OFFSET,  0x20
	.equ AFRH_OFFSET,  0x24

	.equ GPIOC_BASE,   0x48000800

gpio_init:
	push {r0, r1, r2, lr}

	mov  r0, 0b00000000000000000000000000000111
	ldr  r1, =RCC_AHB2ENR
	str  r0, [r1]

	mov  r0, 0b00000000000000000000000000000001
	ldr  r1, =RCC_APB2ENR
	str  r0, [r1]

	ldr  r1, =GPIOA_BASE @ GPIOA_MODER
	ldr  r2, [r1]
	and  r2, 0b11111111111111110000001111111111
	orr  r2, 0b00000000000000000101010000000000
	str  r2, [r1]

	add  r1, 0x4 @ GPIOA_OTYPER
	ldr  r2, [r1]
	and  r2, 0b11111111111111111111111100011111
	str  r2, [r1]

	add  r1, 0x4 @ GPIOA_SPEEDER
	ldr  r2, [r1]
	and  r2, 0b11111111111111110000001111111111
	orr  r2, 0b00000000000000000101010000000000
	str  r2, [r1]

	ldr  r1, =GPIOB_BASE @ GPIOB_MODER
	ldr  r2, [r1]
	and  r2, 0b11111111111111111111111100111111
	orr  r2, 0b00000000000000000000000010000000 @ alternate function mode for PB3
	str  r2, [r1]

	ldr  r1, =GPIOC_BASE @ GPIOC_MODER
	ldr  r2, [r1]
	and  r2, 0b11110011111111111111111111111111
	str  r2, [r1]

	pop  {r0, r1, r2, pc}

max7219_init:
	push {r0, r1, lr}

	ldr  r0, =DECODE_MODE
	ldr  r1, =0xFF
	bl   max7219_send

	ldr  r0, =DISPLAY_TEST
	ldr  r1, =0x0
	bl   max7219_send

	ldr  r0, =SCAN_LIMIT
	ldr  r1, =0x7
	bl   max7219_send

	ldr  r0, =INTENSITY
	ldr  r1, =0xA
	bl   max7219_send

	ldr  r0, =SHUTDOWN
	ldr  r1, =0x1
	bl   max7219_send

	pop  {r0, r1, pc}

max7219_send:
	@ input parameter: r0 is ADDRESS, r1 is DATA
	push {r0, r1, r2, r3, r4, r5, r6, r7, r8, lr}
	lsl  r0, r0, 0x8
	add  r0, r1
	ldr  r1, =GPIOA_BASE
	ldr  r2, =MAX7219_CS
	ldr  r3, =MAX7219_DIN
	ldr  r4, =MAX7219_CLK
	ldr  r5, =BSRR_OFFSET
	ldr  r6, =BRR_OFFSET
	ldr  r7, =0x0F @ currently sending r7-th bit

max7219_send_loop:
	mov  r8, #0x1
	lsl  r8, r8, r7
	str  r4, [r1, r6] @ clk -> 0
	tst  r0, r8 @ ANDS but discard result
	beq  max7219_send_clear_bit
	str  r3, [r1, r5] @ din -> 1
	b    max7219_send_check_done

max7219_send_clear_bit:
	str  r3, [r1, r6] @ din -> 0

max7219_send_check_done:
	str  r4, [r1, r5] @ clk -> 1
	subs r7, #0x1
	bge  max7219_send_loop
	str  r2, [r1, r6] @ cs -> 0
	str  r2, [r1, r5] @ cs -> 1
	pop  {r0, r1, r2, r3, r4, r5, r6, r7, r8, pc}

delay_1s:
	push {r0, lr}
	ldr  r0, =4000000 @ delay 1s
	movs r0, r0
	b    delay_1s_loop

delay_1s_loop:
	beq  delay_1s_end
	subs r0, 0x4
	b    delay_1s_loop

delay_1s_end:
	pop  {r0, pc}

fpu_enable:
	push  {r0, r1, lr}
	ldr.w r0, =0xE000ED88
	ldr   r1, [r0]
	orr   r1, r1, #(0xF << 20)
	str   r1, [r0]
	dsb
	isb
	pop   {r0, r1, pc}
