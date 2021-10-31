#ifndef _FORMAT_OP_H_
#define _FORMAT_OP_H_

//#include "projetemularm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "notify.h"


typedef struct {
    unsigned int imm;    // Contient la valeur brut
    char bits;           // Nb de bits dans la valeur au-dessus
} *IMMEDIATE;




typedef enum { SRType_LSL, SRType_LSR, SRType_ASR, SRType_RRX, SRType_ROR } SRType;



// Fonctions de traitement de valeurs immédiates.

void affiche_immediate(IMMEDIATE  value);

IMMEDIATE immediate(char* s ,unsigned int code);

void decodeIMMshift(IMMEDIATE typebrut,IMMEDIATE imm5, int* shift_t , int* shift_n);

IMMEDIATE thumbexpandIMM(IMMEDIATE imm12);

IMMEDIATE thumbexpandIMM_c(IMMEDIATE imm12,unsigned int* carry);

IMMEDIATE concatenate32(IMMEDIATE valbrut);

IMMEDIATE LSL(IMMEDIATE value , int shift);

IMMEDIATE LSL_C(IMMEDIATE value, int shift,unsigned int* carry);

IMMEDIATE LSR(IMMEDIATE value, int shift);

IMMEDIATE LSR_C(IMMEDIATE value, int shift,unsigned int* carry);

IMMEDIATE ASR( IMMEDIATE value,int shift);

IMMEDIATE ASR_C(IMMEDIATE value,int shift,unsigned int* carry);                                    //Arithmetic shift without car

IMMEDIATE ROR_C(IMMEDIATE value,unsigned int shift,unsigned int* carry);

IMMEDIATE ROR(IMMEDIATE value,unsigned int shift);

IMMEDIATE RRX_C(IMMEDIATE value,unsigned int*carry);

IMMEDIATE RRX(IMMEDIATE value,unsigned int* carry);


#endif
