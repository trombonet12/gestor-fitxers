//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "ficheros_basico.h"

struct STAT
{                          // comprobar que ocupa 128 bytes haciendo un sizeof(inodo)!!!
   unsigned char tipo;     // Tipo ('l':libre, 'd':directorio o 'f':fichero)
   unsigned char permisos; // Permisos (lectura y/o escritura y/o ejecución)

   time_t atime; // Fecha y hora del último acceso a datos: atime
   time_t mtime; // Fecha y hora de la última modificación de datos: mtime
   time_t ctime; // Fecha y hora de la última modificación del inodo: ctime

   /* comprobar el tamaño del tipo time_t para vuestra plataforma/compilador:
   printf ("sizeof time_t is: %d\n", sizeof(time_t)); */

   unsigned int nlinks;             // Cantidad de enlaces de entradas en directorio
   unsigned int tamEnBytesLog;      // Tamaño en bytes lógicos
   unsigned int numBloquesOcupados; // Cantidad de bloques ocupados zona de datos

};


int mi_write_f (unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_read_f (unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_stat_f (unsigned int ninodo, struct STAT *p_stat);
int mi_chmod_f (unsigned int ninodo, unsigned char permisos);
void imprimir_stat (struct STAT *_stat, unsigned int ninodo); //Provisional, nose si passar per un direcció o no.
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes);