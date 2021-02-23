//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "bloques.h"
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
    memset(buffer, 0, BLOCKSIZE);
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

    //Tancam l'enllaç amb el dispositiu virutal.
    if (bumount() < 0)
    {
        //Control d'erros.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
}