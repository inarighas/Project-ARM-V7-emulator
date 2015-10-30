#ifndef _MEMORY_H_
#define _MEMORY_H_

#include<stdio.h>
#include<string.h>
#include<stdlib.h>


//#define NMAX_SEG 7
#define NBSEG 	 5

//segment de mémoire 
typedef struct segment{
  int taille;					//taille actuelle
  int taille_max;				//taille max pour verifier (pas d debordement)
  unsigned int adresse_initiale;
  char nom[512];
  char* contenu;
  char* permis;
}*SEGMENT,**MAPMEM;





unsigned int maxvalue(unsigned int i,unsigned int j);

//fonctions 
 
  MAPMEM init_memory_arm(void);
  SEGMENT init_segment(char* name,unsigned int adr,unsigned int size_max);
  SEGMENT inc_segment(SEGMENT seg);
  SEGMENT dec_segment(SEGMENT seg);
  
  char* get_byte_seg(SEGMENT seg,char* hex);
  SEGMENT change_val_seg(SEGMENT seg,char* hex, char val);
  SEGMENT change_plage_seg(SEGMENT seg, char* hexdep, char* hexarr , char* val);

  void affiche_segment(SEGMENT seg ,char* hexdep, char* hexarr);
  MAPMEM ajout_seg_map(MAPMEM mem ,char* name, unsigned int adr ,unsigned int size_max, int place);

void affiche_section(char* name, unsigned int start, char* content, unsigned int taille);
  


#endif

