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
        int cant_entradas_inodo;
        struct inodo inodo;
        struct entrada entrada;
        struct INFORMACION info;
        char *pidbarra;
        char pidchar[10];
        int pid;
        const char separador = '_';
        int cant_registros_buffer_escrituras = 256;
        struct REGISTRO buffer_escrituras[cant_registros_buffer_escrituras];
        int escrituras_validadas = 0;
        struct REGISTRO ultimaEscrituraValidada;

        //Obtenim el numero del inode corresponent a la ruta.
        if ((error = buscar_entrada(argv[2], &p_inodo_dir, &p_inodo, &p_entrada, 0, 0)) < 0)
        {
            //En cas d'error avisam al usuari.
            mostrar_error_buscar_entrada(error);
            return ERROR;
        }
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
            printf("ERROR: La cantidad de entradas no coincide con el NUMPROCESOS.");
            return ERROR;
        }
        char rutaInforme[60];
        sprintf(rutaInforme, "%sinforme.txt", argv[2]);
        mi_creat(rutaInforme, 6);

        memset(&entrada, 0, sizeof(struct entrada));
        printf("error");
        for (int i = 0; i < cant_entradas_inodo; i++)
        {
            if(mi_read_f(p_inodo, &entrada, i*sizeof(struct entrada), sizeof(struct entrada)==ERROR)){
                return ERROR;
            }
            memset(&entrada, 0, sizeof(struct entrada));
            //EXTRAR PID
            pidbarra = strchr(entrada.nombre, separador);
            strcpy(pidchar,pidbarra[1]);
            pid = atoi(pidchar);
            printf("%d", pid);
            memset(buffer_escrituras, 0, sizeof(buffer_escrituras));
            int offset = 0;
            char rutaProceso[60];
            sprintf(rutaProceso, "%sproceso_%d/prueba.dat", argv[2], pid);
            while ((mi_read(rutaProceso, buffer_escrituras, offset, sizeof(buffer_escrituras)) > 0)){
                for (int i = 0; i < cant_registros_buffer_escrituras; i++)
                {
                    if (buffer_escrituras[i].pid == pid)
                    {
                        if (escrituras_validadas == 0)
                        {
                            info.pid = pid;
                            info.PrimeraEscritura = buffer_escrituras[i];
                            info.UltimaEscritura = buffer_escrituras[i];
                            info.MenorPosicion = buffer_escrituras[i];
                        }
                        else
                        {
                            if (info.PrimeraEscritura.nEscritura > buffer_escrituras[i].nEscritura)
                            {
                                info.PrimeraEscritura = buffer_escrituras[i];
                            }
                            if (info.UltimaEscritura.nEscritura < buffer_escrituras[i].nEscritura)
                            {
                                info.UltimaEscritura = buffer_escrituras[i];
                            }
                        }
                        ultimaEscrituraValidada = buffer_escrituras[i];
                        escrituras_validadas++;
                    }
                }
                memset(buffer_escrituras, 0, sizeof(buffer_escrituras));  
                offset += sizeof(buffer_escrituras);
            }
            info.MayorPosicion = ultimaEscrituraValidada;
            info.nEscrituras = escrituras_validadas;
            escrituras_validadas = 0;
            printf("%d escrituras validadas en %s", info.nEscrituras, rutaProceso);

            //Obtenim el numero del inode corresponent a la ruta.
            if ((error = buscar_entrada(rutaInforme, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0)) < 0)
            {
                //En cas d'error avisam al usuari.
                mostrar_error_buscar_entrada(error);
                return ERROR;
            }
            //Lectura inode tret de la ruta pasada per parametre.
            if (leer_inodo(p_inodo, &inodo) == ERROR)
            {
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return EXIT_FAILURE;
            }
            mi_write(rutaInforme, &info, inodo.tamEnBytesLog, sizeof(info));
        }
    }
    else
    {
        printf("ERROR DE SINTAXIS: Uso correcto verificacion <nombre_dispositivo> <directorio_simulación>");
        return ERROR;
    }
    if (bumount() < 0)
    {
        //Control d'erros.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

    return EXIT_SUCCESS;
}

