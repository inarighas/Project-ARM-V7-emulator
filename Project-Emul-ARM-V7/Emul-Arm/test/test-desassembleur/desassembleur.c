#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "desassembleur.h"
#include "memory.h"
#include "registre.h"

#define DICO32 "dicotest32.txt"
#define DICO16 "dicotest16.txt"

//enum{WHAT, REG, IMM};
// FONCTION TEST INSTRUCTION 32 OU 16 BITS
// Cette fonction prend en prmtr un segment et un pointeur vers l'octet (debut d'instruction)

int is_32(SEGMENT seg, char* adresseptr){			
	

	char*instr_ptr =get_byte_seg(seg,adresseptr);
	unsigned int mask= 0xF8;
	unsigned int a = (*instr_ptr) & mask;
/*
	a= (*instr_ptr) - 232 ;
	if (a<8 && a>=0) return 1;

	a= (*instr_ptr) - 240 ;
	if (a<8 && a>=0) return 1;

	a= (*instr_ptr) - 248 ;
	if (a<8 && a>=0) return 1;*/
	if (a == (mask & 0xE8)) return 1;
	if (a == (mask & 0xF0)) return 1;
	if (a == (mask & 0xF8)) return 1;
	
	
	return 0;
}

// Fonction de saisie de bits --- fct du prof
unsigned int immediate (char* s ,unsigned int code){
	unsigned int debut,fin,val; 
	unsigned int nb_bits;
	char* token;
	token = strtok(s,":");
	val=0;
	
	do{
		sscanf(token,"%u-%u",&fin,&debut);
		nb_bits= fin - debut + 1;
		val= (val << nb_bits) | ((code >> debut) & ((1<<nb_bits)-1));
		//printf("%x \n",val);
		}
	while((token = strtok(NULL,":"))!=NULL);
	return val;
	}



char* registre_extract(char* s , unsigned int code){
	unsigned int indice = 20;
	char* nomregistre;
	
	nomregistre=calloc(5,sizeof(char));
	
	indice=immediate(s,code);
	sprintf(nomregistre,"R%d",indice);
	return nomregistre;
	}


/* en argument:
		--> un segment
		--> 

*/
void main(void){

	FILE *fichier = NULL;
	TYPE_INST* dico=NULL;
	SEGMENT seg=NULL;
	int ligne = 1;
	int success = 0;
	unsigned int code32 = 0;						// Contient l'instruction binaire "brut" 32 ou 16 bits
	int code16 = 0;
	unsigned int code = 0;
	int indice = 0;
	
	
	dico=calloc(NB,sizeof(*dico));				// structure retournée par la fonction de décodage
	
	
	// Initialisation du segment .{text}  --- test--------------------------------
	seg=init_segment("segment",0x0,1000);		
	affiche_segment(seg, NULL, NULL);
	seg->contenu=calloc(4, sizeof(char));
	seg->taille = 4;
	seg->contenu[0] = 0xEA;
	seg->contenu[1] = 0x5F;
	seg->contenu[2] = 0x03;
	seg->contenu[3] = 0x09;
	affiche_segment(seg, NULL, NULL);
	// ------------------------------------------------------------
	
				//CAS INTERUCTION 32 BITS//
	if(is_32(seg, &(seg->contenu[0])) == 1) {
		printf("\n On a bien 32 bits !!! \n");
		fichier = fopen(DICO32,"r"); 			// Ouverture du dictionnaire à instruction 32 
		
	

		// Test etat ouverture dictionnaire----------------------------------------------
				if(fichier == NULL){
					printf("\nOuverture du dico impossible\n");
					free(dico);
					return ;
				}
	//-------------------------------------------------------------------------------
		code32 |= ((seg->contenu[0] << 24)&(0xFF000000));						// Little indian ? normalement oui
		//printf("\ncode apres1 concat: %X\n",(seg->contenu[0] << 24));
		code32 |= ((seg->contenu[1] << 16)&(0x00FF0000));
		//printf("\ncode apres2 concat: %X\n",code);
		code32 |= ((seg->contenu[2] << 8 )&(0x0000FF00));
		//printf("\ncode apres3 concat: %X\n",(seg->contenu[2] << 8));
		code32 |= ((seg->contenu[3] 	   )&(0x000000FF));
		code = code32;
		printf("\ncode 0 décalé : %X\n",code);
		}
	
				
				//CAS INSTRUCTION 16 BITS//
	else {
		printf("\nOn a bien 16 bits !\n");
		fichier = fopen(DICO16,"r"); 			// Ouverture du dictionnaire à instruction 16
		
		// Test etat ouverture dictionnaire----------------------------------------------
				if(fichier == NULL){
					printf("\nOuverture du dico impossible\n");
					free(dico);
					return ;
				}
		code16 |= ((seg->contenu[0] << 8)&(0xFF00));
		
		code16 |= ((seg->contenu[1] << 0)&(0x00FF));
		
		code = code16;
		
		printf("\ncode 0 décalé : %X\n",code);
		}
	
		
	
	while(success == 0 && !feof(fichier)){
		fscanf(fichier,"%s %s %d %X %X %d %d %d %s %s",dico[indice].identifiant, dico[indice].mnemo, &dico[indice].taille,&dico[indice].signature, &dico[indice].masque,  &dico[indice].nb_operande, &dico[indice].typeop1, &dico[indice].typeop2, dico[indice].champop1, dico[indice].champop2);						
		//--------------------------------------------------> NE PAS OUBLIER LORDRE !!!!!!!

		
		if( (dico[indice].signature & dico[indice].masque) == (code & dico[indice].masque) ){
			printf("\nnumero de ligne : %d\n",ligne);
			success = 1;
		}
		ligne++;
	}

	if (success==1) {
		printf("\nidentifiant : %s\nmnemonique : %s\ntaille : %d bits\nmasque : %x\nsignature : %x\nNombre d'operande : %d\n",dico[indice].identifiant, dico[indice].mnemo, dico[indice].taille, dico[indice].masque, dico[indice].signature, dico[indice].nb_operande);
		
		
		affiche_segment(seg, NULL, NULL);
		//switch case suivant nombre d'opérandes
		switch (dico[indice].nb_operande) {
			case 1:
				printf ("\t \t----> %s \t ",dico[indice].mnemo);
				switch (dico[indice].typeop1) {
					case 1:
						printf("%s",registre_extract(dico[indice].champop1 , code));
						break;
					case 2:
						printf("%X",immediate(dico[indice].champop1 , code));
						break;
					default:
						printf("NEVER SHOULD BE HERE\n");
						return;
						break;
					}
				break;
				puts(" ");
			case 2:
				printf ("\t \t----> %s \t",dico[indice].mnemo);
				switch (dico[indice].typeop1) {
					case 1:
						printf("%s",registre_extract(dico[indice].champop1 , code));
						break;
					case 2:
						printf("%X",immediate(dico[indice].champop1 , code));
						break;
					default:
						printf("NEVER SHOULD BE HERE\n");
						return;
						break;
					}
				printf(" , ");
				
				switch (dico[indice].typeop2) {
					case 1:
						printf("%s",registre_extract(dico[indice].champop2 , code));
						break;
					case 2:
						printf("%X",immediate(dico[indice].champop2 , code));
						break;
					default:
						printf("NEVER SHOULD BE HERE\n");
						return;
						break;
					}
				puts(" ");
				
				break;
				
			case 3:
				printf("cas 3 op \n");
				return;
				break;
			
			case 4:
				printf("cas 4 op \n");
				return;
				break;
				
			default:
				printf("cas op inconnu \n");
				return;
				break;
			}
		}
			
	else if (success==0) {
		printf("####Commande "" introuvable#### \n");
		return;
		}
		
	free(dico);
	return ;
 
}
