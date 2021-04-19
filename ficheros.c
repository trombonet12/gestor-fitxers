//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "ficheros.h"

//
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes)
{
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == ERROR)
    {
        return ERROR;
    }
    int numeroBytesEscritos = 0;

    if ((inodo.permisos & 2) != 2)
    {
        printf("No tienes permisos de escritura en el inodo indicado \n");
        return numeroBytesEscritos;
    }
    else
    {
        //Calculam el primer bloc en el que hem d'escriure.
        int primerBL = offset / BLOCKSIZE;
        //Calculam el darrer bloc en el que hem d'escriure.
        int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
        //Calculam el desplaçament dins el bloc.
        int desp1 = offset % BLOCKSIZE;
        //Calculam el desplaçament en el bloc per veure fins on arriben els nbytes escrits.
        int desp2 = (offset + nbytes - 1) % BLOCKSIZE;
        int nbfisico;
        unsigned char buf_bloque[BLOCKSIZE];

        //El buffer a escriure cab a un sol bloc.
        if (primerBL == ultimoBL)
        {
            //Obtenim el nombre de bloc fisic, corresponent a primerBL.
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
            //Llegim el bloc des del Dispositiu Virtual.
            if (bread(nbfisico, buf_bloque) == ERROR)
            {
                //Error en la lectura.
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return ERROR;
            }
            //Escribim els nbytes del buf_original fins els buf_bloque + desp1 (on hem de començar a escriure).
            memcpy(buf_bloque + desp1, buf_original, nbytes);
            //Salvaguardam el buf_orginal, amb el contingut modificat.
            if (bwrite(nbfisico, buf_bloque) == ERROR)
            {
                //Error amb l'escriptura.
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return ERROR;
            }
            numeroBytesEscritos = nbytes;
        }
        else
        {
            //La escriptura afecta a més d'un bloc.
            //Obtenim el nombre del bloc físic, corresponent a primerBL.
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
            if (bread(nbfisico, buf_bloque) == ERROR)
            {
                //Error en la lectura.
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return ERROR;
            }
            //Escriptura del primer bloc lògic.
            memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
            if (bwrite(nbfisico, buf_bloque) == ERROR)
            {
                //Error amb l'escriptura.
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return ERROR;

            } //HEM DE REVISAR AIXÒ.
            numeroBytesEscritos += BLOCKSIZE - desp1;
            //Escriptura dels blocs lògics intermitjos.
            for (int i = primerBL + 1; i < ultimoBL; i++)
            {
                nbfisico = traducir_bloque_inodo(ninodo, i, 1);
                numeroBytesEscritos += bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE);
            }
            nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 1);
            if (bread(nbfisico, buf_bloque) == ERROR)
            {
                //Error en la lectura.
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return ERROR;
            }
            memcpy(buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
            if (bwrite(nbfisico, buf_bloque) == ERROR)
            {
                //Error amb l'escriptura.
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return ERROR;
            }
            numeroBytesEscritos += desp2 + 1;
        }
    }
    //Llegim l'inode actualitzat.
    if (leer_inodo(ninodo, &inodo) == ERROR)
    {
        return ERROR;
    }
    //Comprovam si hem escrit més enllà del EOF del fitxer associat al inode.
    if (offset + nbytes > inodo.tamEnBytesLog)
    {
        //Actualitzam les dades associades al inde.
        inodo.tamEnBytesLog = offset + nbytes;
        inodo.ctime = time(NULL);
    }
    //Actualitzam les dades associades al inde.
    inodo.mtime = time(NULL);
    //Escriptura del node actualitzat.
    if (escribir_inodo(ninodo, inodo) == ERROR)
    {
        return ERROR;
    }

    return numeroBytesEscritos;
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{

    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == ERROR)
    {
        return ERROR;
    }
    int leidos = 0;
    //printf("Numero inodo: %d \n", ninodo);

    if ((inodo.permisos & 4) != 4)
    {
        printf("No tienes permisos de lectura en el inodo indicado \n");
        return leidos;
    }
    else
    {
        //Volem  començar a llegir més enllà del EOF.
        if (offset >= inodo.tamEnBytesLog)
        {
            return leidos;
        }
        //Volem llegir més enllà del EOF.
        if ((offset + nbytes) >= inodo.tamEnBytesLog)
        {
            //Llegim només els bytes que podem desde l'offset fins al EOF.
            nbytes = inodo.tamEnBytesLog - offset;
        }
        //Calculam el primer bloc en el que hem d'escriure.
        int primerBL = offset / BLOCKSIZE;
        //Calculam el darrer bloc en el que hem d'escriure.
        int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
        //Calculam el desplaçament dins el bloc.
        int desp1 = offset % BLOCKSIZE;
        //Calculam el desplaçament en el bloc per veure fins on arriben els nbytes escrits.
        int desp2 = (offset + nbytes - 1) % BLOCKSIZE;

        int nbfisico;
        unsigned char buf_bloque[BLOCKSIZE];

        //El buffer a llegit es troba un ÚNIC bloc.
        if (primerBL == ultimoBL)
        {
            //Obtenim el valor del bloc físic associat al bloc lògic a llegir.
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
            //printf("Valor nbfisico: %d\n",nbfisico);
            //Bloc físic no existeix.
            if (nbfisico != ERROR)
            {
                //El bloc físic si que existeix.
                //Lectura i increment del valor dels bytes llegits.
                if (bread(nbfisico, buf_bloque) == ERROR)
                {
                    //Error en la lectura.
                    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                    return ERROR;
                }
                //leidos += nbytes;
                //Copiam de buf_bloque a buf_original, els nbytes TOTALS  que ens interessen. Ignormal la resta.
                memcpy(buf_original, buf_bloque + desp1, nbytes);
                //Retonam la quantitat de bytes llegits del únic bloc llegit.
            }
            leidos = nbytes;
            return leidos;
        }
        else
        {
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
            //printf("Valor nbfisico: %d\n",nbfisico);
            //Bloc físic no existeix.
            if (nbfisico != ERROR)
            {
                //El bloc físic si que existeix.
                //Tracament per al primer bloc lògic.
                if (bread(nbfisico, buf_bloque) == ERROR)
                {
                    //Error en la lectura.
                    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                    return ERROR;
                }
                //Copiam de buf_bloque a buf_original, els nbytes TOTALS  que ens interessen. Ignormal la resta.
                memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
                //Tractament pels blocs lògics intermitjos.
            }
            leidos += BLOCKSIZE - desp1;

            for (int i = primerBL + 1; i < ultimoBL; i++)
            {
                nbfisico = traducir_bloque_inodo(ninodo, i, 0);
                //printf("Valor nbfisico: %d\n",nbfisico);
                //Bloc físic no existeix.
                if (nbfisico != ERROR)
                {
                    bread(nbfisico, buf_bloque);
                    memcpy(buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);
                }
                leidos += BLOCKSIZE;
            }
            //Tractamnet per al darrer bloc lògic a llegir.
            nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 0);
            //printf("Valor nbfisico: %d\n",nbfisico);
            //Bloc físic no existeix.
            if (nbfisico != ERROR)
            {
                //Bloc físic si existeix.
                if (bread(nbfisico, buf_bloque) == ERROR)
                {
                    //Error en la lectura.
                    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                    return ERROR;
                }
                memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque, desp2 + 1);
            }
            leidos += desp2 + 1;
            //Retonam la quantitat de bytes llegits de n blocs.
            return leidos;
        }

        return EXIT_SUCCESS;
    }
}
//Retorna la metainfromació d'un fitxer o directori (corresonent al inde passat per paràmetre).
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    struct inodo inodo;
    //Lectura de l'inode corresponent al STAT.
    if (leer_inodo(ninodo, &inodo) == ERROR)
        return ERROR;

    //Inicialitzam el valor dels atributs de STAT corresponent.
    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->atime = inodo.atime;
    p_stat->mtime = inodo.mtime;
    p_stat->ctime = inodo.ctime;
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;

    return EXIT_SUCCESS;
}

//Imprimir informació del struct STAT passat per paràmetre.
void imprimir_stat(struct STAT *p_stat)
{
    //Variables per poder imprimir el time
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];

    //Preparam els strings del time
    ts = localtime(&p_stat->atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S\n", ts);
    ts = localtime(&p_stat->mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S\n", ts);
    ts = localtime(&p_stat->ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S\n", ts);

    //Imprimim les dades solicitades
    printf("El tipo del INODE es: %c\n", p_stat->tipo);
    printf("Los permisos del INODE es: %c\n", p_stat->permisos);
    printf("ATIME: %sMTIME: %sCTIME: %s", atime, mtime, ctime);
    printf("Los nlinks del INODE es: %d\n", p_stat->nlinks);
    printf("El tamEnBytesLog del INODE es: %d\n", p_stat->tamEnBytesLog);
    printf("El numBloquesOcupados del INODE es: %d\n", p_stat->numBloquesOcupados);
}

//Mètode que actualitza l'atribut permisos d'un fitxer, passant per paràmetre el valor de l'inode que li correspon.
int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{
    struct inodo inodo;
    //lectura de l'inode.
    if (leer_inodo(ninodo, &inodo) == ERROR)
        return ERROR;
    //Actualitzam valors dels inodes.
    inodo.permisos = permisos;
    inodo.ctime = time(NULL);
    //Escriptura de l'inode actualitzat.
    if (escribir_inodo(ninodo, inodo) == ERROR)
        return ERROR;
    return EXIT_SUCCESS;
}

//Allibera nbytes d'un inode
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes)
{
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == ERROR)
    {
        return ERROR;
    }
    //printf("Numero inodo: %d \n", ninodo);

    //Comprovam que  tengui permisos de lectura.
    if ((inodo.permisos & 2) != 2)
    {
        printf("No tienes permisos de escritura en el inodo indicado \n");
        return ERROR;
    }
    else
    {
        //Comprovam que no trunquem mes del tamany en bytes logics.
        if (nbytes <= inodo.tamEnBytesLog)
        {
            //Declarcions varibales.
            int primerBL;
            int liberados;

            //Calculam el primer bloc logic
            if ((nbytes % BLOCKSIZE) == 0)
            {
                primerBL = nbytes / BLOCKSIZE;
            }
            else
            {
                primerBL = nbytes / BLOCKSIZE + 1;
            }

            //Alliberam tot els blocs a partir de primerBL.
            liberados = liberar_bloques_inodo(primerBL, &inodo);
            if (liberados == ERROR)
            {
                //Error en la lectura.
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return ERROR;
            }
            //Actualitzam les dades del inode.
            inodo.mtime = time(NULL);
            inodo.ctime = time(NULL);
            inodo.tamEnBytesLog = nbytes;
            inodo.numBloquesOcupados -= liberados;

            //Guardam les dades del inode.
            if (escribir_inodo(ninodo, inodo) == ERROR)
                return ERROR;

            return liberados;
        }
        else
        {
            printf("No se puede truncar más allá del tamaño en bytes lógicos del fichero/directorio. \n");
            return ERROR;
        }
    }
}