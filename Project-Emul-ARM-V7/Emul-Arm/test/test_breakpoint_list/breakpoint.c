#include "breakpoint.h"
#include "stdio.h"



BREAKPOINT init_list_break(void){
  return NULL;
}


void free_break(BREAKPOINT p){
  if (p!= NULL) free(p);
}

BREAKPOINT add_list_break(BREAKPOINT list , unsigned int adress){
  BREAKPOINT l = list;
  BREAKPOINT p = list;
  BREAKPOINT t = NULL;
  while (l!=NULL && (l->stop_adr)<(adress)){
    p=l;
    l=l->nxt;
    }
  if( l!=NULL && (l->stop_adr)==adress){
    puts("Arret deja existant");
    return list;
  }

  t=calloc(1,sizeof(*t));
  if(t==NULL) {
    printf("Erreur Allocation ajout liste\n");
    return list;
  }
  
  t->stop_adr = adress;
  t->nxt = l;
  if (list == NULL) return t;
  if(p!=list) p->nxt = t;
  else list = t;
  return list;
}


BREAKPOINT free_break_list(BREAKPOINT list){
  while(list!=NULL){
    list = dehead_list_break(list);
  }
  return list;
}


BREAKPOINT dehead_list_break(BREAKPOINT list){
  BREAKPOINT l = list;
  if (list == NULL) return NULL;
  list = list->nxt;
  free(l);
  return list;
}

BREAKPOINT depile_list_break(BREAKPOINT list){
  BREAKPOINT l = list;
  BREAKPOINT p = list;
  if(list == NULL) return NULL;
  while((l->nxt)!=NULL){
    p = l;
    l = l->nxt;
  }
  free(l);
  p->nxt = NULL;
  return list;
  }

void affiche_break(BREAKPOINT pt){
  if(pt == NULL) puts("\t**Fin/Vide**\t");
  printf("Break point -  Adresse 0x%X \n",pt->stop_adr);
}

void affiche_list(BREAKPOINT list){
  BREAKPOINT l =list;
  int i=1;
  if(list == NULL) puts("Liste des Breakpoints est vide");
  while(l!=NULL){
    printf(" %d -> \t",i);
    affiche_break(l);
    l=l->nxt;
    i++;
  }
  puts("\t\t ***");
}
