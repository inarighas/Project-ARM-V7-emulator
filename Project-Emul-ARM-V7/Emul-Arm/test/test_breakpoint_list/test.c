#include <stdio.h>
#include <string.h>
#include "breakpoint.h"

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


  mylist = dehead_list_break(mylist);
  affiche_list(mylist);
  mylist = depile_list_break(mylist);
  affiche_list(mylist);


  puts("\n libération");
  mylist = free_break_list(mylist);
  affiche_list(mylist);
  mylist = free_break_list(mylist);
  affiche_list(mylist);
}
