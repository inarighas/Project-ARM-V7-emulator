#include "registre.h"


int main (void){
  
  REGISTRE ** T;
  REGISTRE * T1;
  REGISTRE * T2;
  int i =0;
  afficher_table_registre(T1);
  afficher_table_registre_etat(T2);
  T1 = init_table_registre();
  printf("toto\n");
  T2 = init_table_registre_etat();
  	
  T= table_registre_complete(T1,T2);

  afficher_table_registre(T1);
  afficher_table_registre_etat(T2);


  printf("test cas confus \n");
  afficher_table_registre(T2);
  afficher_table_registre_etat(T1);

  inverser_registre_etat(T2[2]);
  inverser_registre_etat(T2[0]);

  afficher_table_registre_etat(T2);

  for(i=0;i<16;i++){
    T[0][i]->valeur=i+5;
  }
  
  afficher_table_registre(T1);
}
