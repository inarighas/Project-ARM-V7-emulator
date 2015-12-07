#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <inter.h>

#define NO_ORDER -1

// Struct break point
typedef struct{
  int indice;
  unsigned int stop_adr;
}*BREAKPOINT;

// Struct list
typedef struct{
  BREAKPOINT br_point;
  BREAKPOINT* nxt;
}


