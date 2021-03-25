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
    tamAI(atoi(argv[2]) / 4);
    initSB(atoi(argv[2]), atoi(argv[2]) / 4);
    initMB();
    printf("PRINCIPI INIT AI\n");
    initAI();
    printf("FINAL INIT AI\n");
    
    printf("Creamos directorio raiz: %d \n", reservar_inodo('d', '7'));
    
    //Tancam l'enllaç amb el dispositiu virutal.
    if (bumount() < 0)
    {
        //Control d'erros.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }
}
