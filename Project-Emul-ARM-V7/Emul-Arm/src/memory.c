//                 Code fonction memoire 31/09/2015                 //
//			Projet info 2a sicom emul ARM		    				//
//			Ali saghiran -- Damien chabannes            			//


#include "memory.h"
#include "inter.h"


unsigned int maxvalue(unsigned int i,unsigned int j) {
    if(i>j) return i;
    else return j;
}

MAPMEM init_memory_arm(void) {
    MAPMEM p=NULL;
    int i=0;
    if((p=calloc(NBSEG,sizeof(*p)))==NULL) {
        WARNING_MSG("Erreur Allocation init_mem");
        return NULL;
    }
    for(i=0; i<NBSEG; i++) {
        p[i]=NULL;
    }
    return p;
}


// Initialisation de segment , @arg(nom et adresse de départ et taille max) ,@return (un pointeur
// sur la structure segment)
SEGMENT init_segment(char* name,unsigned int adr,unsigned int size_max) {
    SEGMENT p;
    if((p=calloc(1,sizeof(*p)))==NULL) {
        WARNING_MSG("erreur init segment");
        return NULL;
    }
    p->flag= 0;
    p->taille = 1;
    p->taille_max = size_max;
    p->adresse_initiale=adr;
    strncpy(p->nom,name,20);
    p->contenu = NULL;
    p->permis=strdup("---");
    p->flag=1;
    return p;
}


//Incrémenter d'un octet un segment particulier
//@arg(segment) @return(segment)

SEGMENT inc_segment(SEGMENT seg) {
    char* cm;           // new section char table
    int i;      //used

    if((seg->taille)>=(seg->taille_max)) {
        INFO_MSG("full segment {%s} ",seg->nom);
        return seg;
    }


    else {
        if((cm = calloc((seg->taille)+1,sizeof(*cm)))==NULL) return NULL;
        for(i=0; i<(seg->taille); i++) {
            cm[i]=seg->contenu[i];
        }
        free(seg->contenu);
        seg->contenu=cm;
        (seg->taille)++;
    }
    return seg;
}


//Decrémenter d'un octer un segment particulier --libération de memoire ss-entendue
//@arg(segment) @return(segment)

SEGMENT  dec_segment(SEGMENT seg) {
    char* cm;
    int i;
    if((seg->taille)<1) {
        INFO_MSG("empty segment {%s} ",seg->nom);
        return seg;
    }

    else {
        if((cm = calloc((seg->taille)-1,sizeof(*cm)))==NULL) return NULL;
        for(i=0; i<(seg->taille)-1; i++) {
            cm[i]=seg->contenu[i];
        }
        free(seg->contenu);
        (seg->contenu)=cm;
        (seg->taille)--;
    }
    return seg;
}

//afficher contenu segment à partir d'une adresse à une autre

void affiche_segment(SEGMENT seg ,char* hexdep, char* hexarr) {
    unsigned int dep =0;
    unsigned int arr =0;
    //int i;
    char s[32];

    if(hexdep!=NULL && hexarr!=NULL) {
        dep = strtol(hexdep,NULL,16);
        arr = strtol(hexarr,NULL,16);
    }

    if (dep>arr) {
        DEBUG_MSG("adresse de départ apres arrivee");
        return;
    }

    if(hexdep==NULL && hexarr==NULL) {
        DEBUG_MSG("**affichage complet**");
        affiche_section((seg->nom), (seg->adresse_initiale), (seg->contenu),(seg->taille));
        puts(" ");
        return;
    }

    else if(dep<(seg->adresse_initiale)) {
        DEBUG_MSG("Erreur depart avt segment");
        sprintf(s,"%x ",seg->adresse_initiale);
        affiche_segment(seg ,s, hexarr);
        return ;
    }

    else if(dep<=(seg->adresse_initiale) && (arr-dep)>(seg->taille)) {
        //printf("Erreur arrivee apres segment\n");
        DEBUG_MSG("Affichage du segment entier");
        affiche_segment(seg ,NULL, NULL);
        return ;
    }

    else if(dep<=(seg->adresse_initiale) && (arr-dep)<=(seg->taille)) {
        //printf("Erreur arrivee apres segment\n");
        DEBUG_MSG("Affichage du segment jusquà larrivée");
        /*for (i=0 ; i<(arr-dep+1); i++){
        	printf("%x ",seg->contenu[i]);
        	}*/
        affiche_section((seg->nom), seg->adresse_initiale, (seg->contenu),(arr+1-(seg->adresse_initiale)));
        puts(" ");
        return;
    }

    else if(dep>(seg->adresse_initiale) && (arr-dep)<=(seg->taille)) {
        DEBUG_MSG("Départ au milieu du seg");
        /*for (i=dep ; i<(arr-dep+1); i++){
        	printf("%x ",seg->contenu[i]);
        	}*/
        affiche_section((seg->nom), dep, (seg->contenu),arr+1-dep);
        puts(" ");
        return;
    }

    else if (dep>(seg->adresse_initiale) && (arr-dep)>(seg->taille)) {
        sprintf(s,"%x ",(seg->taille));
        affiche_segment(seg ,hexdep, s);
        /*for (i= dep; i<(arr+1); i++){
        	printf("%x",seg->contenu[i-(seg->adresse_initiale)]);
        	}
        puts(" ");*/
        return;
    }

    else WARNING_MSG("should never be here - affiche segment");
    return;
}



//ajout d'un segment à la map memoire

MAPMEM ajout_seg_map(MAPMEM mem,char* name, unsigned int adr, unsigned int size_max, int place) {

    SEGMENT seg=init_segment(name,adr,size_max);
    mem[place]=seg;
    return mem;
}



//
char* get_byte_seg(SEGMENT seg,unsigned int hexval) {

    //unsigned int hexval=0;
    // char* str;    //not used
    int lim =0;

    //if(hex!=NULL) hexval=strtol(hex,NULL,16);

    if (hexval<(seg->adresse_initiale)) {
        WARNING_MSG("case introuvable dans ce segment {%s} ",seg->nom);
        return NULL;
    }

    else if (hexval>(seg->adresse_initiale)+(seg->taille_max)) {
        WARNING_MSG("case introuvable dans ce segment{%s} ",seg->nom);
        return NULL;
    }

    else if (hexval>((seg->adresse_initiale)+(seg->taille))) {
        /*str= calloc(hexval-(seg->adresse_initiale),sizeof(char));
        str=memcpy(str,seg->contenu,hexval-(seg->adresse_initiale));
        free(seg->contenu);
        seg->contenu=str;
        seg->taille=hexval-(seg->adresse_initiale)+1;*/
        while(hexval>lim) {
            seg=inc_segment(seg);
            lim = (seg->adresse_initiale)+(seg->taille) ;
        }

        puts(" ");
        return &((seg->contenu)[hexval-(seg->adresse_initiale)]);
    }

    else if (hexval<=((seg->adresse_initiale)+(seg->taille))) {
        return &(seg->contenu)[hexval-(seg->adresse_initiale)];
    }

    else {
        WARNING_MSG("Never Should be Here");
        return NULL;
    }
}



//////////////////
SEGMENT change_val_seg(SEGMENT seg,unsigned int hexval, char val) {
    char *str;//[seg->taille_max];
    str=get_byte_seg(seg,hexval);
    if(str==NULL) {
        WARNING_MSG("Opération impossible");
        return seg;
    }
    *str=val;
    return seg;
}

//
SEGMENT change_plage_seg(SEGMENT seg, unsigned int dep, unsigned int arr , char* val) {
    //unsigned int dep =0;
    //unsigned int arr =0;
    int i =0;
    char* str;
    // char s[32];       //not used

    //if(hexarr==NULL || hexdep==NULL || val==NULL) return seg;

    /*if(hexdep!=NULL && hexarr!=NULL) {
    	dep = strtol(hexdep,NULL,16);
    	arr = strtol(hexarr,NULL,16);
    	}*/

    if (dep>arr) {
        DEBUG_MSG("adresse de départ apres arrivee");
        return seg;
    }

    if(strlen(val)<(arr-dep)) {
        DEBUG_MSG("Zone plus large que les données\n Modification selon largeur donnée");
        arr=dep+strlen(val);
    }
    if(strlen(val)>(arr-dep)) DEBUG_MSG("Seul la plage indiquée sera modifiée");

    printf("%X \n",seg->taille);
    i=0;
    while(((seg->taille)<=(dep+(arr-dep)-(seg->adresse_initiale))) && i<=arr-dep) {
        seg=inc_segment(seg);
        i++;
    }

    printf("%X \n",seg->taille);

    str=get_byte_seg(seg,dep);
    if (str==NULL) {
        WARNING_MSG("Erreur fct get byte seg");
        return seg;
    }

    for(i=0; i<(arr-dep); i++) {

        /*if((seg->taille)<=((str-(seg->contenu))+i))*/// inc_segment(seg);
        bcopy(val+i,str+i,1);
        //printf("%c \n",*(str+i));
    }

    return seg;
}



//-------------------------------------------------------//
void affiche_section(char* name, unsigned int start, char* content, unsigned int taille) {
    int k;
    unsigned char octet =0;
    printf("\n section {%s} **[size: %d bytes] loaded at 0x%x :\n",name,taille,start);
    if (content!=NULL && taille>0) {
        for(k=0; k<taille; k++) {
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

//trouve segment-----------------------------------------------------------
SEGMENT trouve_segment_nom(char*s, MAPMEM map){
	int i =0;
	int success=0;
	
	if (s==NULL) {
		WARNING_MSG("Erreur nom de segment recherché");
		return NULL;
		}
	if (map==NULL) {
		WARNING_MSG("Map mémoire inexistante pour le moment");
		return NULL;
		}
	while(i<NBSEG && success==0 ){
		if (strcmp(s,(map[i])->nom)==0) success=1;
		i++;
		}
	
	if (success==1) {
		DEBUG_MSG("Segment {%s} trouvé",map[i-1]->nom);
		return map[i-1];
		}
		
	else {
		WARNING_MSG("Segment non trouvé");
		return NULL;
		}
	}
	
	
