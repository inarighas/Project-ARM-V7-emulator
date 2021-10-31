#include "exec_loop.h"




// Read and dissably text segment //------------------------------------------------------------------------------------------------------------------------

DESASM_INST* disassembly_txt(SEGMENT seg,interpreteur inter) {
    DESASM_INST* tableau_instruction = NULL;
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

    tableau_instruction = calloc(1,sizeof(*tableau_instruction));

    step=seg->contenu;
    arr=(&(seg->contenu[(seg->taille)]))-1;


    adresse_actuelle = PC_Adress                //    ------------------------------------------->>>>>>>>>>>>>>>>> PC ADRESS
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

 
            code16 |= ((*(step+1)	<< 8)&(0xFF00));
            code16 |= ((*(step+0)	<< 0)&(0x00FF));
            code = code16;
            pas = 2;
            tableau_instruction[indice].code = code;
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

    for(tmp=0;tmp<4;tmp++) {
     if((tableau_instruction[0].op)[tmp]!= NULL) free(tableau_instruction[0].op[tmp]);
    }
    free(tableau_instruction[0].op);
    return tableau_instruction;
}



DESASM_INST*  instruction;



while (1){
  switch(state){
  case NOT_STARTED :
    state = RUN;
    init(Execution);
    break;
  case PAUSE:
    state = RUN;
    break;
  case RUN :
    inst = disassembly_txt;
    i = sizeof(inst);
    if cond_verif(inst)
		   execute_inst();
    else PC = PC + i;
    if (ispresent(PC, breaklist)){
      state = PAUSE;
      return ;
    }
    if (PC>=FIN){
      state = NOT_STARTED;
      return;
    }
  default :
    ERROR_MSG("Error exec");
    return;
  }
 }
