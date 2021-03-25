//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "ficheros_basico.h"

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
    printf("El bit 40000 vale: %d \n", leer_bit(40000));
    printf("El bit 50000 vale: %d \n", leer_bit(50000));
    printf("El bit 60000 vale: %d \n", leer_bit(60000));
    printf("El bit 70000 vale: %d \n", leer_bit(70000));
    printf("El bit 80000 vale: %d \n", leer_bit(80000));
    printf("El bit 99999 vale: %d \n", leer_bit(99999));

    int bloqueAux = reservar_bloque();
    printf("Reservar primer bloque libre: %d\n", bloqueAux);
    liberar_bloque(bloqueAux);
    printf("Lo liberamos a continuación. \n");

    struct inodo inodo;
    leer_inodo(0, &inodo);
    printf("INODO:%c %c %d %d \n", inodo.tipo, inodo.permisos, inodo.nlinks, inodo.tamEnBytesLog);
    traducir_bloque_inodo(0,8,1);
    traducir_bloque_inodo(0,204,1);
    traducir_bloque_inodo(0,30004,1);
    traducir_bloque_inodo(0,400004,1);
    traducir_bloque_inodo(0,468750,1);

    //Tancam l'enllaç amb el dispositiu virutal.
    if (bumount() < 0)
    {
        //Control d'erros.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
}