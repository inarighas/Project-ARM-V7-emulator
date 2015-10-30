/**
 * 
 * @file emulARM.c
 * @author François Cayre, Nicolas Castagné, François Portet
 * @brief Main pour le début du projet émulateur.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "registre.h"
#include "memory.h"
  
/* la librairie readline */
#include <readline/readline.h>
#include <readline/history.h>

/* macros de DEBUG_MSG fournies , etc */
#include "common/notify.h"


/* prompt du mode shell interactif */
#define PROMPT_STRING "ARMShell : > "

/* taille max pour nos chaines de char */
#define MAX_STR 1024


/*************************************************************\
Valeur de retour speciales pour la fonction
	int execute_cmd(interpreteur inter) ;
Toute autre valeur signifie qu'une erreur est survenue
 \*************************************************************/
#define CMD_OK_RETURN_VALUE 0
#define CMD_EXIT_RETURN_VALUE -1
#define CMD_UNKOWN_RETURN_VALUE -2


//--LOAD

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include "common/bits.h"
#include "common/notify.h"
#include "elf/elf.h"
#include "elf/syms.h"
#include "elf/relocator.h"

// On fixe ici une adresse basse dans la mémoire virtuelle. Le premier segment
// ira se loger à cette adresse.
#define START_MEM 0x3000

// nombre max de sections que l'on extraira du fichier ELF
#define NB_SECTIONS 4

// nom de chaque section
#define TEXT_SECTION_STR ".text"
#define RODATA_SECTION_STR ".rodata"
#define DATA_SECTION_STR ".data"
#define BSS_SECTION_STR ".bss"

// fonction affichant les octets d'un segment sur la sortie standard
// parametres :
//   name           : nom de la section dont le contenue est affichée
//   start          : adresse virtuelle d'implantation du contenu de la section dans la memoire 
//   content        : le contenu de la section à afficher
//   taille         : taille en octet de la section à afficher


//--------------------------------------------------
/* type de token (exemple) */
enum {HEXA,DECI,UNKNOWN};

/* mode d'interaction avec l'interpreteur (exemple)*/
typedef enum {INTERACTIF,SCRIPT,DEBUG_MODE} inter_mode;

/* structure passée en parametre qui contient la connaissance de l'état de
 * l'interpréteur
 */
typedef struct {
    inter_mode mode;
    char input[MAX_STR];
    char * from;
    char first_token;
    MAPMEM memory;
    REGISTRE ** fulltable;
} *interpreteur;


//#include "command.h"-------------------------------------------------------------
void print_section_raw_content(char* name, unsigned int start, byte* content, unsigned int taille) ;
int _loadcmd(char *fichier_elf, interpreteur inter);
int _dispcmd(char*string,interpreteur inter,char * adrdep,char*adrarr);



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
    strtol(chaine, &end, 16);
    return (chaine!=NULL 
	    && strlen(chaine)>2
	    && chaine[0]=='0' && chaine[1]=='x'     //si ca commence par 0 et x
	    && sscanf(chaine,"%x",&i)==1 
	    && strlen(chaine)<11 
	    && *end=='\0'); 
}


int is_deci(char* chaine) {
  //int i;
    char *end;
    //long int a;
    strtol(chaine, &end, 10);
    return (chaine!=NULL 
	    && strlen(chaine)>0 
	    &&/* sscanf(chaine,"%x",&i)==1 
	    && strlen(chaine)<11 
	    &&*/ *end=='\0');
}

/* teste si un token est le nom d'un registre */
/* retourne NULL si le token n'est pas le nom d'un registre, le nom du registre sinon
   @param chaine letoken a analyser
   @return une chaine de caractere
*/

char *get_reg(char *token){
    if(token == NULL)return NULL;

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
	return token;
    else return NULL;
}

/**
 * retourne le type du token (fonction très incomplete)
 * @param chaine le token à analyser
 * @return un entier correspondant au type du token
 *
 */
int get_type(char* chaine) {
    if (is_hexa(chaine))
        return HEXA;
    if (is_deci(chaine))
		return DECI;
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
					
    if((token = get_next_token(inter))==NULL){	
	WARNING_MSG("\n****Erreur****\nRentrez un nom de fichier a charger");   // indique "Erreur" si l'utilisateur rentre juste "load"
	return 1;
    }
    else{
	nom_du_fichier = strdup(token);
	printf("\nnom du fichier : %s\n",nom_du_fichier);
    	if((token=get_next_token(inter)) == NULL) return _loadcmd(nom_du_fichier,inter);
	else{
	    if(get_type(token)== HEXA){
		adresse_du_fichier = strdup(token);
		printf("adresse du fichier = %s \n",adresse_du_fichier);
		
	    }
	    else{
		WARNING_MSG("\n****Erreur****\nAdresse non valide\n");
		return 1;
	    }
	}
    }
    DEBUG_MSG("Charger ELF à partir d'une adresse n'est pas diponible");
	return CMD_OK_RETURN_VALUE;
}




/* **********************Commande display*************************** */


int dispcmd(interpreteur inter){

    char *token = NULL;
    char *adresse1 = NULL;
    char *adresse2 = NULL;
    //char *registre = NULL;

    if((token = get_next_token(inter)) == NULL){		//l'utilisateur ne rentre rien après disp
	WARNING_MSG("\n****Erreur****\nIl manque des parametre(mem ou reg)");
	return 1;
    }
       
    else if(strcmp(token,"mem")==0){			//l'utilisateur veut afficher de la mémoire
	if((token=get_next_token(inter)) == NULL){
	    WARNING_MSG("\n****Erreur****\nRentrez une plage mémoire");
	    return 1;
	}
	else if(get_type(token) == HEXA){     //l'utilisateur veut afficher une plage de mémoire
	    adresse1=strdup(token);
	    printf("adresse1 = %s\n",adresse1);
	    if((token=get_next_token(inter)) == NULL){
		WARNING_MSG("\n****Erreur****\nRentrez ':' puis une 2eme adresse\n");
		return 1;
	    }
	    else if(strcmp(token,":") == 0){
		if((token=get_next_token(inter)) == NULL){
		    WARNING_MSG("\n****Erreur****\nRentrez une 2eme adresse valide\n");
		    return 1;
		}
		else if(get_type(token) == HEXA){
		    adresse2=strdup(token);
		    printf("adresse2 = %s\n",adresse2);
		    return _dispcmd("plage",inter,adresse1,adresse2);
		}
		else{
		    WARNING_MSG("\n****Erreur****\n2eme adresse non valide");
		    return 1;
		}
	    }
	    else {
		WARNING_MSG("\n****Erreur****\nRentrez ':' pour séparer vos 2 adresses");
		return 1;
	    }
	}
	else if(strcmp(token,"map") == 0){    //affichage du map mémoire
	    printf("\nAffichage du map mémoire\n");
	    return _dispcmd(token,inter,NULL,NULL);
	}
	else {
	    WARNING_MSG("\n****Erreur****\nRentrez une plage mémoire");
	    return 1;
	}
	}
	

    else if(strcmp(token,"reg")==0){			//l'utilisateur veut afficher des registres
	if((token = get_next_token(inter)) == NULL){
	    WARNING_MSG("\n****Erreur****\nRentrez des registres à afficher");
	    return 1;
	}
	else if(strcmp(token,"all") == 0){	     //l'utilisateur veut afficher tous les registres
	    printf("appel fonction affichage table de registre\n");
	    return _dispcmd("all register",inter,NULL,NULL);
	}
	
	else {
	    while(token != NULL){
		if(get_reg(token) != NULL){
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

int disasmcmd(interpreteur inter){
   char *token = NULL;
   char *adresse1 = NULL;
   char *adresse2 = NULL;
   char *decalage = NULL;


    if((token = get_next_token(inter)) == NULL){
	WARNING_MSG("\n****Erreur****\nRentrez une plage");
	return 1;
    }

   if(is_hexa(token)){
	adresse1 = token;
	printf("\nadresse1 = %s\n",adresse1);

	if((token = get_next_token(inter)) == NULL){
	    WARNING_MSG("\n****Erreur****\nRentrez une 2eme adresse ou un décalage");
	    return 1;
	}
	    

	else if(strcmp(token,":") == 0){
	    if((token = get_next_token(inter)) == NULL){
		WARNING_MSG("\n****Erreur****\nIl manque une deuxieme adresse");
		return 1;
	    }

	    else if(is_hexa(token)){
		adresse2 = token;
		printf("adresse2 = %s\n\n",adresse2);
	    }
	    else {
		WARNING_MSG("\n****Erreur****\nDeuxieme adresse non valable");
		return 1;
	    }
	}
	else if(strcmp(token,"+") == 0) {
	    if((token = get_next_token(inter)) == NULL){
		WARNING_MSG("\n****Erreur****\nRentrez le decalage souhaite");
		return 1;
	    }
	    else if(is_deci(token)){
		decalage = token;
		printf("decalage = %s\n\n",decalage);
	    }
	    else{
		WARNING_MSG("\n****Erreur****\nRentrez un decalage valide");
		return 1;
	    }
	}
	else{
	    WARNING_MSG("\n****Erreur****\nRentrez une 2eme adresse ou un décalage");
	    return 1;
	}
   }
   else {
	WARNING_MSG("\n****Erreur****\nAdresse non valide");
	return 1;
	}
return CMD_OK_RETURN_VALUE;

}


/* ************************* Commande Set ******************************* */

int setcmd(interpreteur inter){
   char *token = NULL;
   //char *type = NULL;
   char *adresse = NULL;
   char *registre = NULL;
   char *valeur = NULL;

   if((token = get_next_token(inter)) == NULL){
	WARNING_MSG("\n****Erreur****\nRentrez un parametre (reg ou mem)");
	return 1;
   }

   if(strcmp(token,"mem") == 0){
	printf("\nModifie une valeur dans la memoire\n");

	if((token = get_next_token(inter)) == NULL){
	    WARNING_MSG("\n****Erreur****\nRentrez un type (byte ou word)");
	    return 1;
	}
	else if(strcmp(token,"byte") == 0){
	    printf("type : byte\n");

	    if((token = get_next_token(inter)) == NULL){
		WARNING_MSG("\n****Erreur****\nRentrez une adresse hexadecimale");
		return 1;
		}
	    else if(get_type(token) == HEXA){
		adresse = token;
		printf("adresse = %s\n",adresse);
		if((token = get_next_token(inter)) == NULL){
		    WARNING_MSG("\n****Erreur****\nRentrez la valeur a mettre a cette adresse");
		    return 1;
		}
		else if(is_deci(token) == DECI){
		    valeur = token;
		    printf("nouvelle valeur = %s\n\n\n",valeur);
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
	else if(strcmp(token,"word") == 0){
	    printf("type : word\n");
	    if((token = get_next_token(inter)) == NULL){
		WARNING_MSG("\n****Erreur****\nRentrez une adresse hexadecimale");
		return 1;
	    }
	    else if(get_type(token) == HEXA){
		adresse = token;
		printf("adresse = %s\n",adresse);
		if((token = get_next_token(inter)) == NULL){
		    WARNING_MSG("\n****Erreur****\nRentrez la valeur à mettre à cette adresse");
		    return 1;
		}	
		else if(get_type(token) == DECI){
		    valeur = token;
		    printf("valeur = %s\n\n\n",valeur);
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
	    registre = token;
	    printf("registre : %s\n",registre);
	    if((token = get_next_token(inter)) == NULL){
		WARNING_MSG("\n****Erreur****\nRentrez une valeur");
		return 1;
	    }
	    else if(get_type(token) == DECI){
		valeur = token;
		printf("valeur = %s\n\n\n",token);
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
}
/* ********************** Commande Assert **************************** */
int assertcmd(interpreteur inter){
    char *token= NULL;
    char *registre = NULL;
    char *valeur = NULL;
    char *adresse = NULL;
    
    if((token=get_next_token(inter)) == NULL){
	WARNING_MSG("\n****Erreur****\nIl manque des paramètres (reg, word ou byte)");
	return 1;
    }

    else if(strcmp(token,"reg") == 0){
	printf("reg\n");
	if((token=get_next_token(inter)) == NULL){
	    WARNING_MSG("\n****Erreur****\nIl manque le nom d'un registre a evaluer");
	    return 1;
	}
	else if(get_reg(token) != NULL){
	    registre = token;
	    printf("registre : %s\n",registre);
	    if((token=get_next_token(inter)) == NULL){
		WARNING_MSG("\n****Erreur****\nIl manque la valeur a tester");
		return 1;
	    }
	    else if(get_type(token) == DECI){
		valeur = token;
		printf("valeur a tester: %s\n",valeur);
	    }
	}
	else{
	   WARNING_MSG("\n****Erreur****\nnom du registre non valide");
	    return 1;
	}	
    }
    else if(strcmp(token,"word") == 0){					//l'utilisatuer veut évaluer un word
	printf("word\n");
	if((token=get_next_token(inter)) == NULL){
	    WARNING_MSG("\n****Erreur****\nIl manque l'adresse du word a evaluer");
	    return 1;
	}
	else if(get_type(token) == HEXA){				//l'adresse est bien en hexa
	    adresse = token;
	    printf("adresse du word : %s\n",adresse);
	    if((token=get_next_token(inter)) == NULL){
		WARNING_MSG("\n****Erreur****\nIl manque la valeur a tester");
		return 1;
	    }
	    else if(get_type(token) == DECI){
		valeur = token;
		printf("valeur a tester : %s\n",valeur);
	    }
	    else{
		WARNING_MSG("\n****Erreur****\nValeur non valide");
		return 1;
	    }
	}
	else{
	    WARNING_MSG("\n****Erreur****\nAdresse non valide");
	    return 1;
	}
    }
    else if(strcmp(token,"byte") == 0){					//l'utilisateur veut évaluer un byte
	printf("byte\n");
	if((token=get_next_token(inter)) == NULL){
	    WARNING_MSG("\n****Erreur****\nIl manque l'adresse du byte a evaluer");
	    return 1;
	}
	else if(get_type(token) == HEXA){				//l'adresse est bien en hexa
	    adresse = token;
	    printf("adresse du byte : %s\n",adresse);
	    if((token=get_next_token(inter)) == NULL){
		WARNING_MSG("\n****Erreur****\nIl manque la valeur a tester");
		return 1;
	    }
	    else if(get_type(token) == DECI){
		valeur = token;
		printf("valeur a tester : %s\n",valeur);
	    }
	    else{
		WARNING_MSG("\n****Erreur****\nValeur non valide");
		return 1;
	    }
	}
	else{
	    WARNING_MSG("\n****Erreur****\nAdresse non valide");
	    return 1;
	}
    }
return CMD_OK_RETURN_VALUE;
}

//***********************Commandes  Debug+ resume :-------------


int debugcmd(interpreteur inter){
  DEBUG_MSG("Chaine : %s", inter->input);
  char* token = NULL;
  if((token = get_next_token(inter))==NULL) {
    INFO_MSG("interruption de l'execution et remise de main au user");
    return CMD_OK_RETURN_VALUE ;
  }
    else WARNING_MSG("commande incorrecte");
  return 1;
}
  

int resumecmd(interpreteur inter){
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
int runcmd(interpreteur inter){
  char* token = NULL;
  char token1[500];
  //DEBUG_MSG("Chaine : %s", inter->input);

  if((token = get_next_token(inter))==NULL) {
    INFO_MSG("Execute from adress saved in the current PC value ");
    return CMD_OK_RETURN_VALUE;
    }
  
  if (token!=NULL) strncpy(token1,token,500);
  if(is_hexa(token) && ((token = get_next_token(inter))==NULL)){
    INFO_MSG("Execute from this adress %s",token1) ;
    return CMD_OK_RETURN_VALUE ;
    }
  
  WARNING_MSG("false Command ");
  return 1;
}

//********** Commande step : -----------------------------------------------
int stepcmd(interpreteur inter){
  char* token = NULL;
  //char token1[500];
  //DEBUG_MSG("Chaine : %s", inter->input);

  if((token = get_next_token(inter))==NULL) {
    INFO_MSG("Avancement jusqu'à l'instruction de retour ");
    return CMD_OK_RETURN_VALUE ;
     }
  
  else if(strcmp( token ,"into")==0){
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

    else if(strcmp(token,"disp")==0){
	return dispcmd(inter);
    }


    else if(strcmp(token,"test") == 0) {
        return testcmd(inter);
    }

    else if(strcmp(token,"disasm") ==0 ){
	return disasmcmd(inter);
    }

    else if(strcmp(token,"set") == 0){
	return setcmd(inter);
    }

    else if(strcmp(token,"assert") == 0){
	return assertcmd(inter);
    }
     
    else if(strcmp(token,"step") == 0){
	return stepcmd(inter);
    }

    else if(strcmp(token,"run") == 0){
	return runcmd(inter);
    }
    
    else if(strcmp(token,"debug") == 0){
	return debugcmd(inter);
    }

    else if(strcmp(token,"resume") == 0){
	return resumecmd(inter);
    }
    
    WARNING_MSG("Unknown Command : '%s'\n", cmdStr);
    return CMD_UNKOWN_RETURN_VALUE;

}





/**
 * @param in Input line (possibly very badly written).
 * @param out Line in a suitable form for further analysis.
 * @return nothing
 * @brief This function will prepare a line for further analysis.
 *
 * This function will prepare a line for further analysis and check for low-level syntax errors.
 * colon, brackets, commas are surrounded with blank; tabs are replaced with blanks.
 * negs '-' are attached to the following token (e.g., "toto -   56" -> "toto -56")  .
 */
void string_standardise( char* in, char* out ) {
    unsigned int i=0, j;

    for ( j= 0; i< strlen(in); i++ ) {

        /* insert blanks around special characters*/
        if (in[i]==':' || in[i]=='+' || in[i]=='~') {
            out[j++]=' ';
            out[j++]=in[i];
            out[j++]=' ';

        }

        /* remove blanks after negation*/
        else if (in[i]=='-') {
            out[j++]=' ';
            out[j++]=in[i];
            while (isblank((int) in[i+1])) i++;
        }

        /* insert one blank before comments */
        else if (in[i]=='#') {
            out[j++]=' ';
            out[j++]=in[i];
        }
        /* translate tabs into white spaces*/
        else if (isblank((int) in[i])) out[j++]=' ';
        else out[j++]=in[i];
    }
}


/**
 *
 * @brief extrait la prochaine ligne du flux fp.
 * Si fp ==stdin, utilise la librairie readline pour gestion d'historique de commande.
 *
 * @return 0 si succes.
 * @return un nombre non nul si aucune ligne lue
 */
int  acquire_line(FILE *fp, interpreteur inter) {
    char* chunk =NULL;

    memset(inter->input, '\0', MAX_STR );
    inter->first_token =0;
    if (inter->mode==SCRIPT) {
        // mode fichier
        // acquisition d'une ligne dans le fichier
        chunk =calloc(MAX_STR, sizeof(*chunk));
        char * ret = fgets(chunk, MAX_STR, fp );
        if(ret == NULL) {
            free(chunk);
            return 1;
        }
        // si windows remplace le \r du '\r\n' (fin de ligne windows) par \0
        if(strlen(ret) >1 && ret[strlen(ret) -2] == '\r') {
            ret[strlen(ret)-2] = '\0';
        }
        // si unix remplace le \n par \0
        else if(strlen(ret) >0 && ret[strlen(ret) -1] == '\n') {
            ret[strlen(ret)-1] = '\0';
        }

    }
    else {
        /* mode shell interactif */
        /* on utilise la librarie libreadline pour disposer d'un historique */
        chunk = readline( PROMPT_STRING );
        if (chunk == NULL || strlen(chunk) == 0) {
            /* commande vide... */
            if (chunk != NULL) free(chunk);
            return 1;
        }
        /* ajout de la commande a l'historique, librairie readline */
        add_history( chunk );

    }
    // standardisation de la ligne d'entrée (on met des espaces là ou il faut)
    string_standardise(chunk, inter->input);

    free( chunk ); // liberation de la mémoire allouée par la fonction readline() ou par calloc()

    DEBUG_MSG("Ligne acquise '%s'\n", inter->input); /* macro DEBUG_MSG : uniquement si compil en mode DEBUG_MSG */
    return 0;
}


/****************/
void usage_ERROR_MSG( char *command ) {
    fprintf( stderr, "Usage: %s [file.emul]\n   If no file is given, executes in Shell mode.", command );
}


/**
 * Programme principal
 */
int main ( int argc, char *argv[] ) {
    /* exemples d'utilisation des macros du fichier notify.h */
    INFO_MSG("Un message INFO_MSG : Debut du programme %s", argv[0]); /* macro INFO_MSG */
    WARNING_MSG("Un message WARNING_MSG !"); /* macro INFO_MSG */
    DEBUG_MSG("Un message DEBUG_MSG !"); /* macro DEBUG_MSG : uniquement si compil en mode DEBUG_MSG */
    interpreteur inter=init_inter(); /* structure gardant les infos et états de l'interpreteur*/
    FILE *fp = NULL; /* le flux dans lequel les commande seront lues : stdin (mode shell) ou un fichier */

    if ( argc > 2 ) {
        usage_ERROR_MSG( argv[0] );
        exit( EXIT_FAILURE );
    }
    if(argc == 2 && strcmp(argv[1], "-h") == 0) {
        usage_ERROR_MSG( argv[0] );
        exit( EXIT_SUCCESS );
    }

    /*par defaut : mode shell interactif */
    fp = stdin;
    inter->mode = INTERACTIF;
    if(argc == 2) {
        /* mode fichier de commandes */
        fp = fopen( argv[1], "r" );
        if ( fp == NULL ) {
            perror( "fopen" );
            exit( EXIT_FAILURE );
        }
        inter->mode = SCRIPT;
    }

    /* boucle infinie : lit puis execute une cmd en boucle */
    while ( 1 ) {



        if (acquire_line( fp,  inter)  == 0 ) {
            /* Une nouvelle ligne a ete acquise dans le flux fp*/

            int res = execute_cmd(inter); /* execution de la commande */

            // traitement des erreurs
            switch(res) {
            case CMD_OK_RETURN_VALUE:
                break;
            case CMD_EXIT_RETURN_VALUE:
                /* sortie propre du programme */
                if ( inter->mode == SCRIPT ) {
                    fclose( fp );
                }
                del_inter(inter);
                exit(EXIT_SUCCESS);
                break;
            default:
                /* erreur durant l'execution de la commande */
                /* En mode "fichier" toute erreur implique la fin du programme ! */
                if (inter->mode == SCRIPT) {
                    fclose( fp );
                    del_inter(inter);
                    /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
                    ERROR_MSG("ERREUR DETECTEE. Aborts");
                }
                break;
            }
        }
        if( inter->mode == SCRIPT && feof(fp) ) {
            /* mode fichier, fin de fichier => sortie propre du programme */
            DEBUG_MSG("FIN DE FICHIER");
            fclose( fp );
            del_inter(inter);
            exit(EXIT_SUCCESS);
        }
    }
    /* tous les cas de sortie du programme sont gérés plus haut*/
    ERROR_MSG("SHOULD NEVER BE HERE\n");
}






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
        for(i=0;i<NBSEG;i++){
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
        for(i=0;i<NBSEG;i++){
            if((inter->memory)[i]!=NULL && (inter->memory)[i]->flag==1) affiche_segment((inter->memory)[i],adrdep,adrarr);
  		 }
        return CMD_OK_RETURN_VALUE;
    }

    else if(strcmp(string,"all register")==0) {
        printf("Nom \t \t Taille \t \t Valeur \n ");
        if((inter->fulltable) == NULL){
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
        ERROR_MSG("cannot open file %s", fichier_elf);
        return 1;
    }

    if (!assert_elf_file(pf_elf)) {
        ERROR_MSG("file %s is not an ELF file", fichier_elf);
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
            next_segment_start+= ((taille+0x1000)>>12 )<<12; // on arrondit au 1k suppérieur
            nsegments++;

            //TODO: copier le contenu dans la memoire avant de liberer
           inter->memory=ajout_seg_map(inter->memory,section_names[i],next_segment_start,taille,nsegments-1);
			
			((inter->memory)[nsegments-1]->flag)=1;           
            ((inter->memory)[nsegments-1]->contenu)=content;
            ((inter->memory)[nsegments-1]->taille)=taille;
            affiche_segment(inter->memory[nsegments-1],NULL,NULL);
            //free(content);
        }


        else DEBUG_MSG("section %s not present in the elf file",section_names[i]);
    }
    free(ehdr);
	

  //TODO: allouer la pile (et donc modifier le nb de segments)
    unsigned int adrdebut=strtol("0xfffff000",NULL,16);
   inter->memory=ajout_seg_map(inter->memory,"STACK/HEAP",adrdebut,100000,4);      // en deci 68585222144
   //stab32_print( symtab );

    // on fait le ménage avant de partir
    //del_stab( symtab );
    del_scntab( section_table );
    fclose(pf_elf);
    puts("");
    return CMD_OK_RETURN_VALUE;
}


/*-----------------------------------------------------------*/
void print_section_raw_content(char* name, unsigned int start, byte* content, unsigned int taille) {
    int k;
    unsigned char octet =0;
    printf("\n section %s loaded at %x :\n",name,start); 
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

