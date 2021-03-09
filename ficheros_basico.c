//AUTORS : Joan Lòpez Ferrer i Miquel Vidal Cortés

//Include de la caçalera associada amb ficheros_basico.c
#include "ficheros_basico.h"
#define tamanoBit 8

//Mètode que calcula el tamany en blocs del Mapa de Bits.
int tamMB(unsigned int nbloques){

    //Declaració i inicialització de dues variables senceres.
    //Una representarà el tamany del MB i l'altre és una auxiliar.
    int tamanoMB = 0;
    int auxiliar = nbloques / tamanoBit;
    //Comprovam si el nombre de blocs necessaris és congrent amb 0.
    if((auxiliar % BLOCKSIZE) == 0){
          tamanoMB = auxiliar / BLOCKSIZE;
    }else{
        //Si no és congruent amb mòdul 0, afegim un bloc més.
        tamanoMB = (auxiliar / BLOCKSIZE) + 1;
    }
    printf("El tamaño del Mapa de Bits es: %d \n", tamanoMB); //Print clarificatiu
    return tamanoMB;
    
}

//Mètode que calcula el tamany en blocs de l'array de inodes.
int tamAI(unsigned int ninodos){
    
    //Declaració i inicialització de dues variables senceres.
    //Una representarà el tamany del AI i l'altre és una auxiliar.
    int tamanoAI = 0;
    int auxiliar = BLOCKSIZE/INODOSIZE;
    //Comprovam si el nombre de blocs necessaris és congrent amb 0.
    if((ninodos % auxiliar)==0){
        tamanoAI = ninodos / auxiliar;
    }else{
        //Si no és congruent amb mòdul 0, afegim un bloc més.
        tamanoAI = (ninodos / auxiliar) + 1;
    }
    printf("El tamaño del array de Inodos es: %d \n", tamanoAI); //Print clarificatiu
    return tamanoAI;
}


int initSB(unsigned int nbloques, unsigned int ninodos){

struct superbloque SB;

SB.posPrimerBloqueMB = posSB + tamSB;
printf("El primer bloque de MB és: %d \n", SB.posPrimerBloqueMB);
SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) -1;

SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;

SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;

SB.posPrimerBloqueDatos = SB.posPrimerBloqueAI + 1;




    
return 0;
}

int initMB(){
  
  return 0;
}