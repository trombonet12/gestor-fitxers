//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "directorios.h"

//Mètode que separa en dos (inicial i final),  la cadena de caràcters camino.
int extraer_camino(const char *camino, char *inicial, char *final)
{
    //Control d'errors.
    if (camino == NULL || camino[0] != '/')
    {
        printf("ERROR extraer_camino: camino vale NULL o no empieza con '/'\n");
        return ERROR;
    }
    //Declaram un string auxiliar per poder modificar el contingut de camino.
    char aux[strlen(camino)];
    //Copiam el contingut de camino a aux.
    strcpy(aux, camino);

    //Declaram el separador que emplearem a strtok.
    const char s[1] = "/";
    //Agafam el contingut de inicial
    inicial = strtok(aux, s);
    //Comprovam que strtok s'ha executat correctament.
    if (inicial == NULL)
    {
        printf("ERROR extraer_camino: Error en la ejecucion del strtok\n");
        return ERROR;
    }
    //Comprovam si el que ens han passat es un fitxer o un directori.
    if ((camino[strlen(inicial) + 1]) == '/')
    {
        //Copiam la resta de la ruta a final.
        strcpy(final, camino + strlen(inicial) + 1);
        //Imprimim les dades per claretat dels tests PROVISIONALS.
        printf("Camino: %s\nInicial: %s\n", camino, inicial);
        printf("Tipo: 'd' \n");
        printf("Final: %s \n", final);
        //Retornam 1 per indicar que es un directori.
        return 1;
    }
    else
    {
        //Copiam el nom del arxiu sense la / inicial.
        strcpy(inicial, camino + 1);
        //Imprimim les dades per claretat dels tests PROVISIONALS.
        printf("Camino: %s\nInicial: %s\n", camino, inicial);
        printf("Tipo: 'f' \n");
        printf("Final: %s \n", final);
        //Retornam 0 per indicar que es un arxiu
        return 0;
    }
}

//Mètode que, donat un nombre, indica quin error està associat a ell-
void mostrar_error_buscar_entrada(int error)
{
    // fprintf(stderr, "Error: %d\n", error);
    switch (error)
    {
    case -1:
        fprintf(stderr, "Error: Camino incorrecto.\n");
        break;
    case -2:
        fprintf(stderr, "Error: Permiso denegado de lectura.\n");
        break;
    case -3:
        fprintf(stderr, "Error: No existe el archivo o el directorio.\n");
        break;
    case -4:
        fprintf(stderr, "Error: No existe algún directorio intermedio.\n");
        break;
    case -5:
        fprintf(stderr, "Error: Permiso denegado de escritura.\n");
        break;
    case -6:
        fprintf(stderr, "Error: El archivo ya existe.\n");
        break;
    case -7:
        fprintf(stderr, "Error: No es un directorio.\n");
        break;
    }
}