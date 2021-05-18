//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
//Cream un vincle a un fitxer
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

        mi_link(argv[2], argv[3]);

        if (bumount() < 0)
        {
            //Control d'erros.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }
    else
    {
        printf("ERROR DE SINTAXIS: Sintaxis correcta: mi_link disco </ruta_fichero_original> </ruta_enlace>\n");
    }
}