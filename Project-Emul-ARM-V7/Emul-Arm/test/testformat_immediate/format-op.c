#include "projetemularm.h"



unsigned int immediate(char* s ,unsigned int code){
	unsigned int debut,fin,val; 
	unsigned int nb_bits;
	char* token;
	token = strtok(s,":");
	val=0;
	
	do{
		sscanf(token,"%u-%u",&fin,&debut);
		nb_bits= fin - debut + 1;
		val= (val << nb_bits) | ((code >> debut) & ((1<<nb_bits)-1));
		//printf("%x \n",val);
		}
	while((token = strtok(NULL,":"))!=NULL);
	return val;
	}



char* registre_extract(char* s , unsigned int code){
	unsigned int indice = 20;
	char* nomregistre;
	
	nomregistre=calloc(5,sizeof(char));
	
	indice=immediate(s,code);
	sprintf(nomregistre,"R%d",indice);
	return nomregistre;
	}



(SRType,int) decodeIMMshift(char* typechamp,unsigned int imm5){
	SRType shift_t;
	SRType shift_n;
	unsigned int type = immediate(typechamp);
	switch(type){
		case 0x0:
			shift_t = SRType_LSL; shift_n=imm5;
			break;
		case 0x1:
			shift_t = SRType_LSR; 
			if(imm5==0) shift_n=32;
			else shift_n=imm5;
			break;
		case 0x2:
			shift_t=SRType_ASR; 
			if(imm5==0) shift_n=32;
			else shift_n=imm5;
			break;
		case 0x3:
			if(imm5==0){
				shift_t = SRType_RRX; shift_n = 1;
				}
			else{
				shift_t = SRType_ROR; shift_n = imm5;
				}
			break;
		}
	return(shift_t,shift_n);
	}


//SIGNEXTEND/////////////////////////////////////////////////////////////////////////
unsigned int signextend(unsigned int imm,int n, int leftmost){
	int	i	= 0;					//DO NOT FORGET to give to this function the place of the leftmost bit
	unsigned int mask = 0x1;
	unsigned int extendbits = 0x80000000;
	unsigned int finvalue = 0;
	 
	if ((leftmost>n-1) || (leftmost<0)){
		puts("A problem is detected concerning the leftmost bit place - signextend");
		return imm;
		}
	mask= (mask << leftmost);
	i = imm & mask;
	i= i >> leftmost;
	printf("%X",i);
		if(i!=1 && i!=0){
			puts("A problem is detected -mask signextend-");
			return imm;
			}
	if (i==1){
		extendbits=(extendbits >> leftmost| ~(0xFFFFFFFF >> leftmost));
		finvalue = (imm | extendbits);
		}
	if (i==0){
		finvalue = imm ;
		}
	return finvalue;
}


//ThumbExpandIMM__//////////////////////////////////////////////////////////////////////
unsigned int thumbexpandIMM(unsigned int imm12){ 		//ici valeur immediate sur 12bits
	// APSR.C argument to following function call does not affect the imm32 result.
	return *thumbexpandIMM_c(imm12,APSR.C);														// >>> à inclure le chemin vers l'interpreteur
	}


//ThumbExpandIMM_C//////////////////////////////////////////////////////////////////////
(unsigned int, 'bit') thumbexpandIMM_c(unsigned int imm12,REGISTRE* "carryin"){
	
	unsigned int unrotated_value=0;
	unsigned int carryout=0;
	unsigned int imm32 =0;
		
	if ((imm12 & 0x0C00)==0x0){
		switch (imm12 & 0x0300){
			case 0x0:
				imm32 = immediate("7-0",imm12);
				break;
			case 0x1:
				if(immediate("7-0",imm12)==0){
					WARNING_MSG("UNPREDICTABLE - thumbexpandIMM_c retourne 0");
					return NULL;
					}
				imm32 = ((imm12 << 16) | imm12);
				break;
			case 0x2:
				if(immediate("7-0",imm12)==0){
					WARNING_MSG("UNPREDICTABLE - thumbexpandIMM_c retourne 0");
					return NULL;
					}
				imm32 = ((imm12 << 24) | (imm12 << 8));
				break;
			case 0x3:
				if(immediate("7-0",imm12)==0){
					WARNING_MSG("UNPREDICTABLE - thumbexpandIMM_c retourne 0");
					return NULL;
					}
				imm32 = ((imm12 << 24) | (imm12 << 16) | (imm12 << 8) | (imm12 << 8));
				break;
			}
			
		carryout=carryin;
		}
	else{
		unrotated_value = 0x0080 + immediate("6-0",imm12);
		(imm32, carry_out) = ROR_C(unrotated_value,immediate("11-7"));
		}
	return (imm32, carry_out);
	}
			
		
//CONCATENATE16	//////////////////////////////////////////////////////////////////////
int concatenate16


//CODE OF SHIFT AND ROTATE OPERATIONS//////////////////////////////////////////////////
unsigned int LSL(unsigned int imm , int shift){						//left shift without carry
	unsigned int result=0;
	if (shift<0) return imm;
	result=(imm << shift);
	return result;
	}
	
	
unsigned int LSL_C(unsigned int imm, int shift){					//left shift with carry
 	unsigned int result=0;
 	if (shift<0) return imm;
	result= (imm << shift-1);
	carry=((imm & 0x80000000) >> 31);
	result= (imm << 1);
	return result;
	}
 	
unsigned int LSR(unsigned int imm , int shift){						//right shift without carry
	unsigned int result=0;
	if (shift<0) return imm;
	result=(imm >> shift);
	return result;
	}
	
	
unsigned int LSR_C(unsigned int imm, int shift){					//right shift with carry
 	unsigned int result=0;
 	if (shift<0) return imm;
	result= (imm >> shift-1);
	carry=(imm & 0x1);
	result= (imm >> 1);
	return result;
	}
	
unsigned int ASR(unsigned int imm,int sizeofimm,int shift){
	
	
	
return (result, carry_out);



