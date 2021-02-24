//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "bloques.h"
//Declaram i inicialitzam una variable int que contindrà el valor del descriptor.
static int descriptor = 0;

//Inicialitzar dispositiu virtual.
int bmount(const char *camino)
{
    //Establim l'enlaç amb el fitxer passat per paràmetre, aplicant els permisos i els flags adiets.
    //umask(000);
    descriptor = open(camino, O_RDWR|O_CREAT, 0666);
    if (descriptor == -1)
    {
        //Cas operació errònia.
        return EXIT_FAILURE;
    }
    else
    {
        //Cas operació completada amb èxit.
        return descriptor;
    }
}

//Tancam enllaç amb el dispositiu virtual.
int bumount()
{
    //Retora EXIT_SUCCESS (Cas operació completada correctament) o EXIT_FAILURE(Cas operació errònia).
    return close(descriptor);
}

//Escriptura d'un bloc al dispositiu virtual.
int bwrite(unsigned int nbloque, const void *buf)
{
    //Declaram varibale int que conté el nombre de bytes que hem escrit al dispositiu virtual per cada crida al mètode.
    int numBytes = 0;
    //Calculam el desplaçament (lloc exacte on escriure el bloc).
    int desplazamineto = nbloque * BLOCKSIZE;
    //Posiciona el punter al lloc exacte.
    lseek(descriptor,desplazamineto,SEEK_SET);
    //Duim a terme l'escriptura d'un bloc al dispositiu virutal.
    numBytes = write(descriptor,buf,BLOCKSIZE);
    //Comprovam el resultat de l'operació.
    if (numBytes<0){
        //Cas operació errònia.
        return EXIT_FAILURE;
    }else{
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
    lseek(descriptor,desplazamineto,SEEK_SET);
    //Duim a terme la lectura d'un bloc al dispositiu virutal.
    numBytes = read(descriptor,buf,BLOCKSIZE);
    //Comprovam el resultat de l'operació.
    if (numBytes<0){
        //Cas operació errònia.
        return EXIT_FAILURE;
    }else{
        //Cas operació completada amb èxit.
        return numBytes;
    }
}