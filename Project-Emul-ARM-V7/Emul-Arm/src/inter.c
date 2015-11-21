//Code source gérant l'interpreteur -Initialisation et libération ..
//Identification des chaines rentrées par l'utilisateur
//Appel des commandes associées

#include "inter.h"
#include "commandes.h"

/**
 * allocation et init interpreteur
 * @return un pointeur vers une structure allouée dynamiquement
 */
interpreteur init_inter(void) {
    interpreteur inter = calloc(1,sizeof(*inter));
    if (inter ==NULL)
        ERROR_MSG("impossible d'allouer un nouvel interpreteur");
    inter->memory = NULL;
    inter->fulltable = calloc(2,sizeof(*(inter->fulltable)));
    inter->fulltable[0]=init_table_registre();
    inter->fulltable[1]=init_table_registre_etat();
    return inter;
}

/**
 * desallocation de l'interpreteur
 * @param inter le pointeur vers l'interpreteur à libérer
 */
void del_inter(interpreteur inter) {
    if (inter !=NULL)
        free(inter);
}

/**
 * return le prochain token de la chaine actuellement
 * analysée par l'interpreteur
 * La fonction utilise une variable interne de l'interpreteur
 * pour gérer les premiers appels a strtok
 * @inter le pointeur vers l'interpreteur
 * @return un pointeur vers le token ou NULL
 */
char* get_next_token(interpreteur inter) {

    char       *token = NULL;
    char       *delim = " \t\n";

    if ( inter->first_token == 0 ) {
        token = strtok_r( inter->input, delim, &(inter->from) );
        inter->first_token = 1;
    }
    else {
        token = strtok_r( NULL, delim, &(inter->from) );
    }

    if ( NULL == token ) {
        inter->first_token = 0;
    }

    return token;
}


char* tolower_string(char* s,int n_max) {
    int i;
    char* new = NULL;
    if (s==NULL) {
        DEBUG_MSG("Erreur, can't lower the string");
        return NULL;
    }
    new = strdup(s);
    for (i = 0; new[i] != '\0'|| i<n_max; i++)
        new[i] = (char)tolower(new[i]);
    return new;
}

/**
 * teste si un token est une valeur hexa
 * ATTENTION cette méthode n'est pas complete et ne fonctionnera pas dans tous les cas
 * essayer avec 0x567ZRT...
 *@param chaine le token à analyser
 *@return 0 si non-hexa, non null autrement
 */

int is_hexa(char* chaine) {
    int i;
    char *end;
    unsigned long val=0;
    val= strtol(chaine, &end, 16);
    return (chaine!=NULL
            && strlen(chaine)>2
            && chaine[0]=='0' && chaine[1]=='x'     //si ca commence par 0 et x
            && sscanf(chaine,"%x",&i)==1
            && strlen(chaine)<11
            && *end=='\0'
            && val <= 0xFFFFFFFF);
}

int is_hexa32(char* chaine) {
    int i;
    char *end;
    unsigned long val=0;
    val= strtoul(chaine, &end, 16);
    return (chaine!=NULL
            && strlen(chaine)>2
            && chaine[0]=='0' && chaine[1]=='x'     //si ca commence par 0 et x
            && sscanf(chaine,"%x",&i)==1
            && strlen(chaine)<11
            && *end=='\0'
            && val <= 0xFFFFFFFF
            && val >  0xFF);
}

int is_hexa8(char* chaine) {
    int i;
    char *end;
    unsigned long val=0;
    val= strtoul(chaine, &end, 16);
    return (chaine!=NULL
            && strlen(chaine)>2
            && chaine[0]=='0' && chaine[1]=='x'     //si ca commence par 0 et x
            && sscanf(chaine,"%x",&i)==1
            && strlen(chaine)<11
            && *end=='\0'
            && val <= 0xFF);
}


int is_deci(char* chaine) {
    long long a = 0;
    char *end;
    a = strtoul(chaine, &end, 10);
    return (chaine!=NULL
            && strlen(chaine)>0
            && *end=='\0'
            && a <= 0xFFFFFFFF);
}

int is_deci32(char* chaine) {
    long long a = 0;
    char *end;
    a = strtol(chaine, &end, 10);
    return (chaine!=NULL
            && strlen(chaine)>0
            && *end=='\0'
            && a <= 0xFFFFFFFF
            && a >  0xFF);
}

int is_deci8(char* chaine) {
    unsigned long a = 0;
    char *end;
    a = strtoul(chaine,&end, 10);
    return (chaine!=NULL
            && strlen(chaine)>0
            && *end=='\0'
            && a <= 0xFF);
}

/* teste si un token est le nom d'un registre */
/* retourne NULL si le token n'est pas le nom d'un registre, le nom du registre sinon
   @param chaine letoken a analyser
   @return une chaine de caractere
*/

int is_reg(char *token) {
    char* str=NULL;
    int i=0;
    if(token == NULL) return 0;
    str=strdup(token);
    while( str[i] ) {
        str[i]=tolower(str[i]);
        i++;
    }

    if(strcmp(str,"r0") == 0
            || strcmp(str,"r1") == 0
            || strcmp(str,"r1") == 0
            || strcmp(str,"r2") == 0
            || strcmp(str,"r3") == 0
            || strcmp(str,"r4") == 0
            || strcmp(str,"r5") == 0
            || strcmp(str,"r6") == 0
            || strcmp(str,"r7") == 0
            || strcmp(str,"r8") == 0
            || strcmp(str,"r9") == 0
            || strcmp(str,"r10") == 0
            || strcmp(str,"r11") == 0
            || strcmp(str,"r12") == 0
            || strcmp(str,"sp") == 0
            || strcmp(str,"lr") == 0
            || strcmp(str,"pc") == 0)
        return 1;
    else return 0;
}


int is_state_reg(char *token2) {

    //char* token=NULL;
    if(token2 == NULL) return 0;

    //token = tolower_string(token2,6);

    if(strcmp(token2,"apsr") == 0)
        return 1;
    else return 0;
}
/*
int is_range(char *chaine) {

    char* token;
    if (chaine==NULL) return 0;
    //token=get_next_token(inter);
    //if(strcmp(token,"+")==0)
    //puts(token);
    if (is_hexa(token)) {
        token=strtok(NULL ," \t\n");
        if(token!=NULL && (is_deci32(token) || is_deci8(token))
                && (strtok(NULL ," \t\n")==NULL)) return 1;
        else return 0;
    }

    else if(is_hexa(strtok(chaine ,":"))) {
        token=strtok(NULL," \t\n");
        if ((token!= NULL)
                && is_hexa(token)
                &&(strtok(NULL ," \t\n")==NULL)) return 1;
        else return 0;
    }

    return 0;

}*/

/**
 * retourne le type du token (fonction très incomplete)
 * @param chaine le token à analyser
 * @return un entier correspondant au type du token
 *
 */
int get_type(char* chaine) {
    if (is_hexa8(chaine))		//-----------------@ entier 8 Hexa
        return HEXA8;

    if (is_hexa32(chaine))		//-----------------@ entier 32 Hexa
        return HEXA32;

    if (is_deci8(chaine))		//----------------entier 32 bits
        return DECI8;

    if (is_deci32(chaine))		//----------------entier 8 bits
        return DECI32;

    if (is_reg(chaine)==1)		//----------------reg
        return REG;

    if (is_state_reg(chaine)==1)
        return STATEREG;

    /*if (is_range(chaine))		//----------------range or plage
        return RANGE;*/
    return UNKNOWN;
}

/*************************************************************\
Les commandes de l'émulateur.

 Dans cette version, deux commandes :
	"test" qui attend un nombre strictement positifs d'hexa strictement positifs et affiche ce(s) nombre() +1 dans le terminal
	"exit" qui quitte l'émulateur

 \*************************************************************/

/**
 * version de la commande test qui prend en entrée un hexa
 * la fonction verifie si le nombre est >=0
 * @param hexValue la valeur à afficher
 * @return 0 en case de succes, un nombre positif sinon
 */
int _testcmd(int hexValue) {
    if(hexValue <= 0) {
        WARNING_MSG("command %s  - invalid call. The argument should be positive.\n", "test");
        return 2;
    }
    fprintf(stdout, "CMD TEST RESULT 0x%x\n", hexValue + 1);

    return CMD_OK_RETURN_VALUE;
}


/**
 * version de la commande test qui analyse la chaîne entrée à
 * l'interpreteur.
 * Si la commande est correcte elle est executée.
 * Si la commande contient plusieurs parametres valides, elle
 * est excutée plusieurs fois.
 * @param inter l'interpreteur qui demande l'analyse
 * @return 0 en case de succes, un nombre positif sinon
 */

int testcmd(interpreteur inter) {
    DEBUG_MSG("Chaine : %s", inter->input);
    int return_value=0;

    int no_args=1;
    /* la commande test attend un argument hexa */
    int hexValue;
    char * token=NULL;

    /* la boucle permet d'executé la commande de manière recurrente*/
    /* i.e., tant qu'il y a un hexa on continue*/
    while((token = get_next_token(inter))!=NULL && return_value==0) {
        no_args=0;
        switch(get_type(token)) {
        case HEXA32 :
        case HEXA8  :
            sscanf(token,"%x",&hexValue);
            return_value = _testcmd(hexValue);
            break;
        default :
            WARNING_MSG("value %s is not a valid argument of command %s\n",token,"testcmd");
            return 1;
        }
    }


    if(no_args) {
        WARNING_MSG("no argument given to command %s\n","testcmd");
        return 1;
    }


    return return_value;
}


/**
 * commande exit qui ne necessite pas d'analyse syntaxique
 * @param inter l'interpreteur qui demande l'analyse
 * @return 0 en case de succes, un nombre positif sinon
 */
int exitcmd(interpreteur inter) {
    INFO_MSG("Bye bye !");
    return CMD_EXIT_RETURN_VALUE;
}


/*   **********************Commande load*************************  */

int loadcmd(interpreteur inter) {
    char *token = NULL;
    char *nom_du_fichier = NULL;
    char *adresse_du_fichier = NULL;

    if((token = get_next_token(inter))==NULL) {
        WARNING_MSG("[**Erreur**] 	Rentrez un nom de fichier a charger");   // indique "Erreur" si l'utilisateur rentre juste "load"
        return 1;
    }
    else {
        nom_du_fichier = strdup(token);
        printf("\nnom du fichier : %s\n",nom_du_fichier);
        if((token=get_next_token(inter)) == NULL)return _loadcmd(nom_du_fichier,inter);
        else {
            if(get_type(token)== HEXA32 || get_type(token) == HEXA8) {
                adresse_du_fichier = strdup(token);
                printf("adresse du fichier = %s \n",adresse_du_fichier);

            }
            else {
                WARNING_MSG("[**Erreur**] 	Adresse non valide\n");
                return 1;
            }
        }
    }
    DEBUG_MSG("Charger ELF à partir d'une adresse n'est pas diponible");
    if ((token = get_next_token(inter))!= NULL) {
        WARNING_MSG("Commande Incorrecte");
        return 1;
    }
    return CMD_OK_RETURN_VALUE;
}




/* **********************Commande display*************************** */



int dispcmd(interpreteur inter) {

    char *token = NULL;
    char *adresse1 = NULL;
    char *adresse2 = NULL;
    //char *registre = NULL;

    if((token = get_next_token(inter)) == NULL) {		//l'utilisateur ne rentre rien après disp
        WARNING_MSG("[**Erreur**] 	Il manque des parametre(mem ou reg)");
        return 1;
    }

    else if(strcmp(token,"mem")==0) {			//l'utilisateur veut afficher de la mémoire
        if((inter->memory)==NULL) {
            WARNING_MSG("Mémoire indisponible pour le moment , charger SVP un fichier ELF d'abord");
            return 1;
        }
        if((token=get_next_token(inter)) == NULL) {
            WARNING_MSG("[**Erreur**] 	Rentrez une plage mémoire");
            return 1;
        }
        else if(get_type(token) == HEXA32 || get_type(token) == HEXA8) {    //l'utilisateur veut afficher une plage de mémoire
            adresse1=strdup(token);
            DEBUG_MSG("adresse1 = %s\n",adresse1);
            if((token=get_next_token(inter)) == NULL) {
                WARNING_MSG("[**Erreur**] 	Rentrez ':' puis une 2eme adresse\n");
                return 1;
            }
            else if(strcmp(token,":") == 0) {
                if((token=get_next_token(inter)) == NULL) {
                    WARNING_MSG("[**Erreur**] 	Rentrez une 2eme adresse valide\n");
                    return 1;
                }
                else if(get_type(token) == HEXA32 || get_type(token) == HEXA8) {
                    adresse2=strdup(token);
                    DEBUG_MSG("adresse2 = %s\n",adresse2);
                    if (get_next_token(inter)!= NULL) {
                        WARNING_MSG("Commande Incorrecte");
                        return 1;
                    }
                    return _dispcmd("plage",inter,adresse1,adresse2);
                }
                else {
                    WARNING_MSG("[**Erreur**] 	2eme adresse non valide");
                    return 1;
                }
            }
            else {
                WARNING_MSG("[**Erreur**] 	Rentrez ':' pour séparer vos 2 adresses");
                return 1;
            }
        }
        else if(strcmp(token,"map") == 0) {   //affichage du map mémoire
            if (get_next_token(inter)!= NULL) {
                WARNING_MSG("Commande Incorrecte");
                return 1;
            }
            INFO_MSG("Affichage du map mémoire");
            return _dispcmd(token,inter,NULL,NULL);
        }
        else {
            WARNING_MSG("[**Erreur**] 	Rentrez une plage mémoire");
            return 1;
        }
    }


    else if(strcmp(token,"reg")==0) {			//l'utilisateur veut afficher des registres
        if((token = get_next_token(inter)) == NULL) {
            WARNING_MSG("[**Erreur**] 	Rentrez des registres à afficher");
            return 1;
        }
        else if(strcmp(token,"all") == 0) {	    //l'utilisateur veut afficher tous les registres
            if (get_next_token(inter)!= NULL) {
                WARNING_MSG("Commande Incorrecte");
                return 1;
            }
            DEBUG_MSG("appel fonction affichage table de registre");
            return _dispcmd("all register",inter,NULL,NULL);
        }

        else {
            while(token != NULL) {
                if(is_reg(token) != 0) {
                    if (get_next_token(inter)!= NULL) {
                        WARNING_MSG("Commande Incorrecte");
                        return 1;
                    }
                    DEBUG_MSG("appel fct affiche reg singulier");
                    return _dispcmd(token,inter,NULL,NULL);
                    token = get_next_token(inter);
                }
                else {
                    WARNING_MSG("[**Erreur**] 	Rentrez des noms de registres valides");
                    return 1;
                }
            }
        }
    }

    else {
        WARNING_MSG("[**Erreur**] 	Rentrez des paramètres valides");
        return 1;
    }
    ERROR_MSG("SHOULD NEVER BE HERE");
    return 1;
}



/* **************************** Commande Disasm *********************************** */

int disasmcmd(interpreteur inter) {
    char *token = NULL;
    char *adresse1 = NULL;
    char *adresse2 = NULL;
    char *decalage = NULL;

    unsigned int add1=0;
    unsigned int add2=0;
    if((token = get_next_token(inter)) == NULL) {
        WARNING_MSG("[**Erreur**] 	Rentrez une plage");
        return 1;
    }

    if(is_hexa(token)) {
        adresse1 = token;
        DEBUG_MSG("adresse1 = %s",adresse1);

        if((token = get_next_token(inter)) == NULL) {
            WARNING_MSG("[**Erreur**] 	Rentrez une 2eme adresse ou un décalage");
            return 1;
        }


        else if(strcmp(token,":") == 0) {
            if((token = get_next_token(inter)) == NULL) {
                WARNING_MSG("[**Erreur**] 	Il manque une deuxieme adresse");
                return 1;
            }

            else if(is_hexa(token)) {
                adresse2 = token;
                DEBUG_MSG("adresse2 = %s",adresse2);
                if(strtol(adresse1,NULL,16)>strtol(adresse2,NULL,16)) {
                    WARNING_MSG("Adresse de départ après Adresse d'arrivée");
                    return 1;
                }
            }
            else {
                WARNING_MSG("[**Erreur**] 	Deuxieme adresse non valable");
                return 1;
            }
            add1=strtoul(adresse1,NULL,16);
            add2=strtoul(adresse2,NULL,16);

        }
        else if(strcmp(token,"+") == 0) {
            if((token = get_next_token(inter)) == NULL) {
                WARNING_MSG("[**Erreur**] 	Rentrez le decalage souhaite");
                return 1;
            }
            else if(is_deci32(token) || is_deci8(token)) {
                decalage = token;
                DEBUG_MSG("decalage = %s",decalage);
            }
            else {
                WARNING_MSG("[**Erreur**] 	Rentrez un decalage valide");
                return 1;
            }
        }
        else {
            WARNING_MSG("[**Erreur**] 	Rentrez une 2eme adresse ou un décalage");
            return 1;
        }
    }
    else {
        WARNING_MSG("[**Erreur**] 	Adresse non valide");
        return 1;
    }
    if (get_next_token(inter)!=NULL) {
        WARNING_MSG("Plage incorrecte ou terme supplementaire parazite :)");
        return 1;
    }
    if(add1%2!=0 || add2%2!=0) {
        WARNING_MSG("[**Erreur**] Adresse impaire");
        return 1;
    }

    SEGMENT seg=trouve_segment_nom(".text",inter->memory);
    return _desasm_cmd(seg, add1 , add2);


}


/* ************************* Commande Set ******************************* */

int setcmd(interpreteur inter) {
    char* token=NULL;
    unsigned int adresse =0;
    unsigned int valeur=0;
    char* registre=NULL;
    char apsrsaisi[20]= {0};
    char octet=0;

    if((token = get_next_token(inter)) == NULL) {
        WARNING_MSG("[**Erreur**] 	Rentrez un parametre (reg ou mem)");
        return 1;
    }

    if(strcmp(token,"reg") == 0) {
        DEBUG_MSG("Modification de la valeur de registres");
        token=get_next_token(inter);
        while(token!=NULL) {

            if (token==NULL) {
                WARNING_MSG("Commande incorrecte: Rentrez un registre");
                return 1;
            }
            switch (get_type(token)) {
            case REG:
                registre=strdup(token);
                token=get_next_token(inter);
                if (token==NULL) {
                    WARNING_MSG("Commande incorrecte: Rentrez une valeur");
                    return 1;
                }
                switch (get_type(token)) {
                case DECI32:
                case DECI8:
                    valeur=strtol(token,NULL,10);
                    DEBUG_MSG("Remise du registre %s à la valeur %d ",registre,valeur);
                    _set_reg_cmd(inter,registre,valeur);
                    break;
                case HEXA32:
                case HEXA8:
                    valeur=strtoul(token,NULL,16);
                    DEBUG_MSG("Remise du registre %s à la valeur 0x%X ",registre,valeur);
                    _set_reg_cmd(inter,registre,valeur);
                    break;
                default:
                    WARNING_MSG("Veuillez rentrer un valeur hexadécimale ou décimale (32bits)");
                    return 1;
                    break;
                }
                break;

            case STATEREG:
                INFO_MSG("Quel registre registre d'état aimeriez vous inverser ?");
                INFO_MSG("N, C,  V ou Z?");
                scanf("%s",apsrsaisi);
                DEBUG_MSG("Inversion du registre APSR.%s",apsrsaisi);
                _set_cmd_apsr(inter,apsrsaisi);
                break;

            default:
                WARNING_MSG("Veuillez rentrer le nom d'un registre");
                return 1;
                break;
            }
            token = get_next_token(inter);
        }
        if (token!=NULL) {
            WARNING_MSG("Commande incorrecte ou terme supplementaire parazite :)");
            return 1;
        }
        return CMD_OK_RETURN_VALUE;
    }


    else if (strcmp(token,"mem")==0) {
        DEBUG_MSG("Modifie un champs dans la memoire");
        token=get_next_token(inter);
        while(token!=NULL) {
            if(token == NULL) {
                WARNING_MSG("[**Erreur**] 	Rentrez une adresse");
                return 1;
            }
            DEBUG_MSG("CAS 'MEM' ");
            puts(token);
            //while(token!=NULL){
            switch (get_type(token)) {
            case HEXA32:
            case HEXA8:
                DEBUG_MSG("CAS HEXA");
                adresse=strtol(token,NULL,16);
                token=get_next_token(inter);
                if (token==NULL) {
                    WARNING_MSG("Commande incorrecte: Rentrez byte ou word");
                    return 1;
                }
                if(strcmp(token,"byte")==0) {
                    DEBUG_MSG("CAS 	BYTE");
                    token=get_next_token(inter);
                    if (token==NULL) {
                        WARNING_MSG("Commande incorrecte: Rentrez une valeur");
                        return 1;
                    }
                    switch (get_type(token)) {
                    case HEXA8:
                        octet=strtoul(token,NULL,16);
                        DEBUG_MSG("Remise de l'octet à l'adresse 0x%X à la valeur 0x%X ",adresse,octet);
                        _set_mem_byte_cmd(inter,adresse,octet);
                        break;
                    case DECI8:
                        octet=strtol(token,NULL,10);
                        DEBUG_MSG("Remise de l'octet à l'adresse 0x%X à la valeur %d ",adresse,octet);
                        _set_mem_byte_cmd(inter,adresse,octet);
                        break;
                    default:
                        WARNING_MSG("Rentrez une valeur héxadécimale ou décimale (8bits)");
                        return 1;
                    }
                }

                else if(strcmp(token,"word")==0) {
                    DEBUG_MSG("CAS WORD");
                    token=get_next_token(inter);
                    if (token==NULL) {
                        WARNING_MSG("Commande incorrecte: Rentrez une valeur");
                        return 1;
                    }
                    switch (get_type(token)) {
                    case HEXA8:
                    case HEXA32:
                        valeur=strtoul(token,NULL,16);
                        DEBUG_MSG("Remise de l'octet à l'adresse 0x%X à la valeur 0x%X ",adresse,valeur);
                        _set_mem_word_cmd(inter,adresse,valeur);
                        break;
                    case DECI32:
                    case DECI8:
                        valeur=strtol(token,NULL,10);
                        DEBUG_MSG("Remise de l'octet à l'adresse 0x%X à la valeur %d ",adresse,valeur);
                        _set_mem_word_cmd(inter,adresse,valeur);
                        break;
                    default:
                        WARNING_MSG("Rentrez une valeur héxadécimale ou décimale (32bits)");
                        return 1;
                    }
                }
                else {
                    WARNING_MSG("Commande incorrecte: Rentrez byte ou word");
                    return 1;
                }
                break;
            default:
                DEBUG_MSG("CAS NOT HEXA");
                WARNING_MSG("Rentrez une adresse");
                return 1;
                break;
            }


            token=get_next_token(inter);
        }
        if (token!=NULL) {
            WARNING_MSG("Commande incorrecte ou terme supplementaire parazite :)");
            return 1;
        }
        return CMD_OK_RETURN_VALUE;
        //}
    }

    else {
        WARNING_MSG("[**Erreur**] 	Rentrez un parametre (reg ou mem)");
        return 1;
    }

    WARNING_MSG("SHOULD NEVER BE HERE");
    return 1;
}


/*if((token = get_next_token(inter)) == NULL){
WARNING_MSG("[**Erreur**] 	Rentrez un parametre (reg ou mem)");
return 1;
}

if(strcmp(token,"mem") == 0){
DEBUG_MSG("Modifie un champs dans la memoire");

if((token = get_next_token(inter)) == NULL){
    WARNING_MSG("[**Erreur**] 	Rentrez un type (byte ou word)");	//!!! erreur------------------
    return 1;
}																		//--------------------sortie
else if(strcmp(token,"byte") == 0){
    DEBUG_MSG("Type : Byte\n");

	token = get_next_token(inter);
    //if((token = get_next_token(inter)) == NULL){									// correction
	if(token == NULL){
	WARNING_MSG("[**Erreur**] 	Rentrez une adresse hexadecimale");
	return 1;
	}

    else if(get_type(token) == HEXA){
	adresse = token;
	DEBUG_MSG("Adresse à modifier = %s",adresse);
	if((token = get_next_token(inter)) == NULL){
	    WARNING_MSG("[**Erreur**] 	Rentrez la valeur a mettre a cette adresse");
	    return 1;
	}

	else if(is_deci(token) == DECI){
	    valeur = token;
	    DEBUG_MSG("Nouvelle valeur = %s",valeur);
	   	valfin=strtol(valeur,NULL,10);
	 	return _setcmd_mem(inter,"byte",adresse,valfin);		// <<<<---------------------------A REFAIRE & FINIR
	}  //int _setcmd_mem(interpreteur inter,char* cas,char* endroit,char* valeur)
	else{
	    WARNING_MSG("[**Erreur**] 	Rentrez une valeur decimale entiere");
	    return 1;
	}
    }
    else{
	WARNING_MSG("[**Erreur**] 	Rentrez une valeur hexadecimale");
	return 1;
    }
}
else if(strcmp(token,"word") == 0){
    printf("type : word\n");
    if((token = get_next_token(inter)) == NULL){
	WARNING_MSG("[**Erreur**] 	Rentrez une adresse hexadecimale");
	return 1;
    }
    else if(get_type(token) == HEXA){
	adresse = strdup(token);
	DEBUG_MSG("Adresse à modifier = %s\n",adresse);
	if((token = get_next_token(inter)) == NULL){
	    WARNING_MSG("[**Erreur**] 	Rentrez la valeur à mettre à cette adresse");
	    return 1;
	}
	else if(get_type(token) == DECI){
	    valeur = strdup(token);
	    DEBUG_MSG("Nouvelle valeur = %s \n",valeur);
	    return _setcmd_mem(inter,"word",adresse,valeur);
	    //return _setcmd("word dans la memoire valeur sur deux octets");		// <<<<---------------------------A REFAIRE & FINIR
	}
	else{
	    WARNING_MSG("[**Erreur**] 	Rentrez une valeur decimale entiere");
	    return 1;
	}
    }
    else{
	WARNING_MSG("[**Erreur**] 	Rentrez une valeur hexadecimale");
	return 1;
    }
}
else{
    WARNING_MSG("[**Erreur**] 	Rentrez un type valide (byte ou word)");
    return 1;
}
 }



 else if(strcmp(token,"reg") == 0){
printf("Modifie un registre\n");
if((token = get_next_token(inter)) == NULL){
    WARNING_MSG("[**Erreur**] 	Rentrez le nom d'un registre");
    return 1;
}
else if(get_reg(token) == NULL){
    WARNING_MSG("[**Erreur**] 	Rentrez le nom d'un registre valide");
    return 1;
}
else{
    registre = strdup(token);
    DEBUG_MSG("Registre à modifier : %s \n",registre);
    if((token = get_next_token(inter)) == NULL){
	WARNING_MSG("[**Erreur**] 	Rentrez une valeur");
	return 1;
    }
    else if(get_type(token) == DECI){
	valeur = strdup(token);
	DEBUG_MSG("Nouvelle valeur = %s \n",valeur);
	valfin=strtol(valeur,NULL,10);
	//return _setcmd("modifier registre -- > valeur sur unsigned int ");		// <<<<---------------------------A REFAIRE & FINIR
    return _setcmd_reg(inter,registre,valfin);
    }
    else{
	WARNING_MSG("[**Erreur**] 	Rentrez une valeur decimale entiere");
	return 1;
    }
}
 }

 else{
WARNING_MSG("[**Erreur**] 	Parametres non valides, rentrez mem ou reg");
return 1;
}
return CMD_OK_RETURN_VALUE;
}*/
/* ********************** Commande Assert **************************** */
int assertcmd(interpreteur inter) {
    char *token= NULL;
    char *registre = NULL;
    unsigned int valeur = NULL;
    char *adresse = NULL;

    if((token=get_next_token(inter)) == NULL) {
        WARNING_MSG("[**Erreur**] 	Il manque des paramètres (reg, word ou byte)");
        return 1;
    }

    else if(strcmp(token,"reg") == 0) {
        DEBUG_MSG("type\t\t\t: registre");
        if((token=get_next_token(inter)) == NULL) {
            WARNING_MSG("[**Erreur**] 	Il manque le nom d'un registre à co évaluer");
            return 1;
        }
        else if(is_reg(token) != 0) {
            registre = token;
            DEBUG_MSG("registre : %s",registre);
            if((token=get_next_token(inter)) == NULL) {
                WARNING_MSG("[**Erreur**] 	Il manque la valeur à tester");
                return 1;
            }
            else if(get_type(token) == DECI32 || get_type(token) == DECI8) {
                valeur =strtol(token,NULL,10);
                DEBUG_MSG("valeur a tester\t\t: %d ",valeur);
            }
            else if(get_type(token) == HEXA32 || get_type(token) == HEXA8) {
                valeur =strtol(token,NULL,16);
                DEBUG_MSG("valeur a tester\t\t: 0x%X ",valeur);
            }
        }
        else {
            WARNING_MSG("[**Erreur**] 	nom du registre non valide");
            return 1;
        }
        if (get_next_token(inter)!=NULL) {
            WARNING_MSG("Commande incorrecte ou terme supplementaire parazite :)");
            return 1;
        }
        return _assert_cmd("registre",registre,valeur);

    }

    else if(strcmp(token,"word") == 0) {					//l'utilisatuer veut évaluer un word
        DEBUG_MSG("type\t\t\t: Word");
        if((token=get_next_token(inter)) == NULL) {
            WARNING_MSG("[**Erreur**] 	Il manque l'adresse du word a evaluer");
            return 1;
        }
        else if(get_type(token) == HEXA32 || get_type(token) == HEXA8) {				//l'adresse est bien en hexa
            adresse = token;
            DEBUG_MSG("adresse du word\t\t: %s",adresse);
            if((token=get_next_token(inter)) == NULL) {
                WARNING_MSG("[**Erreur**] 	Il manque la valeur a tester");
                return 1;
            }
            else if(get_type(token) == DECI32 || get_type(token) == DECI8) {
                valeur = strtol(token,NULL,10);
                DEBUG_MSG("valeur a tester\t\t: %d ",valeur);
            }
            else if(get_type(token) == HEXA32 || get_type(token) == HEXA8) {
                valeur = strtol(token,NULL,16);
                DEBUG_MSG("valeur a tester\t\t: 0x%X ",valeur);
            }
            else {
                WARNING_MSG("[**Erreur**] 	Valeur non valide");
                return 1;
            }
        }
        else {
            WARNING_MSG("[**Erreur**] 	Adresse non valide");
            return 1;
        }
        if (get_next_token(inter)!=NULL) {
            WARNING_MSG("Commande incorrecte ou terme supplementaire parazite :)");
            return 1;
        }
        return _assert_cmd("word",adresse,valeur);
    }


    else if(strcmp(token,"byte") == 0) {					//l'utilisateur veut évaluer un byte
        DEBUG_MSG("type\t\t\t: Byte");
        if((token=get_next_token(inter)) == NULL) {
            WARNING_MSG("[**Erreur**] 	Il manque l'adresse du byte a evaluer");
            return 1;
        }
        else if(get_type(token) == HEXA32 || get_type(token) == HEXA8) {				//l'adresse est bien en hexa
            adresse = token;
            DEBUG_MSG("adresse du byte\t\t: %s",adresse);
            if((token=get_next_token(inter)) == NULL) {
                WARNING_MSG("[**Erreur**] 	Il manque la valeur a tester");
                return 1;
            }
            else if(get_type(token) == DECI8) {
                valeur = strtol(token,NULL,10);
                DEBUG_MSG("valeur a tester\t\t: %d",valeur);
            }
            else if(get_type(token) == HEXA8) {
                valeur = strtol(token,NULL,16);
                DEBUG_MSG("valeur a tester\t\t: 0x%X",valeur);
            }
            else {
                WARNING_MSG("[**Erreur**] 	Valeur non valide");
                return 1;
            }
        }
        else {
            WARNING_MSG("[**Erreur**] 	Adresse non valide");
            return 1;
        }
        if (get_next_token(inter)!=NULL) {
            WARNING_MSG("Commande incorrecte ou terme supplementaire parazite :)");
            return 1;
        }
        return _assert_cmd("byte",adresse,valeur);
    }
    WARNING_MSG("Commande incorrect:Rentrez le type d'element à tester");
    return 1;
}

//***********************Commandes  Debug+ resume :-------------


int debugcmd(interpreteur inter) {
    DEBUG_MSG("Chaine : %s", inter->input);
    char* token = NULL;
    if((token = get_next_token(inter))==NULL) {
        INFO_MSG("Interruption de l'execution et remise de main au user");
        return CMD_OK_RETURN_VALUE ;
    }
    else WARNING_MSG("commande incorrecte");
    return 1;
}


int resumecmd(interpreteur inter) {
    DEBUG_MSG("Chaine : %s", inter->input);
    char* token = NULL;
    if((token = get_next_token(inter))==NULL) {
        if(inter->mode==2) {
            INFO_MSG("Reprise de l'execution du script");
            return CMD_OK_RETURN_VALUE ;
        }
        else INFO_MSG("Pas de mode DEBUG ");
        return CMD_OK_RETURN_VALUE ;
    }
    else WARNING_MSG("commande incorrecte");
    return 1;
}

//***********************Commande run : -----------------------------------
int runcmd(interpreteur inter) {
    char* token = NULL;
    char token1[500];
    //DEBUG_MSG("Chaine : %s", inter->input);

    if((token = get_next_token(inter))==NULL) {
        INFO_MSG("Execute from adress saved in the current PC value ");
        return CMD_OK_RETURN_VALUE;
    }

    if (token!=NULL) strncpy(token1,token,500);
    if(is_hexa(token) && ((token = get_next_token(inter))==NULL)) {
        INFO_MSG("Execute from this adress %s",token1) ;
        return CMD_OK_RETURN_VALUE ;
    }

    WARNING_MSG("false Command ");
    return 1;
}

//********** Commande step : -----------------------------------------------
int stepcmd(interpreteur inter) {
    char* token = NULL;
    //char token1[500];
    //DEBUG_MSG("Chaine : %s", inter->input);

    if((token = get_next_token(inter))==NULL) {
        INFO_MSG("Avancement jusqu'à l'instruction de retour ");
        return CMD_OK_RETURN_VALUE ;
    }

    else if(strcmp( token ,"into")==0) {
        INFO_MSG("Execute one instruction") ;
        if (get_next_token(inter)!=NULL) {
            WARNING_MSG("Commande incorrecte ou terme supplementaire parazite :)");
            return 1;
        }
        return CMD_OK_RETURN_VALUE ;
    }

    else WARNING_MSG("false Command  ");
    return 1;

}


/*************************************************************\
 Les deux fonctions principales de l'émulateur.
	execute_cmd: parse la commande et l'execute en appelant la bonne fonction C
	acquire_line : recupere une ligne (donc une "commande") dans le flux
 \*************************************************************/


/**
*
* @brief parse la chaine courante de l'interpreteur à la recherche d'une commande, et execute cette commande.
* @param inter l'interpreteur qui demande l'analyse
* @return CMD_OK_RETURN_VALUE si la commande s'est exécutée avec succès (0)
* @return CMD_EXIT_RETURN_VALUE si c'est la commande exit. Dans ce cas, le programme doit se terminer. (-1)
* @return CMD_UNKOWN_RETURN_VALUE si la commande n'est pas reconnue. (-2)
* @return tout autre nombre (eg tout nombre positif) si erreur d'execution de la commande
*/
int execute_cmd(interpreteur inter) {
    DEBUG_MSG("input '%s'", inter->input);
    char cmdStr[MAX_STR];
    memset( cmdStr, '\0', MAX_STR );

    /* gestion des commandes vides, commentaires, etc*/
    if(strlen(inter->input) == 0
            || sscanf(inter->input, "%s", cmdStr) == 0
            || strlen(cmdStr) == 0
            || cmdStr[0] == '#') { /* ligne commence par # => commentaire*/
        return CMD_OK_RETURN_VALUE;
    }

    /*on identifie la commande avec un premier appel à get_next_token*/
    char * token = get_next_token(inter);

    if(strcmp(token, "exit") == 0) {
        return exitcmd(inter);
    }



    else if(strcmp(token,"load")==0) {
        return loadcmd(inter);
    }

    else if(strcmp(token,"disp")==0) {
        return dispcmd(inter);
    }


    else if(strcmp(token,"test") == 0) {
        return testcmd(inter);
    }

    else if(strcmp(token,"disasm") ==0 ) {
        return disasmcmd(inter);
    }

    else if(strcmp(token,"set") == 0) {
        return setcmd(inter);
    }

    else if(strcmp(token,"assert") == 0) {
        return assertcmd(inter);
    }

    else if(strcmp(token,"step") == 0) {
        return stepcmd(inter);
    }

    else if(strcmp(token,"run") == 0) {
        return runcmd(inter);
    }

    else if(strcmp(token,"debug") == 0) {
        return debugcmd(inter);
    }

    else if(strcmp(token,"resume") == 0) {
        return resumecmd(inter);
    }

    WARNING_MSG("Unknown Command : '%s'\n", cmdStr);
    return CMD_UNKOWN_RETURN_VALUE;

}




