	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	arr: .byte 0b01111110, 0b00110000, 0b01101101, 0b01111001, 0b00110011, 0b01011011, 0b01011111, 0b01110000, 0b01111111, 0b01111011, 0b01110111, 0b00011111, 0b01001110, 0b00111101, 0b01001111, 0b01000111
	ass: .byte 8, 7, 6, 5, 4, 3, 2, 1, 1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8

.text
	.global main

	.equ RCC_AHB2ENR,  0x4002104C

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

	.equ GPIOC_MODER  , 0x48000800
	.equ GPIOC_PUPDR  , 0x4800080C
	.equ GPIOC_IDR    , 0x48000810

main:
	bl   gpio_init
	bl   max7219_init

display_0_to_f:
	mov  r2, 0x0
	ldr  r3, =arr
	ldr  r10, =ass
	b    loop

gpio_init:
	mov  r0, 0b00000000000000000000000000000101
	ldr  r1, =RCC_AHB2ENR
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

	ldr  r1, =GPIOC_MODER
	ldr  r0, [r1]
	ldr  r2, =0xF3FFFF00
	and  r0, r2
	str  r0, [r1]

	ldr  r1, =GPIOC_PUPDR
	ldr  r0, [r1]
	ldr  r2, =0b01010101
	and  r0, 0xFFFFFF00
	orr  r0, r2
	str  r0, [r1]

	ldr  r12, =GPIOC_IDR @ dip switch

	bx   lr

loop:
	b    loop_init

loop_next:
	ldrh r1, [r12]
	and  r1, 0b1111
	@eor  r1, 0b1111
	add  r1, 1

	mov  r0, 0x1
	ldrb r1, [r3, r1]
	bl   max7219_send

	ldr  r0, =400000 @ delay 0.1s
	movs r0, r0
	bl   delay

	add  r2, 0x1
	cmp  r2, 20
	bne  loop

	mov  r2, 0x0
	b    loop

max7219_send:
	@ input parameter: r0 is ADDRESS , r1 is DATA
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
	mov  r8, 0x1
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
	subs r7, 0x1
	bge  max7219_send_loop
	str  r2, [r1, r6] @ cs -> 0
	str  r2, [r1, r5] @ cs -> 1
	pop  {r0, r1, r2, r3, r4, r5, r6, r7, r8, pc}

max7219_init:
	push {r0, r1, r2, lr}

	ldr  r0, =DECODE_MODE
	ldr  r1, =0x0
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

	pop  {r0, r1, r2, pc}

delay:
	beq  delay_end
	subs r0, 0x4
	b    delay

delay_end:
	bx   lr

loop_init:
	mov  r0, 0x1
	mov  r1, 0x0
	bl   max7219_send

	mov  r0, 0x2
	mov  r1, 0x0
	bl   max7219_send

	mov  r0, 0x3
	mov  r1, 0x0
	bl   max7219_send

	mov  r0, 0x4
	mov  r1, 0x0
	bl   max7219_send

	mov  r0, 0x5
	mov  r1, 0x0
	bl   max7219_send

	mov  r0, 0x6
	mov  r1, 0x0
	bl   max7219_send

	mov  r0, 0x7
	mov  r1, 0x0
	bl   max7219_send

	mov  r0, 0x8
	mov  r1, 0x0
	bl   max7219_send

	b    loop_next
