
#include "inter.h"


/**
 * allocation et init interpreteur
 * @return un pointeur vers une structure allouée dynamiquement
 */
interpreteur init_inter(void) {
    interpreteur inter = calloc(1,sizeof(*inter));
    if (inter ==NULL)
        ERROR_MSG("impossible d'allouer un nouvel interpreteur");
    inter->memory = NULL;
    inter->fulltable = NULL;
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

int is_deci(char* chaine) {
    long long a = 0;
    char *end;
    a = strtol(chaine, &end, 10);
    return (chaine!=NULL
            && strlen(chaine)>0
            && *end=='\0');
}

int is_deci32(char* chaine) {
    long long a = 0;
    char *end;
    a = strtol(chaine, &end, 10);
    return (chaine!=NULL
            && strlen(chaine)>0
            && *end=='\0'
            && a<4294967296);
}

int is_deci8(char* chaine) {
    long long a = 0;
    char *end;
    a = strtol(chaine, &end, 10);
    return (chaine!=NULL
            && strlen(chaine)>0
            && *end=='\0'
            && a<256);
}

/* teste si un token est le nom d'un registre */
/* retourne NULL si le token n'est pas le nom d'un registre, le nom du registre sinon
   @param chaine letoken a analyser
   @return une chaine de caractere
*/

int is_reg(char *token) {

    if(token == NULL) return 0;

    if(strcmp(token,"r0") == 0
            || strcmp(token,"r1") == 0
            || strcmp(token,"r1") == 0
            || strcmp(token,"r2") == 0
            || strcmp(token,"r3") == 0
            || strcmp(token,"r4") == 0
            || strcmp(token,"r5") == 0
            || strcmp(token,"r6") == 0
            || strcmp(token,"r7") == 0
            || strcmp(token,"r8") == 0
            || strcmp(token,"r9") == 0
            || strcmp(token,"r10") == 0
            || strcmp(token,"r11") == 0
            || strcmp(token,"r12") == 0
            || strcmp(token,"sp") == 0
            || strcmp(token,"lr") == 0
            || strcmp(token,"pc") == 0
            || strcmp(token,"apsr") == 0)
        return 1;
    else return 0;
}

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

}

/**
 * retourne le type du token (fonction très incomplete)
 * @param chaine le token à analyser
 * @return un entier correspondant au type du token
 *
 */
int get_type(char* chaine) {
    if (is_hexa(chaine))		//-----------------@ entier 32 Hexa
        return HEXA;
    if (is_deci32(chaine))		//----------------entier 32 bits
        return DECI32;
    if (is_deci8(chaine))		//----------------entier 8 bits
        return DECI8;
    if (is_reg(chaine)==1)		//----------------reg
        return REG;
    if (is_range(chaine))		//----------------range or plage
        return RANGE;
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
        case HEXA:
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
        WARNING_MSG("\n****Erreur****\nRentrez un nom de fichier a charger");   // indique "Erreur" si l'utilisateur rentre juste "load"
        return 1;
    }
    else {
        nom_du_fichier = strdup(token);
        printf("\nnom du fichier : %s\n",nom_du_fichier);
        if((token=get_next_token(inter)) == NULL) return _loadcmd(nom_du_fichier,inter);
        else {
            if(get_type(token)== HEXA) {
                adresse_du_fichier = strdup(token);
                printf("adresse du fichier = %s \n",adresse_du_fichier);

            }
            else {
                WARNING_MSG("\n****Erreur****\nAdresse non valide\n");
                return 1;
            }
        }
    }
    DEBUG_MSG("Charger ELF à partir d'une adresse n'est pas diponible");
    return CMD_OK_RETURN_VALUE;
}




/* **********************Commande display*************************** */



int dispcmd(interpreteur inter) {

    char *token = NULL;
    char *adresse1 = NULL;
    char *adresse2 = NULL;
    //char *registre = NULL;

    if((token = get_next_token(inter)) == NULL) {		//l'utilisateur ne rentre rien après disp
        WARNING_MSG("\n****Erreur****\nIl manque des parametre(mem ou reg)");
        return 1;
    }

    else if(strcmp(token,"mem")==0) {			//l'utilisateur veut afficher de la mémoire
        if((inter->memory)==NULL) {
            WARNING_MSG("Mémoire indisponible pour le moment , charger SVP un fichier ELF d'abord");
            return 1;
        }
        if((token=get_next_token(inter)) == NULL) {
            WARNING_MSG("\n****Erreur****\nRentrez une plage mémoire");
            return 1;
        }
        else if(get_type(token) == HEXA) {    //l'utilisateur veut afficher une plage de mémoire
            adresse1=strdup(token);
            printf("adresse1 = %s\n",adresse1);
            if((token=get_next_token(inter)) == NULL) {
                WARNING_MSG("\n****Erreur****\nRentrez ':' puis une 2eme adresse\n");
                return 1;
            }
            else if(strcmp(token,":") == 0) {
                if((token=get_next_token(inter)) == NULL) {
                    WARNING_MSG("\n****Erreur****\nRentrez une 2eme adresse valide\n");
                    return 1;
                }
                else if(get_type(token) == HEXA) {
                    adresse2=strdup(token);
                    printf("adresse2 = %s\n",adresse2);
                    return _dispcmd("plage",inter,adresse1,adresse2);
                }
                else {
                    WARNING_MSG("\n****Erreur****\n2eme adresse non valide");
                    return 1;
                }
            }
            else {
                WARNING_MSG("\n****Erreur****\nRentrez ':' pour séparer vos 2 adresses");
                return 1;
            }
        }
        else if(strcmp(token,"map") == 0) {   //affichage du map mémoire
            printf("\nAffichage du map mémoire\n");
            return _dispcmd(token,inter,NULL,NULL);
        }
        else {
            WARNING_MSG("\n****Erreur****\nRentrez une plage mémoire");
            return 1;
        }
    }


    else if(strcmp(token,"reg")==0) {			//l'utilisateur veut afficher des registres
        if((token = get_next_token(inter)) == NULL) {
            WARNING_MSG("\n****Erreur****\nRentrez des registres à afficher");
            return 1;
        }
        else if(strcmp(token,"all") == 0) {	    //l'utilisateur veut afficher tous les registres
            printf("appel fonction affichage table de registre\n");
            return _dispcmd("all register",inter,NULL,NULL);
        }

        else {
            while(token != NULL) {
                if(is_reg(token) != 0) {
                    printf("appel fct affiche reg singulier \n");
                    return _dispcmd("token",inter,NULL,NULL);
                    token = get_next_token(inter);
                }
                else {
                    WARNING_MSG("\n****Erreur****\nRentrez des noms de registres valides");
                    return 1;
                }
            }
        }
    }

    else {
        WARNING_MSG("\n****Erreur****\nRentrez des paramètres valides");
        return 1;
    }
    printf("NEVER SHOULD BE HERE\n");
    return 1;
}



/* **************************** Commande Disasm *********************************** */

int disasmcmd(interpreteur inter) {
    char *token = NULL;
    char *adresse1 = NULL;
    char *adresse2 = NULL;
    char *decalage = NULL;


    if((token = get_next_token(inter)) == NULL) {
        WARNING_MSG("\n****Erreur****\nRentrez une plage");
        return 1;
    }

    if(is_hexa(token)) {
        adresse1 = token;
        printf("\nadresse1 = %s\n",adresse1);

        if((token = get_next_token(inter)) == NULL) {
            WARNING_MSG("\n****Erreur****\nRentrez une 2eme adresse ou un décalage");
            return 1;
        }


        else if(strcmp(token,":") == 0) {
            if((token = get_next_token(inter)) == NULL) {
                WARNING_MSG("\n****Erreur****\nIl manque une deuxieme adresse");
                return 1;
            }

            else if(is_hexa(token)) {
                adresse2 = token;
                printf("adresse2 = %s\n\n",adresse2);
            }
            else {
                WARNING_MSG("\n****Erreur****\nDeuxieme adresse non valable");
                return 1;
            }
        }
        else if(strcmp(token,"+") == 0) {
            if((token = get_next_token(inter)) == NULL) {
                WARNING_MSG("\n****Erreur****\nRentrez le decalage souhaite");
                return 1;
            }
            else if(is_deci32(token)) {
                decalage = token;
                printf("decalage = %s\n\n",decalage);
            }
            else {
                WARNING_MSG("\n****Erreur****\nRentrez un decalage valide");
                return 1;
            }
        }
        else {
            WARNING_MSG("\n****Erreur****\nRentrez une 2eme adresse ou un décalage");
            return 1;
        }
    }
    else {
        WARNING_MSG("\n****Erreur****\nAdresse non valide");
        return 1;
    }
    if (get_next_token(inter)!=NULL){
    	WARNING_MSG("Plage incorrecte ou terme supplementaire parazite :)");
    	return 1;
    	}
    
    SEGMENT seg=trouve_segment_nom(".text",inter->memory);
    return _desasm_cmd(seg, 0 , 0);


}


/* ************************* Commande Set ******************************* */
/*
int setcmd(interpreteur inter){
   char *token = NULL;
   //char *type = NULL;
   char *adresse = NULL;
   char *registre = NULL;
   char *valeur = NULL;
   unsigned int valfin=0;

   if((token = get_next_token(inter)) == NULL){
	WARNING_MSG("\n****Erreur****\nRentrez un parametre (reg ou mem)");
	return 1;
   }

   if(strcmp(token,"mem") == 0){
	DEBUG_MSG("Modifie un champs dans la memoire");

	if((token = get_next_token(inter)) == NULL){
	    WARNING_MSG("\n****Erreur****\nRentrez un type (byte ou word)");	//!!! erreur------------------
	    return 1;
	}																		//--------------------sortie
	else if(strcmp(token,"byte") == 0){
	    DEBUG_MSG("Type : byte\n");

		token = get_next_token(inter);
	    //if((token = get_next_token(inter)) == NULL){									// correction
		if(token == NULL){
		WARNING_MSG("\n****Erreur****\nRentrez une adresse hexadecimale");
		return 1;
		}

	    else if(get_type(token) == HEXA){
		adresse = token;
		DEBUG_MSG("Adresse à modifier = %s",adresse);
		if((token = get_next_token(inter)) == NULL){
		    WARNING_MSG("\n****Erreur****\nRentrez la valeur a mettre a cette adresse");
		    return 1;
		}

		else if(is_deci(token) == DECI){
		    valeur = token;
		    DEBUG_MSG("Nouvelle valeur = %s",valeur);
		   	valfin=strtol(valeur,NULL,10);
		 	return _setcmd_mem(inter,"byte",adresse,valfin);		// <<<<---------------------------A REFAIRE & FINIR
		}  //int _setcmd_mem(interpreteur inter,char* cas,char* endroit,char* valeur)
		else{
		    WARNING_MSG("\n****Erreur****\nRentrez une valeur decimale entiere");
		    return 1;
		}
	    }
	    else{
		WARNING_MSG("\n****Erreur****\nRentrez une valeur hexadecimale");
		return 1;
	    }
	}
	else if(strcmp(token,"word") == 0){
	    printf("type : word\n");
	    if((token = get_next_token(inter)) == NULL){
		WARNING_MSG("\n****Erreur****\nRentrez une adresse hexadecimale");
		return 1;
	    }
	    else if(get_type(token) == HEXA){
		adresse = strdup(token);
		DEBUG_MSG("Adresse à modifier = %s\n",adresse);
		if((token = get_next_token(inter)) == NULL){
		    WARNING_MSG("\n****Erreur****\nRentrez la valeur à mettre à cette adresse");
		    return 1;
		}
		else if(get_type(token) == DECI){
		    valeur = strdup(token);
		    DEBUG_MSG("Nouvelle valeur = %s \n",valeur);
		    return _setcmd_mem(inter,"word",adresse,valeur);
		    //return _setcmd("word dans la memoire valeur sur deux octets");		// <<<<---------------------------A REFAIRE & FINIR
		}
		else{
		    WARNING_MSG("\n****Erreur****\nRentrez une valeur decimale entiere");
		    return 1;
		}
	    }
	    else{
		WARNING_MSG("\n****Erreur****\nRentrez une valeur hexadecimale");
		return 1;
	    }
	}
	else{
	    WARNING_MSG("\n****Erreur****\nRentrez un type valide (byte ou word)");
	    return 1;
	}
    }



    else if(strcmp(token,"reg") == 0){
	printf("Modifie un registre\n");
	if((token = get_next_token(inter)) == NULL){
	    WARNING_MSG("\n****Erreur****\nRentrez le nom d'un registre");
	    return 1;
	}
	else if(get_reg(token) == NULL){
	    WARNING_MSG("\n****Erreur****\nRentrez le nom d'un registre valide");
	    return 1;
	}
	else{
	    registre = strdup(token);
	    DEBUG_MSG("Registre à modifier : %s \n",registre);
	    if((token = get_next_token(inter)) == NULL){
		WARNING_MSG("\n****Erreur****\nRentrez une valeur");
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
		WARNING_MSG("\n****Erreur****\nRentrez une valeur decimale entiere");
		return 1;
	    }
	}
    }

    else{
	WARNING_MSG("\n****Erreur****\nParametres non valides, rentrez mem ou reg");
	return 1;
	}
return CMD_OK_RETURN_VALUE;
}*/
/* ********************** Commande Assert **************************** */
int assertcmd(interpreteur inter) {
    char *token= NULL;
    char *registre = NULL;
    char *valeur = NULL;
    char *adresse = NULL;

    if((token=get_next_token(inter)) == NULL) {
        WARNING_MSG("\n****Erreur****\nIl manque des paramètres (reg, word ou byte)");
        return 1;
    }

    else if(strcmp(token,"reg") == 0) {
        printf("reg\n");
        if((token=get_next_token(inter)) == NULL) {
            WARNING_MSG("\n****Erreur****\nIl manque le nom d'un registre àco évaluer");
            return 1;
        }
        else if(is_reg(token) != 0) {
            registre = token;
            printf("registre : %s\n",registre);
            if((token=get_next_token(inter)) == NULL) {
                WARNING_MSG("\n****Erreur****\nIl manque la valeur a tester");
                return 1;
            }
            else if(get_type(token) == DECI) {
                valeur = token;
                printf("valeur a tester: %s\n",valeur);
            }
        }
        else {
            WARNING_MSG("\n****Erreur****\nnom du registre non valide");
            return 1;
        }
    }
    else if(strcmp(token,"word") == 0) {					//l'utilisatuer veut évaluer un word
        printf("word\n");
        if((token=get_next_token(inter)) == NULL) {
            WARNING_MSG("\n****Erreur****\nIl manque l'adresse du word a evaluer");
            return 1;
        }
        else if(get_type(token) == HEXA) {				//l'adresse est bien en hexa
            adresse = token;
            printf("adresse du word : %s\n",adresse);
            if((token=get_next_token(inter)) == NULL) {
                WARNING_MSG("\n****Erreur****\nIl manque la valeur a tester");
                return 1;
            }
            else if(get_type(token) == DECI) {
                valeur = token;
                printf("valeur a tester : %s\n",valeur);
            }
            else {
                WARNING_MSG("\n****Erreur****\nValeur non valide");
                return 1;
            }
        }
        else {
            WARNING_MSG("\n****Erreur****\nAdresse non valide");
            return 1;
        }
    }
    else if(strcmp(token,"byte") == 0) {					//l'utilisateur veut évaluer un byte
        printf("byte\n");
        if((token=get_next_token(inter)) == NULL) {
            WARNING_MSG("\n****Erreur****\nIl manque l'adresse du byte a evaluer");
            return 1;
        }
        else if(get_type(token) == HEXA) {				//l'adresse est bien en hexa
            adresse = token;
            printf("adresse du byte : %s\n",adresse);
            if((token=get_next_token(inter)) == NULL) {
                WARNING_MSG("\n****Erreur****\nIl manque la valeur a tester");
                return 1;
            }
            else if(get_type(token) == DECI) {
                valeur = token;
                printf("valeur a tester : %s\n",valeur);
            }
            else {
                WARNING_MSG("\n****Erreur****\nValeur non valide");
                return 1;
            }
        }
        else {
            WARNING_MSG("\n****Erreur****\nAdresse non valide");
            return 1;
        }
    }
    return CMD_OK_RETURN_VALUE;
}

//***********************Commandes  Debug+ resume :-------------


int debugcmd(interpreteur inter) {
    DEBUG_MSG("Chaine : %s", inter->input);
    char* token = NULL;
    if((token = get_next_token(inter))==NULL) {
        INFO_MSG("interruption de l'execution et remise de main au user");
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
            printf ("reprise de l'execution du script");
            return CMD_OK_RETURN_VALUE ;
        }
        else printf("Pas de mode DEBUG ");
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
    /*
        else if(strcmp(token,"set") == 0){
    	return setcmd(inter);
        }*/

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




