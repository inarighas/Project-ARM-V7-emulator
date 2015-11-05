#include "desasm.h"



int is_32(char*instr_ptr){			
	

	//char*instr_ptr =get_byte_seg(seg,adr);
	unsigned int mask= 0xF8;
	
	if (instr_ptr==NULL) {
		WARNING_MSG("Verif instru 32bits erreur");
		return 50;
		}
	
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
int _desasm_cmd(SEGMENT seg, unsigned int adrdep , unsigned int adrarr){

	FILE *fichier = NULL;
	TYPE_INST* dico=NULL;

	int ligne = 1;
	int success = 0;
	unsigned int code32 = 0;						// Contient l'instruction binaire "brut" 32 ou 16 bits
	int code16 = 0;
	unsigned int code = 0;
	int indice = 0;
	
	int pas=0;
	char* step;
	int i=0;
	
	char *dep=NULL;
	char *arr=NULL;
	
	dico=calloc(NB,sizeof(*dico));				// structure retournée par la fonction de décodage
	
	if (seg==NULL) {
		WARNING_MSG("Segment inexistant pour desassembler");
		return 1;
		}
	
	dep = get_byte_seg(seg,adrdep);
	arr = get_byte_seg(seg,adrarr);
	
	if(dep==NULL ) dep=(seg->contenu);
	if(arr==NULL ) arr=&((seg->contenu)[seg->taille-1]);
	
	//-------------------------------------------------------------------------------------->> à revoir sécurité d'initialisation
	/*unsigned int ecartdep = (dep-(seg->contenu))%4;
		if(ecartdep!=0) dep=dep-ecartdep;
		
	unsigned int ecartarr = ((arr-dep)+1)%4;
		if(ecartarr!=0) {
			arr=arr+ecartarr;
			if((arr-&((seg->contenu)[seg->taille-1]))>0){
				WARNING_MSG("Erreur segment {%s} :s",seg->nom);
				return 1;
				}
			}*/
	//-------------------------------------------------------------------------------------->>
	if(dep==(seg->contenu)) 
			DEBUG_MSG("Desassemblage depuis le debut de {%s}",seg->nom);
	if(arr==&(seg->contenu)[seg->taille-1]) 
			DEBUG_MSG("Desassemblage jusqu'à la fin de {%s}",seg->nom);
	
	
	
	// Initialisation du segment .{text}  --- test--------------------------------
	/*seg=init_segment("segment",0x0,1000);		
	affiche_segment(seg, NULL, NULL);
	seg->contenu=calloc(4, sizeof(char));
	seg->taille = 4;
	seg->contenu[0] = 0xEA;
	seg->contenu[1] = 0x5F;
	seg->contenu[2] = 0x03;
	seg->contenu[3] = 0x09;
	affiche_segment(seg, NULL, NULL);*/
	// ------------------------------------------------------------
	step=dep;
	while((step!=arr) || i<5) {															//--------------------------
				//CAS INTERUCTION 32 BITS//
	
	if(is_32( step+1) == 1) {
		DEBUG_MSG("-Instruction sur 32 bits- \n");
		fichier = fopen(DICO32,"r"); 			// Ouverture du dictionnaire à instruction 32 
		
	

		// Test etat ouverture dictionnaire----------------------------------------------
				if(fichier == NULL){
					WARNING_MSG("\nOuverture du dico impossible\n");
					free(dico);
					return 1;
				}
	//-------------------------------------------------------------------------------
		code32 |= ((*(step+1)	<< 24)&(0xFF000000));						// Little indian ? normalement oui
		//printf("\ncode apres1 concat: %X\n",(seg->contenu[0] << 24));
		code32 |= ((*(step+0)	<< 16)&(0x00FF0000));
		//printf("\ncode apres2 concat: %X\n",code);
		code32 |= ((*(step+3)	<< 8 )&(0x0000FF00));
		//printf("\ncode apres3 concat: %X\n",(seg->contenu[2] << 8));
		code32 |= ((*(step+2)	<< 0 )&(0x000000FF));
		code = code32;
		pas = 4;
		INFO_MSG("CODE : %X\n",code);
		}
	
				
				//CAS INSTRUCTION 16 BITS//
	else if (is_32(step+1) == 0){
	DEBUG_MSG("-Instruction sur 16 bits- \n");
		fichier = fopen(DICO16,"r"); 			// Ouverture du dictionnaire à instruction 16
		
		// Test etat ouverture dictionnaire----------------------------------------------
				if(fichier == NULL){
					WARNING_MSG("\nOuverture du dico impossible\n");
					free(dico);
					return 1;
				}
		code16 |= ((*(step+1)	<< 8)&(0xFF00));
		
		code16 |= ((*(step+0)	<< 0)&(0x00FF));
		
		code = code16;
		pas = 2;
		INFO_MSG("CODE : %X\n",code);
		}
	
	else {
		WARNING_MSG("Erreur identification de l'instruction");
		return 1;
		}
	
	while(success == 0 && !feof(fichier)){
		fscanf(fichier,"%s %s %d %X %X %d %d %d %d %d %s %s %s %s",dico[indice].identifiant, dico[indice].mnemo, 
			&dico[indice].taille,&dico[indice].signature, &dico[indice].masque,  
			&dico[indice].nb_operande, 
			&dico[indice].typeop1,
			&dico[indice].typeop2,
			&dico[indice].typeop3,
			&dico[indice].typeop4, 
			dico[indice].champop1,
			dico[indice].champop2,
			dico[indice].champop3,
			dico[indice].champop4);

		//--------------------------------------------------> NE PAS OUBLIER LORDRE !!!!!!!

		
		if( (dico[indice].signature & dico[indice].masque) == (code & dico[indice].masque) ){
			//printf("\nnumero de ligne : %d\n",ligne);
			success = 1;
		}
		ligne++;
	}

	if (success==1) {
	
		affiche_segment(seg, NULL, NULL);
		
		printf("\nidentifiant : %s\nmnemonique : %s\ntaille : %d bits\nmasque : %x\nsignature : %x\nNombre d'operande : %d\n",dico[indice].identifiant, dico[indice].mnemo, dico[indice].taille, dico[indice].masque, dico[indice].signature, dico[indice].nb_operande);
		
		
		//affiche_segment(seg, NULL, NULL);
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
						return 1;
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
						return 1;
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
						return 1;
						break;
					}
				puts(" ");
				
				break;
				
			case 3:
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
						return 1;
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
						return 1;
						break;
					}
				printf(" , ");
				
				switch (dico[indice].typeop3) {
					case 1:
						printf("%s",registre_extract(dico[indice].champop3 , code));
						break;
					case 2:
						printf("%X",immediate(dico[indice].champop3 , code));
						break;
					default:
						printf("NEVER SHOULD BE HERE\n");
						return 1;
						break;
					}	
				puts(" ");
				
				break;
			
			case 4:
				DEBUG_MSG("cas 4 op -- pas encore pret \n");
				return 1;
				break;
				
			default:
				printf("cas op inconnu \n");
				return 1;
				break;
			}
		}
			
	else if (success==0) {
		WARNING_MSG("####Commande "" introuvable#### ");
		WARNING_MSG("#ARRET DE DESASSEMBLAGE# ");
		return 1;
		}
		
	step=step+pas;
	code=0;
	code32=0;
	code16=0;
	success=0;
	i++;
	
	}
		
	free(dico);
	return CMD_OK_RETURN_VALUE;
 
}
