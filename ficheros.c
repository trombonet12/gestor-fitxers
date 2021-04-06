//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "ficheros.h"

//
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes)
{

    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
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
        printf("Valor primerBL: %d\n", primerBL);
        printf("Valor ultimoBL: %d\n", ultimoBL);
        printf("Valor desp1: %d\n", desp1);
        printf("Valor desp2: %d\n", desp2);
        int nbfisico;
        unsigned char buf_bloque[BLOCKSIZE];

        //El buffer a escriure cab a un sol bloc.
        if (primerBL == ultimoBL)
        {
            //Obtenim el nombre de bloc fisic, corresponent a primerBL.
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
            //Llegim el bloc des del Dispositiu Virtual.
            if (bread(nbfisico, buf_bloque))
            {
                //Lectura realitzada correctament.
            }
            else
            {
                //Error en la lectura.
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return EXIT_FAILURE;
            }
            //Escribim els nbytes del buf_original fins els buf_bloque + desp1 (on hem de començar a escriure).
            memcpy(buf_bloque + desp1, buf_original, nbytes);
            //Salvaguardam el buf_orginal, amb el contingut modificat.
            bwrite(nbfisico, buf_bloque); //HEM DE REVISAR AIXÒ.
            numeroBytesEscritos = nbytes;
        }
        else
        {
            //La escriptura afecta a més d'un bloc.
            //Obtenim el nombre del bloc físic, corresponent a primerBL.
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
            if (bread(nbfisico, buf_bloque))
            {
                //Lectura realitzada correctament.
            }
            else
            {
                //Error en la lectura.
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return EXIT_FAILURE;
            }
            //Escriptura del primer bloc lògic.
            memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
            bwrite(nbfisico, buf_bloque); //HEM DE REVISAR AIXÒ.
            numeroBytesEscritos += nbytes;
            //Escriptura dels blocs lògics intermitjos.
            for (int i = primerBL + 1; i < ultimoBL; i++)
            {
                numeroBytesEscritos += bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE);
            }
            nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 1);
            bread(nbfisico, buf_bloque);
            memcpy(buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
            numeroBytesEscritos += bwrite(nbfisico, buf_bloque);
        }
    }
    //Llegim l'inode actualitzat.
    leer_inodo(ninodo, &inodo);
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
    escribir_inodo(ninodo, inodo);

    return numeroBytesEscritos;
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{

    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    int leidos = 0;
    printf("Numero inodo: %d \n", ninodo);

    if ((inodo.permisos & 4) != 4)
    {
        printf("No tienes permisos de lectura en el inodo indicado \n");
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
        printf("Valor primerBL: %d\n", primerBL);
        printf("Valor ultimoBL: %d\n", ultimoBL);
        printf("Valor desp1: %d\n", desp1);
        printf("Valor desp2: %d\n", desp2);
        int nbfisico;
        unsigned char buf_bloque[BLOCKSIZE];

        //El buffer a llegit es troba un ÚNIC bloc.
        if (primerBL == ultimoBL)
        {
            //Obtenim el valor del bloc físic associat al bloc lògic a llegir.
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
            //Bloc físic no existeix.
            if (nbfisico == -1)
            {
                printf("El bloc de datos a leer no existe \n");
                //No llegim res, però si augmentam el valor del bytes llegits (tamnay del bloc).
                leidos = BLOCKSIZE;
                //Retonam la quantitat de bytes llegits del únic bloc llegit.
                return leidos;
            }
            else
            {
                //El bloc físic si que existeix.
                //Lectura i increment del valor dels bytes llegits.
                leidos = bread(nbfisico, buf_bloque);
                //Copiam de buf_bloque a buf_original, els nbytes TOTALS  que ens interessen. Ignormal la resta.
                memcpy(buf_original, buf_bloque, nbytes);
                //Retonam la quantitat de bytes llegits del únic bloc llegit.
                return leidos;
            }
        }
        else
        {
            nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
            //Bloc físic no existeix.
            if (nbfisico == -1)
            {
                printf("El bloc de datos a leer no existe \n");
                //No llegim res, però si augmentam el valor del bytes llegits (tamnay del bloc).
                leidos += BLOCKSIZE;
            }
            else
            {
                //El bloc físic si que existeix.
                //Tracament per al primer bloc lògic.
                leidos += bread(nbfisico, buf_bloque);
                //Copiam de buf_bloque a buf_original, els nbytes TOTALS  que ens interessen. Ignormal la resta.
                memcpy(buf_original, buf_bloque, nbytes);
                //Tractament pels blocs lògics intermitjos.
                for (int i = primerBL + 1; i < ultimoBL; i++)
                {
                    nbfisico = traducir_bloque_inodo(ninodo, i, 0);
                    //Bloc físic no existeix.
                    if (nbfisico == -1)
                    {
                        printf("El bloc de datos a ller no existe \n");
                        leidos += BLOCKSIZE;
                    }
                    else
                    {
                        //El bloc físic si que existeix.
                        leidos += bread(nbfisico, buf_bloque);
                        memcpy(buf_original + (BLOCKSIZE - desp1) + (i - primerBL - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);
                    }
                }
                //Tractamnet per al darrer bloc lògic a llegir.
                nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 0);
                //Bloc físic no existeix.
                if (nbfisico == -1)
                {
                    leidos += BLOCKSIZE;
                    //Retonam la quantitat de bytes llegits de n blocs.
                    return leidos;
                }
                else
                {
                    //Bloc físic si existeix.
                    leidos += bread(nbfisico, buf_bloque);
                    memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque, desp2 - 1);
                    //Retonam la quantitat de bytes llegits de n blocs.
                    return leidos;
                }
            }
        }
    }

    return EXIT_FAILURE;
}
//Retorna la metainfromació d'un fitxer o directori (corresonent al inde passat per paràmetre).
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) == EXIT_FAILURE)
        return EXIT_FAILURE;

    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->atime = inodo.atime;
    p_stat->mtime = inodo.mtime;
    p_stat->ctime = inodo.ctime;
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;

    return 0;
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

    printf("El tipo del STAT es: %c\n", p_stat->tipo);
    printf("Los permisos del STAT es: %c\n", p_stat->permisos);
    printf("ATIME: %sMTIME: %sCTIME: %s", atime, mtime, ctime);
    printf("Los nlinks del STAT es: %d\n", p_stat->nlinks);
    printf("El tamEnBytesLog del STAT es: %d\n", p_stat->tamEnBytesLog);
    printf("El numBloquesOcupados del STAT es: %d\n", p_stat->numBloquesOcupados);
}

//Mètode que actualitza l'atribut permisos d'un fitxer, passant per paràmetre el valor de l'inode que li correspon.
int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{
    struct inodo inodo;
    //lectura de l'inode.
    if (leer_inodo(ninodo, &inodo) == EXIT_FAILURE)
        return EXIT_FAILURE;
    //Actualitzam valors dels inodes.
    inodo.permisos = permisos;
    inodo.ctime = time(NULL);
    //Escriptura de l'inode actualitzat.
    if (escribir_inodo(ninodo, inodo) == EXIT_FAILURE)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}