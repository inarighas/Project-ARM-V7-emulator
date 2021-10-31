//                 Code fonction registre 27/09/2015                    //
//			Projet info 2a sicom emul ARM								//
//			Ali saghiran -- Damien chabannes							//


#include "registre.h"
#include "common/notify.h"


//Création registre unitaire avec taille à définir
REGISTRE creer_registre(char* name,int size) {
    REGISTRE p=NULL;
    p=calloc(1,sizeof(*p));
    if (p==NULL) {
        WARNING_MSG("erreur allocation registre\n");
        return NULL;
    }

    p->nom=name;
    p->taille=size;
    p->valeur=0;
    return p;
}

// Création de la tables des registres généraux

REGISTRE* init_table_registre(void) {
    //int i;
    //char s[10];    //not used
    REGISTRE* p;
    p=calloc(16,sizeof(*p));             //16 registres dans l'ARM
    if(p==NULL) {
        WARNING_MSG("erreur allocation table de registre \n");
        return NULL;
    }


    p[0]=creer_registre("r0",32);       //registres de taille 32 bits
    p[1]=creer_registre("r1",32);
    p[2]=creer_registre("r2",32);
    p[3]=creer_registre("r3",32);
    p[4]=creer_registre("r4",32);
    p[5]=creer_registre("r5",32);
    p[6]=creer_registre("r6",32);
    p[7]=creer_registre("r7",32);
    p[8]=creer_registre("r8",32);
    p[9]=creer_registre("r9",32);
    p[10]=creer_registre("r10",32);
    p[11]=creer_registre("r11",32);
    p[12]=creer_registre("r12",32);
    p[13]=creer_registre("sp",32);
    p[14]=creer_registre("lr",32);
    p[15]=creer_registre("pc",32);
    return p;
}

//Création de la table de registres d'états
REGISTRE* init_table_registre_etat(void) {
    //int i;
    REGISTRE* p;
    p=calloc(4,sizeof(*p));                // 4 registres d'états
    if(p==NULL) {
        printf("erreur allocation table de registre etat \n");
        return NULL;
    }
    p[0]=creer_registre("N",1);           // registres de taille 1 bit
    p[1]=creer_registre("Z",1);
    p[2]=creer_registre("C",1);
    p[3]=creer_registre("V",1);
    return p;
}


//Libération d'un registre

void free_registre(REGISTRE p) {
    char *s;
    s=strdup(p->nom);
    free (p);
    DEBUG_MSG("registre %s libéré",s);
    free(s);
}

//Liberation de la table des registre

void free_table_registre(REGISTRE* p) {
    int i;
    if ((p[0]->taille)!=32) {
        WARNING_MSG("libération impossible");
        return;
    }
    for(i=0; i<16; i++) {
        free(p[i]);
    }
    free(p);
    INFO_MSG("table de registres libérée");
}


//Liberation de la table des registre d'état

void free_table_registre_etat(REGISTRE* p) {
    int i;
    if ((p[0]->taille)!=1) {
        WARNING_MSG("libération impossible");
        return;
    }
    for(i=0; i<4; i++) {
        free(p[i]);
    }
    free(p);
    INFO_MSG("table de registres d'état libérée");
}

//Afficher registre

void afficher_registre(REGISTRE r) {
    // printf("--Affichage de registre %s --\n",r->nom);
    if(r==NULL) printf("registre vide");
    else {
        //printf("Nom \t \t Taille \t \t Valeur \n ");
        printf("%s \t \t %d \t \t \t %d \n",r->nom,r->taille,r->valeur);
    }
}


//Afficher la table des registres

void afficher_table_registre(REGISTRE *t) {
    int i;
    if(t==NULL) printf("Table vide\n");
    else if ((t[0]->taille)!=32) {
        WARNING_MSG("affichage impossible");
        return;
    }


    else {
        printf("----#Affichage de la tables des registres généraux#---- \n");
        printf("Nom \t \t Taille \t \t Valeur \n ");
        for(i=0; i<16; i++) {
            afficher_registre(t[i]);
        }
        printf("---#Fin table des registres#---\n");
    }
}


//Afficher la table des registres d'etat
void afficher_table_registre_etat(REGISTRE *t) {
    int i;
    if(t==NULL) printf("Table vide\n");
    else if ((t[0]->taille)!=1) {
        WARNING_MSG("affichage impossible");
        return;
    }

    else {
        printf("----#Affichage de la tables des registres d'état#---- \n");
        printf("Nom \t \t Taille \t \t Valeur \n ");
        for(i=0; i<4; i++) {
            afficher_registre(t[i]);
        }
        printf("---#Fin table des registres d'état#---\n");
    }
}

//Inversement de valeur en registre d'état
void inverser_registre_etat(REGISTRE r) {
    if(r->taille!=1) printf("printf erreur taille registre etat\n");
    if(r->valeur==1) (r->valeur)= 0;
    if(r->valeur==0) (r->valeur)= 1;
    else printf("erreur valeur registre d'etat \n");
}


//Fonction permet de trouver le registre à partir de son nom

REGISTRE trouve_registre(char* s,REGISTRE* T) {

    int i;

    for(i=0; i<16; i++) {
        if(strcmp(s,(T[i]->nom))==0) {
            return T[i];
        }
    }
    WARNING_MSG("registre introuvable dans cette table");
    return NULL;
}

//Modifier valeur de registre
REGISTRE modifier_valeur_reg(unsigned int newval,REGISTRE r) {
    if((4*sizeof(&newval))>(r->taille)) {
        WARNING_MSG("Nouvelle valeur trop grande");
        return r;
    }
    (r->valeur) = newval;
    return r;
}

//Return Tout les registres

REGISTRE** table_registre_complete(REGISTRE* Tgen , REGISTRE* Tetat) {
    REGISTRE** p;
    p=calloc(2,sizeof(*p));
    if (p==NULL) {
      ERROR_MSG("Allocation impossible");
      return NULL;
    }
    p[0]=Tgen;
    p[1]=Tetat;
    return p;
}
