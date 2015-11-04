
//                 Header registre 27/09/2015                 				//
//			Projet info 2a sicom emul ARM									//
//			Ali saghiran -- Damien chabannes								//

#ifndef _REGISTRE_H			// Par précaution
#define _REGISTRE_H			// Par précaution


//#include "projetemularm.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Définition de la structure REGISTRE
typedef struct {
  char* nom;
  int taille;                // taille en bits
  unsigned int valeur;
} reg , *REGISTRE;



//fonctions liées aux registres

REGISTRE creer_registre(char* name,int size);
REGISTRE* init_table_registre(void);
REGISTRE* init_table_registre_etat(void);
void free_registre(REGISTRE p);
void free_table_registre(REGISTRE* p);
void afficher_registre(REGISTRE r);
void afficher_table_registre(REGISTRE* T);
void afficher_table_registre_etat(REGISTRE *t);
void inverser_registre_etat(REGISTRE r);

//Fonction permet de trouver le registre à partir de son nom

REGISTRE trouve_registre(char* s,REGISTRE* T);

REGISTRE modifier_valeur_reg(unsigned int newval,REGISTRE r);

REGISTRE** table_registre_complete(REGISTRE* Tgen , REGISTRE* Tetat);
#endif
