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
        if (string!=NULL) return CMD_OK_RETURN_VALUE;
    }
    else if( strcmp(string,"apsr")==0) {
        afficher_table_registre_etat(inter->fulltable[1]);
        if (string!=NULL) return CMD_OK_RETURN_VALUE;
    }

    WARNING_MSG("Registre \"%s\" inconnu",string);
    return 1;
}



//commande load

// le main charge un fichier elf en entrée en utilisant
// les arguments du prototype de la fonction main (cf. fiches infos)

int _loadcmd(char *fichier_elf, interpreteur inter) {

    char* section_names[NB_SECTIONS]= {TEXT_SECTION_STR,RODATA_SECTION_STR,DATA_SECTION_STR,BSS_SECTION_STR};
    scntab section_table;
    unsigned int nsegments;
    int i=0,j=0;
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
    if (inter->memory != NULL) {
        DEBUG_MSG("Libération de la mémoire antérieure");
        for(j=0; j<NBSEG; j++) free((inter->memory)[j]) ;
        free(inter->memory);
    }
    mem=init_memory_arm();
    inter->memory=mem;



    nsegments=0;
    next_segment_start = START_MEM;
    byte *ehdr    = __elf_get_ehdr(pf_elf );
    uint32_t taille;

    for (i=0; i<NB_SECTIONS; i++) {
        printf("\n***** Processing section named %s\n", section_names[i]);
        byte* content = elf_extract_scn_by_name(ehdr, pf_elf, section_names[i], &taille, NULL );
        inter->memory=ajout_seg_map(inter->memory,section_names[i],next_segment_start,taille,nsegments);
        if (content!=NULL) {
            printf("nsegments :: %d \n",nsegments);
            print_section_raw_content(section_names[i],next_segment_start,content,taille);

            next_segment_start+= ((taille+0x1000)>>12 )<<12; // on arrondit au 1k suppérieur
            nsegments++;


            // copier le contenu dans la memoire avant de liberer

            ((inter->memory)[nsegments-1]->flag) = 1;
            ((inter->memory)[nsegments-1]->contenu) = (char*) content;
            ((inter->memory)[nsegments-1]->taille) = taille;
            ((inter->memory)[nsegments-1]->taille_max) = 0x1000;
            affiche_segment(inter->memory[nsegments-1],NULL,NULL);
            //free(content);
        }


        else DEBUG_MSG("section %s not present in the elf file",section_names[i]);
    }
    free(ehdr);


    // allouer la pile (et donc modifier le nb de segments)
    unsigned int adrdebut=strtol("0xfffff000",NULL,16);
    inter->memory=ajout_seg_map(inter->memory,"STACK/HEAP",adrdebut,0x100000,PLACESTHEAP);      // en deci 68585222144
    ((inter->memory)[PLACESTHEAP]->flag)=1;
    ((inter->memory)[PLACESTHEAP]->contenu)=calloc(1,sizeof(*((inter->memory)[PLACESTHEAP]->contenu)));
    ((inter->memory)[PLACESTHEAP]->taille)=0;
    ((inter->memory)[nsegments-1]->taille_max) = 0xFFFF;
    affiche_segment(inter->memory[PLACESTHEAP],NULL,NULL);
    //stab32_print( symtab );

    // on fait le ménage avant de partir
    del_stab( symtab );
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
    char bytes[2];
    bytes[1] = (valeur >> 8) & 0xFF;
    bytes[0] = valeur & 0xFF;
    for(i=0; i<3; i++) {
        seg = (inter->memory)[i];
        seg = change_val_seg(seg,adresse+1,bytes[1]);
        seg = change_val_seg(seg,adresse,bytes[0]);
    }
    return;
}



//Commande break: ------------------------------------------------
int _breakcmd_add(interpreteur inter , unsigned int adr) {
    inter->breaklist = add_list_break(inter->breaklist, adr);
    if (inter->breaklist == NULL) {
        WARNING_MSG("Erreur ajout impossible");
        return 1;
    }
    return CMD_OK_RETURN_VALUE;
}

int _breakcmd_del(BREAKPOINT* list , unsigned int adr) {
    BREAKPOINT l=*list;
    BREAKPOINT p;

    if (*list == NULL) {
        WARNING_MSG("Liste des breakpoints vide");
        return 1;
    }
    if (l->stop_adr == adr) {
        *list = dehead_list_break(*list);
        printf("adresse 0x%X supprimée\n",adr);
        return CMD_OK_RETURN_VALUE;
    }

    while((l->nxt)!=NULL && ((l->nxt)->stop_adr != adr)) {
        l = l->nxt;
    }
    if(l->nxt== NULL) {
        WARNING_MSG(" Point d'arret inexistant");
        return 1;
    }
    p = l->nxt;
    p = dehead_list_break(p);
    l->nxt = p;
    DEBUG_MSG("adresse 0x%X supprimée",adr);
    return CMD_OK_RETURN_VALUE;
}

int _breakcmd_del_all(BREAKPOINT *list) {
    if (*list == NULL) {
        WARNING_MSG("Liste des points d'arret est vide");
        return 1;
    }
    *list = free_break_list(*list);
    DEBUG_MSG("Liste libérée puis supprimée");
    return CMD_OK_RETURN_VALUE;
}


// Assert commande // ----------------------------------------------
/*
int _assert_cmd(interpreteur inter , unsigned int adress , unsigned int value , int type){
}
*/

int _assert_cmd(interpreteur inter, char* string, char* type , unsigned int val) {
    SEGMENT seg;
    int i = 0;
    unsigned int adress;
    unsigned int init;
    char* ptr =NULL; char* ptrd = NULL;
    char bytes[2];
    if (inter == NULL || inter->memory ==NULL) {
        WARNING_MSG("Erreur memoire inaccessible");
        return 1;
    }
    if (string==NULL || type==NULL ) {
        WARNING_MSG("Erreur command assert");
        return 1;
    }
    adress = strtoul(string,NULL,16);
    if(strcmp(type,"byte")==0) {
        for(i=0; i<3; i++) {
            seg = (inter->memory)[i];
            init = (seg->adresse_initiale);
            if (adress >= init ) {
	      ptr = get_byte_seg(seg,adress);
	      if(ptr!=NULL){
		if(*ptr == val){
		  DEBUG_MSG("La valeur est bien à 0x%X",val);
		  return CMD_OK_RETURN_VALUE;
		}
		  
                WARNING_MSG("À 0x%X La valeur est 0x%X et NON 0x%X",adress,seg->contenu[adress-init],val);
                return 1;
	      }
	    }
	}
    }
    if(strcmp(type,"word")==0) {
      adress = strtoul(string,NULL,16);
      if(adress%2 != 0){
	WARNING_MSG("Cannot access to an odd adress");
	return 1;
      }
      bytes[1] = (val >> 8) & 0xFF;
      bytes[0] = val & 0xFF;
      for(i=0;i<3; i++) {
	seg = (inter->memory)[i];
	init = (seg->adresse_initiale);
	  ptr = get_byte_seg(seg,adress);
	  ptrd = get_byte_seg(seg,adress+1);
	  if(ptr != NULL && ptrd != NULL){
	    if(*ptrd == bytes[1] && *ptr == bytes[0]){
	      DEBUG_MSG("La valeur est bien 0x%X",val);
	      return CMD_OK_RETURN_VALUE;
	    }	    
	    WARNING_MSG("À 0x%X La valeur est 0x%X %X et NON 0x%X",adress,(unsigned int)(seg->contenu[adress-init] & 0xFF),(unsigned int)(seg->contenu[adress-init+1] & 0xFF),val);
	  return 1 ;
	  }
      }
      WARNING_MSG("Adresse non atteinte ou Bug non réglé");
      return 1;
    }
    WARNING_MSG("Should never be here");
    return 1;
}



int _assert_cmd_reg(interpreteur inter , char* name , unsigned int value) {

    REGISTRE Reg;
    if(name == NULL) return 1;
    if(inter == NULL || inter->fulltable == NULL || inter->fulltable[0] == NULL ) {
        WARNING_MSG("Table Registre introuvable NULL");
        return 1;
    }

    Reg = trouve_registre(name,inter->fulltable[0]);
    if (Reg == NULL) {
        Reg = trouve_registre(name,inter->fulltable[1]);
        if (Reg == NULL) {
            WARNING_MSG("Registre Introuvable ?");
            return 1;
        }
    }
    if(value != Reg->valeur) {
        WARNING_MSG("Le registre %s de taille %d est de valeur 0x%X et non 0x%X,",Reg->nom,Reg->taille,Reg->valeur,value);
        return 1;
    }
    else if(value == Reg->valeur) {
        DEBUG_MSG(" Oui ! Le registre %s est de valeur 0x%X et de taille %d",Reg->nom,Reg->valeur,Reg->taille);
        return CMD_OK_RETURN_VALUE;
    }
    else {
        WARNING_MSG("Should never be here !");
        return 1;
    }
}
