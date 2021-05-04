//AUTORS : Joan López Ferrer i Miquel Vidal Cortés

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

    if (bwrite(posSB, &SB) == ERROR)
    {
        //Error amb L'escriptura
        return fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }

    return 0;
}

//Mètode que escriu a 1 el bits del MB associats als MetaDatos de manera eficient.
int ponerAUnoBits()
{
    //Decleració d'una varible strcut superbloque.
    struct superbloque SB;

    //Llegim el SuperBloc
    if (bread(posSB, &SB) == ERROR)
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }

    //Càlcul del número de blocs que representen els MetaDatos.
    int numBloquesMetaDatos = tamAI(SB.totInodos) + tamMB(SB.totBloques) + tamSB;
    //Càlcul del nombre de blocs SENCERS a posar a 1.
    int blocsMD = (numBloquesMetaDatos / 8) / BLOCKSIZE;
    //Declaració d'una varibale buffer que representarà els blocs SENCERS  a posar a 1.
    unsigned char bufferMB[BLOCKSIZE];

    //Si tenim com a mínim un bloc SENCER  a posar a 1.
    if (blocsMD > 0)
    {
        //Inicialitzam tot a 1 el buffer
        memset(bufferMB, 255, sizeof(bufferMB));
        //Bucle que coloca 1 els blocs SENCERS associats als Metadatos.
        for (int i = SB.posPrimerBloqueMB; i < (SB.posPrimerBloqueMB + blocsMD); i++)
        {
            //Control de errors durant l'escriptura.
            if (bwrite(i, bufferMB) == ERROR)
            {
                //Error amb l'escriptura
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return ERROR;
            }
        }
    }
    //Declaració d'un buffer que ens ajudara a posar a 1 els bytes i bits restants, que no ocupen un bloc SENCER
    unsigned char bufferAux[BLOCKSIZE];
    //Càlcul del nombre de bytes a posar a 1.
    int bytesMD = (numBloquesMetaDatos / 8) % 1024;
    //Càlcul del nombre de bits a posar a un d'un byte.
    int bitesMD = numBloquesMetaDatos % 8;
    //Posam a 1 els bytes calculats anteriorment
    for (int i = 0; i < bytesMD; i++)
    {
        bufferAux[i] = 255;
    }
    //Si encara queda qualque byte, els bits del cual hem de posar a 1.
    if (bitesMD != 0)
    {
        //Segons el residu, es selecciona el valor.
        unsigned char mascaras[] = {128, 192, 224, 240, 248, 252, 254};
        bufferAux[bytesMD] = mascaras[bitesMD - 1];
    }
    //Finalment, posam a 0 totes les posicons restants del bufferAux.
    for (int i = bytesMD + 1; i < BLOCKSIZE; i++)
    {
        bufferAux[i] = 0;
    }
    //Escriptura del bufferAux a la posició corresponent.
    if (bwrite(SB.posPrimerBloqueMB + blocsMD, bufferAux) == ERROR)
    {
        //Error amb l'escriptura
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }

    //A la quantitat de blocs lliures (introduïts per teclat), li restam els associats als MetaDatos.
    SB.cantBloquesLibres = SB.cantBloquesLibres - numBloquesMetaDatos;
    //Tornam a esciure el SB amb les atributs que hem modificat.
    if (bwrite(posSB, &SB) == ERROR)
    {
        //Error amb l'escriptura
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }

    return EXIT_SUCCESS;
}

//Mètode que inicialitza el MB.
int initMB()
{
    //Declaracio de les variables
    struct superbloque SB;
    unsigned char *buffer[BLOCKSIZE];
    int blocs = 0; //Variable clarificativa

    //Llegim el SuperBloc
    if (bread(posSB, &SB) == ERROR)
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }

    //Preparam el buffer per escriure 0s.
    memset(buffer, 0, sizeof(buffer));
    //Bucle que coloca 0 els blocs associats al MB.
    for (int i = SB.posPrimerBloqueMB; i <= SB.posUltimoBloqueMB; i++)
    {
        //Control de errors durant l'escriptura.
        if (bwrite(i, buffer) == ERROR)
        {
            //Error amb l'escriptura
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
        blocs++; //Variable clarificativa
    }

    //Salvaguardam el SuperBloc dins el Dispositiu Virtual.
    if (bwrite(posSB, &SB) == ERROR)
    {
        //Error amb l'escriptura
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }
    ponerAUnoBits();
    return 0;
}

//Mètode que inicialitza la llista de inodes lliures.
int initAI()
{
    //Declaració e incialització variable Superbloque.
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB) == ERROR)
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
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
        }
        //Escriptura del i-bloc corresponent associat
        if (bwrite(i, inodos) == ERROR)
        {
            //Error en la lectura.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
    }
    return 0;
}

int escribir_bit(unsigned int nbloque, unsigned int bit)
{
    //Declaració e incialització variable Superbloque.
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB) == ERROR)
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }

    //Declaram les variables necesaries
    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;
    unsigned char bufferMB[BLOCKSIZE];

    //LLegim el bloc corresponent
    if (bread(nbloqueabs, bufferMB) == ERROR)
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }

    //Obtenir la posicio dins el rang d'un bloc
    posbyte = posbyte % BLOCKSIZE;
    //Preparam La mascara
    unsigned char mascara = 128;

    mascara >>= posbit;
    //Realtzam la modificacio
    if (bit == 0)
    {
        bufferMB[posbyte] &= ~mascara;
    }
    else if (bit == 1)
    {
        bufferMB[posbyte] |= mascara;
    }
    //Guardam el bloc modificat.
    if (bwrite(nbloqueabs, bufferMB) == ERROR)
    {
        //Escriptura errònia.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }
    return EXIT_SUCCESS;
}

char leer_bit(unsigned int nbloque)
{
    //Declaració e incialització variable Superbloque.
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB) == ERROR)
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }

    //Declaram les variables necesaries
    int posbyte = nbloque / 8;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;
    unsigned char bufferMB[BLOCKSIZE];

    //LLegim el bloc corresponent
    if (bread(nbloqueabs, bufferMB) == ERROR)
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }

    posbyte = posbyte % BLOCKSIZE;
    //Obtenir la posicio dins el rang d'un bloc

    //Preparam la mascara
    unsigned char mascara = 128;
    mascara >>= posbit;
    mascara &= bufferMB[posbyte];
    mascara >>= (7 - posbit);

    return mascara;
}

//Funcio reserva un bloc i ens retorna el seu numero
int reservar_bloque()
{
    //Declaració e incialització variable Superbloque.
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB) == ERROR)
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
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
            if (bread(posBloqueMB, bufferMB) == ERROR)
            {
                //Error en la lectura.
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return ERROR;
            }
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
        //Salvaguardam el superbloc.
        if (bwrite(posSB, &SB) == ERROR)
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            return ERROR;
        }

        unsigned char *buffer[BLOCKSIZE];
        memset(buffer, 0, sizeof(buffer));
        //Escriptura d'un buffer tot a 0 per eliminar "basura".
        if (bwrite(nbloque, buffer) == ERROR)
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
        return nbloque;
    }
    else
    {
        return ERROR;
    }
}

//Funcio que allibera un bloc en concret
int liberar_bloque(unsigned int nbloque)
{
    escribir_bit(nbloque, 0);
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB) == ERROR)
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }

    SB.cantBloquesLibres++;

    if (bwrite(posSB, &SB) == ERROR)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }

    return nbloque;
}

//Funcio que escriu a un inode en concret un inode passat per parametre
int escribir_inodo(unsigned int ninodo, struct inodo inodo)
{
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB) == ERROR)
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }

    //Càlcul del bloc de AI que correspon amb el inode passat per paràmetre.
    unsigned int numBloque = ((ninodo * INODOSIZE) / BLOCKSIZE) + SB.posPrimerBloqueAI;

    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    if (bread(numBloque, inodos) == ERROR)
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }
    inodos[ninodo % (BLOCKSIZE / INODOSIZE)] = inodo;

    if (bwrite(numBloque, inodos) == ERROR)
    {
        //Error amb l'escriptura
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }

    return EXIT_SUCCESS;
}

int leer_inodo(unsigned int ninodo, struct inodo *inodo)
{
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB) == ERROR)
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }

    //Càlcul del bloc de AI que correspon amb el inode passat per paràmetre.
    unsigned int numBloque = ((ninodo * INODOSIZE) / BLOCKSIZE) + SB.posPrimerBloqueAI;

    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    if (bread(numBloque, inodos) == ERROR)
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }
    //Volcam el inodo solicitat al punter passat per parametre.
    *inodo = inodos[ninodo % (BLOCKSIZE / INODOSIZE)];

    return EXIT_SUCCESS;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos)
{
    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB) == ERROR)
    {
        //Error en la lectura.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }
    //Comprovam que hi hagui inodes lliures.
    if (SB.cantInodosLibres > 0)
    {
        //Guardam la posicio del inode reservat.
        unsigned int posInodoReservado = SB.posPrimerInodoLibre;

        //Actualitzam SB perque apunti al seguent inode.
        SB.posPrimerInodoLibre++;

        //Inicialitzam tots els components del inode.
        struct inodo inodo;

        //Inicialitzam els atributs de l'indeo corresponent.
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

        //Escrivim l'inode.
        if ((escribir_inodo(posInodoReservado, inodo) == ERROR))
        {
            return ERROR;
        }

        //Actualitzam i reescrivim SB.
        SB.cantInodosLibres--;
        if (bwrite(posSB, &SB) == ERROR)
        {
            //Error amb l'escriptura
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
        //Retornam la posicio del inode reservat.
        return posInodoReservado;
    }
    else
    {
        printf("No quedan Inodos libres \n");
        return ERROR;
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
        return ERROR;
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
    return ERROR; //Evitam el error "control reaches end of non-void function"
}

//Funcio que retorna el Numero de bloc fisic a partir d'un inode y un bloc logic
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar)
{
    struct inodo inodo;
    unsigned int ptr, ptr_ant, salvar_inodo, nRangoBL, nivel_punteros, indice;
    unsigned int buffer[NPUNTEROS];
    if ((leer_inodo(ninodo, &inodo)) == ERROR)
    {
        return ERROR;
    }

    ptr = 0;
    ptr_ant = 0;
    salvar_inodo = 0;
    nRangoBL = obtener_nRangoBL(inodo, nblogico, &ptr);
    if (nRangoBL == ERROR)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }
    nivel_punteros = nRangoBL;
    //printf("nRangoBL : %d \n", nRangoBL);
    while (nivel_punteros > 0)
    {
        //No penjen blocs de punters.
        if (ptr == 0)
        {
            //Realitzam una consulta i no hi ha cap bloc de indexos.
            if (reservar == 0)
            {
                return ERROR;
            }
            else
            {
                salvar_inodo = 1;
                ptr = reservar_bloque(); // De punters
                if (ptr == ERROR)
                {
                    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                    return ERROR;
                }
                inodo.numBloquesOcupados++;
                inodo.ctime = time(NULL); // Data actual.
                if (nivel_punteros == nRangoBL)
                {
                    //El bloc pescritosenja directament de l'inode.
                    inodo.punterosIndirectos[nRangoBL - 1] = ptr;
                    //fprintf(stderr,"inodo.punterosIndirectos[%d]: %d \n", nRangoBL - 1, ptr);
                }
                else
                {
                    //El bloc penja d'un altre bloc de punters.
                    buffer[indice] = ptr;
                    //fprintf(stderr,"punteros_nivel%d[%d] = %d \n", nivel_punteros + 1, indice, ptr);
                    bwrite(ptr_ant, buffer);
                }
            }
        }
        if (bread(ptr, buffer) == ERROR)
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
        indice = obtener_indice(nblogico, nivel_punteros);
        if (indice == ERROR)
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            return ERROR;
        }

        //Guardam el punter.
        ptr_ant = ptr;
        //Desplaçam el punter al següent nivell.
        ptr = buffer[indice];
        //Dectrementam el nivell de punters en una unitat.
        nivel_punteros--;
    }
    //Ens trobam a nivell de dades.
    //Tot està inicialitzat a 0 (Cap bloque reservat).
    if (ptr == 0)
    {
        //Error, tot està inicialitzat a 0 i feim un consultar.
        if (reservar == 0)
        {
            return ERROR;
        }
        else
        {
            salvar_inodo = 1;
            ptr = reservar_bloque(); // De dades.
            if (ptr == ERROR)
            {
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return ERROR;
            }
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL); //Data actual.
            if (nRangoBL == 0)
            {
                inodo.punterosDirectos[nblogico] = ptr;
            }
            else
            {
                buffer[indice] = ptr;
                if (bwrite(ptr_ant, buffer) == ERROR)
                {
                    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                    return ERROR;
                }
            }
        }
    }
    if (salvar_inodo == 1)
    {
        if (escribir_inodo(ninodo, inodo) == ERROR)
        {
            return ERROR;
        }
    }
    return ptr; //Nbfisico del bloc de dades.
}

//Allibera tot el contigut del inode passat per parametre
int liberar_inodo(unsigned int ninodo)
{

    int bloquesLiberados = 0;
    struct inodo inodo;
    struct superbloque SB;
    if (leer_inodo(ninodo, &inodo) == ERROR)
    {
        return ERROR;
    }
    //Aqui ja incrementam la quantitat de blocs lliure, mitjançant liberar_bloque()
    bloquesLiberados = liberar_bloques_inodo(0, &inodo);
    if (bloquesLiberados == -1)
    {
        printf("ERROR: No se ha podido liberar los bloques.\n");
        return ERROR;
    }
    //Decrementam el valor de la varibale de l'inde amb el retorn de liberar_bloques_inodo.
    inodo.numBloquesOcupados -= bloquesLiberados;
    //Comprovam que hem alliberat el blocs associats al inode.
    if (inodo.numBloquesOcupados == 0)
    {
        //Actualitzam els atributs de l'inode.
        inodo.tipo = 'l';
        inodo.tamEnBytesLog = 0;
        if (bread(posSB, &SB) == ERROR)
        {
            //Error amb la lectura.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
        //L'inode alliberat passa a ser el primer inode de la llista de inodes lliures
        inodo.punterosDirectos[0] = SB.posPrimerInodoLibre;
        SB.posPrimerInodoLibre = ninodo;
        //Incrementam amb una unitat el nombre de inodes lliures.
        SB.cantInodosLibres++;
        //Escriptura de l'inode i el SB actualitzat.
        if (escribir_inodo(ninodo, inodo) == ERROR)
        {
            return ERROR;
        }
        if (bwrite(posSB, &SB) == ERROR)
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
        //Retornam el valor de l'inode actualitzat.
        return ninodo;
    }
    else
    {
        printf("El numero de blocs ocupats no es 0\n");
        //No hem alliberat correctament.
        return ERROR;
    }
}

//Allibera tot el contigut a partir d'un bloc logic
int liberar_bloques_inodo(unsigned int primerBL, struct inodo *inodo)
{
    //Declaracions de les varibles.
    unsigned int nivel_punteros;
    unsigned int indice;
    unsigned int ptr;
    unsigned int nBL;
    unsigned int ultimoBL;
    int nRangoBL;
    unsigned int bloques_punteros[3][NPUNTEROS];
    unsigned int bufAux_punteros[BLOCKSIZE];
    int ptr_nivel[3];
    int indices[3];
    int liberados = 0;

    //Comprovam si hi ha contingut
    if (inodo->tamEnBytesLog == 0)
    {
        fprintf(stderr,"El fichero esta vacio. \n");
        return liberados;
    }
    //Calculam el darrer bloc logic
    if ((inodo->tamEnBytesLog % BLOCKSIZE) == 0)
    {
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE - 1;
    }
    else
    {
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE;
    }

    fprintf(stderr,"liberar_bloques_inodo()→ primer BL: %d, último BL: %d\n", primerBL, ultimoBL);

    memset(bufAux_punteros, 0, sizeof(bufAux_punteros));
    ptr = 0;

    //Buble que recorr tots els blocs logics
    for (nBL = primerBL; nBL <= ultimoBL; nBL++)
    {
        //Obtenim el rang del bloc logic
        nRangoBL = obtener_nRangoBL(*inodo, nBL, &ptr);

        if (nRangoBL == ERROR)
        {
            fprintf(stderr,"ERROR: No se ha podido obtener el rango del BL\n");
            return ERROR;
        }
        nivel_punteros = nRangoBL;
        //Bucle que obte les dades necessaries dels nivells de punters
        while (ptr > 0 && nivel_punteros > 0)
        {
            indice = obtener_indice(nBL, nivel_punteros);
            if (indice == ERROR)
            {
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return ERROR;
            }
            if (indice == 0 || nBL == primerBL)
            {
                //Llegim el dispositiu ja que no es troba en el buffer
                if (bread(ptr, bloques_punteros[nivel_punteros - 1]) == ERROR)
                {
                    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                    return ERROR;
                }
            }
            ptr_nivel[nivel_punteros - 1] = ptr;
            indices[nivel_punteros - 1] = indice;
            ptr = bloques_punteros[nivel_punteros - 1][indice];
            nivel_punteros--;
        }

        //Si existeix un bloc de dades
        if (ptr > 0)
        {
            if (liberar_bloque(ptr) == ERROR)
            {
                return ERROR;
            }
            liberados++;

            fprintf(stderr,"liberar_bloques_inodo()→ liberado BF %d de datos para BL %d\n", ptr, nBL);
            //COmprovam si es directe o indirecte
            if (nRangoBL == 0)
            {
                //Es un punter directe
                inodo->punterosDirectos[nBL] = 0;
            }
            else
            {
                nivel_punteros = 1;
                //Bucle que va alliberant tot els blocs als punters indirectes
                while (nivel_punteros <= nRangoBL)
                {
                    indice = indices[nivel_punteros - 1];
                    bloques_punteros[nivel_punteros - 1][indice] = 0;
                    ptr = ptr_nivel[nivel_punteros - 1];

                    //Comprovam si hi ha bloc que penjen
                    if (memcmp(bloques_punteros[nivel_punteros - 1], bufAux_punteros, BLOCKSIZE) == 0)
                    {
                        //No penjen mes blocs ocupats per tant queda alliberar el bloc de punters
                        if (liberar_bloque(ptr) == ERROR)
                        {
                            return ERROR;
                        }
                        liberados++;
                        fprintf(stderr,"liberar_bloques_inodo()→ liberado BF %d de punteros_nivel%d correspondiente al BL %d\n", ptr, nivel_punteros, nBL);

                        //Aqui es pot afegir una millora per botar-nos els blocs que no fa falta explorar

                        if (nivel_punteros == nRangoBL)
                        {
                            inodo->punterosIndirectos[nRangoBL - 1] = 0;
                        }
                        nivel_punteros++;
                    }
                    else
                    {
                        //Escrivim en el fitxer el bloc de punters modificat
                        if (bwrite(ptr, bloques_punteros[nivel_punteros - 1]) == ERROR)
                        {
                            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                            return ERROR;
                        }

                        //Hem de sortir del bucle ja que no es necessari alliberar els blocs dels nivells superiors
                        nivel_punteros = nRangoBL + 1;
                    }
                }
            }
        }
    }
    printf("liberar_bloques_inodo()→ total bloques liberados: %d \n", liberados);
    return liberados;
}
