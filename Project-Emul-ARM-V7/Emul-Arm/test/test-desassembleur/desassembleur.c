#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "desassembleur.h"
#include "memory.h"
#include "registre.h"


// FONCTION TEST INSTRUCTION 32 OU 16 BITS
// Cette fonction prend en prmtr un segment et un pointeur vers l'octet (debut d'instruction)

int is_32(SEGMENT seg, char* adresseptr){			
	int a;

	char*instr_ptr =get_byte_seg(seg,adresseptr);

	a= (*instr_ptr) - 232 ;
	if (a<8 && a>=0) return 1;

	a= (*instr_ptr) - 240 ;
	if (a<8 && a>=0) return 1;

	a= (*instr_ptr) - 248 ;
	if (a<8 && a>=0) return 1;
	
	return 0;
}



void main(void){

	FILE *fichier = NULL;
	TYPE_INST* dico=NULL;
	SEGMENT seg=NULL;
	int ligne = 1;
	int success = 0;
	unsigned int code = 0;						// Contient l'instruction binaire "brut" 32 ou 16 bits
	fichier = fopen("dictionnaire.txt","r"); 			// Ouverture du dictionnaire à instruction

	

	dico=calloc(NB,sizeof(*dico));				// structure retournée par la fonction de décodage
	
	
	// Initialisation du segment .{text}  --- test--------------------------------
	seg=init_segment("segment",0x0,1000);		
	affiche_segment(seg, NULL, NULL);
	seg->contenu=calloc(4, sizeof(char));
	seg->taille = 4;
	seg->contenu[0] = 245;
	seg->contenu[1] = 179;
	seg->contenu[2] = 15;
	seg->contenu[3] = 127;
	affiche_segment(seg, NULL, NULL);
	// ------------------------------------------------------------
	
	
	if(is_32(seg, &(seg->contenu[1])) == 1)printf("\n On a bien 32 bits !!! \n");
	else printf("\nProblème !\n");

	

	// Test etat ouverture dictionnaire----------------------------------------------
	if(fichier == NULL){
		printf("\nOuverture du dico impossible\n");
		free(dico);
		return ;
	}
	//-------------------------------------------------------------------------------
	while(success == 0 && !feof(fichier)){
		fscanf(fichier,"%s %s %d %x %x %d",dico[0].identifiant, dico[0].mnemo, &dico[0].taille, &dico[0].masque, &dico[0].signature, &dico[0].nb_operande);

		code |= (seg->contenu[0] << 24);
		code |= (seg->contenu[1] << 16);
		code |= (seg->contenu[2] << 8);
		code |= (seg->contenu[3] );
		//printf("\ncode : %x\n",code);
		if( (dico[0].signature & dico[0].masque) == (code & dico[0].masque) ){
			printf("\nnumero de ligne : %d\n",ligne);
			success = 1;
		}
		ligne++;
	}

	if (success==1) {
		printf("\nidentifiant : %s\nmnemonique : %s\ntaille : %d bits\nmasque : %x\nsignature : %x\nNombre d'operande : %d\n",dico[0].identifiant, dico[0].mnemo, dico[0].taille, dico[0].masque, dico[0].signature, dico[0].nb_operande);
		}
	else if (success==0) {
		printf("####Commande "" introuvable#### \n");
		return;
		}
	free(dico);
	return ;
 
}
