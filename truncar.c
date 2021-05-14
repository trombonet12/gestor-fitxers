//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
#include "ficheros.h"

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

        //Alliberam l'inode
        if (atoi(argv[3]) == 0)
        {
            //Alliberam tot l'inode
            if (liberar_inodo(atoi(argv[2])) == ERROR)
            {
                printf("ERROR: liberar inodo no se ha ejecutado correctamente\n");
                //exit(0);
            }
        }
        else
        {
            int liberados;
            //Alliberam nbytes del inode
            liberados = mi_truncar_f(atoi(argv[2]), atoi(argv[3]));

            printf("Liberados: %d\n\n", liberados);
        }

        //Imprimim les dades del inode
        struct STAT p_stat;

        mi_stat_f(atoi(argv[2]), &p_stat);
        if (mi_stat_f(atoi(argv[2]), &p_stat) == ERROR)
        {
            printf("ERROR_TRUNCAR: Error a la hora de obtener los datos de un inodo.\n");
            return ERROR;
        }

        //Imprimim les dades del inode
        printf("\n");
        imprimir_stat(&p_stat, atoi(argv[2]));
        printf("\n");

        //Tancam l'enllaç amb el dispositiu virutal.
        if (bumount() < 0)
        {
            //Control d'erros.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }
    else
    {
        printf("ERROR DE SINTAXIS: Sintaxis correcta: truncar <nombre_dispositivo> <ninodo> <nbytes>\n");
    }
}