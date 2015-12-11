#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 

// Struct break point
typedef struct brlst{
  unsigned int stop_adr;
  struct brlst* nxt;
}*BREAKPOINT;



//Fonctions
BREAKPOINT init_list_break(void);
void free_break(BREAKPOINT p);
BREAKPOINT add_list_break(BREAKPOINT list ,unsigned int p);
BREAKPOINT free_break_list(BREAKPOINT list);
BREAKPOINT dehead_list_break(BREAKPOINT list);
BREAKPOINT depile_list_break(BREAKPOINT list);
void affiche_break(BREAKPOINT pt);
void affiche_list(BREAKPOINT list);
