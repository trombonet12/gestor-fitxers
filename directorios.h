//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "ficheros.h"

#define tamnombre 60
#define ERROR_CAMINO_INCORRECTO -1
#define ERROR_PERMISO_LECTURA -2
#define ERROR_NO_EXISTE_ENTRADA_CONSULTA -3
#define ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO -4
#define ERROR_PERMISO_ESCRITURA -5
#define ERROR_ENTRADA_YA_EXISTENTE -6
#define ERRROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO -7

//Declaració del strcut entrada.
struct entrada
{
    char nombre[tamnombre];
    unsigned int ninodo;
};

void mostrar_error_buscar_entrada(int error);
int extraer_camino(const char *camino, char *inicial, char *final);
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos);

