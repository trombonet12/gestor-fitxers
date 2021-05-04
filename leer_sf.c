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

    printf("El primer bloque de MB és: %d \n", SB.posPrimerBloqueMB);                        //Print clarificatiu
    printf("El darrer bloc del MB és: %d \n", SB.posUltimoBloqueMB);                         //Print clarificatiu
    printf("El primer bloc de l'array inodos és: %d \n", SB.posPrimerBloqueAI);              //Print clarificatiu
    printf("El darrer bloc de l'array inodos és: %d \n", SB.posUltimoBloqueAI);              //Print clarificatiu
    printf("El primer bloc de Datos és: %d \n", SB.posPrimerBloqueDatos);                    //Print clarificatiu
    printf("El darrer bloc de Datoes és: %d \n", SB.posUltimoBloqueDatos);                   //Print clarificatiu
    printf("La posició del inode arrel en l'array inodes és: %d \n", SB.posInodoRaiz);       //Print clarificatiu
    printf("El primer inode lliure dins l'arrar inodes és: %d \n", SB.posPrimerInodoLibre);  //Print clarificatiu
    printf("Quantitat de blocs lliures : %d \n", SB.cantBloquesLibres);                      //Print clarificatiu
    printf("Quantitat de inodes lliures al array de inodes és: %d \n", SB.cantInodosLibres); //Print clarificatiu
    printf("Quantitat de blocs totals és: %d \n", SB.totBloques);                            //Print clarificatiu
    printf("Quantitat de inodes és: %d \n\n", SB.totInodos);                                 //Print clarificatiu

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