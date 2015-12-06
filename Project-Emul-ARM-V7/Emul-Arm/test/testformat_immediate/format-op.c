// Fonctions de mise en forme pour les valeurs ainsi que les registres
#include "format-op.h"


void affiche_immediate(IMMEDIATE  value) {
    if (value == NULL) printf("Pointeur IMMEDIATE null \n");
    printf(" Valeur immediate 0x%X ## Nombre de bits %d \n",value->imm, value->bits);
}

IMMEDIATE immediate(char* str ,unsigned int code) {
    unsigned int debut,fin,val;
    unsigned int nb_bits;
    char* token;
    char* s = strdup(str);
    IMMEDIATE returnvalue = calloc(1,sizeof(*returnvalue));
    token = strtok(s,":");
    val=0;

    do {
        sscanf(token,"%u-%u",&fin,&debut);
        nb_bits= fin - debut + 1;
        val= (val << nb_bits) | ((code >> debut) & ((1<<nb_bits)-1));
        //printf("%x \n",val);
    }
    while((token = strtok(NULL,":"))!=NULL);
    returnvalue->imm=val;
    returnvalue->bits=nb_bits;
    return returnvalue;
}



char* registre_extract(char* s , unsigned int code) {
    unsigned int indice = 20;
    char* nomregistre;

    nomregistre=calloc(5,sizeof(char));

    indice=(immediate(s,code))->imm;
    sprintf(nomregistre,"R%d",indice);
    return nomregistre;
}


// Decode IMM Shift ---------------------------------------------------------
void decodeIMMshift(IMMEDIATE typebrut,IMMEDIATE imm5, int* shift_t , int* shift_n) {
   
    if((typebrut->bits)!=2) {
        WARNING_MSG("Nombre de bits type Incorrect");
        return ;
    }
    switch(typebrut->imm) {
    case 0x0:
        *shift_t = SRType_LSL;
        *shift_n = imm5->imm;
        break;
    case 0x1:
        *shift_t = SRType_LSR;
        if(imm5->imm==0) *shift_n=32;
        else *shift_n=imm5->imm;
        break;
    case 0x2:
        *shift_t=SRType_ASR;
        if(imm5->imm==0) *shift_n=32;
        else *shift_n=imm5->imm;
        break;
    case 0x3:
        if(imm5->imm==0) {
            *shift_t = SRType_RRX;
            *shift_n = 1;
        }
        else {
            *shift_t = SRType_ROR;
            *shift_n = imm5->imm;
        }
        break;
    default:
        WARNING_MSG("Valeur type incorrecte");
        return ;
        break;

    }
    return ;
}


//SIGNEXTEND/////////////////////////////////////////////////////////////////////////
IMMEDIATE signextend(IMMEDIATE value,int n) {
    int i = 0;					//DO NOT FORGET to give to this function the place of the leftmost bit
    unsigned int mask = 0x1;
    unsigned int extendbits = 0x80000000;
    IMMEDIATE finalvalue = calloc(1,sizeof(*finalvalue));

    if ((value->bits>n-1) || (value->bits<0)) {
        puts("A problem is detected concerning the value->bits bit place - signextend");
        return value;
    }
    mask= (mask << value->bits);
    i = value->imm & mask;
    i= i >> value->bits;
    printf("%X",i);
    if(i!=1 && i!=0) {
        puts("A problem is detected -mask signextend-");
        return value;
    }
    if (i==1) {
        extendbits=(extendbits >> value->bits| ~(0xFFFFFFFF >> value->bits));
        finalvalue->imm = (value->imm | extendbits);
    }
    if (i==0) {
        finalvalue->imm = value->imm ;
    }
    finalvalue->bits = n;
    value->imm = value->imm & ((0x1 << (value->bits))-1);
    return finalvalue;
}


//ThumbExpandIMM__//////////////////////////////////////////////////////////////////////
IMMEDIATE thumbexpandIMM(IMMEDIATE imm12) { 	//ici valeur immediate sur 12bits
    // APSR.C argument to following function call does not affect the imm32 result.
    unsigned int carry = 0;
    return thumbexpandIMM_c(imm12, &carry);	
    													// >>> à inclure le chemin vers l'interpreteur
}


//ThumbExpandIMM_C//////////////////////////////////////////////////////////////////////
IMMEDIATE thumbexpandIMM_c(IMMEDIATE imm12,unsigned int* carry) {

    unsigned int unrotated_value=0;
    unsigned int carryout=0;
    IMMEDIATE unrotated_imm=NULL;
    IMMEDIATE imm32 = calloc(1,sizeof(*imm32));

    if ((imm12->imm & 0x0C00)==0x0) {
        switch ((imm12->imm & 0x300)) {
        case 0x000:
            imm32 = immediate("7-0",imm12->imm);
	    	imm32->bits=32;
            break;
        case 0x100:
	  if((immediate("7-0",imm12->imm)->imm)==0) {
                WARNING_MSG("UNPREDICTABLE - thumbexpandIMM_c retourne 0");
                return NULL;
            }
      imm32 = immediate("7-0",imm12->imm);
	  imm32->imm = (((imm32->imm) << 16) | (imm32->imm));
	  imm32->bits=32;
            break;
        case 0x200:
	  if((immediate("7-0",imm12->imm)->imm)==0) {
                WARNING_MSG("UNPREDICTABLE - thumbexpandIMM_c retourne 0");
                return NULL;
            }
      imm32 = immediate("7-0",imm12->imm);
	  imm32->imm = (((imm32->imm) << 24) | ((imm32->imm) << 8));
	  imm32->bits = 32;
            break;
        case 0x300:
	 	 if((immediate("7-0",imm12->imm)->imm)==0) {
                WARNING_MSG("UNPREDICTABLE - thumbexpandIMM_c retourne 0");
                return NULL;
            }
        imm32 = immediate("7-0",imm12->imm);
        imm32->imm = ((imm32->imm << 24) + (imm32->imm << 16) + (imm32->imm << 8) + (imm32->imm << 0));
	    imm32->bits=32;
            break;
        default:
            WARNING_MSG("UNPREDICTABLE -- cas inattendu");
            return NULL;
            break;

        }

        carryout=*carry;
    }
    else {
      unrotated_value = 0x0080 + (immediate("6-0",imm12->imm)->imm);
      unrotated_imm=calloc(1,sizeof(*unrotated_imm));
      unrotated_imm->imm = unrotated_value;
      unrotated_imm->bits= 32;
      imm32 = ROR_C(unrotated_imm,immediate("11-7",imm12->imm)->imm,carry);
    }
    *carry= carryout;
    return imm32;
}


//CONCATENATE16	//////////////////////////////////////////////////////////////////////
IMMEDIATE concatenate16(IMMEDIATE valbrut) {
    if(valbrut->bits>16) {
        WARNING_MSG("valeur plus grande que 16b");
        return valbrut;
    }
    valbrut->bits=16;
    valbrut->imm = valbrut->imm & ((0x1 << (valbrut->bits))-1);
    return valbrut;
}

///Concatenate32///////////////////////////////////////////////////////////////////////
IMMEDIATE concatenate32(IMMEDIATE valbrut) {
    if(valbrut->bits>16) {
        WARNING_MSG("valeur plus grande que 32b");
        return valbrut;
    }
    valbrut->bits=32;
    return valbrut;
}


//CODE OF SHIFT AND ROTATE OPERATIONS//////////////////////////////////////////////////
IMMEDIATE LSL(IMMEDIATE value , int shift) {						//left shift without *carry
    if (shift<=0) return value;
    value->imm=(value->imm << shift);
    value->bits=value->bits;
    value->imm = value->imm & ((0x1 << (value->bits))-1);
    return value;
}


IMMEDIATE LSL_C(IMMEDIATE value, int shift,unsigned int *carry) {		//left shift with carry
    if (shift<=0) return value;
    value->imm = (value->imm << shift-1);
    *carry=((value->imm & 0x80000000) >> 31);
    value->imm = (value->imm << 1);
    value->imm = value->imm & ((0x1 << (value->bits))-1);
    return value ;
}

IMMEDIATE LSR(IMMEDIATE value, int shift) {						//right shift without *carry

    if (shift<=0) return value;
    value->imm = (value->imm >> shift);
    value->imm = value->imm & ((0x1 << (value->bits))-1);
    return value;
}


IMMEDIATE LSR_C(IMMEDIATE value, int shift,unsigned int *carry) {					//right shift with *carry

    if (shift<=0) return value;
    value->imm= (value->imm >> shift-1);
    *carry=(value->imm & 0x1);
    value->imm = (value->imm >> 1);
    value->imm = value->imm & ((0x1 << (value->bits))-1);
    return value;
}

IMMEDIATE ASR( IMMEDIATE value,int shift) {                                            // Aritmetic shift right

    if (shift<=0) return value;
    signed int result = value->imm;
    result = result >> shift;
    bcopy(&result,&(value->imm), 4);
    value->imm = value->imm & ((0x1 << (value->bits))-1);

    return value ;
}



IMMEDIATE ASR_C(IMMEDIATE value,int shift,unsigned int *carry) {                                   //Arithmetic shift without *carry

    if (shift<=0) return value;
    signed int result = value->imm;
    result = (result >> shift-1);
    *carry  = (result & 0x1);
    result = (result >> 1);
    bcopy(&result,&(value->imm), 4);
    value->imm = value->imm & ((0x1 << (value->bits))-1);
    return value ;
}


IMMEDIATE ROR_C(IMMEDIATE value,unsigned int shift,unsigned int *carry) {			//Rotate right with *carry
    IMMEDIATE x = calloc(1,sizeof(*x));
    IMMEDIATE y = calloc(1,sizeof(*x));

    if (shift<=0) return value;
    x->imm = value->imm;
    x->bits= value->bits;
    y->imm = value->imm;
    y->bits= value->bits;
    unsigned int m = shift % (value->bits);
    value->imm = (LSR(x,m)->imm | LSL(y,m)->imm);
    *carry = (value->imm & (0x1 <<((value->bits)-1)));
    value->imm = value->imm & ((0x1 << (value->bits))-1);
    free(x); free(y);
    return value;
}


IMMEDIATE ROR(IMMEDIATE value,unsigned int shift) {                                 //Rotate right without *carry
    IMMEDIATE x = calloc(1,sizeof(*x));
    IMMEDIATE y = calloc(1,sizeof(*x));

    if (shift<=0) return value;
    x->imm = value->imm;
    x->bits= value->bits;
    y->imm = value->imm;
    y->bits= value->bits;
    unsigned int m = shift % (value->bits);
    value->imm = (LSR(x,m)->imm | LSL(y,((value->bits)-m))->imm);
    free(x); free(y);
    value->imm = value->imm & ((0x1 << (value->bits))-1);
    return value;
}


IMMEDIATE RRX_C(IMMEDIATE value,unsigned int *carry) {
    unsigned int leftbit = *carry;
leftbit = (leftbit << (value->bits-1));
    *carry = value->imm & 0x1;
    value->imm = (value->imm >> 1);
    value->imm = (value->imm | leftbit );
    value->imm = value->imm & ((0x1 << (value->bits))-1);
    return value;
}

IMMEDIATE RRX(IMMEDIATE value,unsigned int *carry) {
    unsigned int leftbit = *carry;
leftbit = (leftbit << (value->bits-1));
    value->imm = (value->imm >> 1);
    value->imm = (value->imm | leftbit );
    value->imm = value->imm & ((0x1 << (value->bits))-1);
    return value;
}
