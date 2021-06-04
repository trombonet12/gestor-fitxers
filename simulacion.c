//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
//Crea uns procesos de prova que accedeixen concurrentment al sistema de fitxers.
#include "simulacion.h"

static int acabados = 0;

//Mètode enterrador.
void reaper()
{
    pid_t ended;
    signal(SIGCHLD, reaper);
    while ((ended = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        acabados++;
    }
}
//Main.
int main(int argc, char **argv)
{
    //Declaracions de les varibels.
    char directorioSimulacion[30];
    time_t tiempo = time(NULL);
    struct tm *tm = localtime(&tiempo);
    //Assignam la senyal SIGCHLD a l'enterrador.
    signal(SIGCHLD, reaper);
    //Comprovam sintaxis.
    if (argc == 2)
    {
        //Establim enllaç amb el dispositiu virtual.
        if (bmount(argv[1]) == ERROR)
        {
            //Control d'errors.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
        //Cream el superString.
        sprintf(directorioSimulacion, "/simul_%d%02d%02d%02d%02d%02d/", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        //printf("Nombre directorio de simulación: %s \n", directorioSimulacion);
        //Cream el directori de simulació.
        if ((mi_creat(directorioSimulacion, 6)) == ERROR)
        {
            //Control d'errors.
            printf("ERROR: Error durante la creación del directorio de la simulación. \n");
            return ERROR;
        }
        //Iteram per a cada un dels procesos.
        for (int proceso = 1; proceso <= NUMPROCESOS; proceso++)
        {
            //Cream un fill.
            pid_t pid = fork();
            //El fill executara el codi de dedins els if.
            if (pid == 0)
            {
                //Montar el dispositiu.
                if (bmount(argv[1]) == ERROR)
                {
                    //Control d'errors.
                    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                    return ERROR;
                }
                //Cream el string que contendra la ruta del directori que empleara el fill.
                char directorioHijo[50];
                char ficheroHijo[60];
                sprintf(directorioHijo, "%sproceso_%d/", directorioSimulacion, getpid());
                //printf("Nombre del directorio hijo :%d --> %s\n", proceso, directorioHijo);

                //Cream el directori que empleara el fill.
                if ((mi_creat(directorioHijo, 6)) == ERROR)
                {
                    //Control d'errors.
                    printf("ERROR: Error durante la creación del directorio de la simulación.\n");
                    return ERROR;
                }
                sprintf(ficheroHijo, "%sprueba.dat",directorioHijo);
                if (mi_creat(ficheroHijo,6)==ERROR)
                {
                    printf("ERROR: Error durante la creación de fichero de la simulación.");
                    return ERROR;
                }

                //Generam una llavor nova per el fill.
                srand(time(NULL) + getpid());
                struct REGISTRO registro;
                //Inicialitzam a ceros registro.
                memset(&registro, 0, sizeof(struct REGISTRO));
                int nescrituras;
                //Realitzam totes les escritres partinents per a cada un dels procesos.
                for (nescrituras = 1; nescrituras <= NUMESCRITURAS; nescrituras++)
                {
                    //Emmagatzema la informació necessària.
                    registro.fecha = time(NULL);
                    registro.pid = getpid();
                    registro.nEscritura = nescrituras;
                    registro.nRegistro = rand() % REGMAX;
                    //printf("[simulacion.c--> Escritura %d en %sprueba.dat] \n", nescrituras, directorioHijo);
                    if((mi_write(ficheroHijo, &registro, registro.nRegistro*sizeof(registro), sizeof(registro)) == ERROR)){
                        printf("ERROR: Error durante la escritura en un proceso hijo. \n");
                        return ERROR;
                    }
                    //Esperam 0,05 segons.
                    unsigned msec1 = 50000;
                    usleep(msec1);
                }
                printf("[Poceso %d: Completadas %d escrituras en %sprueba.dat] \n", proceso, (nescrituras-1), directorioHijo);
                //Desmontam el dispositiu associat al fi.
                if (bumount() < 0)
                {
                    //Control d'erros.
                    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                    return ERROR;
                }
                exit(0);
            }
            //Esperam 0,2 segons.
            unsigned msec = 200000;
            usleep(msec);
        }
        while (acabados < NUMPROCESOS)
        {
            pause();
        }
        if (bumount() < 0)
        {
            //Control d'erros.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            return ERROR;
        }
        return EXIT_SUCCESS;
    }
    else
    {
        printf("ERROR DE SINTAXIS: Uso correcto simulacion <disco>");
        return ERROR;
    }
}
