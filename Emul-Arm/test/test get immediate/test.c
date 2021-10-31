#include <stdio.h>
#include <string.h>
#include <stdlib.h>


unsigned int immediate (char* s ,unsigned int code){
	unsigned int debut,fin,val; 
	unsigned int nb_bits;
	char* token;
	token = strtok(s,":");
	val=0;
	
	do{
		sscanf(token,"%u-%u",&fin,&debut);
		nb_bits= fin - debut + 1;
		val= (val << nb_bits) | ((code >> debut) & ((1<<nb_bits)-1));
		printf("%x \n",val);
		}
	while((token = strtok(NULL,":"))!=NULL);
	return val;
	}


void main(void){
	unsigned int code = 0xF0F0F0F0;
	unsigned int val= 0;
	char field[10]="31-28:5-2";
	
	printf("code rentré 0x%X \n",code) ;
	
	val=immediate(field,code);
	printf("val extraite %X \n",val);
	}
	
	
	
		
