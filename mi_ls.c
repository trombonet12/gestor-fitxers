//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
#include "directorios.h"

int main(int argc, char **argv)
{
    //Comprovam la sintaxis
    if (argc >= 3)
    {
        //Establim enllaç amb el dispositiu virtual.
        if (bmount(argv[1]) == ERROR)
        {
            //Control d'errors.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
        char buffer[TAMBUFFER];
        if (argc == 3)
        {
            mi_dir(argv[2], buffer, 1);
        }
        else
        {
            mi_dir(argv[2], buffer, atoi(argv[3]));
        }

        printf("%s", buffer);
        printf("\n");
        if (bumount() < 0)
        {
            //Control d'erros.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }
    else
    {
        printf("ERROR DE SINTAXIS: Sintaxis correcta: mi_ls <disco> </ruta_directorio> <tipo(0 = Version reducida)>\n");
    }
}