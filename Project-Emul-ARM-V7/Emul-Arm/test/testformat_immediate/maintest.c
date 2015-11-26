#include "format-op.h"





int main (void){
  IMMEDIATE myteststruct = NULL;
  unsigned int testcode=0;
  unsigned int testcarry = 1;
  printf("Hello \n");
  printf("Please enter an unsigned integer value -- future code\n");
  scanf("%x",&testcode);
  printf("Value entered 0x%X \n", testcode);
 
  myteststruct = immediate("12-4",testcode);
  affiche_immediate(myteststruct);
  
  myteststruct = LSL(myteststruct,2);
  affiche_immediate(myteststruct);

  myteststruct = ROR(myteststruct,2);
  affiche_immediate(myteststruct);
  
  myteststruct = LSR(myteststruct,2);
  affiche_immediate(myteststruct);

  myteststruct = RRX(myteststruct,&testcarry);
  affiche_immediate(myteststruct);

}
