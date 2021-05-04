//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "directorios.h"

static struct UltimaEntrada UltimaEntrada[2];

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
    //inicial = strtok(aux, s);
    strcpy(inicial, strtok(aux, s));
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
        //printf("Extraer camino --> Camino: %s, Inicial: %s, Final:%s+ , Tipo: d (1)\n", camino, inicial, final);
        //Retornam 1 per indicar que es un directori.
        return 1;
    }
    else
    {
        //Copiam el nom del arxiu sense la / inicial.
        strcpy(inicial, camino + 1);
        strcpy(final, "");
        //Imprimim les dades per claretat dels tests PROVISIONALS.
        //printf("Extraer camino --> Camino: %s, Inicial: %s, Final:%s+ , Tipo: f (0) \n", camino, inicial, final);
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

    //printf("**p_inodo: %d \n", *p_inodo);
    //printf("**p_entrada: %d \n", *p_entrada);
    //Lectura del SB.
    if (bread(posSB, &SB) == ERROR)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }
    //Comprovam si ens trobam al directori arrel.
    if (strcmp(camino_parcial, "/") == 0)
    {
        *p_inodo = SB.posInodoRaiz;
        //És la primera entrada de totes.
        *p_entrada = 0;
        //printf("Som al directori arrel\n");
        return 0;
    }

    //Dividir el camino_parcial amb inicial i final.
    tipo = extraer_camino(camino_parcial, inicial, final);
    //printf("Tipo 1--> directorio// Tipo 0--> fichero: %d \n", tipo);
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
    //printf("Calculo cant_entradas_inodo de inodo_dir: %d\n", cant_entradas_inodo);
    //Número de entrada inicial.
    num_entrada_inodo = 0;
    //printf("num_entrada_inodo antes del while: %d\n", num_entrada_inodo);
    //struct entrada array_entradas[BLOCKSIZE / sizeof(entrada)];

    memset(&entrada, 0, sizeof(entrada));
    //memset(array_entradas, 0, sizeof(array_entradas));
    int offset = 0;
    if (cant_entradas_inodo > 0)
    {
        mi_read_f(*p_inodo_dir, &entrada, offset, sizeof(entrada));
        //printf("primer read_f, entrad.nombre: %s, entrada.ninodo: %d \n", entrada.nombre, entrada.ninodo);
        //mi_read_f(*p_inodo_dir, array_entradas, offset, BLOCKSIZE);
        while ((num_entrada_inodo < cant_entradas_inodo) && (strcmp(inicial, entrada.nombre)) != 0)
        {
            num_entrada_inodo++;
            offset += sizeof(entrada);
            memset(&entrada, 0, sizeof(entrada));
            mi_read_f(*p_inodo_dir, &entrada, offset, sizeof(entrada));
            //printf("dentro while--> read_f, entrad.nombre: %s, entrada.ninodo: %d \n", entrada.nombre, entrada.ninodo);
        }
    }
    //printf("num_entrada_inodo después del while: %d\n", num_entrada_inodo);
    //Cas entrada no existeix.
    //printf("antes del switch Inicial: %s \n", inicial);
    //printf("antes del switch entrada.nombre val: %s \n", entrada.nombre);
    if ((strcmp(inicial, entrada.nombre) != 0))
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
                //printf(" Antes del strcpy --> Inicial: %s, entrada.nombre: %s \n", inicial, entrada.nombre);
                strcpy(entrada.nombre, inicial);
                //printf(" Despues del strcpy --> Inicial: %s, entrada.nombre: %s \n", inicial, entrada.nombre);
                //Directori

                if (tipo == 1)
                {
                    if (strcmp(final, "/") == 0)
                    {
                        inodoReservado = reservar_inodo('d', permisos);
                        //printf("Reservado inodo %d tipo d con permisos %d para %s \n", inodoReservado, permisos, inicial);
                        if (inodoReservado == ERROR)
                        {
                            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                            return EXIT_FAILURE;
                        }
                        entrada.ninodo = inodoReservado;
                    }
                    else
                    {
                        //Penjen més directoris o fitxers.
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else
                {
                    inodoReservado = reservar_inodo('f', permisos);
                    //printf("Reservado inodo %d tipo f con permisos %d para %s \n", inodoReservado, permisos, inicial);
                    if (inodoReservado == ERROR)
                    {
                        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                        return EXIT_FAILURE;
                    }
                    entrada.ninodo = inodoReservado;
                }
                //printf("entrada.ninodo: %d \n", entrada.ninodo);
                //printf("entrada.nombre: %s \n", entrada.nombre);
                if (mi_write_f(*p_inodo_dir, &entrada, inodo_dir.tamEnBytesLog, sizeof(entrada)) == ERROR)
                {
                    //printf("Error en la escritura");
                    if (entrada.ninodo != -1)
                    {
                        if (liberar_inodo(entrada.ninodo) == ERROR)
                        {
                            //printf("Liberamos el inodo \n");
                            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                        }
                    }
                    return EXIT_FAILURE;
                }
            }
        }
    }
    if ((strcmp(final, "/") == 0) || (strcmp(final, "") == 0))
    {
        //printf("Cortamos la recursividad \n");
        if ((num_entrada_inodo < cant_entradas_inodo) && (reservar == 1))
        {
            return ERROR_ENTRADA_YA_EXISTENTE;
        }
        *p_inodo = entrada.ninodo;
        //printf("*p_inodo: %d \n", *p_inodo);
        *p_entrada = num_entrada_inodo;
        //printf("*p_entrada: %d \n", *p_entrada);
        return EXIT_SUCCESS;
    }
    else
    {
        //printf("Hay recursividad \n");
        *p_inodo_dir = entrada.ninodo;
        //printf("*p_inodo_dir: %d \n", *p_inodo_dir);
        //printf("final : %s \n", final);

        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }
    return EXIT_SUCCESS;
}

int mi_creat(const char *camino, unsigned char permisos)
{
    //Declaram les variables necessaries.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    //Crida a buscar_entrada per reservar un inode amb control d'errors.
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos)) < 0)
    {
        mostrar_error_buscar_entrada(error);
        return ERROR;
    }
    return EXIT_SUCCESS;
}

//Funcio que canvia els permisos del inode associat a la ruta passada per parametre.
int mi_chmod(const char *camino, unsigned char permisos)
{
    //Declaram les variables necessaries.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    //Obtenim el numero del inode corresponent a la ruta.
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, permisos)) < 0)
    {
        //En cas d'error avisam al usuari.
        mostrar_error_buscar_entrada(error);
        return ERROR;
    }

    //Canviam els permisos del inode obtingut.
    if (ERROR == (mi_chmod_f(p_inodo, permisos)))
    {
        //Control d'errors.
        printf("ERROR: Error durante el cambio de permisos. \n");
        return ERROR;
    }
    return EXIT_SUCCESS;
}

//Funcio que imprimeix les dades d'un inode en base a una ruta pasada per parametre.
int mi_stat(const char *camino, struct STAT *p_stat)
{
    //Declaram les variables necessaries.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    //Obtenim el numero del inode corresponent a la ruta.
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0)) < 0)
    {
        //En cas d'error avisam al usuari.
        mostrar_error_buscar_entrada(error);
        return ERROR;
    }
    //Obtenim les dades del inode retornat per buscar_entrada.
    if (ERROR == (mi_stat_f(p_inodo, p_stat)))
    {
        //Control d'errors.
        printf("ERROR: Error durante la obtencion de los datos del inodo. \n");
        return ERROR;
    }
    //Imprimim les dades obtingudes.
    imprimir_stat(p_stat);
    return EXIT_SUCCESS;
}

int mi_dir(const char *camino, char *buffer)
{
    //Declaram les variables necessaries.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    //Obtenim el numero del inode corresponent a la ruta.
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0)) < 0)
    {
        //En cas d'error avisam al usuari.
        mostrar_error_buscar_entrada(error);
        return ERROR;
    }

    //Declaram les variables necesaries per fer la llista d'inodes.
    struct entrada entrada;
    struct inodo inodo;
    int cant_entradas_inodo;

    //Lectura inode tret de la ruta pasada per parametre.
    if (leer_inodo(p_inodo, &inodo) == ERROR)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }
    //Comprovam si te els permisos de lectura.
    if ((inodo.permisos & 4) != 4)
    {
        printf("No tienes permisos de lectura en el directorio indicado \n");
        return EXIT_FAILURE;
    }
    else
    {
        //Calculam quants d'elements tendra la llista.
        cant_entradas_inodo = inodo.tamEnBytesLog / sizeof(entrada);
        //Preparam les variables que ens ajudaran a costruir el buffer.
        int offset = 0;
        char aux[50];
        struct tm *tm;

        //Guardam al buffer el titol del ls.
        sprintf(aux, "Total: %d\n", cant_entradas_inodo);
        strcat(buffer, aux);
        strcat(buffer, "Nombre \tTipo \tModo \tmTime \t\t\tTamaño\n----------------------------------------------------------------\n");
        //Bucle que llegira totes les entrades del inode pare.
        for (int i = 0; i < cant_entradas_inodo; i++)
        {
            //Llegim una entrada del directori.
            memset(&entrada, 0, sizeof(entrada));
            mi_read_f(p_inodo, &entrada, offset, sizeof(entrada));
            offset += sizeof(entrada);
            //Obtenim les dades del inode associat.
            if (leer_inodo(entrada.ninodo, &inodo) == ERROR)
            {
                fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
                return EXIT_FAILURE;
            }
            //Ficam al buffer les dades obtingudes.
            strcat(buffer, entrada.nombre);
            strcat(buffer, "\t");
            sprintf(aux, "%c\t", inodo.tipo);
            strcat(buffer, aux);
            if (inodo.permisos & 4)
                strcat(buffer, "r");
            else
                strcat(buffer, "-");
            if (inodo.permisos & 2)
                strcat(buffer, "w");
            else
                strcat(buffer, "-");
            if (inodo.permisos & 1)
                strcat(buffer, "x");
            else
                strcat(buffer, "-");
            strcat(buffer, "\t");

            tm = localtime(&inodo.mtime);
            sprintf(aux, "%d-%02d-%02d %02d:%02d:%02d\t", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
            strcat(buffer, aux);
            sprintf(aux, "%d\n", inodo.tamEnBytesLog);
            strcat(buffer, aux);
        }
    }
    return EXIT_SUCCESS;
}
int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes)
{
    unsigned int p_inodo = 0;
    //Accés a "cache" per comprovar si la ruta passada per paràmetre, hem accedit a ella anteriorment.
    if (strcmp(UltimaEntrada[0].camino, camino) == 0)
    {
        //Obtenim l'inode al qual escriure.
        p_inodo = UltimaEntrada[0].p_inodo;
        printf("[mi_write()--> Utilizamos la caché de escritura en vez de llamar a buscar_entrada()]\n");
    }
    else
    {
        //El cami passat per paràemtre no ha estat accedit anteriorment.
        //Declaram les variables necessaries.
        unsigned int p_inodo_dir = 0;
        unsigned int p_entrada = 0;
        int error;

        //Obtenim el numero del inode corresponent a la ruta.
        if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0)) < 0)
        {
            //En cas d'error avisam al usuari.
            mostrar_error_buscar_entrada(error);
            return ERROR;
        }
        printf("[mi_write() --> Actualizamos la caché de escritura]\n");
        //Actualitzame els valor de la "cache".
        strcpy(UltimaEntrada[0].camino, camino);
        UltimaEntrada[0].p_inodo = p_inodo;
    }

    int bytes;

    //Cridam a la funcio que escriura el document.
    bytes = mi_write_f(p_inodo, buf, offset, nbytes);
    //Control d'errors,
    if (bytes == ERROR)
    {
        printf("ERROR_ESCRITURA: La funcion mi_write ha devuelto un ERROR.\n");
        return ERROR;
    }
    //Retorn el nombre de bytes escrits.
    return bytes;
}

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes)
{
    unsigned int p_inodo = 0;
    char string[128];
    //Accés a "cache" per comprovar si la ruta passada per paràmetre, hem accedit a ella anteriorment.
    if (strcmp(UltimaEntrada[1].camino, camino) == 0)
    {
        sprintf(string, "[mi_read() --> Utilizamos la caché de lectura en vez de llamar a buscar_entrada()]\n");
        write(2,string, strlen(string));
        //Obtenim l'inode al qual escriure.
        p_inodo = UltimaEntrada[1].p_inodo;
    }
    else
    {
        //El cami passat per paràemtre no ha estat accedit anteriorment.
        //Declaram les variables necessaries.
        unsigned int p_inodo_dir = 0;
        unsigned int p_entrada = 0;
        int error;

        //Obtenim el numero del inode corresponent a la ruta.
        if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0)) < 0)
        {
            //En cas d'error avisam al usuari.
            mostrar_error_buscar_entrada(error);
            return ERROR;
        }
        sprintf(string," [mi_read() --> Actualizamos la caché de lectura]\n");
        write(2,string, strlen(string));
        //Actualitzame els valor de la "cache".
        strcpy(UltimaEntrada[1].camino, camino);
        UltimaEntrada[1].p_inodo = p_inodo;
    }

    int bytes;

    //Cridam a la funcio que llegirà el document.
    bytes = mi_read_f(p_inodo, buf, offset, nbytes);
    //Control d'errors,
    if (bytes == ERROR)
    {
        sprintf(string,"ERROR_LECTURA: La funcion mi_read ha devuelto un ERROR.\n");
        write(2,string,strlen(string));
        return ERROR;
    }
    //Retorn el nombre de bytes llegits.
    return bytes;
}