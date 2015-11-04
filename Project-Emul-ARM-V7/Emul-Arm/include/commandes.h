#ifndef _COMMANDES_H_
#define _COMMANDES_H_
 

#include "projetemularm.h"



void print_section_raw_content(char* name, unsigned int start, byte* content, unsigned int taille) ;

int _loadcmd(char *fichier_elf, interpreteur inter);

int _dispcmd(char*string, interpreteur inter, char * adrdep, char*adrarr);





#endif
