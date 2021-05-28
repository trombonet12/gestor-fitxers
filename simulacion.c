//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
//Crea uns procesos de prova que accedeixen concurrentment al sistema de fitxers.
#include "simulacion.h"

static int acabados = 0;

void my_sleep(unsigned msec)
{ //recibe tiempo en milisegundos
    struct timespec req, rem;
    int err;
    req.tv_sec = msec / 1000;              //conversión a segundos
    req.tv_nsec = (msec % 1000) * 1000000; //conversión a nanosegundos
    while ((req.tv_sec != 0) || (req.tv_nsec != 0))
    {
        if (nanosleep(&req, &rem) == 0)
            break;
        err = errno;
        // Interrupted; continue
        if (err == EINTR)
        {
            req.tv_sec = rem.tv_sec;
            req.tv_nsec = rem.tv_nsec;
        }
    }
}

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
        }
        //Cream el superString.
        sprintf(directorioSimulacion, "/simul_%d%02d%02d%02d%02d%02d/", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        //printf("Nombre directorio de simulación: %s \n", directorioSimulacion);
        //Cream el directori de simulació.
        if ((mi_creat(directorioSimulacion, 6)) == ERROR)
        {
            //Control d'errors.
            printf("ERROR: Error durante la creación del directorio de la simulación. \n");
        }
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
                }
                sprintf(ficheroHijo, "%sprueba.dat",directorioHijo);
                if (mi_creat(ficheroHijo,6)==ERROR)
                {
                    printf("ERROR: Error durante la creación de fichero de la simulación.");
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
                        printf("ERROR ESCRITURA REGISTRO \n");
                    }
                    //Esperam 0,05 segons.
                    unsigned msec1 = 50000;
                    usleep(msec1);
                    //my_sleep(msec1);
                }
                printf("[Poceso %d: Completadas %d escrituras en %sprueba.dat] \n", proceso, (nescrituras-1), directorioHijo);
                //Desmontam el dispositiu associat al fi.
                if (bumount() < 0)
                {
                    //Control d'erros.
                    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                }
                exit(0);
            }
            //Esperam 0,2 segons.
            unsigned msec = 200000;
            //my_sleep(msec);
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
        }
        return EXIT_SUCCESS;
    }
    else
    {
        printf("ERROR DE SINTAXIS: Uso correcto simulacion <disco>");
        return ERROR;
    }
}
