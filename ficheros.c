//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes)
{

    struct inodo inodo;
    leer_inodo(ninodo, &inodo);

    if ((inodo.permisos & 2) != 2)
    {
        printf("No tienes permisos de escritura en el inodo indicado \n");
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
            return bwrite(nbfisico, buf_bloque);//HEM DE REVISAR AIXÒ.
        }
        else
        {
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
            memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
            bwrite(nbfisico,buf_bloque) //HEM DE REVISAR AIXÒ.
            for(int i = primerBL + 1; i<ultimoBL; i++){
                bwrite(nbfisico, buf_original + (BLOCKSIZE -desp1) + (i-primerBL-1)*BLOCKSIZE);
            }
            

        }
    }
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    struct inodo inodo;
    if(leer_inodo(ninodo, &inodo)==EXIT_FAILURE) return EXIT_FAILURE;

    p_stat->tipo=inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->atime=inodo.atime;
    p_stat->mtime=inodo.mtime;
    p_stat->ctime=inodo.ctime;
    p_stat->nlinks=inodo.nlinks;
    p_stat->tamEnBytesLog=inodo.tamEnBytesLog;
    p_stat->numBloquesOcupados=inodo.numBloquesOcupados;
    
    return 0;
    
}