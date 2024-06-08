#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

/**
 * TODO
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode in;
    if (inode_iget(fs, inumber, &in) < 0) {
        return -1;  // Caso de error al obtener el inodo
    }

    int16_t diskBlockNum = inode_indexlookup(fs, &in, blockNum);
    
    if (diskBlockNum < 0) {
        return -1;  // caso de error al obtener el bloque
    }

    if (diskimg_readsector(fs->dfd, diskBlockNum, buf) < 0) {
        return -1;  // caso de error al leer el bloque
    }

    int64_t fileSize = inode_getsize(&in); // obtengo el tamaño del archivo
    int64_t completeBlocks = fileSize / DISKIMG_SECTOR_SIZE;  // obtengo la cantidad de bloques completos
    int64_t lastBlockSize = fileSize % DISKIMG_SECTOR_SIZE;  // obtengo lo que es valido en el ultimo bloque


    if (blockNum < completeBlocks) { // si el bloque es menor a la cantidad de bloques completos
        return DISKIMG_SECTOR_SIZE;
    } else if (blockNum == completeBlocks) { // si el bloque es igual a la cantidad de bloques completos
        return lastBlockSize;
    } else {
        return 0; // si el bloque es mayor a la cantidad de bloques completos
    }
}

