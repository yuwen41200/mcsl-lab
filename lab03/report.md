# 實驗三 實驗結報 #

0316213 蒲郁文 & 0316323 薛世恩

## 實驗名稱 ##

ARM Assembly II

## 實驗目的 ##

熟悉基本 ARMv7 組合語言語法使用

## 實驗步驟 ##

### Postfix Arithmetic ###

```assembly
	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	copy_expr:   .zero 256
	user_stack:  .zero 128
	expr_result: .word 0

.text
	.global main
	postfix_expr: .asciz "-100 10 20 + - 10 +"

main:
	ldr  R0, =postfix_expr
	mov  R1, 0x0
	bl   strlen

	ldr  R0, =postfix_expr
	ldr  R1, =copy_expr
	bl   memcpy

	ldr  R0, =user_stack
	msr  msp, R0
	ldr  R2, =postfix_expr
	mov  R3, 0x0
	ldr  R4, =copy_expr
	b    arithmetic

arithmetic:
	@ arithmetic-related
	ldrb R1, [R2, R3]
	cmp  R1, 0x0
	beq  arithmetic_end
	cmp  R1, 0x20   @ '[space]'
	beq  duplicate_spaces
	ldrh R1, [R2, R3]
	ldr  R9, =0x202B
	cmp  R1, R9     @ " +"
	beq  addition
	cmp  R1, 0x2B   @ "\0+"
	beq  addition_last
	ldr  R9, =0x202D
	cmp  R1, R9     @ " -"
	beq  substraction
	cmp  R1, 0x2D   @ "\0-"
	beq  substraction_last
	add  R0, R4, R3
	mov  R5, R0
	push {R2, R3, R4, R5}
	bl   atoi
	pop  {R2, R3, R4, R5}
	push {R1}
	sub  R1, R0, R5
	add  R3, R1
	b    arithmetic

addition:
	@ arithmetic-related
	pop  {R5}
	pop  {R6}
	add  R6, R5
	push {R6}
	add  R3, 0x2
	b    arithmetic

addition_last:
	@ arithmetic-related
	pop  {R5}
	pop  {R6}
	add  R6, R5
	push {R6}
	b    arithmetic_end

substraction:
	@ arithmetic-related
	pop  {R5}
	pop  {R6}
	sub  R6, R5
	push {R6}
	add  R3, 0x2
	b    arithmetic

substraction_last:
	@ arithmetic-related
	pop  {R5}
	pop  {R6}
	sub  R6, R5
	push {R6}
	b    arithmetic_end

duplicate_spaces:
	@ arithmetic-related
	add  R3, 0x1
	b    arithmetic

arithmetic_end:
	@ arithmetic-related
	pop  {R2}
	ldr  R1, =user_stack
	mov  R9, sp
	cmp  R1, R9
	bne  error
	ldr  R0, =expr_result
	str  R2, [R0]
	b    program_end

memcpy:
	@ R0: source address
	@ R1: destination address
	push {lr}
	b    memcpy_inner

memcpy_inner:
	@ called by memcpy
	ldrb R2, [R0]
	cmp  R2, 0x0
	beq  go_back
	cmp  R2, 0x20 @ '[space]'
	it   eq
	bleq space_to_zero
	strb R2, [R1]
	add  R0, 0x1
	add  R1, 0x1
	b    memcpy_inner

space_to_zero:
	@ called by memcpy
	mov  R2, 0x0
	bx   lr

go_back:
	@ called by memcpy
	pop  {pc}

strlen:
	@ R0: start address of the string
	@ R1: string length (return)
	ldrb R2, [R0]
	cmp  R2, 0x0
	it   eq
	bxeq lr
	add  R0, 0x1
	add  R1, 0x1
	b    strlen

atoi:
	@ R0: start address of the string
	@     start address of the next token (return)
	@ R1: integer value (return)
	ldrb R1, [R0]
	mov  R2, 0x0  @ is_negative flag
	cmp  R1, 0x2B @ '+'
	beq  atoi_pos
	cmp  R1, 0x2D @ '-'
	beq  atoi_neg
	cmp  R1, 0x0
	beq  error
	mov  R1, 0x0
	b    atoi_inner

atoi_pos:
	@ called by atoi
	add  R0, 0x1
	mov  R1, 0x0
	b    atoi_inner

atoi_neg:
	@ called by atoi
	add  R0, 0x1
	mov  R1, 0x0
	mov  R2, 0x1
	b    atoi_inner

atoi_inner:
	@ called by atoi
	ldrb R3, [R0]
	cmp  R3, 0x0
	beq  atoi_end
	cmp  R3, 0x3A @ character after '9'
	bge  error
	cmp  R3, 0x2F @ character before '0'
	ble  error
	sub  R3, 0x30 @ '0'
	mov  R9, 0xA
	mul  R1, R9
	add  R1, R3
	add  R0, 0x1
	b    atoi_inner

atoi_end:
	@ called by atoi
	add  R0, 0x1
	cmp  R2, 0x0
	bne  additive_inverse
	bx   lr

additive_inverse:
	@ called by atoi
	mov  R9, 0x0
	sub  R1, R9, R1
	bx   lr

error:
	@ error handling
	ldr  R0, =expr_result
	ldr  R1, =0xFFFFFFFF
	str  R1, [R0]
	b    program_end

program_end:
	@ infinate loop
	b    program_end
```

### 求最大公因數並計算最多用了多少 Stack Size ###

```assembly
	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	result:   .word 0
	max_size: .word 0

.text
	.global main
	m: .word 0x5E
	n: .word 0x60

main:
	ldr  r2, =m
	ldr  r0, [r2]
	ldr  r3, =n
	ldr  r1, [r3]

	mov  r11, sp       @ r11: start of stack
	mov  r10, 0x0      @ r10: max stack size

	push {r0, r1, lr}
	bl   gcd
	pop  {r0, r1, r9}

	ldr  r3, =result
	str  r2, [r3]

	ldr  r3, =max_size
	str  r10, [r3]

	b    forever

gcd:
	ldr  r0, [sp]      @ param a
	ldr  r1, [sp, 0x4] @ param b

	mov  r7, sp
	sub  r8, r11, r7
	cmp  r8, r10
	bgt  update_r10
	b    gcd_final

update_r10:
	mov  r10, r8

gcd_final:
	cmp  r0, 0x0
	beq  return_b

	cmp  r1, 0x0
	beq  return_a

	mov  r2, 0x1
	and  r3, r0, r2
	and  r4, r1, r2

	mov  r8, 0x1
	eor  r3, r8        @ r3: bool, a is even
	eor  r4, r8        @ r4: bool, b is even

	ands r2, r3, r4
	bne  case_1

	cmp  r3, 0x0
	bne  case_2

	cmp  r4, 0x0
	bne  case_3

	b    case_4

return_a:
	mov  r2, r0
	bx   lr

return_b:
	mov  r2, r1
	bx   lr

case_1:
	asr  r0, r0, 0x1
	asr  r1, r1, 0x1
	push {r0, r1, lr}
	bl   gcd
	pop  {r0, r1, r9}
	mov  r8, 0x2
	mul  r2, r8
	mov  pc, r9

case_2:
	asr  r0, r0, 0x1
	push {r0, r1, lr}
	bl   gcd
	pop  {r0, r1, r9}
	mov  pc, r9

case_3:
	asr  r1, r1, 0x1
	push {r0, r1, lr}
	bl   gcd
	pop  {r0, r1, r9}
	mov  pc, r9

case_4:
	cmp r0, r1
	bgt a_is_bigger
	sub r2, r1, r0     @ b - a
	mov r1, r0
	mov r0, r2
	b   case_4_final

a_is_bigger:
	sub r0, r1         @ a - b

case_4_final:
	push {r0, r1, lr}
	bl   gcd
	pop  {r0, r1, r9}
	mov  pc, r9

forever:
	b    forever
```

## 實驗結果與問題回答 ##

### Postfix Arithmetic ###

![p1a](p1a.png)

1. 程式執行結束後 `r2` 值為多少？如何觀察？  
   值為多少：`0x69`。  
   如何觀察：進入 debug 模式，讓程式執行到最後，用眼睛看右上角的 "Registers" 視窗。

### 求最大公因數並計算最多用了多少 Stack Size ###

![p2a](p2a.png)

觀察 `X` 內容值變化：  
初始值為 `0x64 (=100)`，第 18 行執行後變為 `0xb9`。

1. 變數 `X` 與 `str` 的初始值是由誰在何處初始化的？  
   由誰：微控制器啟動（或 reset）時執行的程式。  
   在何處：`startup_stm32.s` 中的 `LoopCopyDataInit` 與 `CopyDataInit` 標籤。

2. 若將 `X` 宣告改在 text section 對其程式執行結果會有何改變？  
   `X` 的位址會改變（從 data section 移至 text section）。  
   `str` 的位址會改變（往前移四個位元組）。  
   因為 text section 是唯讀的，所以第 18 行（將值寫入 `X` 的位址）無效。

3. 程式執行完畢後 `r2` 內容與 `str` 字串在 memory 前 4 個 byte 呈現內容有何差異？  
   `r2` 的值是 `0x6c6c6548`，也就是 "lleH"。  
   `str` 的前四個位元組是 `0x48656c6c`，也就是 "Hell"。  
   （所以這個微控制器是 little-endian。）

4. 變數 `str` "Hello World!" 有無其他種宣告方式？若有請說明其中一種。  
   有，因為變數的記憶體位址是連續的，所以可以將第 7 行改為：  
   ```assembly
   str:  .word 0x6c6c6548 @ "lleH"
   str1: .word 0x6f57206f @ "oW o"
   str2: .word 0x21646c72 @ "!dlr"
   str3: .byte 0x00       @ "\0"
   ```

## 心得討論與應用聯想 ##

* 這套開發工具雖然有 Linux 版，但是在 Ubuntu 16.04 LTS 下並不會 work。
* 原來 ARM 的組合語言的註解是用 "@" 符號。
* 指令是 case-insensitive。
