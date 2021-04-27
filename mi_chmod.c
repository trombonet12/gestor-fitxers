//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
#include "directorios.h"

int main(int argc, char **argv)
{
    //Comprovam la sintaxis
    if (argc == 4)
    {
        //Establim enllaç amb el dispositiu virtual.
        if (bmount(argv[1]) == ERROR)
        {
            //Control d'errors.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }

        //Comprovam que els permisos introduits siguin un valor valid.
        if ((atoi(argv[2]) >= 0) && (atoi(argv[2]) <= 7))
        {
            //Crida a la funcio que canviara els permisos del inode associat a la ruta.
            mi_chmod(argv[3],argv[2][0]);
        }
        else
        {
            printf("ERROR DE PERMISOS: El valor introducido para permisos no es valido.\n");
        }

        if (bumount() < 0)
        {
            //Control d'erros.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }
    else
    {
        printf("ERROR DE SINTAXIS: Sintaxis correcta: mi_chmod <disco> <permisos> </ruta>\n");
    }
}