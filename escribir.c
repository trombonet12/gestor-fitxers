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

        unsigned int offset[] = {9000, 209000, 30725000, 409605000, 480000000};
        unsigned int length = strlen(argv[2]);

        //Comprovam si ho hem d'escriure tot dins un inode
        if (*argv[3] == '0')
        {
            printf("Escribimos el texto \"%s\" en los offsets  9000, 209000, 30725000, 409605000 y 480000000 de un mismo inodo.\n", argv[2]);
            printf("Longitud texto: %d\n", length);

            unsigned int ninodo;
            struct STAT p_stat;

            ninodo = reservar_inodo('f', 6);

            for (int i = 0; i < 5; i++)
            {
                printf("Nº inodo reservado: %d\n", ninodo);
                printf("Offset: %d\n", offset[i]);
                int nbfisico = mi_write_f(ninodo, argv[2], offset[i], length);
                printf("Numero de Bytes escritos: %d\n",nbfisico );
                mi_stat_f(ninodo, &p_stat);
                fprintf(stderr,"Tamaño en bytes logicos: %d: \n", p_stat.tamEnBytesLog);
                fprintf(stderr,"Num Bloques Ocupados: %d \n\n", p_stat.numBloquesOcupados);
            }
        }
        else
        {
            printf("Escribimos el texto \"%s\" en los offsets  9000, 209000, 30725000, 409605000 y 480000000 de inodos diferentes.\n", argv[2]);
            unsigned int ninodo;
            struct STAT p_stat;

            for (int i = 0; i < 5; i++)
            {
                ninodo = reservar_inodo('f', 6);
                printf("Nº inodo reservado: %d\n", ninodo);
                printf("Offset: %d\n", offset[i]);
                printf("Numero de Bytes escritos: %d\n", mi_write_f(ninodo, argv[2], offset[i], length));
                mi_stat_f(ninodo, &p_stat);
                printf("Tamaño en bytes logicos: %d: \n", p_stat.tamEnBytesLog);
                printf("Num Bloques Ocupados: %d \n\n", p_stat.numBloquesOcupados);
            }
        }

        //Tancam l'enllaç amb el dispositiu virutal.
        if (bumount() < 0)
        {
            //Control d'erros.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }
    else
    {
        printf("ERROR DE SINTAXIS: Sintaxis correcta: escribir <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>\n");
        printf("Si diferentes_inodos=0 se reserva un solo inodo para todos los offsets.\n");
    }
}