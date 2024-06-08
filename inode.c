#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"

#define MAXFILEBLOCKS (DISKIMG_SECTOR_SIZE / sizeof(uint16_t))

/**
 * TODO
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    int inodes_per_block = DISKIMG_SECTOR_SIZE / sizeof(struct inode);  // 16
    int block_num = (inumber - 1) / inodes_per_block;  
    int offset = (inumber - 1) % inodes_per_block;

    struct inode block[inodes_per_block]; // 16 inodes donde se guardan los inodos

    if (diskimg_readsector(fs->dfd, INODE_START_SECTOR + block_num, block) < 0) { // leo el bloque que contiene el inodo
        fprintf(stderr, "Error reading inode block\n"); 
        return -1;
    }

    *inp = block[offset]; // guardo el inodo en inp
    
    return 0;
}

/**
 * TODO
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inode, int blockNum) {

    int diskBlockNum = -1;

    if ((inode->i_mode & ILARG) == 0) {  // caso de archivo pequeño -> 8 bloques directos
        return inode->i_addr[blockNum]; 

    } else if ((inode->i_mode & ILARG) == ILARG){

        if (blockNum < (int)MAXFILEBLOCKS * 7){ // caso de archivo grande -> 7 bloques directos + 1 indirecto
            // printf("indirect block\n");
            uint16_t sector_offset = blockNum / MAXFILEBLOCKS;
            uint16_t address_offset = blockNum % MAXFILEBLOCKS;

            uint16_t indirect_block[MAXFILEBLOCKS];
            if (diskimg_readsector(fs->dfd, inode->i_addr[sector_offset], indirect_block) < 0) {
                fprintf(stderr, "Error reading indirect block\n");
                return -1;
            }

            diskBlockNum = indirect_block[address_offset];  //guardo el bloque que quiero leer

        } else { 
            // printf("double indirect block\n");
            uint16_t double_indirect_block = blockNum - MAXFILEBLOCKS * 7;
            uint16_t sector1_offset = 7;
            uint16_t sector2_offset = double_indirect_block / MAXFILEBLOCKS;
            uint16_t address_offset = blockNum % MAXFILEBLOCKS;

            uint16_t first_block[DISKIMG_SECTOR_SIZE]; //aca guardo el primer bloque indirecto

            if (diskimg_readsector(fs->dfd, inode->i_addr[sector1_offset], first_block) < 0) { //leo el primer bloque indirecto
                fprintf(stderr, "Error reading indirect block\n");
                return -1;
            }

            uint16_t indirect_block[MAXFILEBLOCKS]; //aca guardo el segundo bloque indirecto
            if (diskimg_readsector(fs->dfd, first_block[sector2_offset], indirect_block) < 0) { //leo el segundo bloque indirecto
                fprintf(stderr, "Error reading indirect block\n");
                return -1;
            }

            diskBlockNum = indirect_block[address_offset];   //guardo el bloque que quiero leer    
        }
    }
    return diskBlockNum;
}


int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1);
}
