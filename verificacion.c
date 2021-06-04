//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
//Verifica les escritures realitzades amb simulacion.c i genera un informe.txt
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
            return ERROR;
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
        //Lectura inode obtingut de la ruta pasada per parametre.
        if (leer_inodo(p_inodo, &inodo) == ERROR)
        {
            fprintf(stderr, "Error leer_inodo %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
        //Comprovam si té els permisos de lectura.
        if ((inodo.permisos & 4) != 4)
        {
            printf("No tienes permisos de lectura en el directorio indicado \n");
            return ERROR;
        }

        //Calculam quantitat d'entrades que té l'inode corresponent al directori de simulació.
        cant_entradas_inodo = inodo.tamEnBytesLog / sizeof(struct entrada);
        //Comprovam que la quantitat d'entrades és igual al nombre de processos.
        if (cant_entradas_inodo != NUMPROCESOS)
        {
            printf("ERROR: La cantidad de entradas no coincide con el NUMPROCESOS.");
            return ERROR;
        }
        //Generam el string que contendra la ruta del "informe.txt".
        char rutaInforme[80];
        strcpy(rutaInforme, argv[2]);
        strcat(rutaInforme, "informe.txt");
        //Cream el fitxer "informe.txt".
        if ((mi_creat(rutaInforme, 6) == ERROR))
        {
            fprintf(stderr, "Error mi_creat %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
        //Cream l'array que contendra totes les entrades del directori de la simulació.
        struct entrada entradas[NUMPROCESOS * sizeof(struct entrada)];
        //Inicialitzam l'array d'entrades a 0 per evitar possibles errors.
        memset(&entrada, 0, sizeof(entradas));
        //Llegim totes les entrades del directori de la simulació.
        if ((mi_read(argv[2], entradas, 0, NUMPROCESOS * sizeof(struct entrada))) == ERROR)
        {
            fprintf(stderr, "Error mi read %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
        int offestInforme = 0;
        //Iteram per a cada un dels processos.
        for (int i = 0; i < NUMPROCESOS; i++)
        {
            //Obtenim el pid del proces que va generar les escriptures mitjançant el nom de la carpeta.
            pidbarra = strchr(entradas[i].nombre, separador);
            //Desplaçam el punter una posició per evitar la _ (Obtinguda de realitzar el strchr()).
            pidbarra++;
            //Converisó de char a iny.
            pid = atoi(pidbarra);
            //Inicialitzam l'array que contendra les escriptures a 0 per evitar problemes.
            memset(buffer_escrituras, 0, sizeof(buffer_escrituras));
            int offset = 0;
            //Cream un string amb la ruta del fitxer a llegir ("prueba.dat").
            char rutaProceso[100];
            sprintf(rutaProceso, "%sproceso_%d/prueba.dat", argv[2], pid);

            //Inicialitzar dades del struct info corresponent. (Facilitarà les comprovacions a fer).
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
            //Bucle que recorr tot el fitxer.
            while ((mi_read(rutaProceso, buffer_escrituras, offset, sizeof(buffer_escrituras)) > 0))
            {
                //Recorrem tot el buffer de lectura.
                for (int i = 0; i < cant_registros_buffer_escrituras; i++)
                {
                    //Comporvació escriptura és vàlida.
                    if (buffer_escrituras[i].pid == info.pid)
                    {

                        //Primera escriptura validada (Serà la de menor posició).
                        if (info.MenorPosicion.nEscritura == 0)
                        {
                            //Escriptura menor posició.
                            info.MenorPosicion.nEscritura = buffer_escrituras[i].nEscritura;
                            info.MenorPosicion.nRegistro = buffer_escrituras[i].nRegistro;
                            info.MenorPosicion.fecha = buffer_escrituras[i].fecha;
                        }
                        else
                        {
                            //Comprovam si la escriptura que hem llegit es la primera escritura.
                            if (buffer_escrituras[i].nEscritura < info.PrimeraEscritura.nEscritura)
                            {
                                info.PrimeraEscritura.nEscritura = buffer_escrituras[i].nEscritura;
                                info.PrimeraEscritura.nRegistro = buffer_escrituras[i].nRegistro;
                                info.PrimeraEscritura.fecha = buffer_escrituras[i].fecha;
                            }
                            //Comprovam si la escriptura que hem llegit es la darrera escritura.
                            else if (buffer_escrituras[i].nEscritura > info.UltimaEscritura.nEscritura)
                            {
                                info.UltimaEscritura.nEscritura = buffer_escrituras[i].nEscritura;
                                info.UltimaEscritura.nRegistro = buffer_escrituras[i].nRegistro;
                                info.UltimaEscritura.fecha = buffer_escrituras[i].fecha;
                            }
                        }
                        //Aumentam en una unitat la quantitat d'enyrades validades.
                        info.nEscrituras++;
                        //Obtenim la informació sobre l'escriptura de major posició.
                        info.MayorPosicion.nEscritura = buffer_escrituras[i].nEscritura;
                        info.MayorPosicion.nRegistro = buffer_escrituras[i].nRegistro;
                        info.MayorPosicion.fecha = buffer_escrituras[i].fecha;
                    }
                }
                //Posam a 0s tot l'array per preparar la següent lectura.
                memset(buffer_escrituras, 0, sizeof(buffer_escrituras));
                //Augmentam el offset per llegir la següent porció del fitxer.
                offset += sizeof(buffer_escrituras);
            }
            printf("%d escrituras validadas en %s \n", info.nEscrituras, rutaProceso);
            //introduim la informació del strcut info al fitxer "infomre.txt".
            //Declaració de dos buffers que ens permetran construir el fitxer informe.txt.
            char buffer1[500];
            char buffer2[100]; 
            memset(buffer1, 0, 500);
            memset(buffer2, 0, 100);
            sprintf(buffer1, "PID: %d\n", info.pid);
            sprintf(buffer2, "Numero escrituras: %d\n", info.nEscrituras);
            strcat(buffer1, buffer2);
            memset(buffer2, 0, 100);
            sprintf(buffer2, "Primera escritura\t%d\t%d\t%s", info.PrimeraEscritura.nEscritura, info.PrimeraEscritura.nRegistro, asctime(localtime(&info.PrimeraEscritura.fecha)));
            strcat(buffer1, buffer2);
            memset(buffer2, 0, 100);
            sprintf(buffer2, "Ultima escritura\t%d\t%d\t%s", info.UltimaEscritura.nEscritura, info.UltimaEscritura.nRegistro, asctime(localtime(&info.UltimaEscritura.fecha)));
            strcat(buffer1, buffer2);
            memset(buffer2, 0, 100);
            sprintf(buffer2, "Menor Posicion\t\t%d\t%d\t%s", info.MenorPosicion.nEscritura, info.MenorPosicion.nRegistro, asctime(localtime(&info.MenorPosicion.fecha)));
            strcat(buffer1, buffer2);
            memset(buffer2, 0, 100);
            sprintf(buffer2, "Mayor Posicion\t\t%d\t%d\t%s\n", info.MayorPosicion.nEscritura, info.MayorPosicion.nRegistro, asctime(localtime(&info.MayorPosicion.fecha)));
            strcat(buffer1, buffer2);
            //Introduim la informació recollida dins el fitxer.
            offestInforme += mi_write(rutaInforme, buffer1, offestInforme, strlen(buffer1));
        }
    }
    else
    {
        printf("ERROR DE SINTAXIS: Uso correcto verificacion <nombre_dispositivo> <directorio_simulación>");
        return ERROR;
    }
    //Descomtam el dispositiu virtual.
    if (bumount() < 0)
    {
        //Control d'erros.
        fprintf(stderr, "Error bumount %d: %s\n", errno, strerror(errno));
        return ERROR;
    }
    //Valor del retorn si el funcioanament és correcte.
    return EXIT_SUCCESS;
}
