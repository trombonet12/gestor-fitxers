//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
#include "verificacion.h"

int main(int argc, char **argv)
{
    //Comprovam sintaxis.
    if (argc == 3)
    {
        //Establim enllaç amb el dispositiu virtual.
        if (bmount(argv[1]) == ERROR)
        {
            //Control d'errors.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
        //Declaram les variables necessaries.
        unsigned int p_inodo_dir = 0;
        unsigned int p_inodo = 0;
        unsigned int p_entrada = 0;
        int error;
        struct inodo inodo;

        //Obtenim el numero del inode corresponent a la ruta.
        if ((error = buscar_entrada(argv[2], &p_inodo_dir, &p_inodo, &p_entrada, 0, 0)) < 0)
        {
            //En cas d'error avisam al usuari.
            mostrar_error_buscar_entrada(error);
            return ERROR;
        }

        int cant_entradas_inodo;

        //Lectura inode tret de la ruta pasada per parametre.
        if (leer_inodo(p_inodo, &inodo) == ERROR)
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            return EXIT_FAILURE;
        }
        //Comprovam si te els permisos de lectura.
        if ((inodo.permisos & 4) != 4)
        {
            printf("No tienes permisos de lectura en el directorio indicado \n");
            return EXIT_FAILURE;
        }

        //Calculam quants d'elements tendra la llista.
        cant_entradas_inodo = inodo.tamEnBytesLog / sizeof(struct entrada);

        if (cant_entradas_inodo != NUMPROCESOS)
        {
            printf("");
        }

        if (bumount() < 0)
        {
            //Control d'erros.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
        return EXIT_SUCCESS;
    }
    else
    {
        printf("ERROR DE SINTAXIS: Uso correcto verificacion <nombre_dispositivo> <directorio_simulación>");
        return ERROR;
    }
}
