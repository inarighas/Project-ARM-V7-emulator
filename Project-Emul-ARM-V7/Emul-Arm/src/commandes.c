//Fichier Source Commande
//Implantation des commandes de l'interpreteur

#include "commandes.h"




//IMPLANTATION COMMANDE--------------------------------------------------------------


//commade display implantation -------------------------------------

int _dispcmd(char*string,interpreteur inter,char * adrdep,char*adrarr) {
    int i =0;
    REGISTRE r=NULL;

    //if ((inter->memory) != NULL) p=inter->memory;
    if(strcmp(string,"map")==0) {
        if ((inter->memory)==NULL) {
            WARNING_MSG("Impossible  accès à la mémoire - introuvable");
            return 1;
        }
        for(i=0; i<NBSEG; i++) {
            if((inter->memory)[i]!=NULL && (inter->memory)[i]->flag==1)
                affiche_segment((inter->memory)[i],NULL,NULL);
        }
        return CMD_OK_RETURN_VALUE;
    }

    else if(strcmp(string,"plage")==0) {
        if ((inter->memory)==NULL) {
            WARNING_MSG("Impossible  accès à la mémoire - introuvable");
            return 1;
        }
        for(i=0; i<NBSEG; i++) {
            if((inter->memory)[i]!=NULL && (inter->memory)[i]->flag==1) affiche_segment((inter->memory)[i],adrdep,adrarr);
        }
        return CMD_OK_RETURN_VALUE;
    }

    else if(strcmp(string,"all register")==0) {
        printf("Nom \t \t Taille \t \t Valeur \n ");
        if((inter->fulltable) == NULL) {
            printf ("Pas de registre\n");
            return 1;
        }
        else {
            afficher_table_registre((inter->fulltable)[0]);
            afficher_table_registre_etat((inter->fulltable)[1]);
            return CMD_OK_RETURN_VALUE;
        }
    }

    else if( (r=trouve_registre(string,*(inter->fulltable)))!=NULL) {
        afficher_registre(r);
        if (string==NULL) return CMD_OK_RETURN_VALUE;
    }
    else if( strcmp(string,"apsr")==0) {
        afficher_table_registre_etat(inter->fulltable[1]);
        if (string==NULL) return CMD_OK_RETURN_VALUE;
    }

    printf("SHOULD NEVER BE HERE \n");
    return 1;
}



//commande load

// le main charge un fichier elf en entrée en utilisant
// les arguments du prototype de la fonction main (cf. fiches infos)

int _loadcmd(char *fichier_elf, interpreteur inter) {

    char* section_names[NB_SECTIONS]= {TEXT_SECTION_STR,RODATA_SECTION_STR,DATA_SECTION_STR,BSS_SECTION_STR};
    scntab section_table;
    unsigned int nsegments;
    int i=0;
    unsigned int type_machine;
    unsigned int endianness;   //little ou big endian
    unsigned int bus_width;    // 32 bits ou 64bits
    unsigned int next_segment_start = START_MEM; // compteur pour designer le début de la prochaine section

    //TODO declarer une memoire virtuelle, c'est elle qui contiendra toute les données du programme
    MAPMEM mem=NULL;

    stab symtab= new_stab(0); // table des symboles
    FILE * pf_elf;


    if ((pf_elf = fopen(fichier_elf,"r")) == NULL) {
        WARNING_MSG("cannot open file %s", fichier_elf); //----------------------------------> change error/warning
        return 1;
    }

    if (!assert_elf_file(pf_elf)) {
        WARNING_MSG("file %s is not an ELF file", fichier_elf);	//---------------------------> change error/warning
        return 1;
    }


    // recuperation des info de l'architecture
    elf_get_arch_info(pf_elf, &type_machine, &endianness, &bus_width);
    elf_load_scntab(pf_elf, bus_width, &section_table);
    // et des symboles
    elf_load_symtab(pf_elf, bus_width, endianness, &symtab);

    //TODO allouer la memoire virtuelle
    mem=init_memory_arm();
    inter->memory=mem;



    nsegments=0;
    next_segment_start = START_MEM;
    byte *ehdr    = __elf_get_ehdr(pf_elf );
    uint32_t taille;

    for (i=0; i<NB_SECTIONS; i++) {
        printf("\n***** Processing section named %s\n", section_names[i]);


        byte* content = elf_extract_scn_by_name(ehdr, pf_elf, section_names[i], &taille, NULL );

        if (content!=NULL) {
            printf("nsegments :: %d \n",nsegments);
            print_section_raw_content(section_names[i],next_segment_start,content,taille);
            inter->memory=ajout_seg_map(inter->memory,section_names[i],next_segment_start,taille,nsegments);
            next_segment_start+= ((taille+0x1000)>>12 )<<12; // on arrondit au 1k suppérieur
            nsegments++;

            // copier le contenu dans la memoire avant de liberer
            //  inter->memory=ajout_seg_map(inter->memory,section_names[i],next_segment_start,taille,nsegments-1);

            ((inter->memory)[nsegments-1]->flag)=1;
            ((inter->memory)[nsegments-1]->contenu)=content;
            ((inter->memory)[nsegments-1]->taille)=taille+1;
            ((inter->memory)[nsegments-1]->taille_max)=next_segment_start-((inter->memory)[nsegments-1]->adresse_initiale);
            affiche_segment(inter->memory[nsegments-1],NULL,NULL);
            //free(content);
        }


        else DEBUG_MSG("section %s not present in the elf file",section_names[i]);
    }
    free(ehdr);


    // allouer la pile (et donc modifier le nb de segments)
    unsigned int adrdebut=strtol("0xfffff000",NULL,16);
    inter->memory=ajout_seg_map(inter->memory,"STACK/HEAP",adrdebut,100000,PLACESTHEAP);      // en deci 68585222144
    ((inter->memory)[PLACESTHEAP]->flag)=1;
    ((inter->memory)[PLACESTHEAP]->contenu)=NULL;
    ((inter->memory)[PLACESTHEAP]->taille)=0;
    affiche_segment(inter->memory[PLACESTHEAP],NULL,NULL);
    //stab32_print( symtab );

    // on fait le ménage avant de partir
    //del_stab( symtab );
    del_scntab( section_table );
    fclose(pf_elf);
    puts("");
    return CMD_OK_RETURN_VALUE;
}


/*---Affiche fichier objet brut--------------------------------------------------------*/
void print_section_raw_content(char* name, unsigned int start, byte* content, unsigned int taille) {
    int k;
    unsigned char octet =0;
    printf("\n section %s (size %d bytes) loaded at %x :\n",name,taille,start);
    if (content!=NULL && taille>0) {
        for(k=0; k<taille; k+=1) {
            // on affiche le contenu de la section qui devrait se retrouver
            // en "memoire virtuelle" à l'adresse virtuelle start+k
            // (*(content + k) dans la mémoire physique)
            if(k%16==0) printf("\n  0x%08x ", start + k);
            octet = *((unsigned char *) (content+k));
            printf("%02x ",	octet);
        }
    }
    printf("\n");
}


/*Commande set
	3 cas possible
		-byte /-word /-reg
		*/
void _set_reg_cmd(interpreteur inter, char*reg, unsigned int valeur) {
    REGISTRE registre;
    registre = trouve_registre(reg,(inter->fulltable)[0]);
    if(registre!= NULL) {
        registre = modifier_valeur_reg(valeur,registre);
    }
    return;
}

void _set_cmd_apsr(interpreteur inter, char*reg) {
    REGISTRE registre;
    registre = trouve_registre(reg,(inter->fulltable)[1]);
    if(registre!= NULL) {
        inverser_registre_etat(registre);
    }
    return;
}

void _set_mem_byte_cmd(interpreteur inter, unsigned int adresse, char valeur) {
    SEGMENT seg;
    int i=0;
    for(i=0; i<3; i++) {
        seg = (inter->memory)[i];
        seg = change_val_seg(seg,adresse,valeur);
    }
    return;
}

void _set_mem_word_cmd(interpreteur inter, unsigned int adresse, unsigned int valeur) {
    SEGMENT seg;
    int i=0;
    char bytes[4];
    bytes[0] = (valeur >> 24) & 0xFF;
    bytes[1] = (valeur >> 16) & 0xFF;
    bytes[2] = (valeur >> 8) & 0xFF;
    bytes[3] = valeur & 0xFF;
    for(i=0; i<3; i++) {
        seg = (inter->memory)[i];
        seg = change_val_seg(seg,adresse+3,bytes[3]);
        seg = change_val_seg(seg,adresse+2,bytes[2]);
        seg = change_val_seg(seg,adresse+1,bytes[1]);
        seg = change_val_seg(seg,adresse,bytes[0]);
    }
    return;
}
/*
int _setcmd_mem(interpreteur inter,char* cas,char* endroit,unsigned int valeur){		// la valeur est un uint!!!!
	char* mode=strdup(cas);
	char valmodb=0;
	char valmodw[2]={0};
	unsigned int adresse=0;


	if(strcmp(mode,"byte")==0) {
		adresse=strtol(endroit,NULL,16);
		valmodb=valeur;
		change_val_seg((inter->memory)[PLACESTHEAP],adresse,valmodb);
		affiche_segment((inter->memory)[PLACESTHEAP],NULL,NULL);
		return CMD_OK_RETURN_VALUE;
		}

	else if(strcmp(mode,"word")==0) {
		adresse=strtol(endroit,NULL,16);
		valmodw[0] = (valeur >> 8) & 0xFF;
		valmodw[1] = valeur & 0xFF;
		change_plage_seg((inter->memory)[PLACESTHEAP],adresse,adresse+2,valmodw);
		affiche_segment((inter->memory)[PLACESTHEAP],NULL,NULL);
		return CMD_OK_RETURN_VALUE;
		}
	else {
		ERROR_MSG("Should never be here! \n");
		return 0;
		}
	}

int _setcmd_reg(interpreteur inter,char* endroit,unsigned int valeur){			// Modifer valeur registre
																// Valeur en u-int et endroit en string
	REGISTRE regis = NULL;

	if ((regis= trouve_registre(endroit,(inter->fulltable)[0]))!=NULL){							//trouver le registre
		regis=modifier_valeur_reg(valeur,regis);											//Ensuite modifier sa valeur
		afficher_table_registre((inter->fulltable)[0]);
		return CMD_OK_RETURN_VALUE;
		}

	else {
		//regis= trouve_registre(endroit,(inter->fulltable)[1]);
		//regis=modifier_valeur_reg(valeur,regis);
		afficher_table_registre((inter->fulltable)[0]);
		return CMD_OK_RETURN_VALUE;
		}

	//else {
		ERROR_MSG("Should never be here! \n");
		return 0;
		}*/
/*}*/



//Commande Assert----
int _assert_cmd(char* string, char* type , unsigned int val) {


    if (string==NULL || type==NULL || val==NULL) {
        WARNING_MSG("Erreur command assert");
        return 1;
    }


    return CMD_OK_RETURN_VALUE;
}
