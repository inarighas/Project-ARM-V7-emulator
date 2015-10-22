#include"memory.h"



void main(void){
	MAPMEM m=NULL;
	SEGMENT p=NULL;
	char name[512];
	char* str;
	char s[512];
	
	strncpy(s,"abcdefghijklmnop",sizeof("abcdefghijklmnop"));
	strcpy(name,"section1");
	m=init_memory_arm();
	p=init_segment(name,00,2000);
	(p->contenu)=strdup(s);
	(p->taille)=strlen(s);
	
	printf("%d \n",p->taille);
	

	affiche_segment(p,NULL,NULL);

	affiche_segment(p,"0x100" , "0x0");
	printf("%x \n",p->taille_max);

	
	p = change_val_seg(p,"5",10);
	affiche_segment(p,NULL,NULL);
	
	
	str=get_byte_seg(p,"5");
	
	printf("----------------------test et byte plus loin------------------------\n");
	str=get_byte_seg(p,"100");
	
	
	printf("%x \n",str[0]);
	affiche_segment(p,NULL,NULL);
	
	
	m=ajout_seg_map(m,name,256,2000,0);
	m[0]->contenu=strdup("0000000000000000000000000000000000");
	m[0]->taille=strlen("0000000000000000000000000000000000");

	printf("-----------------------affichage mémoire----------------------------\n");
	affiche_segment(m[0],NULL,NULL);
	//affiche_segment(m[0],"105","2020");
	
	change_plage_seg(m[0],"120","130","fuckyouallofyousorry");

	affiche_segment(m[0],NULL,NULL);
	
	change_plage_seg(m[0],"100","115","------------");
	affiche_segment(m[0],"0","2900");
	}
