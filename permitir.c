//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
#include "ficheros.h"

int main(int argc, char **argv)
{
    if (argc == 4)
    {
        //Establim enllaç amb el dispositiu virtual.
        if (bmount(argv[1]))
        {
            //printf("Operació realitzada correctament \n");
        }
        else
        {
            //Control d'errors.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }

        mi_chmod_f(atoi(argv[2]),*argv[3]);

        //Tancam l'enllaç amb el dispositiu virutal.
        if (bumount() < 0)
        {
            //Control d'erros.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }
    else
    {
        printf("ERROR DE SINTAXIS: Sintaxis correcta: permitir <nombre_dispositivo> <ninodo> <permisos>\n");
    }
}