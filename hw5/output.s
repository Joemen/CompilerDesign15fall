.data
_g_k: .word 0
.text
_start_try:
str x30, [sp, #0]
str x29, [sp, #-8]
add x29, sp, #-8
add sp, sp, #-16
ldr x30, =_frameSize_try
ldr x30, [x30, #0]
sub sp, sp, w30
str x9, [sp, #8]
str x10, [sp, #16]
str x11, [sp, #24]
str x12, [sp, #32]
str x13, [sp, #40]
str x14, [sp, #48]
str x15, [sp, #56]
str s16, [sp, #64]
str s17, [sp, #68]
str s18, [sp, #72]
str s19, [sp, #76]
str s20, [sp, #80]
str s21, [sp, #84]
str s22, [sp, #88]
str s23, [sp, #92]
ldr x9, =_g_k
ldr w10, [x9, #0]
.data
_CONSTANT_0: .word 10
.align 3
.text
ldr w9, _CONSTANT_0
cmp w10, w9
cset w11, lt
ldr x12, =_g_k
str w10, [x12, #0]
cmp w11, #0
beq _if_end_0
ldr x9, =_g_k
ldr w10, [x9, #0]
mov w0, w10
bl _write_int
ldr x9, =_g_k
str w10, [x9, #0]
.data
_CONSTANT_1: .ascii "\n\000"
.align 3
.text
ldr x9, =_CONSTANT_1
mov x0, x9
bl _write_str
ldr x9, =_g_k
ldr w10, [x9, #0]
.data
_CONSTANT_2: .word 1
.align 3
.text
ldr w9, _CONSTANT_2
add w12, w10, w9
ldr x13, =_g_k
str w10, [x13, #0]
ldr x9, =_g_k
ldr w10, [x9, #0]
mov w10, w12
ldr x9, =_g_k
str w10, [x9, #0]
bl _start_try
b _else_end_0
_if_end_0:
_else_end_0:
_end_try:
ldr x9, [sp, #8]
ldr x10, [sp, #16]
ldr x11, [sp, #24]
ldr x12, [sp, #32]
ldr x13, [sp, #40]
ldr x14, [sp, #48]
ldr x15, [sp, #56]
ldr s16, [sp, #64]
ldr s17, [sp, #68]
ldr s18, [sp, #72]
ldr s19, [sp, #76]
ldr s20, [sp, #80]
ldr s21, [sp, #84]
ldr s22, [sp, #88]
ldr s23, [sp, #92]
ldr x30, [x29, #8]
mov sp, x29
add sp, sp, #8
ldr x29, [x29,#0]
RET x30
.data
_frameSize_try: .word 92
.text
_start_MAIN:
str x30, [sp, #0]
str x29, [sp, #-8]
add x29, sp, #-8
add sp, sp, #-16
ldr x30, =_frameSize_MAIN
ldr x30, [x30, #0]
sub sp, sp, w30
str x9, [sp, #8]
str x10, [sp, #16]
str x11, [sp, #24]
str x12, [sp, #32]
str x13, [sp, #40]
str x14, [sp, #48]
str x15, [sp, #56]
str s16, [sp, #64]
str s17, [sp, #68]
str s18, [sp, #72]
str s19, [sp, #76]
str s20, [sp, #80]
str s21, [sp, #84]
str s22, [sp, #88]
str s23, [sp, #92]
.data
_CONSTANT_3: .word 0
.align 3
.text
ldr w9, _CONSTANT_3
ldr x10, =_g_k
ldr w12, [x10, #0]
mov w12, w9
ldr x9, =_g_k
str w12, [x9, #0]
bl _start_try
.data
_CONSTANT_4: .word 0
.align 3
.text
ldr w9, _CONSTANT_4
mov w0, w9
b _end_MAIN
_end_MAIN:
ldr x9, [sp, #8]
ldr x10, [sp, #16]
ldr x11, [sp, #24]
ldr x12, [sp, #32]
ldr x13, [sp, #40]
ldr x14, [sp, #48]
ldr x15, [sp, #56]
ldr s16, [sp, #64]
ldr s17, [sp, #68]
ldr s18, [sp, #72]
ldr s19, [sp, #76]
ldr s20, [sp, #80]
ldr s21, [sp, #84]
ldr s22, [sp, #88]
ldr s23, [sp, #92]
ldr x30, [x29, #8]
mov sp, x29
add sp, sp, #8
ldr x29, [x29,#0]
RET x30
.data
_frameSize_MAIN: .word 92
_start_read:
b _read_int
_start_fread:
b _read_float
