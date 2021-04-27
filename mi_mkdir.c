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
            //printf("Lenght: %d \n",strlen(argv[3]));
            //Comprovam que el darrer element de la ruta sigui un directori.
            if (argv[3][strlen(argv[3])-1] == '/')
            {
                printf("Iniciamos creacion del directorio\n"); //Print clarificatiu
                if(mi_creat(argv[3],argv[2][0])==ERROR){
                    printf("ERROR: Error durante la creacion del directorio.\n");
                }
            }
            else
            {
                printf("ERROR DE RUTA: La ruta no acaba en un directorio.\n");
            }
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
        printf("ERROR DE SINTAXIS: Sintaxis correcta: mi_mkdir <disco> <permisos> </ruta>\n");
    }
}