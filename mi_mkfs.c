//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "bloques.h"

unsigned char *buffer[BLOCKSIZE];

int main(int argc, char **argv)
{
    if (bmount(argv[1]))
    {
        printf("Operació realitzada correctamnet");
    }
    else
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
    memset(buffer, 0, BLOCKSIZE);
    for (int i = 0; i < atoi(argv[2]); i++)
    {

        if (bwrite(i, buffer) < 0)
        {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }

    if(bumount()<0){
        
    }
    
}