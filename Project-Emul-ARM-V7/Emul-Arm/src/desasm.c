//Code source pour le désassemblage
//Implatation de la commande disasm 

#include "desasm.h"



int is_32(char*instr_ptr){			
	

	//char*instr_ptr =get_byte_seg(seg,adr);
	unsigned int mask= 0xF8;
	
	if (instr_ptr==NULL) {
		WARNING_MSG("Verif instru 32bits erreur");
		return 50;
		}
	
	unsigned int a = (*instr_ptr) & mask;
	 

	if (a == (mask & 0xE8)) return 1;
	if (a == (mask & 0xF0)) return 1;
	if (a == (mask & 0xF8)) return 1;
	
	
	return 0;
}

unsigned int immediate (char* s ,unsigned int code){
	unsigned int debut,fin,val; 
	unsigned int nb_bits;
	char* token;
	char* str=strdup(s);
	token = strtok(str,":");
	if (token==NULL) {
			ERROR_MSG("Problème lors de la concaténation");
			return 0;
		}

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
void cond(TYPE_INST *instruction, unsigned int code){
	unsigned int condition =0;
	char* champ;
	if(strcmp(instruction->mnemo,"B") == 0){
		champ=strdup("11-8");
		condition = immediate("11-8",code);
		if(condition == EQ)strcpy(instruction->mnemo,"BEQ");
		if(condition == NE)strcpy(instruction->mnemo,"BNE");
		if(condition == CS)strcpy(instruction->mnemo,"BCS");
		if(condition == CC)strcpy(instruction->mnemo,"BCC");
		if(condition == MI)strcpy(instruction->mnemo,"BMI");
		if(condition == PL)strcpy(instruction->mnemo,"BPL");
		if(condition == VS)strcpy(instruction->mnemo,"BVS");
		if(condition == VC)strcpy(instruction->mnemo,"BVC");
		if(condition == HI)strcpy(instruction->mnemo,"BHI");
		if(condition == LS)strcpy(instruction->mnemo,"BLS");
		if(condition == GE)strcpy(instruction->mnemo,"BGE");
		if(condition == LT)strcpy(instruction->mnemo,"BLT");
		if(condition == GT)strcpy(instruction->mnemo,"BGT");
		if(condition == LE)strcpy(instruction->mnemo,"BLE");
		if(condition == AL)strcpy(instruction->mnemo,"BAL");	
	}
	return;
}

int affiche_instruction_1operande(DESASM_INST* stockage_inst, int indice, unsigned int code){
	printf ("\t \t----> %s \t ",stockage_inst[indice].inst->mnemo);
	switch (stockage_inst[indice].inst->typeop1) {
		case 1:
			printf("%s\n\n\n",registre_extract(stockage_inst[indice].inst->champop1, code));
			break;
		case 2:
			printf("%X\n\n\n",immediate(stockage_inst[indice].inst->champop1, code));
			break;
		default:
			printf("NEVER SHOULD BE HERE\n");
			return 1;
	return 0;
	}
}


int affiche_instruction_2operandes(DESASM_INST* stockage_inst, int indice, unsigned int code){

	printf ("\t \t----> %s \t",stockage_inst[indice].inst->mnemo);
	switch (stockage_inst[indice].inst->typeop1) {
		case 1:
			printf("%s",registre_extract(stockage_inst[indice].inst->champop1 , code));
			break;
		case 2:
			break;
		default:
			printf("NEVER SHOULD BE HERE\n");
			return 1;
			break;
		}
	printf(" , ");
	
	switch (stockage_inst[indice].inst->typeop2) {
		case 1:
			printf("%s\n\n\n",registre_extract(stockage_inst[indice].inst->champop2 , code));
			break;
		case 2:
			printf("%X\n\n\n",immediate(stockage_inst[indice].inst->champop2, code));
			break;
		default:
			printf("NEVER SHOULD BE HERE\n");
			return 1;
			break;
		}
	return 0;
}


int affiche_instruction_3operandes(DESASM_INST* stockage_inst, int indice, unsigned int code){
	printf ("\t \t----> %s \t",stockage_inst[indice].inst->mnemo);
	switch (stockage_inst[indice].inst->typeop1) {
		case 1:
			printf("%s",registre_extract(stockage_inst[indice].inst->champop1, code));
			break;
		case 2:
			printf("%X",immediate(stockage_inst[indice].inst->champop1, code));
			break;
		default:
			printf("NEVER SHOULD BE HERE\n");
			return 1;
			break;
		}
	printf(" , ");
	
	switch (stockage_inst[indice].inst->typeop2) {
		case 1:
			printf("%s",registre_extract(stockage_inst[indice].inst->champop2, code));
			break;
		case 2:
			printf("%X",immediate(stockage_inst[indice].inst->champop2, code));
			break;
		default:
			printf("NEVER SHOULD BE HERE\n");
			return 1;
			break;
		}
	printf(" , ");
	
	switch (stockage_inst[indice].inst->typeop3) {
		case 1:
			printf("%s\n\n\n",registre_extract(stockage_inst[indice].inst->champop3 , code));
			break;
		case 2:
			printf("%X\n\n\n",immediate(stockage_inst[indice].inst->champop3, code));
			break;
		default:
			printf("NEVER SHOULD BE HERE\n");
			return 1;
			break;
		}	
	return 0;
}

int _desasm_cmd(SEGMENT seg, unsigned int adrdep , unsigned int adrarr){

	printf("\n\nLancement de _desasm_cmd\n\n");

	FILE *fichier = NULL;
	DESASM_INST* stockage_inst=NULL;

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
	
	stockage_inst=calloc(NB,sizeof(*stockage_inst));				// structure retournée par la fonction de décodage
	stockage_inst->inst=calloc(1,sizeof(TYPE_INST));

	/************************************************************************************************************/
	/*char* cond[15] = {"EQ","NE","CS","CC","MI","PL","VS","VC","HI","LS","GE","LT","GT","LE","AL"};

	typedef struct {}*/


	/************************************************************************************************************/
	
	if (seg==NULL) {
		WARNING_MSG("Segment inexistant pour desassembler");
		return 1;
		}
	
	dep = get_byte_seg(seg,adrdep);
	arr = get_byte_seg(seg,adrarr);
	
	if(dep==NULL ) dep=(seg->contenu);
	if(arr==NULL ) arr=&((seg->contenu)[seg->taille-1]);
	
	//-------------------------------------------------------------------------------------->> à revoir sécurité d'initialisation >> adresse de départ
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
		DEBUG_MSG("-Instruction sur 32 bits-");
		fichier = fopen(DICO32,"r"); 			// Ouverture du dictionnaire à instruction 32 
		
	

		// Test etat ouverture dictionnaire----------------------------------------------
				if(fichier == NULL){
					WARNING_MSG("\nOuverture du dico impossible\n");
					free(stockage_inst);
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
		INFO_MSG("CODE : %X",code);
		}
	
				
				//CAS INSTRUCTION 16 BITS//
	else if (is_32(step+1) == 0){
	DEBUG_MSG("-Instruction sur 16 bits-");
		fichier = fopen(DICO16,"r"); 			// Ouverture du dictionnaire à instruction 16
		
		// Test etat ouverture dictionnaire----------------------------------------------
				if(fichier == NULL){
					WARNING_MSG("\nOuverture du dico impossible\n");
					free(stockage_inst);
					return 1;
				}
		code16 |= ((*(step+1)	<< 8)&(0xFF00));
		
		code16 |= ((*(step+0)	<< 0)&(0x00FF));
		
		code = code16;
		pas = 2;
		INFO_MSG("CODE : %X",code);
		}
	
	else {
		WARNING_MSG("Erreur identification de l'instruction");
		return 1;
		}

	while(success == 0 && !feof(fichier)){
		fscanf(fichier,"%s %s %d %X %X %d %d %d %d %d %s %s %s %s",
			stockage_inst[indice].inst->identifiant,
			stockage_inst[indice].inst->mnemo, 
			&stockage_inst[indice].inst->taille,
			&stockage_inst[indice].inst->signature,
			&stockage_inst[indice].inst->masque,  
			&stockage_inst[indice].inst->nb_operande, 
			&stockage_inst[indice].inst->typeop1,
			&stockage_inst[indice].inst->typeop2,
			&stockage_inst[indice].inst->typeop3,
			&stockage_inst[indice].inst->typeop4, 
			stockage_inst[indice].inst->champop1,
			stockage_inst[indice].inst->champop2,
			stockage_inst[indice].inst->champop3,
			stockage_inst[indice].inst->champop4);

		//--------------------------------------------------> NE PAS OUBLIER L'ORDRE !!!!!!!

		
		if( ( ((stockage_inst[indice].inst)->signature) & ((stockage_inst[indice].inst)->masque)  ) == (code & (stockage_inst[indice].inst)->masque) ){
			//printf("\nnumero de ligne : %d\n",ligne);
			success = 1;
		}
		ligne++;
	}

	cond(stockage_inst[indice].inst, code);				//si mnemo == B, on remplace B par BEQ ou BNE ou .....

	if(strcmp(stockage_inst[indice].inst->mnemo, "IT") == 0){			//si mnemo == IT

}

	if (success==1) {
	
		affiche_segment(seg, NULL, NULL);

		
		//printf("\nidentifiant : %s\nmnemonique : %s\ntaille : %d bits\nmasque : %x\nsignature : %x\nNombre d'operande : %d\n",dico[indice].identifiant, dico[indice].mnemo, dico[indice].taille, dico[indice].masque, dico[indice].signature, dico[indice].nb_operande);
		
		
		//affiche_segment(seg, NULL, NULL);
		//switch case suivant nombre d'opérandes
		switch (stockage_inst[indice].inst->nb_operande) {
			case 1:
				affiche_instruction_1operande(stockage_inst, indice, code);
				puts(" ");
				break;

			case 2:
				affiche_instruction_2operandes(stockage_inst, indice, code);
				puts(" ");
				break;
				
			case 3:
				affiche_instruction_3operandes(stockage_inst, indice,code);
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
	free(stockage_inst);
	return CMD_OK_RETURN_VALUE;
 
}
