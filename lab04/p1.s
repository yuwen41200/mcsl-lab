	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	leds: .byte 0
	.align

.text
	.global main
	.equ RCC_AHB2ENR  , 0x4002104C
	.equ GPIOB_MODER  , 0x48000400
	.equ GPIOB_OTYPER , 0x48000404
	.equ GPIOB_OSPEEDR, 0x48000408
	.equ GPIOB_PUPDR  , 0x4800040C
	.equ GPIOB_ODR    , 0x48000414

@ r0 gpio output destination
@ r8 left or right direction flag
@ r9 light or close leds

main:
	bl   gpio_init
	mov  r9, 0b00001100
	ldrb r8, =leds
	strb r9, [r8]
	mov  r8, 0b0
	b    loop

gpio_init:
	movs r0, 0b10
	ldr  r1, =RCC_AHB2ENR
	str  r0, [r1]

	movs r0, 0b01010101000000
	ldr  r1, =GPIOB_MODER
	ldr  r2, [r1]
	and  r2, 0xFFFFC03F
	orrs r2, r0
	str  r2, [r1]

	movs r0, 0b10101010000000
	ldr  r1, =GPIOB_OSPEEDR
	strh r0, [r1]

	ldr  r0, =GPIOB_ODR
	bx   lr

loop:
	bl   display_led

	ldr  r5, =4000000
	movs r5, r5
	bl   delay

	cmp  r8, 0b0
	it   eq
	bleq move_left

	cmp  r8, 0b1
	it   eq
	bleq move_right

	cmp  r9, 0b11000000
	it   eq
	bleq set_right_flag

	cmp  r9, 0b00001100
	it   eq
	bleq set_left_flag

	b    loop

display_led:
	eor  r5, r9, 0xFFFFFFFF
	strh r5, [r0]
	bx   lr

move_left:
	lsl  r9, r9, 0b1
	bx   lr

move_right:
	lsr  r9, r9, 0b1
	bx   lr

set_right_flag:
	mov  r8, 0b1
	bx   lr

set_left_flag:
	mov  r8, 0b0
	bx   lr

delay:
	beq  delay_end
	subs r5, 4
	b    delay

delay_end:
	bx   lr
