#ifndef _DESASM_H_
#define _DESASM_H_


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "projetemularm.h"
#include "memory.h"
#include "registre.h"

#define DICO32 "dicos/dicos_champs/Dico32.txt"
#define DICO16 "dicos/dicos_champs/Dico16.txt"
#define NB 1


typedef struct 
{
	char identifiant[16];
	char mnemo[16];
	int taille;
	unsigned int masque, signature;
	int nb_operande;					// On se limite à deux opérandes pour le moment
	int typeop1;
	int typeop2;
	int typeop3;
	int typeop4;
	char champop1[20];
	char champop2[20];
	char champop3[20];
	char champop4[20];
	
}TYPE_INST;


//Fonction desassemblage

int is_32(char*instr_ptr);
unsigned int immediate (char* s ,unsigned int code);
char* registre_extract(char* s , unsigned int code);
int _desasm_cmd(SEGMENT seg, unsigned int adrdep , unsigned int adrarr);


#endif
