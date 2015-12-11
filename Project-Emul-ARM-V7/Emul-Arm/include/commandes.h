#ifndef _COMMANDES_H_
#define _COMMANDES_H_


#include "projetemularm.h"



void print_section_raw_content(char* name, unsigned int start, byte* content, unsigned int taille) ;

int _loadcmd(char *fichier_elf, interpreteur inter);

int _dispcmd(char*string, interpreteur inter, char * adrdep, char*adrarr);

int _assert_cmd(char* string, char* type , unsigned int val);


void _set_reg_cmd(interpreteur inter, char*reg, unsigned int valeur);
void _set_cmd_apsr(interpreteur inter, char*reg);
void _set_mem_byte_cmd(interpreteur inter, unsigned int adresse, char valeur);
void _set_mem_word_cmd(interpreteur inter, unsigned int adresse, unsigned int valeur);

int _breakcmd_add(interpreteur inter, unsigned int adr);
int _breakcmd_del(BREAKPOINT* list, unsigned int adr);
int _breakcmd_del_all(BREAKPOINT* list);

#endif
