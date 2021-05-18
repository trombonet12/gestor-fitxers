//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
//Imprimim les dades d'un inode
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
        struct STAT p_stat;

        //Cridam a la funcio que s'encarrega de trobar l'inode e imprimir el seu contingut.
        mi_stat(argv[2],&p_stat);

        if (bumount() < 0)
        {
            //Control d'erros.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }
    else
    {
        printf("ERROR DE SINTAXIS: Sintaxis correcta: mi_stat <disco> </ruta>\n");
    }
}