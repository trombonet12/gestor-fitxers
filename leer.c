//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
#include "ficheros.h"
#define TAMBUFFER 1024

int main(int argc, char **argv)
{
    if (argc == 3)
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

        unsigned char buffer_texto[TAMBUFFER];
        unsigned char buffer_aux[TAMBUFFER];
        //Posam a 0 totes les posicions del buffer.
        memset(buffer_texto, 0, TAMBUFFER);
        memset(buffer_aux, 0, TAMBUFFER);
        unsigned int leidos;
        unsigned int total_leidos = 0;
        unsigned int offset = 0;
        char string[128];
        leidos = mi_read_f(atoi(argv[2]), buffer_texto, offset, TAMBUFFER);
        offset += TAMBUFFER;
        //sprintf(string, "Bytes leídos %d\n", leidos);
        //write(2, string, strlen(string));
        while (leidos > 0)
        {               
            if (memcmp(buffer_texto, buffer_aux, TAMBUFFER) != 0)
            {
                write(1, buffer_texto, leidos);
                memset(buffer_texto, 0, TAMBUFFER);
                

                //Ho imprimim per la sortida d'errors (2) per que no surti a l'arxiu si redireccionam l'output de la terminal.
                //sprintf(string, "Bytes leídos %d\n", leidos);
                //write(2, string, strlen(string));
            }
            memset(buffer_texto,0,TAMBUFFER);
            total_leidos += leidos;
            leidos = mi_read_f(atoi(argv[2]), buffer_texto, offset, TAMBUFFER);
            offset += TAMBUFFER;
        }
        //Obtenim el tamany en bytes logics del inode en concret.
        //Ho imprimim per la sortida d'errors (2) per que no surti a l'arxiu si redireccionam l'output de la terminal.
        struct STAT p_stat;
        mi_stat_f(atoi(argv[2]), &p_stat);
        sprintf(string, "Tamaño en bytes logicos: %d: \n", p_stat.tamEnBytesLog);
        write(2, string, strlen(string));
        sprintf(string, "Total_leidos: %d\n", total_leidos);
        write(2, string, strlen(string));

        //Tancam l'enllaç amb el dispositiu virutal.
        if (bumount() < 0)
        {
            //Control d'erros.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }
    else
    {
        printf("ERROR DE SINTAXIS: Sintaxis correcta: leer <nombre_dispositivo> <numero_inodo>\n");
    }
}