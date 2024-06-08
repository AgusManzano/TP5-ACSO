#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>


#define MAX_DIRENTS (DISKIMG_SECTOR_SIZE/sizeof(struct direntv6))



/**
 * TODO
 */
int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {

  if (dirinumber < 0) {
    printf("El inodo es negativo\n");
    return -1;
  }

  struct inode in;
  if (inode_iget(fs, dirinumber, &in) < 0) {
    printf("No se pudo obtener el inodo %d\n", dirinumber);
    return -1;
  }

  if ((in.i_mode & IFMT) != IFDIR) {
    printf("El inodo %d no es un directorio\n", dirinumber);
    return -1;
  }

  //Primero saco cuantos bloques ocupa el archivo
  int numBlocks = (inode_getsize(&in)-1) / DISKIMG_SECTOR_SIZE + 1;

  //Si el archivo ocupa mas de un bloque, entonces tengo que recorrer todos los bloques
  for (int i = 0; i < numBlocks; i++) {
    
    //me hago un buffer de direntv6's para guardar los direntv6's de un bloque
    struct direntv6 buffer[MAX_DIRENTS];
    if (file_getblock(fs, dirinumber, i, buffer) < 0) {
      printf("No se pudo obtener el bloque %d del inodo %d\n", i, dirinumber);
      return -1;
    }

    //Recorro todos los direntv6's del buffer
    for (int j = 0; j < MAX_DIRENTS; j++) {
      // printf("Comparando %s con %s\n", buffer[j].d_name, name);
      //Si el nombre del direntv6 es igual al nombre que busco, entonces lo copio en dirEnt
      if (strcmp(buffer[j].d_name, name) == 0) {
        *dirEnt = buffer[j];
        return 0;
      }
    }
  }
  return -1;
}
