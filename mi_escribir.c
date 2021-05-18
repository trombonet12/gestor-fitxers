//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
//Funcio que escriu contingut a un fitxer
#include "directorios.h"

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
        if (argv[2][strlen(argv[2]) - 1] != '/')
        {
            printf("Longitud texto %ld \n",strlen(argv[3]));
            printf("Bytes escritos: %d \n", mi_write(argv[2], argv[3], atoi(argv[4]), strlen(argv[3])));
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
        printf("ERROR DE SINTAXIS: Sintaxis correcta: mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
    }
}