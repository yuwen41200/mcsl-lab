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
	.equ GPIOC_MODER  , 0x48000800
	.equ GPIOC_IDR    , 0x48000810

@ r0 gpio led output [init in gpio_init]
@ r7 running or stopped [init in main]
@ r8 left or right direction flag [init in main]
@ r9 turn on or off leds [init in main]
@ r10 gpio button input [init in gpio_init]
@ (r11, r12): (latest, confirmed) button value [init in main]
@ (1, 1) -> (0, 1) -> (0, 0) -> (1, 0) -> (1, 1)

main:
	bl   gpio_init
	mov  r9, 0b00001100
	ldrb r8, =leds
	strb r9, [r8]
	mov  r7, 0b0
	mov  r8, 0b0
	mov  r11, 0b1
	mov  r12, 0b1
	b    loop

gpio_init:
	movs r0, 0b110
	ldr  r1, =RCC_AHB2ENR
	str  r0, [r1]

	movs r0, 0b01010101000000
	ldr  r1, =GPIOB_MODER
	ldr  r2, [r1]
	and  r2, 0xFFFFC03F
	orrs r2, r0
	str  r2, [r1]

	ldr  r1, =GPIOC_MODER
	ldr  r0, [r1]
	ldr  r2, =0xF3FFFFFF
	and  r0, r2
	str  r0, [r1]

	movs r0, 0b10101010000000
	ldr  r1, =GPIOB_OSPEEDR
	strh r0, [r1]

	ldr  r0, =GPIOB_ODR
	ldr  r10, =GPIOC_IDR
	bx   lr

loop:
	bl   display_led

	ldr  r5, =4000000 @ cpu is 4mhz
	movs r5, r5
	bl   delay

	cmp  r7, 0b1
	beq  loop

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
	strh r5, [r0] @ r0 output data reg
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
	ldr  r1, =0b11111111111111111
	ands r1, r5, r1
	beq  check_button @ branch every 32.768 ms
	subs r5, 8
	b    delay

check_button:
	ldrh r1, [r10] @ r10 input data reg
	lsr  r1, 13
	mov  r2, 1
	and  r1, r2
	cmp  r1, r11
	mov  r11, r1
	beq  button_confirmed
	subs r5, 8
	b    delay

button_confirmed:
	subs r1, r11, r12
	cmp  r1, 1
	mov  r12, r11
	beq  switch
	subs r5, 8
	b    delay

switch:
	eor  r7, 0b1
	subs r5, 8
	b    delay

delay_end:
	bx   lr
