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
            fprintf(stderr, "Error bmount %d: %s\n", errno, strerror(errno));
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
        int pid;
        const char separador = '_';
        int cant_registros_buffer_escrituras = 256;
        struct REGISTRO buffer_escrituras[cant_registros_buffer_escrituras];

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
            fprintf(stderr, "Error leer_inodo %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
        //Comprovam si te els permisos de lectura.
        if ((inodo.permisos & 4) != 4)
        {
            printf("No tienes permisos de lectura en el directorio indicado \n");
            return ERROR;
        }

        //Calculam quants d'elements tendra la llista.
        cant_entradas_inodo = inodo.tamEnBytesLog / sizeof(struct entrada);

        if (cant_entradas_inodo != NUMPROCESOS)
        {
            printf("ERROR: La cantidad de entradas no coincide con el NUMPROCESOS.");
            return ERROR;
        }
        char rutaInforme[80];
        strcpy(rutaInforme, argv[2]);
        strcat(rutaInforme, "informe.txt");
        if ((mi_creat(rutaInforme, 6) == ERROR))
        {
            fprintf(stderr, "Error mi_creat %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
        struct entrada entradas[NUMPROCESOS * sizeof(struct entrada)];
        memset(&entrada, 0, sizeof(entradas));
        if ((mi_read(argv[2], entradas, 0, NUMPROCESOS * sizeof(struct entrada))) == ERROR)
        {
            fprintf(stderr, "Error mi read %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
        int posinforme = 0;
        for (int i = 0; i < NUMPROCESOS; i++)
        {
            pidbarra = strchr(entradas[i].nombre, separador);
            pidbarra++;
            pid = atoi(pidbarra);

            memset(buffer_escrituras, 0, sizeof(buffer_escrituras));
            int offset = 0;
            char rutaProceso[100];
            sprintf(rutaProceso, "%sproceso_%d/prueba.dat", argv[2], pid);

            //Inicialitzar dades del strcut info corresponent.
            info.pid = pid;
            info.nEscrituras = 0;
            info.PrimeraEscritura.fecha = time(NULL);
            info.PrimeraEscritura.nEscritura = NUMESCRITURAS;
            info.PrimeraEscritura.nRegistro = 0;
            info.UltimaEscritura.fecha = 0;
            info.UltimaEscritura.nEscritura = 0;
            info.UltimaEscritura.nRegistro = 0;
            info.MenorPosicion.fecha = time(NULL);
            info.MenorPosicion.nEscritura = 0;
            info.MenorPosicion.nRegistro = REGMAX;
            info.MayorPosicion.fecha = time(NULL);
            info.MayorPosicion.nEscritura = 0;
            info.MayorPosicion.nRegistro = 0;

            while ((mi_read(rutaProceso, buffer_escrituras, offset, sizeof(buffer_escrituras)) > 0))
            {
                for (int i = 0; i < cant_registros_buffer_escrituras; i++)
                {
                    //Comporvació escriptura és vàlida.
                    if (buffer_escrituras[i].pid == info.pid)
                    {

                        //Primera escriptura validada.
                        if (info.MenorPosicion.nEscritura == 0)
                        {
                            //Escriptura menor posició.
                            info.MenorPosicion.nEscritura = buffer_escrituras[i].nEscritura;
                            info.MenorPosicion.nRegistro = buffer_escrituras[i].nRegistro;
                            info.MenorPosicion.fecha = buffer_escrituras[i].fecha;
                        }
                        else
                        {

                            if (buffer_escrituras[i].nEscritura < info.PrimeraEscritura.nEscritura)
                            {
                                info.PrimeraEscritura.nEscritura = buffer_escrituras[i].nEscritura;
                                info.PrimeraEscritura.nRegistro = buffer_escrituras[i].nRegistro;
                                info.PrimeraEscritura.fecha = buffer_escrituras[i].fecha;
                            }
                            else if (buffer_escrituras[i].nEscritura > info.UltimaEscritura.nEscritura)
                            {
                                info.UltimaEscritura.nEscritura = buffer_escrituras[i].nEscritura;
                                info.UltimaEscritura.nRegistro = buffer_escrituras[i].nRegistro;
                                info.UltimaEscritura.fecha = buffer_escrituras[i].fecha;
                            }
                        }
                        info.nEscrituras++;
                        info.MayorPosicion.nEscritura = buffer_escrituras[i].nEscritura;
                        info.MayorPosicion.nRegistro = buffer_escrituras[i].nRegistro;
                        info.MayorPosicion.fecha = buffer_escrituras[i].fecha;
                    }
                }
                memset(buffer_escrituras, 0, sizeof(buffer_escrituras));
                offset += sizeof(buffer_escrituras);
            }
            printf("%d escrituras validadas en %s \n", info.nEscrituras, rutaProceso);
                        // Añadir la información del struct info  al fichero informe.txt por el final
            char aux1[500]; //Buffer que ira recopilando la información para luego escribirla al fichero
            char aux2[100]; //Buffer que recogerá la información del proceso y la colocara en el otro buffer
            memset(aux1, 0, 500);
            memset(aux2, 0, 100);
            sprintf(aux1, "PID: %d\n", info.pid);
            sprintf(aux2, "Numero escrituras: %d\n", info.nEscrituras);
            strcat(aux1, aux2);
            memset(aux2, 0, 100);
            sprintf(aux2, "Primera escritura\t%d\t%d\t%s", info.PrimeraEscritura.nEscritura, info.PrimeraEscritura.nRegistro, asctime(localtime(&info.PrimeraEscritura.fecha)));
            strcat(aux1, aux2);
            memset(aux2, 0, 100);
            sprintf(aux2, "Ultima escritura\t%d\t%d\t%s", info.UltimaEscritura.nEscritura, info.UltimaEscritura.nRegistro, asctime(localtime(&info.UltimaEscritura.fecha)));
            strcat(aux1, aux2);
            memset(aux2, 0, 100);
            sprintf(aux2, "Menor Posicion\t\t%d\t%d\t%s", info.MenorPosicion.nEscritura, info.MenorPosicion.nRegistro, asctime(localtime(&info.MenorPosicion.fecha)));
            strcat(aux1, aux2);
            memset(aux2, 0, 100);
            sprintf(aux2, "Mayor Posicion\t\t%d\t%d\t%s\n", info.MayorPosicion.nEscritura, info.MayorPosicion.nRegistro, asctime(localtime(&info.MayorPosicion.fecha)));
            strcat(aux1, aux2);
            posinforme += mi_write(rutaInforme, aux1, posinforme, strlen(aux1));
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
        fprintf(stderr, "Error bumount %d: %s\n", errno, strerror(errno));
    }

    return EXIT_SUCCESS;
}
