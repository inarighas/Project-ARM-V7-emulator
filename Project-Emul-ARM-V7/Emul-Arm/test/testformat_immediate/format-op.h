#ifndef _FORMAT-OP_H_
#define _FORMAT-OP_H_

#include "projetemularm.h"

typedef enum { SRType_LSL, SRType_LSR, SRType_ASR, SRType_RRX, SRType_ROR } SRType;


typedef struct {
  unsigned int imm;    // Contient la valeur brut
  char bits;           // Nb de bits dans la valeur au-dessus
} *IMMEDIATE
  






#endif
