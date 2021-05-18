//AUTORS: Joan López Ferrer i Miquel Vidal Cortés
//Inicialitzam el sistema de fitxers

#include "directorios.h"
//Declaram una varibale que contendrà el contigut a llegir o escriure. REALEMNT QUE FA
unsigned char *buffer[BLOCKSIZE];

//Main del programa.
int main(int argc, char **argv)
{
    //Establim enllaç amb el dispositiu virtual.
    if (bmount(argv[1]) == ERROR)
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

    //Inicialitzam tots el metadatos
    if (tamMB(atoi(argv[2])) == ERROR)
    {
        printf("ERROR_MKFS: Error en la funcion tamMB\n");
        return ERROR;
    }

    if (tamAI(atoi(argv[2]) / 4) == ERROR)
    {
        printf("ERROR_MKFS: Error en la funcion tamAI\n");
        return ERROR;
    }

    if (initSB(atoi(argv[2]), atoi(argv[2]) / 4) == ERROR)
    {
        printf("ERROR_MKFS: Error en la funcion initSB\n");
        return ERROR;
    }

    if (initMB() == ERROR)
    {
        printf("ERROR_MKFS: Error en la funcion initMB\n");
        return ERROR;
    }

    if (initAI() == ERROR)
    {
        printf("ERROR_MKFS: Error en la funcion initAI\n");
        return ERROR;
    }

    //printf("Creamos directorio raiz: %d \n", reservar_inodo('d', '7'));
    reservar_inodo('d', '7');

    //Tancam l'enllaç amb el dispositiu virutal.
    if (bumount() < 0)
    {
        //Control d'erros.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    }

}
