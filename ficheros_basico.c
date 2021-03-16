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
    printf("El tamaño del Mapa de Bits es: %d \n", tamanoMB); //Print clarificatiu
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
    printf("El tamaño del array de Inodos es: %d \n", tamanoAI); //Print clarificatiu
    return tamanoAI;
}

//Mètode que inicialitza les dades del SB.
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
    printf("Quantitat de inodes és: %d \n", SB.totInodos); //Print clarificatiu
    //printf("Tamany de SB: %lu \n", sizeof(struct superbloque)); //Print clarificatiu

    if (bwrite(posSB, &SB) == BLOCKSIZE)
    {

        printf("Escriptura del SB al dispositu virtual realitzat correctament.\n");
        return 0;
    }
    else
    {
        return fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
}

//Mètode que inicialitza el MB.
int initMB()
{
    //Declaracio de les variables
    struct superbloque *SB = (struct superbloque *)malloc(sizeof(struct superbloque));
    unsigned char *buffer[BLOCKSIZE];
    int blocs = 0; //Variable clarificativa

    //Llegim el SuperBloc
    if (bread(posSB, SB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del Superbloqe realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    //Preparam el buffer per escriure 0s.
    memset(buffer, 0, sizeof(buffer));
    //Bucle que coloca 0 els blocs associats al MB.
    for (int i = SB->posPrimerBloqueMB; i <= SB->posUltimoBloqueMB; i++)
    {
        //Control de errors durant l'escriptura.
        if (bwrite(i, buffer) < 0)
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
        blocs++; //Variable clarificativa
    }
    printf("Num de blocs escrits: %d\n", blocs); //Print Clarificatiu

    return 0;
}

//Mètode que inicialitza la llista de inodes lliures.
int initAI()
{
    //Declaració e incialització variable Superbloque.
    struct superbloque *SB = (struct superbloque *)malloc(sizeof(struct superbloque));
    //Llegim el SuperBloc
    if (bread(posSB, SB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
    //Declaració variable inode.
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    int contInodos = SB->posPrimerInodoLibre;
    //Recorrem tots els blocs associats a AI.
    for (int i = SB->posPrimerBloqueAI; i <= SB->posUltimoBloqueAI; i++)
    {
        //Recorrem byte byte cada bloc.
        for (int j = 0; j < BLOCKSIZE / INODOSIZE; j++)
        {
            //Assignam el valor de libre a la varibale tipo de l'j-inode;
            inodos[j].tipo = 'l';
            //Mentre no arribem al inode final, encadenam inodes un a un.
            if (contInodos < SB->totInodos)
            {
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;
            }
            else
            {
                inodos[j].punterosDirectos[0] = UINT_MAX;
                if (i == SB->posUltimoBloqueAI)
                {
                    break;
                }
            }
            //printf("%d ", inodos[j].punterosDirectos[0]); //Print Clarificatiu
            //printf("\n");
        }
        bwrite(i, inodos);
    }
    return 0;
}

int escribir_bit(unsigned int nbloque, unsigned int bit)
{
    //Declaració e incialització variable Superbloque.
    struct superbloque *SB = (struct superbloque *)malloc(sizeof(struct superbloque));
    //Llegim el SuperBloc
    if (bread(posSB, SB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    //Declaram les variables necesaries
    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB->posPrimerBloqueMB + nbloqueMB;
    unsigned char *bufferMB[BLOCKSIZE];

    //LLegim el bloc corresponent
    if (bread(nbloqueabs, bufferMB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del bloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    //Obtenir la posicio dins el rang d'un bloc
    posbyte = posbyte % BLOCKSIZE;

    //Preparam La mascara
    unsigned char mascara = 128;
    mascara >>= posbit;

    //Realtzam la modificacio
    if (bit == 0)
    {
        printf("Posam a 0 el bit \n"); //Print Clarificatiu
        bufferMB[posbyte] |= mascara;
    }
    else
    {
        printf("Posam a 1 el bit \n"); //Print Clarificatiu
        bufferMB[posbyte] &= ~mascara;
    }
    //Guardam el bloc
    if (bwrite(nbloqueabs, bufferMB) == BLOCKSIZE)
    {
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

char leer_bit(unsigned int nbloque)
{
    //Declaració e incialització variable Superbloque.
    struct superbloque *SB = (struct superbloque *)malloc(sizeof(struct superbloque));
    //Llegim el SuperBloc
    if (bread(posSB, SB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    //Declaram les variables necesaries
    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB->posPrimerBloqueMB + nbloqueMB;
    unsigned char *bufferMB[BLOCKSIZE];

    //LLegim el bloc corresponent
    if (bread(nbloqueabs, bufferMB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del bloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    //Obtenir la posicio dins el rang d'un bloc
    posbyte = posbyte % BLOCKSIZE;

    //Preparam la mascara
    unsigned char mascara = 128;
    mascara >>= posbit;
    mascara &= bufferMB[posbyte];
    mascara >>= (7 - posbit);

    printf("Valor máscara es: %d ", mascara);
    return mascara;
}

int reservar_bloque()
{

    //Declaració e incialització variable Superbloque.
    struct superbloque *SB = (struct superbloque *)malloc(sizeof(struct superbloque));
    //Llegim el SuperBloc
    if (bread(posSB, SB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    if (SB->cantBloquesLibres > 0)
    {
        unsigned char *bufferMB[BLOCKSIZE];
        unsigned char *bufferAux[BLOCKSIZE];
        memset(bufferAux, 255, sizeof(bufferAux));
        int posBloqueMB = SB->posPrimerBloqueMB;
        for (;; posBloqueMB++)
        {
            bread(posBloqueMB, bufferMB);
            if (memcmp(bufferMB, bufferAux, sizeof(bufferAux)) < 0)
            {
                break;
            }
        }
        int posbyte = 0;
        for (;; posbyte++)
        {
            if (*bufferMB[posbyte] < 255)
            {
                break;
            }
        }
        unsigned char mascara = 128;
        int posbit = 0;

        while (bufferMB[posbyte] & mascara)
        {
            bufferMB[posbyte] <<= 1;

            posbit++;
        }

        unsigned int nbloque = ((posBloqueMB - SB->posPrimerBloqueMB) * BLOCKSIZE + posbyte) * 8 + posbit;

        escribir_bit(nbloque, 1);

        SB->cantBloquesLibres--;

        if (bwrite(posSB, &SB) == BLOCKSIZE)
        {
            printf("Escriptura del SB al dispositu virtual realitzat correctament.\n");
        }
        else
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
        unsigned char *buffer[BLOCKSIZE];
        memset(buffer, 0, sizeof(buffer));

        if (bwrite(nbloque, buffer) == BLOCKSIZE)
        {
            printf("Escriptura del bloc al dispositu virtual realitzat correctament.\n");
        }
        else
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
        return nbloque;
    }
    else
    {
        return EXIT_FAILURE;
    }
}
int liberar_bloque(unsigned int nbloque)
{

    escribir_bit(nbloque, 0);
    struct superbloque *SB = (struct superbloque *)malloc(sizeof(struct superbloque));
    //Llegim el SuperBloc
    if (bread(posSB, SB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    SB->cantBloquesLibres++;

    if (bwrite(posSB, &SB) == BLOCKSIZE)
    {
        printf("Escriptura del SB al dispositu virtual realitzat correctament.\n");
    }
    else
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    return nbloque;
}

int escribir_inodo(unsigned int ninodo, struct inodo inodo)
{
    struct superbloque *SB = (struct superbloque *)malloc(sizeof(struct superbloque));
    //Llegim el SuperBloc
    if (bread(posSB, SB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    //Càlcul del bloc de AI que correspon amb el inode passat per paràmetre.
    unsigned int numBloque = (ninodo / 8) + SB->posPrimerBloqueAI;

    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    if (bread(numBloque, inodos))
    {
        //Lectura realitzada correctament.
        printf("Lectura del bloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
    inodos[ninodo % (BLOCKSIZE / INODOSIZE)] = inodo;

    if (bwrite(numBloque, inodos) == BLOCKSIZE)
    {
        printf("Escriptura del bloc al dispositu virtual realitzat correctament.\n");
    }
    else
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
}