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
    printf("El tamaño del array de Inodos es: %d \n", tamanoAI); //Print clarificatiu)
    return tamanoAI;
}

//Mètode que inicialitza les dades del SB.
int initSB(unsigned int nbloques, unsigned int ninodos)
{
    struct superbloque SB;

    SB.posPrimerBloqueMB = posSB + tamSB;
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
    SB.posUltimoBloqueDatos = nbloques - 1;
    SB.posInodoRaiz = 0;
    SB.posPrimerInodoLibre = 0;
    SB.cantBloquesLibres = nbloques;
    SB.cantInodosLibres = ninodos;
    SB.totBloques = nbloques;
    SB.totInodos = ninodos;

    if (bwrite(posSB, &SB) == BLOCKSIZE)
    {
        //printf("Escriptura del SB al dispositu virtual realitzat correctament.\n");
        return 0;
    }
    else
    {
        return fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
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
        //printf("Lectura del Superbloque realitzada correctament \n");
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
        //printf("Escriptura del bloc al dispositu virtual realitzat correctament.\n");
    }
    else
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    printf("\n");
}

int initMB()
{
    //Declaracio de les variables
    struct superbloque SB;
    unsigned char *buffer[BLOCKSIZE];
    int blocs = 0; //Variable clarificativa

    //Llegim el SuperBloc
    if (bread(posSB, &SB))
    {
        //Lectura realitzada correctament.
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
    int numBloquesMetaDatos = tamSB + tamMB(SB.totBloques) + tamAI(SB.totInodos);
    for (int i = 0; i < numBloquesMetaDatos; i++)
    {
        escribir_bit(i, 1);
    }
    //Actualització de la quantitat de bloc lliures.
    SB.cantBloquesLibres = SB.cantBloquesLibres - numBloquesMetaDatos;
    //Salvaguardam el SuperBloc dins el Dispositiu Virtual.
    if (bwrite(posSB, &SB) == BLOCKSIZE)
    {
        //printf("Escriptura del superbloc al dispositu virtual realitzat correctament.\n");
    }
    else
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
    return 0;
}

//Mètode que inicialitza la llista de inodes lliures.
int initAI()
{
    //Declaració e incialització variable Superbloque.
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB))
    {
        //Lectura realitzada correctament.
        //printf("Lectura del Superbloque realitzada correctament \n");
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
            //printf("Escriptura del bloc associat als 8 inodes corresponents realitzat correctament \n");
        }
        else
        {
            //Error en la lectura.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }
    return 0;
}

int escribir_bit(unsigned int nbloque, unsigned int bit)
{
    //Declaració e incialització variable Superbloque.
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB))
    {
        //Lectura realitzada correctament.
        //printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    //Declaram les variables necesaries
    int posbyte = nbloque / 8;
    //printf("El valor de posbyte és: %d \n", posbyte);
    int posbit = nbloque % 8;
    //printf("El valor de posbit és: %d \n", posbit);
    int nbloqueMB = posbyte / BLOCKSIZE;
    //printf("Valor de nbloqueMB %d \n", nbloqueMB);
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;
    //printf("Valor de nbloqueabs %d \n", nbloqueabs);
    unsigned char bufferMB[BLOCKSIZE];

    //LLegim el bloc corresponent
    if (bread(nbloqueabs, bufferMB))
    {
        //Lectura realitzada correctament.
        //printf("Lectura del bloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    //Obtenir la posicio dins el rang d'un bloc
    posbyte = posbyte % BLOCKSIZE;
    //printf("Nou valor de posbyte %d \n", posbyte);
    //Preparam La mascara
    unsigned char mascara = 128;

    mascara >>= posbit;
    //printf("Nou valor de mascara despres daplicar mascara ==> posbit és: %d \n", mascara);
    //Realtzam la modificacio
    if (bit == 0)
    {
        //printf("Posam a 0 el bit \n"); //Print Clarificatiu
        bufferMB[posbyte] &= ~mascara;
        //printf("Valor de bufferMB[posbyte] es: %d \n", bufferMB[posbyte]);
    }
    else if (bit == 1)
    {
        //printf("Posam a 1 el bit \n"); //Print Clarificatiu
        bufferMB[posbyte] |= mascara;
        //printf("Valor de bufferMB[posbyte] es: %d  \n", bufferMB[posbyte]);
    }
    //Guardam el bloc modificat.
    if (bwrite(nbloqueabs, bufferMB) == BLOCKSIZE)
    {
        //Escriptura correcta.
        //printf("Lectura del Superbloque realitzada correctament \n");
        return EXIT_SUCCESS;
    }
    else
    {
        //Escriptura errònia.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }
}

char leer_bit(unsigned int nbloque)
{
    //Declaració e incialització variable Superbloque.
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB))
    {
        //Lectura realitzada correctament.
        //printf("Lectura del Superbloque realitzada correctament \n");
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
        //printf("Lectura del bloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    posbyte = posbyte % BLOCKSIZE;
    //Obtenir la posicio dins el rang d'un bloc

    //Preparam la mascara
    unsigned char mascara = 128;
    mascara >>= posbit;
    //printf("Valor de mascara despres de mascara>>= posbit %d \n", mascara);
    mascara &= bufferMB[posbyte];
    //printf("Valor de mascara despres de mascara&= buffer.. %d \n", mascara);
    mascara >>= (7 - posbit);
    //printf("Valor de mascara despres de mascara>>= (7-posbit) %d \n", mascara);
    return mascara;
}

int reservar_bloque()
{
    //Declaració e incialització variable Superbloque.
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB))
    {
        //Lectura realitzada correctament.
        //printf("Lectura del Superbloque realitzada correctament \n");
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
            //printf("Escriptura del SB al dispositu virtual realitzat.\n");
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
            //printf("Escriptura del bloc al dispositu virtual realitzat correctament.\n");
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
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB))
    {
        //Lectura realitzada correctament.
        //printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    printf("Cantidad de bloques libres antes de liberar el bloque: %d\n", SB.cantBloquesLibres);
    SB.cantBloquesLibres++;
    printf("Cantidad de bloques libres despues de liberar el  bloque: %d\n", SB.cantBloquesLibres);

    if (bwrite(posSB, &SB) == BLOCKSIZE)
    {
        //printf("Escriptura del SB al dispositu virtual realitzat correctament.\n");
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
        //printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    //Càlcul del bloc de AI que correspon amb el inode passat per paràmetre.
    unsigned int numBloque = ((ninodo * INODOSIZE) / BLOCKSIZE) + SB.posPrimerBloqueAI;

    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    if (bread(numBloque, inodos))
    {
        //Lectura realitzada correctament.
        //printf("Lectura del bloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
    inodos[ninodo % (BLOCKSIZE / INODOSIZE)] = inodo;

    if (bwrite(numBloque, inodos) == BLOCKSIZE)
    {
        //printf("Escriptura del bloc al dispositu virtual realitzat correctament.\n");
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
        //printf("Lectura del Superbloque realitzada correctament \n");
    }
    else
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    //Càlcul del bloc de AI que correspon amb el inode passat per paràmetre.
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
    *inodo = inodos[ninodo % (BLOCKSIZE / INODOSIZE)];

    return EXIT_SUCCESS;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos)
{
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB))
    {
        //Lectura realitzada correctament.
        //printf("Lectura del Superbloque realitzada correctament \n");
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
            //printf("Escriptura del SB al dispositu virtual realitzat correctament.\n");
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

//Funció que retorna el rang  de punters on es situa el bloc logic que cercam
//y obtenim la direccio enmagetzemada en el punter
int obtener_nRangoBL(struct inodo inodo, unsigned int nblogico, unsigned int *ptr)
{
    //Recorrem els diferents rangs per trobar en quin es troba el bloc logic passat per parametre

    //Es troba dins de DIRECTOS
    if (nblogico < DIRECTOS)
    {
        *ptr = inodo.punterosDirectos[nblogico];
        return 0;
    }
    //Es troba dins de INDIRECTOS0
    else if (nblogico < INDIRECTOS0)
    {
        *ptr = inodo.punterosIndirectos[0];
        return 1;
    }
    //Es troba dins de INDIRECTOS1
    else if (nblogico < INDIRECTOS1)
    {
        *ptr = inodo.punterosIndirectos[1];
        return 2;
    }
    //Es troba dins de INDIRECTOS2
    else if (nblogico < INDIRECTOS2)
    {
        *ptr = inodo.punterosIndirectos[2];
        return 3;
    }
    //EL bloc no es troba dins el rang
    else
    {
        *ptr = 0;
        fprintf(stderr, "Bloque lógico fuera de rango \n");
        return EXIT_FAILURE;
    }
}
//Funcio que obte els indexs dels blocs de punters
int obtener_indice(int nblogico, int nivel_punteros)
{
    //Es troba dins de DIRECTOS
    if (nblogico < DIRECTOS)
    {
        return nblogico;
    }
    //Es troba dins de INDIRECTOS0
    else if (nblogico < INDIRECTOS0)
    {
        return (nblogico - DIRECTOS);
    }
    //Es troba dins de INDIRECTOS1
    else if (nblogico < INDIRECTOS1)
    {
        if (nivel_punteros == 2)
        {
            return ((nblogico - INDIRECTOS0) / NPUNTEROS);
        }
        else if (nivel_punteros == 1)
        {
            return ((nblogico - INDIRECTOS0) % NPUNTEROS);
        }
    }
    //Es troba dins de INDIRECTOS2
    else if (nblogico < INDIRECTOS2)
    {
        if (nivel_punteros == 3)
        {
            return ((nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS));
        }
        else if (nivel_punteros == 2)
        {
            return (((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS);
        }
        else if (nivel_punteros == 1)
        {
            return (((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) % NPUNTEROS);
        }
    }
    return EXIT_FAILURE; //Evitam el error "control reaches end of non-void function"
}

int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar)
{
    struct inodo inodo;
    unsigned int ptr, ptr_ant, salvar_inodo, nRangoBL, nivel_punteros, indice;
    unsigned int buffer[NPUNTEROS];
    leer_inodo(ninodo, &inodo);
    ptr = 0;
    ptr_ant = 0;
    salvar_inodo = 0;
    nRangoBL = obtener_nRangoBL(inodo, nblogico, &ptr);
    nivel_punteros = nRangoBL;
    while (nivel_punteros > 0)
    {
        //No penjen blocs de punters.
        if (ptr == 0)
        {
            //Bloc de punters inexistent.
            if (reservar == 0)
            {
                return EXIT_FAILURE;
            }
            else
            {
                salvar_inodo = 1;
                ptr = reservar_bloque(); // de punters
                inodo.numBloquesOcupados++;
                inodo.ctime = time(NULL);// data actual.
                if (nivel_punteros == nRangoBL)
                {
                    //El bloc penja directament de l'inode.
                    inodo.punterosIndirectos[nRangoBL - 1] = ptr;
                    printf("inodo.punterosIndirectos[%d]: %d \n" , nRangoBL-1, ptr);
                }
                else
                {
                    //El bloc penja d'un altre bloc de punters.
                    buffer[indice] = ptr;
                    printf("punteros_nivel1[%d] = %d \n", indice, ptr);
                    bwrite(ptr_ant, buffer);
                }
            }
        }
        bread(ptr, buffer);
        indice = obtener_indice(nblogico, nivel_punteros);
        //Guardam el punter.
        ptr_ant = ptr;
        //Desplaçam el punter al següent nivell.
        ptr = buffer[indice];
        nivel_punteros--;
    }
    //Ens trobam a nivell de dades.
    if (ptr == 0)
    {
        if (reservar == 0)
        {
            return EXIT_FAILURE;
        }
        else
        {
            salvar_inodo = 1;
            ptr = reservar_bloque(); // De dades.
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL);
            if (nRangoBL == 0)
            {
                inodo.punterosDirectos[nblogico] = ptr;
                printf("inodo.punterosDirectos[%d] =  %d \n", nblogico, ptr);
            }
            else
            {
                buffer[indice] = ptr;
                 printf("punteros_nivel1[%d] = %d \n" , indice, ptr);
                bwrite(ptr_ant, buffer);
            }
        }
    }
    if (salvar_inodo == 1)
    {
        escribir_inodo(ninodo, inodo);
    }
    return ptr;
}