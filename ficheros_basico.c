//AUTORS : Joan Lòpez Ferrer i Miquel Vidal Cortés

//Include de la caçalera associada amb ficheros_basico.c
#include "ficheros_basico.h"
#define tamanoBit 8

//Mètode que calcula el tamany en blocs del Mapa de Bits.
int tamMB(unsigned int nbloques)
{
    printf("DINS tamMB \n");
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
    printf("\n");
    return tamanoMB;
}

//Mètode que calcula el tamany en blocs de l'array de inodes.
int tamAI(unsigned int ninodos)
{
    printf("DINS tamAI \n");
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
    printf("El tamaño del array de Inodos es: %d \n", tamanoAI); //Print clarificatiu)
    printf("\n");
    return tamanoAI;
}

//Mètode que inicialitza les dades del SB.
int initSB(unsigned int nbloques, unsigned int ninodos)
{
    printf("DINS initSB \n");
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

    if (bwrite(posSB, &SB) == BLOCKSIZE)
    {

        printf("Escriptura del SB al dispositu virtual realitzat correctament.\n");
        return 0;
    }
    else
    {
        return fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    printf("\n");
}

//Mètode que inicialitza el MB.

void ponerAUnoBits()
{
    printf("DINS PONER A UNO BITS");
    struct superbloque SB;

    //Llegim el SuperBloc
    if (bread(posSB, &SB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    unsigned int numeroBitsPonerUno = tamAI(SB.totInodos) + tamMB(SB.totBloques) + tamSB;
    unsigned char bufferMB[BLOCKSIZE];
    int numeroBytesPonerUno = numeroBitsPonerUno / 8;
    printf("NumeroBytesPonerUNo: %d", numeroBytesPonerUno);
    for (int i = 0; i < numeroBytesPonerUno; i++)
    {
        bufferMB[i] = 255;
    }
    if (numeroBitsPonerUno % 8 == 0)
    {
        printf("Tots els bits ja estan possats a 1");
    }
    else
    {
        printf("Falten bits per posar a 1");
        // int bitsFalten1 = numeroBytesPonerUno % 8;
        bufferMB[numeroBytesPonerUno] = 224;
    }

    for (int j = 393; j < BLOCKSIZE; j++)
    {
        bufferMB[j] = 0;
    }
    printf("POSICIO PRIMER BLOQUE MB: %d", SB.posPrimerBloqueMB);
    bwrite(SB.posPrimerBloqueMB, bufferMB);
    printf("Cantidad de bloques libres: %d", SB.cantBloquesLibres);

    SB.cantBloquesLibres = SB.cantBloquesLibres - numeroBitsPonerUno;
    printf("Cantidad de bloques libres: %d", SB.cantBloquesLibres);
    if (bwrite(posSB, &SB) == BLOCKSIZE)
    {
        printf("Escriptura del bloc al dispositu virtual realitzat correctament.\n");
    }
    else
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    printf("\n");
}

int initMB(unsigned int nbloques,unsigned int ninodos)
{
    printf("DINS initMB");
    //Declaracio de les variables
    struct superbloque SB;
    unsigned char *buffer[BLOCKSIZE];
    int blocs = 0; //Variable clarificativa

    //Llegim el SuperBloc
    if (bread(posSB, &SB))
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
    for (int i = SB.posPrimerBloqueMB; i <= SB.posUltimoBloqueMB; i++)
    {
        //Control de errors durant l'escriptura.
        if (bwrite(i, buffer) < 0)
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
        blocs++; //Variable clarificativa
    }
    printf("Num de blocs escrits: %d\n", blocs); //Print Clarificatiu

    //Posar a 1 el bits corresponents als blocs dels Metadatos.
    for (int i = 0; i < (tamSB + tamMB(nbloques) + tamAI(ninodos)); i++)
    {
        escribir_bit(i, 1);
    }
    //Actualització de la quantitat de bloc lliures.
    SB.cantBloquesLibres = SB.cantBloquesLibres - (tamSB + tamMB(nbloques) + tamAI(ninodos));
    printf("Cantidad de bloques libres: %d", SB.cantBloquesLibres);
    //Salvaguardam el SuperBloc dins el Dispositiu Virtual.
    if (bwrite(posSB, &SB) == BLOCKSIZE)
    {
        printf("Escriptura del superbloc al dispositu virtual realitzat correctament.\n");
    }
    else
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
    printf("\n");
    return 0;
}

//Mètode que inicialitza la llista de inodes lliures.
int initAI()
{
    printf("DINS initAI");
    //Declaració e incialització variable Superbloque.
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del Superbloque realitzada correctament 2\n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
    //Declaració variable inode.
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    int contInodos = SB.posPrimerInodoLibre;
    //Recorrem tots els blocs associats a AI.
    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++)
    {
        //Recorrem byte byte cada bloc.
        for (int j = 0; j < BLOCKSIZE / INODOSIZE; j++)
        {
            //Assignam el valor de libre a la varibale tipo de l'j-inode;
            inodos[j].tipo = 'l';
            //Mentre no arribem al inode final, encadenam inodes un a un.
            if (contInodos < SB.totInodos)
            {
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;
            }
            else
            { //Ens trobam al darrer inode. Apunta al "infinit".
                inodos[j].punterosDirectos[0] = UINT_MAX;
                if (i == SB.posUltimoBloqueAI)
                {
                    break;
                }
            }
            //printf("%d ", inodos[j].punterosDirectos[0]); //Print Clarificatiu
            //printf("\n");
        }
        //Escriptura del i-bloc corresponent associat
        if (bwrite(i, inodos) == BLOCKSIZE)
        {
            //Lectura realitzada correctament.
            printf("Escriptura del bloc associat als 8 inodes corresponents realitzat correctament \n");
        }
        else
        {
            //Error en la lectura.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }
    printf("\n");
    return 0;
}

int escribir_bit(unsigned int nbloque, unsigned int bit)
{
    printf("DINS escribir_bit \n");
    //Declaració e incialització variable Superbloque.
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB))
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
    printf("El valor de posbyte és: %d \n", posbyte);
    int posbit = nbloque % 8;
    printf("El valor de posbit és: %d \n", posbit);
    int nbloqueMB = posbyte / BLOCKSIZE;
    printf("Valor de nbloqueMB %d \n", nbloqueMB);
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;
    printf("Valor de nbloqueabs %d \n", nbloqueabs);
    unsigned char bufferMB[BLOCKSIZE];

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
    printf("Nou valor de posbyte %d \n", posbyte);
    //Preparam La mascara
    unsigned char mascara = 128;

    mascara >>= posbit;
    printf("Nou valor de mascara despres daplicar mascara ==> posbit és: %d \n", mascara);
    //Realtzam la modificacio
    if (bit == 0)
    {
        printf("Posam a 0 el bit \n"); //Print Clarificatiu
        bufferMB[posbyte] &= ~mascara;
        printf("Valor de bufferMB[posbyte] es: %d \n", bufferMB[posbyte]);
    }
    else if (bit == 1)
    {
        printf("Posam a 1 el bit \n"); //Print Clarificatiu
        bufferMB[posbyte] |= mascara;
        printf("Valor de bufferMB[posbyte] es: %d  \n", bufferMB[posbyte]);
    }
    //Guardam el bloc modificat.
    if (bwrite(nbloqueabs, bufferMB) == BLOCKSIZE)
    {
        //Escriptura correcta.
        printf("Lectura del Superbloque realitzada correctament \n");
        return EXIT_SUCCESS;
    }
    else
    {
        //Escriptura errònia.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }
    printf("\n");
}

char leer_bit(unsigned int nbloque)
{
    printf("DINS ller_bit");
    //Declaració e incialització variable Superbloque.
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB))
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
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;
    unsigned char bufferMB[BLOCKSIZE];

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

    posbyte = posbyte % BLOCKSIZE;
    printf("Contingut de bufferMB: %d \n", bufferMB[posbyte]);
    //Obtenir la posicio dins el rang d'un bloc

    //Preparam la mascara
    unsigned char mascara = 128;
    mascara >>= posbit;
    printf("Valor de mascara despres de mascara>>= posbit %d \n", mascara);
    mascara &= bufferMB[posbyte];
    printf("Valor de mascara despres de mascara&= buffer.. %d \n", mascara);
    mascara >>= (7 - posbit);
    printf("Valor de mascara despres de mascara>>= (7-posbit) %d \n", mascara);
    printf("\n");
    return mascara;
}

int reservar_bloque()
{

    printf("DINS reservar_bloque \n");
    //Declaració e incialització variable Superbloque.
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
    //Encara queden bloc lliures.
    if (SB.cantBloquesLibres > 0)
    { //Declarció d'un buffer.
        unsigned char bufferMB[BLOCKSIZE];
        //Declaració d'un buffer auxiliar.
        unsigned char bufferAux[BLOCKSIZE];
        //Posam a 1 tot el bufferAux
        memset(bufferAux, 255, sizeof(bufferAux));
        //Recorrem tots els blocs corresponents al MB.
        int posBloqueMB = SB.posPrimerBloqueMB;
        for (;; posBloqueMB++)
        { //Lectura del dispositiu virtual del bloc corresponent a posBloqueMB.
            bread(posBloqueMB, bufferMB);
            //Comparam cada bloc llegit amb el bufferAux. Cercam el primer bloc que tengui qualque byte a 0.
            if (memcmp(bufferMB, bufferAux, sizeof(bufferAux)) < 0)
            {
                //Sortim del bloc, tenim el primer bloc que té un byte a 0 dins bufferMb.
                break;
            }
        }
        int posbyte = 0;
        //Cercam el primer byte que no val 1.
        for (;; posbyte++)
        {
            if (bufferMB[posbyte] < 255)
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
        //Calculam el nbloque que hem reservat.
        unsigned int nbloque = ((posBloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE + posbyte) * 8 + posbit;
        //Escribim el bit a 1 que correspon associat al nbloc reservat.
        escribir_bit(nbloque, 1);
        //Decrementam en una unitat la quantitat de bloc lliure que hi ha.
        SB.cantBloquesLibres--;
        printf("Cantidad de bloques libres después de reservar: %d \n", SB.cantBloquesLibres);
        //Salvaguardam el superbloc.
        if (bwrite(posSB, &SB) == BLOCKSIZE)
        {
            printf("Escriptura del SB al dispositu virtual realitzat.\n");
        }
        else
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }

        unsigned char *buffer[BLOCKSIZE];
        memset(buffer, 0, sizeof(buffer));
        //Escriptura d'un buffer tot a 0 per eliminar "basura".
        if (bwrite(nbloque, buffer) == BLOCKSIZE)
        {
            printf("Escriptura del bloc al dispositu virtual realitzat correctament.\n");
        }
        else
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
        printf("\n");
        return nbloque;
    }
    else
    {
        return EXIT_FAILURE;
    }
}
int liberar_bloque(unsigned int nbloque)
{
    printf("\n");
    printf("DENTRO DE LIBERRAR BLOQUE");
    escribir_bit(nbloque, 0);
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    printf("Cantidad de bloques libres antes de liberar el bloque: %d", SB.cantBloquesLibres);
    SB.cantBloquesLibres++;
    printf("Cantidad de bloques libres despues de liberar el  bloque: %d", SB.cantBloquesLibres);

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
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB))
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
    printf("lol: %d:", SB.posPrimerBloqueAI);
    unsigned int numBloque = ((ninodo * INODOSIZE) / BLOCKSIZE) + SB.posPrimerBloqueAI;

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
        return EXIT_SUCCESS;
    }
    else
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }
}

int leer_inodo(unsigned int ninodo, struct inodo *inodo)
{
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    //Càlcul del bloc de AI que correspon amb el inode passat per paràmetre.
    printf("Primer bloque AI: %d:", SB.posPrimerBloqueAI);
    unsigned int numBloque = ((ninodo * INODOSIZE) / BLOCKSIZE) + SB.posPrimerBloqueAI;

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
        return EXIT_FAILURE;
    }
    //Volcam el inodo solicitat al punter passat per parametre.
    inodo = &inodos[ninodo % (BLOCKSIZE / INODOSIZE)];

    return EXIT_SUCCESS;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos)
{
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB))
    {
        //Lectura realitzada correctament.
        printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
    //Comprovam que hi hagui inodes lliures
    if (SB.cantInodosLibres > 0)
    {
        //Guardam la posicio del inode reservat
        unsigned int posInodoReservado = SB.posPrimerInodoLibre;

        //Actualitzam SB perque apunti al seguent inode
        SB.posPrimerInodoLibre++;

        //Inicialitzam tots els components del inode
        struct inodo inodo;

        inodo.tipo = tipo;
        inodo.permisos = permisos;
        inodo.nlinks = 1;
        inodo.tamEnBytesLog = 0;
        inodo.atime = time(NULL);
        inodo.mtime = time(NULL);
        inodo.ctime = time(NULL);
        inodo.numBloquesOcupados = 0;
        for (int i = 0; i < 12; i++)
        {
            inodo.punterosDirectos[i] = 0;
        }
        for (int i = 0; i < 3; i++)
        {
            inodo.punterosIndirectos[i] = 0;
        }

        //Escrivim l'inode
        escribir_inodo(posInodoReservado, inodo);

        //Actualitzam i reescrivim SB
        SB.cantInodosLibres--;
        if (bwrite(posSB, &SB) == BLOCKSIZE)
        {
            printf("Escriptura del SB al dispositu virtual realitzat correctament.\n");
        }
        else
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
        //Retornam la posicio del inode reservat
        return posInodoReservado;
    }
    else
    {
        printf("No quedan Inodos libres \n");
        return EXIT_FAILURE;
    }
}
