#ifndef _DESASSEMBLEUR_H
#define _DESASSEMBLEUR_H

#define NB 1
typedef struct 
{
	char identifiant[16];
	char mnemo[16];
	int taille;
	unsigned int masque, signature;
	int nb_operande;
}TYPE_INST;




#endif