//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
//Imprimim el contingut d'un fitxer
#include "directorios.h"
#define BUFFERTAM 1500

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
        if (argv[2][strlen(argv[2]) - 1] != '/')
        {
            unsigned char buffer_texto[BUFFERTAM];
            unsigned char buffer_aux[BUFFERTAM];
            //Posam a 0 totes les posicions del buffer.
            memset(buffer_texto, 0, BUFFERTAM);
            memset(buffer_aux, 0, BUFFERTAM);
            unsigned int leidos;
            unsigned int total_leidos = 0;
            unsigned int offset = 0;
            char string[128];
            leidos = mi_read(argv[2], buffer_texto, offset, BUFFERTAM);
            offset += BUFFERTAM;
            //Bucle que va llegint tot el contingut y el imprimeix per pantalla
            while (leidos > 0)
            {
                write(1, buffer_texto, leidos);
                memset(buffer_texto, 0, BUFFERTAM);
                total_leidos += leidos;
                leidos = mi_read(argv[2], buffer_texto, offset, BUFFERTAM);
                offset += BUFFERTAM;
            }
            sprintf(string, "\n");
            write(2, string, strlen(string));
            sprintf(string, "Total_leidos: %d\n", total_leidos);
            write(2, string, strlen(string));
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