//                 Code fonction memoire 31/09/2015                 //
//			Projet info 2a sicom emul ARM							//
//			Ali saghiran -- Damien chabannes						//


#include"memory.h"




unsigned int maxvalue(unsigned int i,unsigned int j){
	if(i>j) return i;
	else return j;
	}

MAPMEM init_memory_arm(void){
	MAPMEM p=NULL;
	if((p=calloc(NBSEG,sizeof(*p)))==NULL){
		printf("Erreur Allocation init_mem\n");
		return NULL;
		}
	return p;
	}
	

// Initialisation de segment , @arg(nom et adresse de départ et taille max) ,@return (un pointeur
// sur la structure segment)
SEGMENT init_segment(char name[],unsigned int adr,unsigned int size_max){
  SEGMENT p;
  if((p=calloc(1,sizeof(*p)))==NULL){
    printf("erreur init segment\n");
    return NULL;
    }
 
  p->taille = 1;
  p->taille_max = size_max;
  p->adresse_initiale=adr;
  strncpy(p->nom,name,sizeof(name));
  p->contenu = NULL;
  p->permis=strdup("---");
  return p;
}


//Incrémenter d'un octet un segment particulier
//@arg(segment) @return(segment)

SEGMENT inc_segment(SEGMENT seg){
  char* cm;
  int i;

  if((seg->taille)>=(seg->taille_max)){
    printf("full segment \n");
    return seg;
  }
  
 
   else{
     if((cm = calloc((seg->taille)+1,sizeof(*cm)))==NULL) return NULL;
     for(i=0;i<(seg->taille);i++){
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

SEGMENT  dec_segment(SEGMENT seg){
	 char* cm;
	 int i;
  if((seg->taille)<1){
    printf("empty segment \n");
    return seg;
  }

  else {
    if((cm = calloc((seg->taille)-1,sizeof(*cm)))==NULL) return NULL;
    for(i=0;i<(seg->taille)-1;i++){
    	cm[i]=seg->contenu[i];
    	}
    free(seg->contenu);
    (seg->contenu)=cm;
    (seg->taille)--;
  }
  return seg;
}

//afficher contenu segment à partir d'une adresse à une autre

void affiche_segment(SEGMENT seg ,char* hexdep, char* hexarr){
  unsigned int dep =0;
  unsigned int arr =0;
  int i;
  char s[32];
 
 	if(hexdep!=NULL && hexarr!=NULL) {
 		dep = strtol(hexdep,NULL,16);
		arr = strtol(hexarr,NULL,16);
		}
		
	if (dep>arr) {
		printf("adresse de départ apres arrivee\n");
		return;
		}
	
	if(hexdep==NULL && hexarr==NULL){
    	printf("**affichage complet**\n");
    	affiche_section((seg->nom), (seg->adresse_initiale), (seg->contenu),(seg->taille));
    //	for (i=0 ; i<(seg->taille); i++){
	//		printf("%x ",seg->contenu[i]);
     //	}
     puts(" ");
     return;
     }
    
  	else if(dep<(seg->adresse_initiale)){
    	printf("Erreur depart avt segment\n");
    	sprintf(s,"%x ",seg->adresse_initiale);
    	affiche_segment(seg ,s, hexarr);
    	return ;
  		}
  
  	else if(dep<=(seg->adresse_initiale) && (arr-dep)>(seg->taille)){
    	//printf("Erreur arrivee apres segment\n");
    	printf("Affichage du segment entier\n");
    	affiche_segment(seg ,NULL, NULL);
    	return ;
  		}
  	
  	else if(dep<=(seg->adresse_initiale) && (arr-dep)<=(seg->taille)){
    	//printf("Erreur arrivee apres segment\n");
    	printf("Affichage du segment jusquà larrivée\n");
    	/*for (i=0 ; i<(arr-dep+1); i++){
			printf("%x ",seg->contenu[i]);
     		}*/
     	affiche_section((seg->nom), seg->adresse_initiale, (seg->contenu),arr+1);
     	puts(" ");
     	return;
  		}
  
  	else if(dep>(seg->adresse_initiale) && (arr-dep)<=(seg->taille)){
    	printf("Départ au milieu du seg\n");
		/*for (i=dep ; i<(arr-dep+1); i++){
			printf("%x ",seg->contenu[i]);
     		}*/
     	affiche_section((seg->nom), dep, (seg->contenu),arr+1);
     	puts(" ");
     	return;
  		}
   
	else if (dep>(seg->adresse_initiale) && (arr-dep)>(seg->taille)){
      sprintf(s,"%x ",(seg->taille));
      affiche_segment(seg ,hexdep, s);
      /*for (i= dep; i<(arr+1); i++){
			printf("%x",seg->contenu[i-(seg->adresse_initiale)]);
      	}
      puts(" ");*/
      return;
    	}
    
    else printf("should never be here - affiche segment\n");
    return;
}



//ajout d'un segment à la map memoire

MAPMEM ajout_seg_map(MAPMEM mem,char* name, unsigned int adr, unsigned int size_max, int place){

 	SEGMENT seg=init_segment(name,adr,size_max);
  	mem[place]=seg;
  	return mem;
}



//
char* get_byte_seg(SEGMENT seg,char* hex){

	unsigned int hexval=0;
	char* str;
	int lim =0;
	
	if(hex!=NULL) hexval=strtol(hex,NULL,16);
	
	
	if (hexval<(seg->adresse_initiale)){
		printf("case introuvable dans ce segment\n");
		return NULL;
		}
		
	else if (hexval>(seg->adresse_initiale)+(seg->taille_max)){
		printf("case introuvable dans ce segment\n");
		return NULL;
		}
	
	else if (hexval>((seg->adresse_initiale)+(seg->taille))){
		/*str= calloc(hexval-(seg->adresse_initiale),sizeof(char));
		str=memcpy(str,seg->contenu,hexval-(seg->adresse_initiale));
		free(seg->contenu);
		seg->contenu=str;
		seg->taille=hexval-(seg->adresse_initiale)+1;*/
		while(hexval>lim){
			seg=inc_segment(seg);
			lim = (seg->adresse_initiale)+(seg->taille) ;
			}
		
		puts(" ");	
		return &((seg->contenu)[hexval-(seg->adresse_initiale)]);
		}
	
	else if (hexval<=((seg->adresse_initiale)+(seg->taille))){
		return &(seg->contenu)[hexval-(seg->adresse_initiale)];
		}
	
	else {
		printf("Never Should be Here\n");
		return NULL;
		}
	}



//////////////////
SEGMENT change_val_seg(SEGMENT seg,char* hex, char val){
	char *str;//[seg->taille_max];
	str=get_byte_seg(seg,hex);
	if(str==NULL) {
		printf("Opération impossible\n");
		return seg;
		}
	*str=val;
	return seg;
	}
	
//
 SEGMENT change_plage_seg(SEGMENT seg, char* hexdep, char* hexarr , char* val){
 	unsigned int dep =0;
	unsigned int arr =0;
	int i =0;
	char* str;
	char s[32];
	
	if(hexarr==NULL || hexdep==NULL || val==NULL) return seg;

 	if(hexdep!=NULL && hexarr!=NULL) {
 		dep = strtol(hexdep,NULL,16);
		arr = strtol(hexarr,NULL,16);
		}
		
	if (dep>arr) {
		printf("adresse de départ apres arrivee\n");
		return seg;
		}
			
	if(strlen(val)<(arr-dep)) {
		printf("Zone plus large que les données\n Modification selon largeur donnée\n");
		arr=dep+strlen(val);
		}
	if(strlen(val)>(arr-dep)) printf("Seul la plage indiquée sera modifiée\n");
	
	printf("%X \n",seg->taille);
	i=0;
	while(((seg->taille)<=(dep+(arr-dep)-(seg->adresse_initiale))) && i<=arr-dep) {
		seg=inc_segment(seg);
		i++;
		}
	
		printf("%X \n",seg->taille);
	
	str=get_byte_seg(seg,hexdep);
			if (str==NULL){
				printf("Erreur fct get byte seg \n");
				return seg;
			}
		
	for(i=0;i<(arr-dep);i++){

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
    printf("\n section {%s} ** loaded at 0x%x :\n",name,start); 
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
