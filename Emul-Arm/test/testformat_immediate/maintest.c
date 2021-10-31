#include "format-op.h"





int main (void){
  IMMEDIATE myteststruct = NULL;
  IMMEDIATE myteststruct2 = NULL;
  unsigned int testcode=0;
  unsigned int testcarry = 1;
  printf("Hello \n");
  /*rintf("Please enter an unsigned integer value -- future code\n");
  scanf("%x",&testcode);
  printf("Value entered 0x%X \n", testcode);
 
  myteststruct = immediate("12-4",testcode);*/
  		myteststruct = calloc(1,sizeof(*myteststruct));
   	myteststruct->imm  = 0x305;
 	myteststruct->bits = 12;
  affiche_immediate(myteststruct);
  
  myteststruct = LSL(myteststruct,2);
  affiche_immediate(myteststruct);

  myteststruct = ROR(myteststruct,2);
  affiche_immediate(myteststruct);
  
  myteststruct = LSR(myteststruct,2);
  affiche_immediate(myteststruct);

  myteststruct = RRX(myteststruct,&testcarry);
  affiche_immediate(myteststruct);

	printf("Evey thing is cool now \n \n");
	
	free(myteststruct);
	myteststruct = calloc(1,sizeof(*myteststruct));
 	
 	myteststruct->imm  = 0x1ab;
 	myteststruct->bits = 12;
 	affiche_immediate(myteststruct);
 	
 	myteststruct2 = thumbexpandIMM(myteststruct);
 	free(myteststruct);
 	if (myteststruct!= NULL) affiche_immediate(myteststruct2);
 	if (myteststruct!= NULL) free(myteststruct2);
 	}
