#include "header.h"

typedef enum Register
{
    w0, w1, w2, w3, w4, w5, w6, w7, w8, w9,
    w10, w11, w12, w13, w14, w15, w16, w17, w18, w19,
    w20, w21, w22, w23, w24, w25, w26, w27, w28, w29, w30,
    x0, x1, x2, x3, x4, x5, x6, x7, x8, x9,
    x10, x11, x12, x13, x14, x15, x16, x17, x18, x19,
    x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30,
    s0, s1, s2, s3, s4, s5, s6, s7, s8, s9,
    s10, s11, s12, s13, s14, s15, s16, s17, s18, s19,
    s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31
} Register;
typedef enum RegisterTableState{ FREE, CLEAN, DIRTY} RegisterTableState;
typedef enum RegType{W, X, S} RegType;
char* regName[94] = {"w0", "w1", "w2", "w3", "w4", "w5", "w6", "w7", "w8", "w9",
    "w10", "w11", "w12", "w13", "w14", "w15", "w16", "w17", "w18", "w19",
    "w20", "w21", "w22", "w23", "w24", "w25", "w26", "w27", "w28", "w29", "w30",
    "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9",
    "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x18", "x19",
    "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "x29", "x30",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9",
    "s10", "s11", "s12", "s13", "s14", "s15", "s16", "s17", "s18", "s19",
    "s20", "s21", "s22", "s23", "s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31"};


typedef struct RegisterTableEntry
{
    AST_NODE* node;
    RegType regType;
    RegisterTableState state;
} RegisterTableEntry;
