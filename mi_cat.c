//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
#include "directorios.h"
#define TAMBUFFER (BLOCKSIZE * 4)

int main(int argc, char **argv)
{
    //Comprovam la sintaxis
    if (argc == 5)
    {
        //Establim enllaç amb el dispositiu virtual.
        if (bmount(argv[1]) == ERROR)
        {
            //Control d'errors.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
        if (argv[3][strlen(argv[2]) - 1] != '/')
        {
            unsigned char buffer_texto[TAMBUFFER];
            memset(buffer_texto, 0, TAMBUFFER);
            mi_read(argv[2], buffer_texto, 0, 0);
        }
        else
        {
            printf("ERROR DE RUTA: La ruta no acaba en un fichero.\n");
        }

        if (bumount() < 0)
        {
            //Control d'erros.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }
    else
    {
        printf("ERROR DE SINTAXIS: Sintaxis correcta: mi_cat <disco> </ruta_fichero>\n");
    }
}