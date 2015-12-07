#include "breakpoint.h"




BREAKPOINT* init_list_break(void){
  return NULL;
}

BREAKPOINT init_break(unsigned int adress){
  BREAKPOINT p;
  if((p = calloc(1, sizeof(*p)))==NULL) return NULL;
  p->stop_adr = adress;
  p->indice = NO_ORDER;
  return p;
}

void free_break(BREAKPOINT p){
  if (p!= NULL) free(p);
}

BREAKPOINT* add_list_break(BREAKPOINT* list ,BREAKPOINT p){
  BREAKPOINT* l = list;
  int i = 0;
  while (l!=NULL){
    l=l->suiv;
    i++;
  }
  l=calloc(1,sizeof(*l));
  l->br_point = p;
  l->nxt = NULL;
  p->indice = 1;
}


void free_break_list(BREAKPOINT *list){
  BREAKPOINT* l = list;
  while(l!=NULL){
    l = list;
    while(l!=NULL){
      free_break(l->br_point);
      free(l);
    }
  }
}

