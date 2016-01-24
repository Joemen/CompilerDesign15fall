.data
_g_dim: .word 2
_g_a: .space 32
_g_b: .space 32
_g_c: .space 32
.text
_start_print:
str x30, [sp, #0]
str x29, [sp, #-8]
add x29, sp, #-8
add sp, sp, #-16
ldr x30, =_frameSize_print
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
_for_start_0:
.data
_CONSTANT_0: .word 0
.align 3
.text
ldr w9, _CONSTANT_0
ldr w10, [x29, #-4]
mov w10, w9
str w10, [x29, #-4]
_for_loop_0:
ldr w9, [x29, #-4]
ldr x11, =_g_dim
ldr w10, [x11, #0]
cmp w9, w10
cset w11, lt
str w9, [x29, #-4]
ldr x9, =_g_dim
str w10, [x9, #0]
cmp w11, #0
beq _for_end_0
_for_start_1:
.data
_CONSTANT_1: .word 0
.align 3
.text
ldr w9, _CONSTANT_1
ldr w10, [x29, #-8]
mov w10, w9
str w10, [x29, #-8]
_for_loop_1:
ldr w9, [x29, #-8]
ldr x12, =_g_dim
ldr w10, [x12, #0]
cmp w9, w10
cset w12, lt
str w9, [x29, #-8]
ldr x9, =_g_dim
str w10, [x9, #0]
cmp w12, #0
beq _for_end_1
_for_start_2:
.data
_CONSTANT_2: .word 0
.align 3
.text
ldr w9, _CONSTANT_2
ldr w10, [x29, #-12]
mov w10, w9
str w10, [x29, #-12]
_for_loop_2:
ldr w9, [x29, #-12]
ldr x13, =_g_dim
ldr w10, [x13, #0]
cmp w9, w10
cset w13, lt
str w9, [x29, #-12]
ldr x9, =_g_dim
str w10, [x9, #0]
cmp w13, #0
beq _for_end_2
.data
_CONSTANT_3: .word 0
.align 3
.text
ldr w9, _CONSTANT_3
.data
_CONSTANT_4: .word 2
.align 3
.text
ldr w10, _CONSTANT_4
mul w9, w9, w10
ldr w10, [x29, #-4]
add w9, w9, w10
str w10, [x29, #-4]
.data
_CONSTANT_5: .word 2
.align 3
.text
ldr w10, _CONSTANT_5
mul w9, w9, w10
ldr w10, [x29, #-8]
add w9, w9, w10
str w10, [x29, #-8]
.data
_CONSTANT_6: .word 2
.align 3
.text
ldr w10, _CONSTANT_6
mul w9, w9, w10
ldr w10, [x29, #-12]
add w9, w9, w10
str w10, [x29, #-12]
ldr x10, =_g_c
lsl w9, w9, #2
add x14, x10, w9, sxtw
ldr w15, [x14, #0]
mov w0, w15
bl _write_int
.data
_CONSTANT_8: .ascii " \000"
.align 3
.text
ldr x9, =_CONSTANT_8
mov x0, x9
bl _write_str
ldr w9, [x29, #-12]
.data
_CONSTANT_9: .word 1
.align 3
.text
ldr w14, _CONSTANT_9
add w15, w9, w14
str w9, [x29, #-12]
ldr w9, [x29, #-12]
mov w9, w15
str w9, [x29, #-12]
b _for_loop_2
_for_end_2:
.data
_CONSTANT_10: .ascii "\n\000"
.align 3
.text
ldr x9, =_CONSTANT_10
mov x0, x9
bl _write_str
ldr w9, [x29, #-8]
.data
_CONSTANT_11: .word 1
.align 3
.text
ldr w14, _CONSTANT_11
add w15, w9, w14
str w9, [x29, #-8]
ldr w9, [x29, #-8]
mov w9, w15
str w9, [x29, #-8]
b _for_loop_1
_for_end_1:
.data
_CONSTANT_12: .ascii "\n\000"
.align 3
.text
ldr x9, =_CONSTANT_12
mov x0, x9
bl _write_str
ldr w9, [x29, #-4]
.data
_CONSTANT_13: .word 1
.align 3
.text
ldr w14, _CONSTANT_13
add w15, w9, w14
str w9, [x29, #-4]
ldr w9, [x29, #-4]
mov w9, w15
str w9, [x29, #-4]
b _for_loop_0
_for_end_0:
_end_print:
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
_frameSize_print: .word 104
.text
_start_arraymult:
str x30, [sp, #0]
str x29, [sp, #-8]
add x29, sp, #-8
add sp, sp, #-16
ldr x30, =_frameSize_arraymult
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
_for_start_3:
.data
_CONSTANT_14: .word 0
.align 3
.text
ldr w9, _CONSTANT_14
ldr w10, [x29, #-4]
mov w10, w9
str w10, [x29, #-4]
_for_loop_3:
ldr w9, [x29, #-4]
ldr x11, =_g_dim
ldr w10, [x11, #0]
cmp w9, w10
cset w11, lt
str w9, [x29, #-4]
ldr x9, =_g_dim
str w10, [x9, #0]
cmp w11, #0
beq _for_end_3
_for_start_4:
.data
_CONSTANT_15: .word 0
.align 3
.text
ldr w9, _CONSTANT_15
ldr w10, [x29, #-8]
mov w10, w9
str w10, [x29, #-8]
_for_loop_4:
ldr w9, [x29, #-8]
ldr x12, =_g_dim
ldr w10, [x12, #0]
cmp w9, w10
cset w12, lt
str w9, [x29, #-8]
ldr x9, =_g_dim
str w10, [x9, #0]
cmp w12, #0
beq _for_end_4
_for_start_5:
.data
_CONSTANT_16: .word 0
.align 3
.text
ldr w9, _CONSTANT_16
ldr w10, [x29, #-12]
mov w10, w9
str w10, [x29, #-12]
_for_loop_5:
ldr w9, [x29, #-12]
ldr x13, =_g_dim
ldr w10, [x13, #0]
cmp w9, w10
cset w13, lt
str w9, [x29, #-12]
ldr x9, =_g_dim
str w10, [x9, #0]
cmp w13, #0
beq _for_end_5
.data
_CONSTANT_17: .word 0
.align 3
.text
ldr w9, _CONSTANT_17
.data
_CONSTANT_18: .word 2
.align 3
.text
ldr w10, _CONSTANT_18
mul w9, w9, w10
ldr w10, [x29, #-4]
add w9, w9, w10
str w10, [x29, #-4]
.data
_CONSTANT_19: .word 2
.align 3
.text
ldr w10, _CONSTANT_19
mul w9, w9, w10
ldr w10, [x29, #-8]
add w9, w9, w10
str w10, [x29, #-8]
.data
_CONSTANT_20: .word 2
.align 3
.text
ldr w10, _CONSTANT_20
mul w9, w9, w10
ldr w10, [x29, #-12]
add w9, w9, w10
str w10, [x29, #-12]
ldr x10, =_g_a
lsl w9, w9, #2
add x14, x10, w9, sxtw
ldr w15, [x14, #0]
.data
_CONSTANT_22: .word 0
.align 3
.text
ldr w9, _CONSTANT_22
.data
_CONSTANT_23: .word 2
.align 3
.text
ldr w14, _CONSTANT_23
mul w9, w9, w14
ldr w14, [x29, #-4]
add w9, w9, w14
str w14, [x29, #-4]
.data
_CONSTANT_24: .word 2
.align 3
.text
ldr w14, _CONSTANT_24
mul w9, w9, w14
ldr w14, [x29, #-8]
add w9, w9, w14
str w14, [x29, #-8]
.data
_CONSTANT_25: .word 2
.align 3
.text
ldr w14, _CONSTANT_25
mul w9, w9, w14
ldr w14, [x29, #-12]
add w9, w9, w14
str w14, [x29, #-12]
ldr x14, =_g_b
lsl w9, w9, #2
add x16, x14, w9, sxtw
ldr w17, [x16, #0]
mul w9, w15, w17
.data
_CONSTANT_27: .word 0
.align 3
.text
ldr w15, _CONSTANT_27
.data
_CONSTANT_28: .word 2
.align 3
.text
ldr w16, _CONSTANT_28
mul w15, w15, w16
ldr w16, [x29, #-4]
add w15, w15, w16
str w16, [x29, #-4]
.data
_CONSTANT_29: .word 2
.align 3
.text
ldr w16, _CONSTANT_29
mul w15, w15, w16
ldr w16, [x29, #-8]
add w15, w15, w16
str w16, [x29, #-8]
.data
_CONSTANT_30: .word 2
.align 3
.text
ldr w16, _CONSTANT_30
mul w15, w15, w16
ldr w16, [x29, #-12]
add w15, w15, w16
str w16, [x29, #-12]
ldr x16, =_g_c
lsl w15, w15, #2
add x17, x16, w15, sxtw
str w9, [x17, #0]
ldr w9, [x29, #-12]
.data
_CONSTANT_32: .word 1
.align 3
.text
ldr w15, _CONSTANT_32
add w17, w9, w15
str w9, [x29, #-12]
ldr w9, [x29, #-12]
mov w9, w17
str w9, [x29, #-12]
b _for_loop_5
_for_end_5:
ldr w9, [x29, #-8]
.data
_CONSTANT_33: .word 1
.align 3
.text
ldr w15, _CONSTANT_33
add w17, w9, w15
str w9, [x29, #-8]
ldr w9, [x29, #-8]
mov w9, w17
str w9, [x29, #-8]
b _for_loop_4
_for_end_4:
ldr w9, [x29, #-4]
.data
_CONSTANT_34: .word 1
.align 3
.text
ldr w15, _CONSTANT_34
add w17, w9, w15
str w9, [x29, #-4]
ldr w9, [x29, #-4]
mov w9, w17
str w9, [x29, #-4]
b _for_loop_3
_for_end_3:
str x16, [x29, #-20]
bl _start_print
ldr x16, [x29, #-20]
_end_arraymult:
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
_frameSize_arraymult: .word 112
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
_CONSTANT_35: .ascii "Enter matrix 1 of dim 2 x 2 x 2: \n\000"
.align 3
.text
ldr x9, =_CONSTANT_35
mov x0, x9
bl _write_str
_for_start_6:
.data
_CONSTANT_36: .word 0
.align 3
.text
ldr w9, _CONSTANT_36
ldr w10, [x29, #-4]
mov w10, w9
str w10, [x29, #-4]
_for_loop_6:
ldr w9, [x29, #-4]
ldr x11, =_g_dim
ldr w10, [x11, #0]
cmp w9, w10
cset w11, lt
str w9, [x29, #-4]
ldr x9, =_g_dim
str w10, [x9, #0]
cmp w11, #0
beq _for_end_6
_for_start_7:
.data
_CONSTANT_37: .word 0
.align 3
.text
ldr w9, _CONSTANT_37
ldr w10, [x29, #-8]
mov w10, w9
str w10, [x29, #-8]
_for_loop_7:
ldr w9, [x29, #-8]
ldr x12, =_g_dim
ldr w10, [x12, #0]
cmp w9, w10
cset w12, lt
str w9, [x29, #-8]
ldr x9, =_g_dim
str w10, [x9, #0]
cmp w12, #0
beq _for_end_7
_for_start_8:
.data
_CONSTANT_38: .word 0
.align 3
.text
ldr w9, _CONSTANT_38
ldr w10, [x29, #-12]
mov w10, w9
str w10, [x29, #-12]
_for_loop_8:
ldr w9, [x29, #-12]
ldr x13, =_g_dim
ldr w10, [x13, #0]
cmp w9, w10
cset w13, lt
str w9, [x29, #-12]
ldr x9, =_g_dim
str w10, [x9, #0]
cmp w13, #0
beq _for_end_8
bl _read_int
mov w9, w0
.data
_CONSTANT_39: .word 0
.align 3
.text
ldr w10, _CONSTANT_39
.data
_CONSTANT_40: .word 2
.align 3
.text
ldr w14, _CONSTANT_40
mul w10, w10, w14
ldr w14, [x29, #-4]
add w10, w10, w14
str w14, [x29, #-4]
.data
_CONSTANT_41: .word 2
.align 3
.text
ldr w14, _CONSTANT_41
mul w10, w10, w14
ldr w14, [x29, #-8]
add w10, w10, w14
str w14, [x29, #-8]
.data
_CONSTANT_42: .word 2
.align 3
.text
ldr w14, _CONSTANT_42
mul w10, w10, w14
ldr w14, [x29, #-12]
add w10, w10, w14
str w14, [x29, #-12]
ldr x14, =_g_a
lsl w10, w10, #2
add x15, x14, w10, sxtw
str w9, [x15, #0]
ldr w9, [x29, #-12]
.data
_CONSTANT_44: .word 1
.align 3
.text
ldr w10, _CONSTANT_44
add w15, w9, w10
str w9, [x29, #-12]
ldr w9, [x29, #-12]
mov w9, w15
str w9, [x29, #-12]
b _for_loop_8
_for_end_8:
ldr w9, [x29, #-8]
.data
_CONSTANT_45: .word 1
.align 3
.text
ldr w10, _CONSTANT_45
add w15, w9, w10
str w9, [x29, #-8]
ldr w9, [x29, #-8]
mov w9, w15
str w9, [x29, #-8]
b _for_loop_7
_for_end_7:
ldr w9, [x29, #-4]
.data
_CONSTANT_46: .word 1
.align 3
.text
ldr w10, _CONSTANT_46
add w15, w9, w10
str w9, [x29, #-4]
ldr w9, [x29, #-4]
mov w9, w15
str w9, [x29, #-4]
b _for_loop_6
_for_end_6:
.data
_CONSTANT_47: .ascii "Enter matrix 2 of dim 2 x 2 x 2 : \n\000"
.align 3
.text
ldr x9, =_CONSTANT_47
mov x0, x9
bl _write_str
_for_start_9:
.data
_CONSTANT_48: .word 0
.align 3
.text
ldr w9, _CONSTANT_48
ldr w10, [x29, #-4]
mov w10, w9
str w10, [x29, #-4]
_for_loop_9:
ldr w9, [x29, #-4]
ldr x15, =_g_dim
ldr w10, [x15, #0]
cmp w9, w10
cset w15, lt
str w9, [x29, #-4]
ldr x9, =_g_dim
str w10, [x9, #0]
cmp w15, #0
beq _for_end_9
_for_start_10:
.data
_CONSTANT_49: .word 0
.align 3
.text
ldr w9, _CONSTANT_49
ldr w10, [x29, #-8]
mov w10, w9
str w10, [x29, #-8]
_for_loop_10:
ldr w9, [x29, #-8]
ldr x16, =_g_dim
ldr w10, [x16, #0]
cmp w9, w10
cset w16, lt
str w9, [x29, #-8]
ldr x9, =_g_dim
str w10, [x9, #0]
cmp w16, #0
beq _for_end_10
_for_start_11:
.data
_CONSTANT_50: .word 0
.align 3
.text
ldr w9, _CONSTANT_50
ldr w10, [x29, #-12]
mov w10, w9
str w10, [x29, #-12]
_for_loop_11:
ldr w9, [x29, #-12]
ldr x17, =_g_dim
ldr w10, [x17, #0]
cmp w9, w10
cset w17, lt
str w9, [x29, #-12]
ldr x9, =_g_dim
str w10, [x9, #0]
cmp w17, #0
beq _for_end_11
bl _read_int
mov w9, w0
.data
_CONSTANT_51: .word 0
.align 3
.text
ldr w10, _CONSTANT_51
.data
_CONSTANT_52: .word 2
.align 3
.text
ldr w18, _CONSTANT_52
mul w10, w10, w18
ldr w18, [x29, #-4]
add w10, w10, w18
str w18, [x29, #-4]
.data
_CONSTANT_53: .word 2
.align 3
.text
ldr w18, _CONSTANT_53
mul w10, w10, w18
ldr w18, [x29, #-8]
add w10, w10, w18
str w18, [x29, #-8]
.data
_CONSTANT_54: .word 2
.align 3
.text
ldr w18, _CONSTANT_54
mul w10, w10, w18
ldr w18, [x29, #-12]
add w10, w10, w18
str w18, [x29, #-12]
ldr x18, =_g_b
lsl w10, w10, #2
add x19, x18, w10, sxtw
str w9, [x19, #0]
ldr w9, [x29, #-12]
.data
_CONSTANT_56: .word 1
.align 3
.text
ldr w10, _CONSTANT_56
add w19, w9, w10
str w9, [x29, #-12]
ldr w9, [x29, #-12]
mov w9, w19
str w9, [x29, #-12]
b _for_loop_11
_for_end_11:
ldr w9, [x29, #-8]
.data
_CONSTANT_57: .word 1
.align 3
.text
ldr w10, _CONSTANT_57
add w19, w9, w10
str w9, [x29, #-8]
ldr w9, [x29, #-8]
mov w9, w19
str w9, [x29, #-8]
b _for_loop_10
_for_end_10:
ldr w9, [x29, #-4]
.data
_CONSTANT_58: .word 1
.align 3
.text
ldr w10, _CONSTANT_58
add w19, w9, w10
str w9, [x29, #-4]
ldr w9, [x29, #-4]
mov w9, w19
str w9, [x29, #-4]
b _for_loop_9
_for_end_9:
str x18, [x29, #-20]
bl _start_arraymult
ldr x18, [x29, #-20]
.data
_CONSTANT_59: .word 0
.align 3
.text
ldr w9, _CONSTANT_59
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
_frameSize_MAIN: .word 112
_start_read:
b _read_int
_start_fread:
b _read_float
