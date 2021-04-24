//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "directorios.h"

//Mètode que separa en dos (inicial i final),  la cadena de caràcters camino.
int extraer_camino(const char *camino, char *inicial, char *final)
{
    //Control d'errors.
    if (camino == NULL || camino[0] != '/')
    {
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
        printf("Extraer camino --> Camino: %s, Inicial: %s, Final: %s , Tipo: d \n", camino, inicial, final);
        //Retornam 1 per indicar que es un directori.
        return 1;
    }
    else
    {
        //Copiam el nom del arxiu sense la / inicial.
        strcpy(inicial, camino + 1);
        //Imprimim les dades per claretat dels tests PROVISIONALS.
        printf("Extraer camino --> Camino: %s, Inicial: %s, Final: %s , Tipo: f \n", camino, inicial, final);
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

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos)
{
    //Declaració de les varibales.
    struct entrada entrada;
    struct inodo inodo_dir;
    struct superbloque SB;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    int tipo;
    int cant_entradas_inodo, num_entrada_inodo;
    int inodoReservado = 0;

    //Lectura del SB.
    if (bread(posSB, &SB) == ERROR)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }
    printf("camino_parcial: %s: \n", camino_parcial);
    //Comprovam si ens trobam al directori arrel.
    if (strcmp(camino_parcial, "/") == 0)
    {
        *p_inodo = SB.posInodoRaiz;
        //És la primera entrada de totes.
        *p_entrada = 0;
        printf("Som al dirctori arrel");
        return 0;
    }

    //Dividr el camino_parcial amb inicial i final.
    tipo = extraer_camino(camino_parcial, inicial, final);
    printf("camino parcial: %s", camino_parcial);
    printf("inical: %s", inicial);
    printf("final: %s", final);
    printf("tipo: %d \n", tipo);
    if (tipo == ERROR)
    {
        return ERROR_CAMINO_INCORRECTO;
    }

    //Lectura inode corresponent.
    if (leer_inodo(*p_inodo_dir, &inodo_dir) == ERROR)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    //Comprovam que l'inode llegit té permisos de lectura.
    if ((inodo_dir.permisos & 4) != 4)
    {
        return ERROR_PERMISO_LECTURA;
    }

    //Calculam la quantitat de entrades que té l'inode corresponent.
    cant_entradas_inodo = inodo_dir.tamEnBytesLog / sizeof(entrada);
    //Número de entrada inicial.
    num_entrada_inodo = 0;
    //struct entrada array_entradas[BLOCKSIZE / sizeof(entrada)];

    memset(&entrada, 0, sizeof(entrada));
    //memset(array_entradas, 0, sizeof(array_entradas));
    int offset = 0;
    if (cant_entradas_inodo > 0)
    {
        mi_read_f(*p_inodo_dir, &entrada, offset, sizeof(entrada));
        //mi_read_f(*p_inodo_dir, array_entradas, offset, BLOCKSIZE);
        while ((num_entrada_inodo < cant_entradas_inodo) && (!strcmp(inicial, entrada.nombre)))
        {
            num_entrada_inodo++;
            memset(&entrada, 0, sizeof(entrada));
            mi_read_f(*p_inodo_dir, &entrada, offset, sizeof(entrada));
        }
    }
    //Cas entrada no existeix.
    if ((num_entrada_inodo == cant_entradas_inodo) && (strcmp(inicial, entrada.nombre) != 0))
    {
        switch (reservar)
        {
        case 0:
            //Mode consulta. com no existeix, retornam error.
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            break;
        case 1:
            //Mode escriptura.
            //Cream l'entrada en el directori referenicat per *p_inodo_dir.
            //Si es un fitxer, no permet escriptura.
            if (inodo_dir.tipo == 'f')
            {
                return ERRROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
            }
            //Si es directori comprovar que té permisos d'escriptura.
            if ((inodo_dir.permisos & 2) != 2)
            {
                return ERROR_PERMISO_ESCRITURA;
            }
            else
            {
                printf("Inicial: %s" ,inicial);
                strcpy(entrada.nombre, inicial);
                //printf("Inicial: %s, entrada.nombre: %s", inicial, entrada.nombre);
                //Directori
                
                if (tipo == 1)
                {
                    printf("final: %s", final);
                    if (strcmp(final, "/") == 0)
                    {
                        inodoReservado = reservar_inodo('d', permisos);
                        if (inodoReservado == ERROR)
                        {
                            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                            return EXIT_FAILURE;
                        }

                    printf("inodoReservado: %d \n", inodoReservado);
                        entrada.ninodo = inodoReservado;
                    }
                    else
                    {
                        printf("No son iguals");
                        //Penjen més directoris o fitxers.
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else
                {
                    inodoReservado = reservar_inodo('f', permisos);
                    if (inodoReservado == ERROR)
                    {
                        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                        return EXIT_FAILURE;
                    }
                    printf("inodoReservado: %d \n", inodoReservado);
                    entrada.ninodo = inodoReservado;
                }
                if (mi_write_f(inodoReservado, &entrada, num_entrada_inodo*sizeof(entrada), sizeof(entrada)) == ERROR)
                {
                    if (entrada.ninodo != -1)
                    {
                        if (liberar_inodo(entrada.ninodo) == ERROR)
                        {
                            printf("Liberamos el inodo");
                            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                        }
                    }
                    return EXIT_FAILURE;
                }
            }
        }
    }
    if (strcmp(final, "/") == 0){
        printf("num_entradas: %d \n", num_entrada_inodo);
        printf("cant_entrdas_inodo: %d\n", cant_entradas_inodo);
        if((num_entrada_inodo < cant_entradas_inodo) && (reservar = 1)){
            return ERROR_ENTRADA_YA_EXISTENTE;
        }
        *p_inodo = inodoReservado;
        *p_entrada = num_entrada_inodo;
        return EXIT_SUCCESS;

    }else{
        *p_inodo_dir = entrada.ninodo;
        return buscar_entrada(final,p_inodo_dir,p_inodo, p_entrada, reservar, permisos);
    }
    return EXIT_SUCCESS;
}
