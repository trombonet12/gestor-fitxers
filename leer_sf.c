//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "ficheros.h"

int main(int argc, char **argv)
{
    //Establim enllaç amb el dispositiu virtual.
    if (bmount(argv[1]) == ERROR)
    {
        //Control d'errors.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    struct superbloque SB;
    //Llegim el SuperBloc
    if (bread(posSB, &SB) == ERROR)
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

    //Tancam l'enllaç amb el dispositiu virutal.
    if (bumount() < 0)
    {
        //Control d'erros.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
}