	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	password: .byte 0b1100
	.align

.text
	.global main
	.equ RCC_AHB2ENR  , 0x4002104C
	.equ GPIOB_MODER  , 0x48000400
	.equ GPIOB_OTYPER , 0x48000404
	.equ GPIOB_OSPEEDR, 0x48000408
	.equ GPIOB_PUPDR  , 0x4800040C
	.equ GPIOB_ODR    , 0x48000414
	.equ GPIOC_MODER  , 0x48000800
	.equ GPIOC_IDR    , 0x48000810
	.equ GPIOA_MODER  , 0x48000000
	.equ GPIOA_PUPDR  , 0x4800000C
	.equ GPIOA_IDR    , 0x48000010

main:
	b    init_gpio

init_gpio:
	mov  r0, 0b111
	ldr  r1, =RCC_AHB2ENR
	str  r0, [r1]

	mov  r0, 0b01010101000000
	ldr  r1, =GPIOB_MODER
	ldr  r2, [r1]
	and  r2, 0xFFFFC03F
	orr  r2, r0
	str  r2, [r1]

	ldr  r1, =GPIOC_MODER
	ldr  r0, [r1]
	ldr  r2, =0xF3FFFFFF
	and  r0, r2
	str  r0, [r1]

	ldr  r1, =GPIOA_MODER
	ldr  r0, [r1]
	ldr  r2, =0xFFFFFF00
	and  r0, r2
	str  r0, [r1]

	mov  r0, 0b10101010000000
	ldr  r1, =GPIOB_OSPEEDR
	str  r0, [r1]

	ldr  r1, =GPIOA_PUPDR
	ldr  r0, [r1]
	ldr  r2, =0b01010101
	orr  r0, r2
	str  r0, [r1]

	ldr  r10, =GPIOB_ODR @ leds
	ldr  r11, =GPIOC_IDR @ user button
	ldr  r12, =GPIOA_IDR @ dip switch

	mov  r8, 1
	mov  r9, 1
	movs r0, 0
	b    poll_button

poll_button:
	beq  poll_button_restart
	ldr  r1, =0b11111111111111111
	ands r1, r0, r1
	beq  poll_button_check
	subs r0, 8
	b    poll_button

poll_button_check:
	ldrh r1, [r11]
	lsr  r1, 13
	mov  r2, 1
	and  r1, r2
	cmp  r1, r8
	mov  r8, r1
	beq  poll_button_confirm
	subs r0, 8
	b    poll_button

poll_button_confirm:
	sub  r1, r8, r9
	cmp  r1, 1
	mov  r9, r8
	beq  read_switch
	subs r0, 8
	b    poll_button

poll_button_restart:
	ldr  r0, =4000000
	movs r0, r0
	b    poll_button

read_switch:
	ldrh r1, [r12]
	and  r1, 0b1111
	eor  r1, 0b1111

	ldr  r0, =password
	ldrb r0, [r0]

	cmp  r0, r1
	beq  leds_3x
	b    leds_1x
