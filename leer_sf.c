//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "ficheros.h"

int main(int argc, char **argv)
{
    //Establim enllaç amb el dispositiu virtual.
    if (bmount(argv[1]))
    {
        printf("Operació realitzada correctament \n");
    }
    else
    {
        //Control d'errors.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

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
    printf("El primer bloque de MB és: %d \n", SB.posPrimerBloqueMB);                        //Print clarificatiu
    printf("El darrer bloc del MB és: %d \n", SB.posUltimoBloqueMB);                         //Print clarificatiu
    printf("El primer bloc de l'array inodos és: %d \n", SB.posPrimerBloqueAI);              //Print clarificatiu
    printf("El darrer bloc de l'array inodos és: %d \n", SB.posUltimoBloqueAI);              //Print clarificatiu
    printf("El primer bloc de Datos és: %d \n", SB.posPrimerBloqueDatos);                    //Print clarificatiu
    printf("El darrer bloc de Datoes és: %d \n", SB.posUltimoBloqueDatos);                   //Print clarificatiu
    printf("La posició del inode arrel en l'array inodes és: %d \n", SB.posInodoRaiz);       //Print clarificatiu
    printf("El primer inode lliure dins l'arrar inodes és: %d \n", SB.posPrimerInodoLibre);  //Print clarificatiu
    printf("Quantitat de blocs lliures : %d \n", SB.cantBloquesLibres);                      //Print clarificatiu
    printf("Quantitat de inodes lliures al array de inodes és: %d \n", SB.cantInodosLibres); //Print clarificatiu
    printf("Quantitat de blocs totals és: %d \n", SB.totBloques);                            //Print clarificatiu
    printf("Quantitat de inodes és: %d \n", SB.totInodos);                                   //Print clarificatiu
    printf("Tamany de SB: %lu \n", sizeof(struct superbloque));                              //Print clarificatiu

    printf("Sizeof struct inodo is: %lu\n", sizeof(struct inodo));

    printf("El bit 0 vale: %d \n", leer_bit(0));
    printf("El bit 1 vale: %d \n", leer_bit(1));
    printf("El bit 13 vale: %d \n", leer_bit(13));
    printf("El bit 14 vale: %d \n", leer_bit(14));
    printf("El bit 3138 vale: %d \n", leer_bit(3138));
    printf("El bit 3139 vale: %d \n", leer_bit(3139));
    printf("El bit 31373 vale: %d \n", leer_bit(31373));
    printf("El bit 31374 vale: %d \n", leer_bit(31374));
    printf("El bit 40000 vale: %d \n", leer_bit(400000));
    printf("El bit 50000 vale: %d \n", leer_bit(500000));
    printf("El bit 60000 vale: %d \n", leer_bit(600000));
    printf("El bit 70000 vale: %d \n", leer_bit(700000));
    printf("El bit 80000 vale: %d \n", leer_bit(800000));
    printf("El bit 99999 vale: %d \n", leer_bit(999999));

    int bloqueAux = reservar_bloque();
    printf("Reservar primer bloque libre: %d\n", bloqueAux);
    liberar_bloque(bloqueAux);
    printf("Lo liberamos a continuación. \n");

    struct inodo inodo;
    leer_inodo(0, &inodo);
    printf("INODO:%c %c %d %d \n", inodo.tipo, inodo.permisos, inodo.nlinks, inodo.tamEnBytesLog);
    reservar_inodo('f', '6');
    leer_inodo(1, &inodo);
    printf("INODO:%c %c %d %d \n", inodo.tipo, inodo.permisos, inodo.nlinks, inodo.tamEnBytesLog);
    traducir_bloque_inodo(1, 8, 1);
    traducir_bloque_inodo(1, 204, 1);
    traducir_bloque_inodo(1, 30004, 1);
    traducir_bloque_inodo(1, 400004, 1);
    traducir_bloque_inodo(1, 468750, 1);

    /*
    struct STAT p_stat;
    mi_stat_f(1, &p_stat);
    imprimir_stat(&p_stat);
    sleep(5); //L'he posat per poder veure clarament es canvi en ctime
    mi_chmod_f(1, '7');
    mi_stat_f(1, &p_stat);
    imprimir_stat(&p_stat);
    */

    //Tancam l'enllaç amb el dispositiu virutal.
    if (bumount() < 0)
    {
        //Control d'erros.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
}