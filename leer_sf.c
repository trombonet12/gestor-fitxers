//AUTORS: Joan López Ferrer i Miquel Vidal Cortés

#include "directorios.h"

/*
void mostrar_buscar_entrada(char *camino, char reservar){
  unsigned int p_inodo_dir = 0;
  unsigned int p_inodo = 0;
  unsigned int p_entrada = 0;
  int error;
  printf("\ncamino: %s, reservar: %d\n", camino, reservar);
  if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, 6)) < 0) {
    mostrar_error_buscar_entrada(error);
  }
  printf("**********************************************************************\n");
  return;
}
*/

int main(int argc, char **argv)
{
  //Establim enllaç amb el dispositiu virtual.
  if (bmount(argv[1]) == ERROR)
  {
    //Control d'errors.
    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
  }

  struct superbloque SB;
  //Llegim el SuperBloc
  if (bread(posSB, &SB) == ERROR)
  {
    //Error en la lectura.
    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
  }

  printf("posPrimerBloqueMB: %d \n", SB.posPrimerBloqueMB);       //Print clarificatiu
  printf("posUltimoBloqueMB: %d \n", SB.posUltimoBloqueMB);       //Print clarificatiu
  printf("posPrimerBloqueAI: %d \n", SB.posPrimerBloqueAI);       //Print clarificatiu
  printf("posUltimoBloqueAI: %d \n", SB.posUltimoBloqueAI);       //Print clarificatiu
  printf("posPrimerBloqueDatos: %d \n", SB.posPrimerBloqueDatos); //Print clarificatiu
  printf("posUltimoBloqueDatos: %d \n", SB.posUltimoBloqueDatos); //Print clarificatiu
  printf("posInodoRaiz: %d \n", SB.posInodoRaiz);                 //Print clarificatiu
  printf("posPrimerInodoLibre: %d \n", SB.posPrimerInodoLibre);   //Print clarificatiu
  printf("cantBloquesLibres : %d \n", SB.cantBloquesLibres);      //Print clarificatiu
  printf("cantInodosLibres: %d \n", SB.cantInodosLibres);         //Print clarificatiu
  printf("totBloques: %d \n", SB.totBloques);                     //Print clarificatiu
  printf("totInodos: %d \n\n", SB.totInodos);                     //Print clarificatiu

  /* 
  mostrar_buscar_entrada("pruebas/", 1); //ERROR_CAMINO_INCORRECTO
  mostrar_buscar_entrada("/pruebas/", 0); //ERROR_NO_EXISTE_ENTRADA_CONSULTA
  mostrar_buscar_entrada("/pruebas/docs/", 1); //ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO
  mostrar_buscar_entrada("/pruebas/", 1); // creamos /pruebas/
  mostrar_buscar_entrada("/pruebas/docs/", 1); //creamos /pruebas/docs/
  mostrar_buscar_entrada("/pruebas/docs/doc1", 1); //creamos /pruebas/docs/doc1
  mostrar_buscar_entrada("/pruebas/docs/doc1/doc11", 1); //ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO
  mostrar_buscar_entrada("/pruebas/", 1); //ERROR_ENTRADA_YA_EXISTENTE
  mostrar_buscar_entrada("/pruebas/docs/doc1", 0); //consultamos /pruebas/docs/doc1
  mostrar_buscar_entrada("/pruebas/docs/doc1", 1); //creamos /pruebas/docs/doc1
  mostrar_buscar_entrada("/pruebas/casos/", 1); //creamos /pruebas/casos/
  mostrar_buscar_entrada("/pruebas/docs/doc2", 1); //creamos /pruebas/docs/doc2
  */

  //Tancam l'enllaç amb el dispositiu virutal.
  if (bumount() < 0)
  {
    //Control d'erros.
    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
  }
}