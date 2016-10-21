	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	arr1: .byte 0x19, 0x34, 0x14, 0x32, 0x52, 0x23, 0x61, 0x29
	arr2: .byte 0x18, 0x17, 0x33, 0x16, 0xFA, 0x20, 0x55, 0xAC
	len:  .byte 0x08

.text
	.global main

bubble_sort:
	@ r2: iterator of outer loop, i
	mov  r2, #0x0
	push {lr}

start_outer:
	@ condition: i < len - 1
	sub  r4, r1, #0x1
	cmp  r2, r4
	bge  end_outer

	@ r3: iterator of inner loop, j
	mov  r3, #0x0

start_inner:
	@ condition: j < len - 1 - i
	sub  r5, r4, r2
	cmp  r3, r5
	bge  end_inner

	@ if arr[j] > arr[j+1] then swap
	ldrb r5, [r0, r3]
	add  r7, r3, #0x1
	ldrb r6, [r0, r7]
	cmp  r5, r6
	it   gt
	blgt swap

	add  r3, r3, #0x1
	b    start_inner

end_inner:
	add  r2, r2, #0x1
	b    start_outer

end_outer:
	pop  {pc}

swap:
	strb r5, [r0, r7]
	strb r6, [r0, r3]
	bx   lr

main:
	ldr  r0, =arr1
	ldr  r2, =len
	ldr  r1, [r2]
	@ http://www.iis.sinica.edu.tw/~cmwang/arm/Lecture08.ppt
	@ r0: array pointer
	@ r1: array length
	bl   bubble_sort
	ldr  r0, =arr2
	bl   bubble_sort

forever:
	b    forever
