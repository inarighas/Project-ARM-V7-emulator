#ifndef _DESASM_H_
#define _DESASM_H_


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "projetemularm.h"
#include "memory.h"
#include "registre.h"


#define DICO32 "dicos/dicos_champs/Dico32.txt"
#define DICO16 "dicos/dicos_champs/Dico16.txt"
#define NB 1

#define EQ 0x0
#define NE 0x1
#define CS 0x2
#define CC 0x3
#define MI 0x4
#define PL 0x5
#define VS 0x6
#define VC 0x7
#define HI 0x8
#define LS 0x9
#define GE 0xA
#define LT 0xB
#define GT 0xC
#define LE 0xD
#define AL 0xE


typedef enum{NONE,RGSTR,IMMEDIATE32,IMMEDIATE32P00,THEXPIMM,THEXPIMM_C,DECIMMSHFT,SGNEXTP0,SGNEXT_B} TYPE_OPERANDE;
/*
 NONE        0
 RGSTR       1
IMMEDIATE3   2
,IMMEDIAT0   3
,THEXPIMM    4
,THEXPIMM_C  5
,DECIMMSHFT  6
,SGNEXTP0    7
,SGNEXT_B    8
*/
typedef struct {
    unsigned int imm;    // Contient la valeur brut
    char bits;           // Nb de bits dans la valeur au-dessus
} *IMMEDIATE;



typedef union{
  IMMEDIATE unsigned_value;
  IMMEDIATE signed_value;					//IMMEDIATE BE Carefull!!!
  char* register_name;
} *OPERANDE;


typedef struct
{
    char identifiant[16];
    char mnemo[16];
    int taille;
    unsigned int masque, signature;
    int nb_operande;					// On se limite à deux opérandes pour le moment
    int typeop[4];
    char champop[4][20];

} TYPE_INST;


typedef struct
{   unsigned int code;
    unsigned char setflag, shift, blockIT,condition;
    OPERANDE op[4];
    TYPE_INST *inst;

} DESASM_INST;


//Fonction desassemblage
 
int is_32(char*instr_ptr);
unsigned int extract_uint(char* s ,unsigned int code);
DESASM_INST* get_operande(DESASM_INST* stockage_inst , int indice,int indice_operande, unsigned int code);
DESASM_INST* lecture_txt(SEGMENT seg);
void cond(TYPE_INST *instruction, unsigned int code);
void IT(DESASM_INST inst, unsigned int code);
char* registre_extract(char* s , unsigned int code);
int _desasm_cmd(SEGMENT seg, unsigned int adrdep , unsigned int adrarr);


#endif
