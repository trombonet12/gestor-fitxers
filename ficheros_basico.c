//AUTORS : Joan Lòpez Ferrer i Miquel Vidal Cortés

//Include de la caçalera associada amb ficheros_basico.c
#include "ficheros_basico.h"
#define tamanoBit 8

//Mètode que calcula el tamany en blocs del Mapa de Bits.
int tamMB(unsigned int nbloques)
{

    //Declaració i inicialització de dues variables senceres.
    //Una representarà el tamany del MB i l'altre és una auxiliar.
    int tamanoMB = 0;
    int auxiliar = nbloques / tamanoBit;
    //Comprovam si el nombre de blocs necessaris és congrent amb 0.
    if ((auxiliar % BLOCKSIZE) == 0)
    {
        tamanoMB = auxiliar / BLOCKSIZE;
    }
    else
    {
        //Si no és congruent amb mòdul 0, afegim un bloc més.
        tamanoMB = (auxiliar / BLOCKSIZE) + 1;
    }
    //printf("El tamaño del Mapa de Bits es: %d \n", tamanoMB); //Print clarificatiu
    return tamanoMB;
}

//Mètode que calcula el tamany en blocs de l'array de inodes.
int tamAI(unsigned int ninodos)
{

    //Declaració i inicialització de dues variables senceres.
    //Una representarà el tamany del AI i l'altre és una auxiliar.
    int tamanoAI = 0;
    int auxiliar = BLOCKSIZE / INODOSIZE;
    //Comprovam si el nombre de blocs necessaris és congrent amb 0.
    if ((ninodos % auxiliar) == 0)
    {
        tamanoAI = ninodos / auxiliar;
    }
    else
    {
        //Si no és congruent amb mòdul 0, afegim un bloc més.
        tamanoAI = (ninodos / auxiliar) + 1;
    }
    // printf("El tamaño del array de Inodos es: %d \n", tamanoAI); //Print clarificatiu
    return tamanoAI;
}

int initSB(unsigned int nbloques, unsigned int ninodos)
{

    struct superbloque SB;

    SB.posPrimerBloqueMB = posSB + tamSB;
    printf("El primer bloque de MB és: %d \n", SB.posPrimerBloqueMB); //Print clarificatiu
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
    printf("El darrer bloc del MB és: %d \n", SB.posUltimoBloqueMB); //Print clarificatiu
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
    printf("El primer bloc de l'array inodos és: %d \n", SB.posPrimerBloqueAI); //Print clarificatiu
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;
    printf("El darrer bloc de l'array inodos és: %d \n", SB.posUltimoBloqueAI); //Print clarificatiu
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
    printf("El primer bloc de Datos és: %d \n", SB.posPrimerBloqueDatos); //Print clarificatiu
    SB.posUltimoBloqueDatos = nbloques - 1;
    printf("El darrer bloc de Datoes és: %d \n", SB.posUltimoBloqueDatos); //Print clarificatiu
    SB.posInodoRaiz = 0;
    printf("La posició del inode arrel en l'array inodes és: %d \n", SB.posInodoRaiz); //Print clarificatiu
    SB.posPrimerInodoLibre = 0;
    printf("El primer inode lliure dins l'arrar inodes és: %d \n", SB.posPrimerInodoLibre); //Print clarificatiu
    SB.cantBloquesLibres = nbloques;
    printf("Quantitat de blocs lliures : %d \n", SB.cantBloquesLibres); //Print clarificatiu
    SB.cantInodosLibres = ninodos;
    printf("Quantitat de inodes lliures al array de inodes és: %d \n", SB.cantInodosLibres); //Print clarificatiu
    SB.totBloques = nbloques;
    printf("Quantitat de blocs totals és: %d \n", SB.totBloques); //Print clarificatiu
    SB.totInodos = ninodos;
    printf("Quantitat de inodes és: %d \n", SB.totInodos);      //Print clarificatiu
    printf("Tamany de SB: %lu \n", sizeof(struct superbloque)); //Print clarificatiu

    if (bwrite(posSB, &SB) < 0)
    {
        return fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
    else
    {
        printf("Escriptura del SB al dispositu virtual realitzat correctament.\n");
        return 0;
    }
}

int initMB()
{
    //Declaracio de les variables
    struct superbloque *SB = (struct superbloque *)malloc(sizeof(struct superbloque));
    unsigned char *buffer[BLOCKSIZE];
    int bits = 0; //Variable clarificativa

    //Llegim el SuperBloc
    bread(0, SB);

    //Preparam el buffer per escriure 0s
    memset(buffer, 0, sizeof(buffer));
    //Bucle que coloca 0 a les posicions del MB
    for (int i = SB->posPrimerBloqueMB; i <= SB->posUltimoBloqueMB; i++)
    {   
        //Control de errors durant l'escriptura
        if (bwrite(i, buffer) < 0)
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
        bits++; //Variable clarificativa
    }
    printf("Num de bits escrits: %d\n", bits); //Print Clarificatiu

    return 0;
}


int initAI(){
    struct superbloque *SB = (struct superbloque *)malloc(sizeof(struct superbloque));
    bread(0, SB);
    struct inodo inodos [BLOCKSIZE/INODOSIZE];

    int contInodos = SB->posPrimerInodoLibre;
    for(int i = SB->posPrimerBloqueAI; i<= SB->posUltimoBloqueAI;i++){
        for(int j = 0; j<BLOCKSIZE/INODOSIZE; j++){
            inodos[j].tipo = 'l';
            if(contInodos < SB->totInodos){
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;
            }else{
                inodos[j].punterosDirectos[0] = UINT_MAX;
                if(i==SB->posUltimoBloqueAI){
                    break;
                }
            }
            printf("%d ", contInodos); //Print Clarificatiu
        }
        bwrite(i,&inodos);
    }
    return 0;
}
