//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "directorios.h"

//Mètode que separa en dos (inicial i final),  la cadena de caràcters camino.
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{
    //Declaram un string auxiliar per poder modificar el contingut de camino.
    char aux[strlen(camino)];
    //Copiam el contingut de camino a aux.
    strcpy(aux, camino);

    //Declaram el separador que emplearem a strtok.
    const char s[1] = "/";
    printf("Camino: %s\n", aux);
    //Agafam el contingut de inicial
    inicial = strtok(aux, s);
    printf("Longitud inicial %ld \n", strlen(inicial));
    printf("Lletra: %c\n", camino[strlen(inicial) + 1]);
    printf("Camino: %s\nInicial: %s\n", aux, inicial);

    if ((camino[strlen(inicial) + 1]) == '/')
    {
        printf("Longitud inicial %ld \n", strlen(inicial));
        printf("Longitud camino %ld \n", strlen(camino));
        printf("Longitud camino-inicial %ld \n", strlen(camino) - strlen(inicial));
        printf("CAmino+inicial: %s\n", camino + (strlen(inicial) + 1));
        strcpy(final, camino + strlen(inicial) + 1);
        printf("Final: %s \n", final);
    }
    printf("LOLOLOLOLLOL\n");
    return 0;
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