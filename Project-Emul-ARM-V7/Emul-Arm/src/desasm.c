//Code source pour le désassemblage
//Implatation de la commande disasm

#include "desasm.h"
#include "format_op.h"


//Init_Dico//-------------------------------------------------------------------------------------------------------------------
TYPE_INST** init_dico32(TYPE_INST** dico) {
    int indice = 0;
    FILE* fichier = fopen(DICO32,"r"); 			// Ouverture du dictionnaire à instruction 32
    if(fichier == NULL) {
        ERROR_MSG("\nOuverture du dico impossible\n");
        return NULL;
    }
    dico = calloc(SIZE32,sizeof(*dico));
    if(dico == NULL) {
        ERROR_MSG("Allocation Error");
        return NULL;
    }
    while(!feof(fichier) && indice<SIZE32) {
      dico[indice] = calloc(1, sizeof(**dico));
        if (dico[indice]==NULL){
          ERROR_MSG("Erreur Allocation ");
          return NULL;
	}
        fscanf(fichier,"%s %s %d %X %X %d %d %d %d %d %s %s %s %s",
               (dico)[indice]->identifiant,
               (dico)[indice]->mnemo,
               &((dico)[indice]->taille),
               &((dico[indice])->signature),
               &((dico)[indice]->masque),
               &((dico)[indice]->nb_operande),
               &((dico)[indice]->typeop[1]),
               &((dico)[indice]->typeop[2]),
               &((dico)[indice]->typeop[3]),
               &((dico)[indice]->typeop[4]),
               (dico)[indice]->champop[1],
               (dico)[indice]->champop[2],
               (dico)[indice]->champop[3],
               (dico)[indice]->champop[4]);
	//printf("---> %s ",(dico[indice])->identifiant);
	//printf("---> %X \n ",(dico[indice])->signature);
        indice++;
    }
    fclose(fichier);
    return dico;
}
/*---------------------------------------------------------------*/

TYPE_INST** init_dico16(TYPE_INST** dico ) {
    int indice =0;
    FILE* fichier = fopen(DICO16,"r"); 			// Ouverture du dictionnaire à instruction 16
    if(fichier == NULL) {
        ERROR_MSG("\nOuverture du dico impossible\n");
        return NULL;
    }
    dico = calloc(SIZE16,sizeof(*dico));
    if (dico == NULL) {
        ERROR_MSG("Error Allocation");
        return NULL;
    }
    while( !feof(fichier) && indice<SIZE16 ) {
          dico[indice]= calloc(1, sizeof(**dico));
          if (dico[indice]==NULL){
	      ERROR_MSG("Erreur Allocation ");
          	return NULL;
                }
        fscanf(fichier,"%s %s %d %X %X %d %d %d %d %d %s %s %s %s",
               (dico)[indice]->identifiant,
               (dico)[indice]->mnemo,
               &((dico)[indice]->taille),
               &((dico)[indice]->signature),
               &((dico)[indice]->masque),
               &((dico)[indice]->nb_operande),
               &((dico)[indice]->typeop[1]),
               &((dico)[indice]->typeop[2]),
               &((dico)[indice]->typeop[3]),
               &((dico)[indice]->typeop[4]),
               (dico)[indice]->champop[1],
               (dico)[indice]->champop[2],
               (dico)[indice]->champop[3],
               (dico)[indice]->champop[4]);
	//printf("---> %s \n",(dico[indice])->identifiant);
        indice++;
    }
    fclose(fichier);
    return dico;
}



// If 32bit Instruction //------------------------------------------------------------------------------------------------------------------
int is_32(char*instr_ptr) {
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

//Extract with concatenation //--------------------------------------------------------------------------------------------------------------
unsigned int extract_uint (char* s ,unsigned int code) {
    unsigned int debut,fin,val;
    unsigned int nb_bits=0;
    char* token;
    char* str=strdup(s);
    token = strtok(str,":");
    if (token==NULL) {
        ERROR_MSG("Problème lors de la concaténation");
        return 0;
    }

    val=0;

    do {
        sscanf(token,"%u-%u",&fin,&debut);
        nb_bits= fin - debut + 1;
        val= (val << nb_bits) | ((code >> debut) & ((1<<nb_bits)-1));
        //printf("%x \n",val);
    }
    while((token = strtok(NULL,":"))!=NULL);
    return val;
}


//Exctract register name "fastversion" //----------------------------------------------------------------------------------------------------
char* registre_extract(char* s , unsigned int code) {
    unsigned int indice = 20;
    char* nomregistre;

    nomregistre=calloc(5,sizeof(char));

    indice=extract_uint(s,code);
    sprintf(nomregistre,"R%d",indice);
    return nomregistre;
}



// Detect condition//------------------------------------------------------------------------------------------------------------------------
/* en argument:
		--> un segment
		-->

*/
void cond(TYPE_INST *instruction, unsigned int code) {
    unsigned int condition =0;
    if(strcmp(instruction->mnemo,"B") == 0) {
        condition = (code & 0xF00) >> 8;
        if(condition == EQ) strcpy(instruction->mnemo,"BEQ");
        if(condition == NE) strcpy(instruction->mnemo,"BNE");
        if(condition == CS) strcpy(instruction->mnemo,"BCS");
        if(condition == CC) strcpy(instruction->mnemo,"BCC");
        if(condition == MI) strcpy(instruction->mnemo,"BMI");
        if(condition == PL) strcpy(instruction->mnemo,"BPL");
        if(condition == VS) strcpy(instruction->mnemo,"BVS");
        if(condition == VC) strcpy(instruction->mnemo,"BVC");
        if(condition == HI) strcpy(instruction->mnemo,"BHI");
        if(condition == LS) strcpy(instruction->mnemo,"BLS");
        if(condition == GE) strcpy(instruction->mnemo,"BGE");
        if(condition == LT) strcpy(instruction->mnemo,"BLT");
        if(condition == GT) strcpy(instruction->mnemo,"BGT");
        if(condition == LE) strcpy(instruction->mnemo,"BLE");
        if(condition == AL) strcpy(instruction->mnemo,"BAL");
    }
    return;
}


// Detect IT Block and Status ------------------------------------------------------------------------------------------------------------------------
void IT(DESASM_INST desasm_inst, unsigned int code) {
    if(strcmp(desasm_inst.inst->mnemo, "IT") == 0) {
        unsigned int firstcond0 = (0x10 & code)>>4;
        unsigned int mask[4];
        mask[0] = (0x1 & code);
        mask[1] = (0x2 & code)>>1;
        mask[2] = (0x4 & code)>>2;
        mask[3] = (0x8 & code)>>3;

        desasm_inst.blockIT = (0xF & code);
        desasm_inst.condition = (0xF0 & code)>>4;

        DEBUG_MSG("code = %X %X %X %X",extract_uint("15-12",code), extract_uint("11-8", code), extract_uint("7-4", code), extract_uint("3-0", code));
        DEBUG_MSG("mask = %X%X%X%X", extract_uint("3",code), extract_uint("2",code),extract_uint("1",code),extract_uint("0", code));

        DEBUG_MSG("block_IT : %X",desasm_inst.blockIT);
        DEBUG_MSG("condition : %X",desasm_inst.condition);

        if(mask[3]==0 && mask[2]==0 && mask[1]==0 && mask[0]==0)strcpy(desasm_inst.inst->mnemo, "IT");
        if(mask[3]==firstcond0 && mask[2]==1 && mask[1]==0 && mask[0]==0)strcpy(desasm_inst.inst->mnemo, "ITT");
        if(mask[3]==(!firstcond0) && mask[2]==0 && mask[10]==0 && mask[0]==0)strcpy(desasm_inst.inst->mnemo, "ITE");
        if(mask[3]==firstcond0 && mask[2]==firstcond0 && mask[1]==1 && mask[0]==0)strcpy(desasm_inst.inst->mnemo, "ITTT");
        if(mask[3]==(!firstcond0) && mask[2]==firstcond0 && mask[1]==1 && mask[0]==0)strcpy(desasm_inst.inst->mnemo, "ITET");
        if(mask[3]==firstcond0 && mask[2]==(!firstcond0) && mask[1]==1 && mask[0]==0)strcpy(desasm_inst.inst->mnemo, "ITTE");
        if(mask[3]==(!firstcond0) && mask[2]==(!firstcond0) && mask[1]==1 && mask[0]==0)strcpy(desasm_inst.inst->mnemo, "IEE");
        if(mask[3]==firstcond0 && mask[2]==firstcond0 && mask[1]==firstcond0 && mask[0]==1)strcpy(desasm_inst.inst->mnemo, "ITTTT");
        if(mask[3]==(!firstcond0) && mask[2]==firstcond0 && mask[1]==firstcond0 && mask[0]==1)strcpy(desasm_inst.inst->mnemo, "ITETT");
        if(mask[3]==firstcond0 && mask[2]==(!firstcond0) && mask[1]==firstcond0 && mask[0]==1)strcpy(desasm_inst.inst->mnemo, "ITTET");
        if(mask[3]==(!firstcond0) && mask[2]==(!firstcond0) && mask[1]==firstcond0 && mask[0]==1)strcpy(desasm_inst.inst->mnemo, "ITEET");
        if(mask[3]==firstcond0 && mask[2]==firstcond0 && mask[1]==(!firstcond0) && mask[0]==1)strcpy(desasm_inst.inst->mnemo, "ITTTE");
        if(mask[3]==(!firstcond0) && mask[2]==firstcond0 && mask[1]==(!firstcond0) && mask[0]==1)strcpy(desasm_inst.inst->mnemo, "ITETE");
        if(mask[3]==firstcond0 && mask[2]==(!firstcond0) && mask[1]==(!firstcond0) && mask[0]==1)strcpy(desasm_inst.inst->mnemo, "ITTEE");
        if(mask[3]==!firstcond0 && mask[2]==!firstcond0 && mask[1]==!firstcond0 && mask[0]==1)strcpy(desasm_inst.inst->mnemo, "ITEEE");
        return ;
    }
    else {
        WARNING_MSG("didn't get IT");
        return;
    }
}



// Get Operand according to its type ------------------------------------------------------------------------------------------------------------------------

DESASM_INST* get_operande(DESASM_INST* stockage_inst , int indice,int indice_operande, unsigned int code) {
    IMMEDIATE local = NULL;
    int shift_t[1]  ;
    int shift_n[1];
    IMMEDIATE type;
    int carry[1];
    char* tmp_reg = NULL;
    IMMEDIATE tmp_imm = NULL;
    *shift_t =0 ;
    *shift_n = 0;
    *carry=0;

    if(stockage_inst == NULL) {
        WARNING_MSG("Stockage_inst pointeur NULL");
        return NULL;
    }

    switch(stockage_inst[indice].inst->typeop[indice_operande]) {
    case RGSTR:
      tmp_reg = registre_extract_imm(stockage_inst[indice].inst->champop[indice_operande],code);
      (stockage_inst[indice].op)[indice_operande]->register_name=strdup(tmp_reg);
        //DEBUG_MSG("Operande N° %d: %s",indice_operande,(stockage_inst[indice].op)[indice_operande]->register_name);
        printf(" %s ",(stockage_inst[indice].op)[indice_operande]->register_name);
	free(tmp_reg);
        break;
    case IMMEDIATE32:
        local = (immediate(((stockage_inst[indice].inst)->champop)[indice_operande],code));
        local->bits =32;
        (stockage_inst[indice].op)[indice_operande]->unsigned_value = local;
        //DEBUG_MSG("Operande N° %d: #%u",indice_operande,(stockage_inst[indice].op)[indice_operande]->unsigned_value->imm);
        printf(" #%u ",(stockage_inst[indice].op)[indice_operande]->unsigned_value->imm);
        break;
    case IMMEDIATE32P00:
        local =(immediate(((stockage_inst[indice].inst)->champop)[indice_operande],code));
        local->imm = local->imm << 2 ;
        local->bits = 32;
        (stockage_inst[indice].op)[indice_operande]->unsigned_value = local;
        //DEBUG_MSG("Operande N° %d: #%u",indice_operande,(stockage_inst[indice].op)[indice_operande]->unsigned_value->imm);
        printf(" #%u ",(stockage_inst[indice].op)[indice_operande]->unsigned_value->imm);
        //free(local);
        break;

    case THEXPIMM:
        local = immediate(((stockage_inst[indice].inst)->champop)[indice_operande],code);
        //    affiche_immediate(local);
        local = thumbexpandIMM(local);
        (stockage_inst[indice].op)[indice_operande]->unsigned_value = local;
        //DEBUG_MSG("Operande N° %d: #%u ",indice_operande,(stockage_inst[indice].op)[indice_operande]->unsigned_value->imm);
        printf(" #%u ",(stockage_inst[indice].op)[indice_operande]->unsigned_value->imm);
        //free(local);
        break;

    case THEXPIMM_C:
        local = immediate(((stockage_inst[indice].inst)->champop)[indice_operande],code);
        unsigned int i = 0;                                      //--------------------------->>> Inclure Registre
        //	affiche_immediate(local);
        local = thumbexpandIMM_c(local , &i);
        (stockage_inst[indice].op)[indice_operande]->unsigned_value = local;
        //DEBUG_MSG("Operande N° %d: #%u  --- CarryOut: %d",indice_operande,(stockage_inst[indice].op)[indice_operande]->unsigned_value->imm,i);
        printf(" #%u ",(stockage_inst[indice].op)[indice_operande]->unsigned_value->imm);
        //free(local);
        break;

    case SGNEXTP0:
        local = immediate(((stockage_inst[indice].inst)->champop)[indice_operande],code);
        local = signextend(local,32);
        local->imm = ((local->imm) << 1);
        local->bits = 32;
        (stockage_inst[indice].op)[indice_operande]->unsigned_value = local;
        //DEBUG_MSG("Operande N° %d: #%d",indice_operande,(stockage_inst[indice].op)[indice_operande]->unsigned_value->imm);
        printf(" #%d ",(stockage_inst[indice].op)[indice_operande]->unsigned_value->imm);
        //free(local);
        break;

    case DECIMMSHFT:
        local = immediate(((stockage_inst[indice].inst)->champop)[indice_operande],code);
        type = immediate("5-4",code);
        decodeIMMshift(type,local,shift_t,shift_n);
        free(local);
        //DEBUG_MSG("Operande N° %d shift :#%u",indice_operande,*shift_n);
        printf("{ %s #%u }",TYPE_ENUM[type->imm],*shift_n);
        free(type);
        break;


    default:
        local = calloc(1,sizeof(*local));
        local->imm=0;
        local->bits=0;
        (stockage_inst[indice].op)[indice_operande]->unsigned_value=local;
        printf(" Operande '%d'  inconnu",indice_operande);
        free(local);
        break;
    }
    return  stockage_inst;
}



// Read and dissably text segment //------------------------------------------------------------------------------------------------------------------------

DESASM_INST* lecture_txt(SEGMENT seg) {
    DESASM_INST* tableau_instruction = NULL;
    TYPE_INST** dico16 = NULL;
    TYPE_INST** dico32 = NULL;
    TYPE_INST** dico = NULL;
    int indiceinst =0 ;
    int sizeinst = 0;
    char* step;
    char* arr;
    int indice=0;
    unsigned int code=0;
    unsigned int code32=0;
    unsigned int code16=0;
    int success=0;
    int i=0;
    int pas =0;
        int tmp = 0 ;
    unsigned int adresse_actuelle = 0;
                dico16 = init_dico16(dico16);
                dico32 = init_dico32(dico32);
		DEBUG_MSG("Chargement Dictionnaire Instruction : FAIT");
		//	if(dico32 != NULL) printf("\t Exemple \t %s \n",(dico32[2])->identifiant);
		//	else WARNING_MSG("TOTO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    tableau_instruction = calloc(1,sizeof(*tableau_instruction));

    step=seg->contenu;
    arr=(&(seg->contenu[(seg->taille)]))-1;

    while((step<arr) || i<1000) {                                            //-----------------------------------------
        adresse_actuelle = (seg->adresse_initiale)+(step-(seg->contenu));
        if(tableau_instruction == NULL) {
            WARNING_MSG("Erreur allocation du type instruction");
            return NULL;
        }


        //DEBUG_MSG("step = 0x%X",step);
        if(step>arr) break;

        //CAS INTERUCTION 32 BITS//
        printf(" 0x%X \t", adresse_actuelle);
        if(is_32(step+1) == 1) {
	  //DEBUG_MSG("-Instruction sur 32 bits-");

            //-------------------------------------------------------------------------------
            code32 |= ((*(step+1)	<< 24)&(0xFF000000));
            code32 |= ((*(step+0)	<< 16)&(0x00FF0000));
            code32 |= ((*(step+3)	<< 8 )&(0x0000FF00));
            code32 |= ((*(step+2)	<< 0 )&(0x000000FF));
            code = code32;
            pas = 4;
            tableau_instruction[indice].code = code;
            //INFO_MSG("Code copié : %X",tableau_instruction[indice].code);
            printf(" %X \t",code);
	    dico = dico32;
	    sizeinst = SIZE32;
        }


        //CAS INSTRUCTION 16 BITS//
        else if (is_32(step+1) == 0) {
	  //DEBUG_MSG("-Instruction sur 16 bits-");

            /*       fichier = fopen(DICO16,"r"); 			// Ouverture du dictionnaire à instruction 16

                       // Test etat ouverture dictionnaire----------------------------------------------
                       if(fichier == NULL) {
                           WARNING_MSG("\nOuverture du dico impossible\n");
                           free(tableau_instruction);
                           return NULL;
                       }*/
            code16 |= ((*(step+1)	<< 8)&(0xFF00));
            code16 |= ((*(step+0)	<< 0)&(0x00FF));
            code = code16;
            pas = 2;
            tableau_instruction[indice].code = code;
            //INFO_MSG("Code copié : %X",tableau_instruction[indice].code);
            printf(" %X \t\t",code);
	    dico = dico16;
	    sizeinst = SIZE16;
        }

        else {
	  WARNING_MSG("Erreur identification de l'instruction");
            return NULL;
        }

        // supression alloc vers début fct
	indiceinst = 0;
        while(success == 0 && indiceinst < sizeinst) {
	  if( ((dico[indiceinst]->signature) & (dico[indiceinst]->masque)) == (code & (dico[indiceinst]->masque))) {
	           success = 1; 
	           tableau_instruction[indice].inst = dico[indiceinst];
	         }
	  indiceinst++;
	}
        cond(dico[indiceinst], tableau_instruction[indice].code);				//si mnemo == B, on remplace B par BEQ ou BNE ou .....

        if(strcmp((tableau_instruction[indice].inst)->mnemo, "IT") == 0) {			//si mnemo == IT
            IT(tableau_instruction[indice],tableau_instruction[indice].code);
        }

        if (success==1) {
	  //DEBUG_MSG("tableau_instruction[%d].inst->nom = %s",indice,(tableau_instruction[indice].inst)->identifiant);
            printf(" %s ",(tableau_instruction[indice].inst)->mnemo);
            //switch case suivant nombre d'opérandes
            int toto =0;
            for(toto=0; toto<4; toto++) {
	      tableau_instruction[indice].op[toto]=calloc(1,sizeof(*(tableau_instruction[indice].op[toto])));
	      if((tableau_instruction[indice].op[toto])==NULL) ERROR_MSG("Erreur Alloc");
            }
            switch ((tableau_instruction[indice].inst)->nb_operande) {
            case 1:
                tableau_instruction=get_operande(tableau_instruction,indice,1,tableau_instruction[indice].code);
                puts(" ");
                break;

            case 2:
                tableau_instruction=get_operande(tableau_instruction,indice,1,tableau_instruction[indice].code);
                printf(",");
                tableau_instruction=get_operande(tableau_instruction,indice,2,tableau_instruction[indice].code);
                puts(" ");
                break;

            case 3:
                tableau_instruction=get_operande(tableau_instruction,indice,1,tableau_instruction[indice].code);
                printf(",");
                tableau_instruction=get_operande(tableau_instruction,indice,2,tableau_instruction[indice].code);
                printf(",");
                tableau_instruction=get_operande(tableau_instruction,indice,3,tableau_instruction[indice].code);
                puts(" ");
                break;

            case 4:
                tableau_instruction=get_operande(tableau_instruction,indice,1,tableau_instruction[indice].code);
                printf(",");
                tableau_instruction=get_operande(tableau_instruction,indice,2,tableau_instruction[indice].code);
                printf(",");
                tableau_instruction=get_operande(tableau_instruction,indice,3,tableau_instruction[indice].code);
                printf(",");
                tableau_instruction=get_operande(tableau_instruction,indice,4,tableau_instruction[indice].code);
                puts(" ");
                break;

            default:
                WARNING_MSG("cas op inconnu \n");
                return NULL;
                break;
            }
        }

        else if (success==0) {
            WARNING_MSG("####Commande  Introuvable#### ");
            WARNING_MSG("#ARRET DE DESASSEMBLAGE# ");
            return NULL;
        }

        step=step+pas;
        code=0;
        code32=0;
        code16=0;
        success=0;
	indiceinst = 0;
        i++;
       for(tmp=0;tmp;tmp++) {
	  free(tableau_instruction[0].op[tmp]);
       }
       free(tableau_instruction[0].op);	 
    }


    if(dico16 != NULL){
      /*for(tmp = 0;tmp<SIZE16;tmp++) {
	printf("%d",tmp); 
	free(dico16[tmp]);
	}*/
	free(dico16);
    }
    if(dico32 != NULL){
      /*for(tmp = 0;tmp<SIZE32;tmp++) {
	printf("%d",tmp); 
	free(dico32[tmp]);
	}*/
      free(dico32);
      }
    for(tmp=0;tmp<4;tmp++) {
     if((tableau_instruction[0].op)[tmp]!= NULL) free(tableau_instruction[0].op[tmp]);
    }
    free(tableau_instruction[0].op);
    return tableau_instruction;
}



//Fonction de déssassemblage//------------------------------------------------------------------------------------------------------------------------

int _desasm_cmd(SEGMENT seg, unsigned int adrdep , unsigned int adrarr) {

    DEBUG_MSG("Lancement de _desasm_cmd");

    //FILE *fichier = NULL;
    DESASM_INST* stockage_inst= NULL;
    int indice = 0;
    char *dep=NULL;
    char *arr=NULL;
    char reponse=0;



    if (seg==NULL) {
        WARNING_MSG("Segment inexistant pour desassembler");
        return 1;
    }

    if (adrdep<(seg->adresse_initiale)) {
        WARNING_MSG("Adresse de départ incorrecte");
        return 1;
    }

    if (adrarr>((seg->adresse_initiale)+(seg->taille))) {
        WARNING_MSG("Adresse d'arrivée incorrecte");
        return 1;
    }

    dep = get_byte_seg(seg,adrdep);
    arr = get_byte_seg(seg,adrarr);
    DEBUG_MSG("dep = 0x%x ",dep);
    DEBUG_MSG("fin = 0x%x ",arr);
    if(dep==NULL || arr==NULL ) {
        WARNING_MSG("#Arrêt de désassemblage#");
        return 1;
        // }
    }

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
    
    stockage_inst=lecture_txt(seg);

    if(stockage_inst==NULL) {
        WARNING_MSG("Erreur lecture txt :s");
        return 1;
    }
    //DEBUG_MSG("Indice : %d",indice);

    free(stockage_inst);
    INFO_MSG("##FIN DU DESASSEMBLAGE##");
    return CMD_OK_RETURN_VALUE;
}
