#ifndef _INTER_H_
#define _INTER_H_



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>


#include "registre.h"
#include "memory.h"
#include "breakpoint.h"

#include "common/types.h"
#include "common/bits.h"
#include "common/notify.h"
#include "elf/elf.h"
#include "elf/syms.h"
#include "elf/relocator.h"



/* prompt du mode shell interactif */
#define PROMPT_STRING "ARMShell : > "

/* taille max pour nos chaines de char */
#define MAX_STR 1024


/*************************************************************\
Valeur de retour speciales pour la fonction
	int execute_cmd(interpreteur inter) ;
Toute autre valeur signifie qu'une erreur est survenue
 \*************************************************************/
#define CMD_OK_RETURN_VALUE 0
#define CMD_EXIT_RETURN_VALUE -1
#define CMD_UNKOWN_RETURN_VALUE -2



// On fixe ici une adresse basse dans la mémoire virtuelle. Le premier segment
// ira se loger à cette adresse.
#define START_MEM 0x3000

// nombre max de sections que l'on extraira du fichier ELF
#define NB_SECTIONS 4
#define PLACESTHEAP 4

// nom de chaque section
#define TEXT_SECTION_STR ".text"
#define RODATA_SECTION_STR ".rodata"
#define DATA_SECTION_STR ".data"
#define BSS_SECTION_STR ".bss"

// fonction affichant les octets d'un segment sur la sortie standard
// parametres :
//   name           : nom de la section dont le contenue est affichée
//   start          : adresse virtuelle d'implantation du contenu de la section dans la memoire
//   content        : le contenu de la section à afficher
//   taille         : taille en octet de la section à afficher


//--------------------------------------------------
/* type de token (exemple) */
enum {HEXA32,HEXA8,DECI32,DECI8,REG,STATEREG,UNKNOWN};

/* mode d'interaction avec l'interpreteur (exemple)*/
typedef enum {INTERACTIF,SCRIPT,DEBUG_MODE} inter_mode;

/* structure passée en parametre qui contient la connaissance de l'état de
 * l'interpréteur
 */
typedef struct {
    inter_mode mode;
    char input[MAX_STR];
    char * from;
    char first_token;
    MAPMEM memory;
    REGISTRE ** fulltable;
    BREAKPOINT breaklist;
  //TYPE_INST* dico32;
  //TYPE_INST* dico16;
} * interpreteur;


//Fonction associées à l'interpreteur---------------------------------------
interpreteur init_inter(void) ;
void del_inter(interpreteur inter);
char* get_next_token(interpreteur inter);
char* tolower_string(char* s,int n_max);

int is_hexa(char* chaine)	;
int is_hexa8(char* chaine)	;
int is_hexa32(char* chaine)	;
int is_deci32(char* chaine)	;
int is_deci8(char* chaine)	;
int is_deci(char* chaine)	;  //-----------------------> A SUPPRIMER
int is_reg(char *token)		;
int is_state_reg(char *token);
//int is_range(char* token)	;
int get_type(char* chaine)	;

int _testcmd(int hexValue) ;
int testcmd(interpreteur inter);
int exitcmd(interpreteur inter) ;
int loadcmd(interpreteur inter) ;
int dispcmd(interpreteur inter) ;
int disasmcmd(interpreteur inter) ;
int setcmd(interpreteur inter) ;
int assertcmd(interpreteur inter) ;
int debugcmd(interpreteur inter) ;
int resumecmd(interpreteur inter) ;
int runcmd(interpreteur inter) ;
int stepcmd(interpreteur inter) ;
int breakcmd(interpreteur inter);
int helpcmd(void);


int execute_cmd(interpreteur inter);



#endif
