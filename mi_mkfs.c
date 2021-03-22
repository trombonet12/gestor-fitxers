//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "ficheros_basico.h"
//Declaram una varibale que contendrà el contigut a llegir o escriure. REALEMNT QUE FA
unsigned char *buffer[BLOCKSIZE];

//Main del programa.
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
    //Inicialitzam a 0 totes les posicions del buffer.
    memset(buffer, 0, sizeof(buffer));
    //Recorrem tots els blocs.
    for (int i = 0; i < atoi(argv[2]); i++)
    {
        //Escriputra del bloc i-èssim.
        if (bwrite(i, buffer) < 0)
        {
            //Control d'erros.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }
    tamMB(atoi(argv[2]));
    tamAI(atoi(argv[2]) / 4);
    initSB(atoi(argv[2]), atoi(argv[2]) / 4);
    initMB();
    //ponerAUnoBits();
    initAI();
    int bloqueAux = reservar_bloque();
    printf("Reservar primer bloque libre: %d",bloqueAux);
    liberar_bloque(bloqueAux);

/*
    printf("El bit 0 vale: %d \n", leer_bit(0));
    printf("El bit 1 vale: %d \n", leer_bit(1));
    printf("El bit 13 vale: %d ", leer_bit(13));
    printf("El bit 14 vale: %d ", leer_bit(14));
    printf("El bit 3138 vale: %d ", leer_bit(3138));
    printf("El bit 3139 vale: %d ", leer_bit(3139));
    printf("El bit 40000 vale: %d ", leer_bit(40000));
    printf("El bit 50000 vale: %d ", leer_bit(50000));
    printf("El bit 60000 vale: %d ", leer_bit(60000));
    printf("El bit 70000 vale: %d ", leer_bit(70000));
    printf("El bit 80000 vale: %d ", leer_bit(80000));
    printf("El bit 99999 vale: %d ", leer_bit(99999));
*/
  //  printf("inodo: %d ", reservar_inodo('d', 'd'));
    /*
    struct inodo inodo;
    leer_inodo(reservar_inodo('d','d'),&inodo);

    printf("INODO:%c %c %d %d \n",inodo.tipo,inodo.permisos,inodo.nlinks,inodo.tamEnBytesLog);
    */

    //Tancam l'enllaç amb el dispositiu virutal.
    if (bumount() < 0)
    {
        //Control d'erros.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
}
