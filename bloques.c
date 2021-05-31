//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

//Include de la capçalera associada a bloques.c
#include "bloques.h"
#include "semaforo_mutex_posix.h"

//Declaram i inicialitzam una variable int que contindrà el valor del descriptor.
static int descriptor = 0;
//Decalarm una variable semàfor.
static sem_t *mutex;
static unsigned int inside_sc = 0;

//Inicialitzar dispositiu virtual.
int bmount(const char *camino)
{
    if (descriptor > 0)
    {
        close(descriptor);
    }

    //Instrucció que ens permet tenir els permisos d'accés i edició desitjats.
    umask(000);
    //Establim l'enlaç amb el fitxer passat per paràmetre, aplicant els permisos i els flags adiets.
    descriptor = open(camino, O_RDWR | O_CREAT, 0666);
    //Cream el semafor que controlara la concurrencia
    if (!mutex)
    {
        mutex = initSem();
        if (mutex == SEM_FAILED)
        {
            return -1;
        }
    }

    return descriptor;
}

//Tancam enllaç amb el dispositiu virtual.
int bumount()
{
    descriptor = close(descriptor); // Fer control de errors
    if (descriptor == -1){
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return ERROR;
    }
    //Eliminam el semàfor.
    deleteSem();
    return EXIT_SUCCESS;
}

//Escriptura d'un bloc al dispositiu virtual.
int bwrite(unsigned int nbloque, const void *buf)
{
    //Declaram varibale int que conté el nombre de bytes que hem escrit al dispositiu virtual per cada crida al mètode.
    int numBytes = 0;
    //Calculam el desplaçament (lloc exacte on escriure el bloc).
    int desplazamineto = nbloque * BLOCKSIZE;
    //Posiciona el punter al lloc exacte.
    lseek(descriptor, desplazamineto, SEEK_SET);
    //Duim a terme l'escriptura d'un bloc al dispositiu virutal.
    numBytes = write(descriptor, buf, BLOCKSIZE);
    //Comprovam el resultat de l'operació.
    if (numBytes < 0)
    {
        //Cas operació errònia.
        return ERROR;
    }
    else
    {
        //Cas operació completada amb èxit.
        return numBytes;
    }
}

//Lectura d'un bloc des del dispositiu virtual.
int bread(unsigned int nbloque, void *buf)
{
    //Declaram varibale int que conté el nombre de bytes que hem llegit des del dispositiu virtual per cada crida al mètode.
    int numBytes = 0;
    //Calculam el desplaçament (lloc exacte on llegirem el bloc).
    int desplazamineto = nbloque * BLOCKSIZE;
    //Posiciona el punter al lloc exacte.
    lseek(descriptor, desplazamineto, SEEK_SET);
    //Duim a terme la lectura d'un bloc al dispositiu virutal.
    numBytes = read(descriptor, buf, BLOCKSIZE);
    //Comprovam el resultat de l'operació.
    if (numBytes < 0)
    {
        //Cas operació errònia.
        return ERROR;
    }
    else
    {
        //Cas operació completada amb èxit.
        return numBytes;
    }
}

//Funcio que fara que el proces esperi si ja n'hi ha un accedint a una zona sensibles.
void mi_waitSem()
{
    if (!inside_sc)
    { // inside_sc==0
        waitSem(mutex);
    }
    inside_sc++;
}

//Funcio que avisa quan es surt d'una zona critica.
void mi_signalSem()
{
    inside_sc--;
    if (!inside_sc)
    {
        signalSem(mutex);
    }
}
