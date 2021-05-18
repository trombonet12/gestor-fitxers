//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
//Borram un fitxer o directori
#include "directorios.h"

int main(int argc, char **argv)
{
    //Comprovam la sintaxis
    if (argc == 3)
    {
        //Establim enllaç amb el dispositiu virtual.
        if (bmount(argv[1]) == ERROR)
        {
            //Control d'errors.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }

        //Comprovam que el darrer element de la ruta sigui un fichero.
        if (strcmp(argv[2], "/") != 0)
        {
            mi_unlink(argv[2]);
        }
        else
        {
            printf("ERROR: No se puede eliminar el directorio raiz.\n");
        }

        if (bumount() < 0)
        {
            //Control d'erros.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }
    else
    {
        printf("ERROR DE SINTAXIS: Sintaxis correcta: mi_rm <disco> </ruta>\n");
    }
}