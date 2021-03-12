//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "ficheros_basico.h"
//Declaram una varibale que contendrà el contigut a llegir o escriure. REALEMNT QUE FA
unsigned char *buffer[BLOCKSIZE];

//Main del programa.
int main(int argc, char **argv)
{
    //Establim enllaç amb el dispositiu virtual.
    if (bmount(argv[1]))
    {
        printf("Operació realitzada correctament \n");
    }
    else
    {
        //Control d'errors.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
    //Inicialitzam a 0 totes les posicions del buffer.
    memset(buffer, 0, sizeof(buffer));
    //Recorrem tots els blocs.
    for (int i = 0; i < atoi(argv[2]); i++)
    {
        //Escriputra del bloc i-èssim.
        if (bwrite(i, buffer) < 0)
        {
            //Control d'erros.
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        }
    }
    tamMB(atoi(argv[2]));
    tamAI(atoi(argv[2])/4);
    initSB(atoi(argv[2]),atoi(argv[2])/4);
    initMB();
    initAI();

/*
    struct superbloque *SB = (struct superbloque *)malloc(sizeof(struct superbloque));
    bread(posSB, SB);
    printf("Soc a nes mian comprovant tamany de SB \n");
    printf("Tamany de SB: %lu \n", sizeof(*SB)); 
    printf("Tamany de SB: %lu \n", sizeof(struct superbloque)); //Print clarificatiu
    
    struct inodo inodos [BLOCKSIZE/INODOSIZE];
    printf("Tamany de INODE: %lu \n", sizeof(struct inodo)); //Print clarificatiu

*/
    //Tancam l'enllaç amb el dispositiu virutal.
    if (bumount() < 0)
    {
        //Control d'erros.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
   
}


