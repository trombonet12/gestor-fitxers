//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "bloques.h"
static int descriptor = 0;

//Inicialitzar dispositiu virtual.
int bmount(const char *camino)
{
    descriptor = open(camino, O_RDWR | O_CREAT, 0666);
    if (descriptor == -1)
    {
        //Error.
        return EXIT_FAILURE;
    }
    else
    {
        return descriptor;
    }
}

//Tancam enllaç amb el dispositiu virtual.
int bumount()
{
    //Retora EXIT_SUCCESS o EXIT_FAILURE.
    return close(descriptor);
}

//Escriptura d'un bloc al dispositiu virtual.
int bwrite(unsigned int nbloque, const void *buf)
{
    int numBytes = 0;
    //Calculam el desplaçament.
    int desplazamineto = nbloque * BLOCKSIZE;

    lseek(descriptor,desplazamineto,SEEK_SET);

    numBytes = write(descriptor,buf,BLOCKSIZE);

    if (numBytes<0){
        return EXIT_FAILURE;
    }else{
        return numBytes;
    }

}

//Lectura d'un bloc des del dispositiu virtual.
int bread(unsigned int nbloque, void *buf)
{
    int numBytes = 0;
    //Calculam el desplaçament.
    int desplazamineto = nbloque * BLOCKSIZE;

    lseek(descriptor,desplazamineto,SEEK_SET);

    numBytes = read(descriptor,buf,BLOCKSIZE);

    if (numBytes<0){
        return EXIT_FAILURE;
    }else{
        return numBytes;
    }
}