#include <stdio.h>
#include <string.h>
#include "breakpoint.h"



void  _breakcmd_del(BREAKPOINT* list , unsigned int adr){
  BREAKPOINT l=*list;
  BREAKPOINT p;

  if (l->stop_adr == adr){
    *list = dehead_list_break(*list);
    printf("adresse 0x%X supprimée\n",adr);
    return ;
  }
    
  while((l->nxt)!=NULL && ((l->nxt)->stop_adr != adr)){
    l = l->nxt;
    }

 if(l->nxt== NULL){
      puts("liste des points d'arret est vide ou term inexistant");
      return ;
    }
    p = l->nxt;
    p = dehead_list_break(p);
    l->nxt = p;
    printf("adresse 0x%X supprimée\n",adr);
    return ;
}

int main (void){
  unsigned int adr = 0 ;
  BREAKPOINT mylist = NULL;

  puts("Hello");
  scanf("%x",&adr);


  mylist = init_list_break();
  mylist = add_list_break(mylist,adr);
  affiche_list(mylist);
  
  mylist = add_list_break(mylist,0x321);
  affiche_list(mylist);

  mylist = add_list_break(mylist,0x12);
  affiche_list(mylist);

  mylist = add_list_break(mylist,0x12);
  affiche_list(mylist);

  mylist = add_list_break(mylist,0x1545);
  mylist = add_list_break(mylist,0x584);
  affiche_list(mylist);


  //mylist = dehead_list_break(mylist);
  //affiche_list(mylist);
  mylist = depile_list_break(mylist);
  affiche_list(mylist);


  puts("* break del *");
  scanf("%x",&adr);
  _breakcmd_del(&mylist,adr);
  affiche_list(mylist);

  puts("\n libération");
  mylist = free_break_list(mylist);
  affiche_list(mylist);
  mylist = free_break_list(mylist);
  affiche_list(mylist);
}
