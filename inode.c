#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"

// #define NINDIRECT (DISKIMG_SECTOR_SIZE / sizeof(int))
#define MAXFILEBLOCKS (DISKIMG_SECTOR_SIZE / sizeof(int))

/**
 * TODO
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    int inodes_per_block = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
    int block_num = (inumber - 1) / inodes_per_block;
    int offset = (inumber - 1) % inodes_per_block;

    struct inode block[inodes_per_block];

    if (diskimg_readsector(fs->dfd, INODE_START_SECTOR + block_num, block) < 0) {
        fprintf(stderr, "Error reading inode block\n");
        return -1;
    }

    *inp = block[offset];
    
    return 0;
}

/**
 * TODO
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inode, int blockNum) {

    int diskBlockNum = -1;

    if ( !(inode->i_mode & ILARG)) {  // caso de archivo pequeño -> 8 bloques directos
        return inode->i_addr[blockNum]; 
    } else {
        if (blockNum < MAXFILEBLOCKS * 7){ // caso de archivo grande -> 7 bloques directos + 1 indirecto
            int sector_offset = blockNum / MAXFILEBLOCKS;
            int address_offset = blockNum % MAXFILEBLOCKS;

            int indirect_block[MAXFILEBLOCKS];
            if (diskimg_readsector(fs->dfd, inode->i_addr[sector_offset], indirect_block) < 0) {
                fprintf(stderr, "Error reading indirect block\n");
                return -1;
            }

            diskBlockNum = indirect_block[address_offset];
        } else { 
            int double_indirect_block = blockNum - MAXFILEBLOCKS * 7;
            int sector1_offset = 7;
            int sector2_offset = double_indirect_block / MAXFILEBLOCKS;
            int address_offset = double_indirect_block % MAXFILEBLOCKS;

            int indirect_block[MAXFILEBLOCKS];
            if (diskimg_readsector(fs->dfd, inode->i_addr[sector1_offset], indirect_block) < 0) {
                fprintf(stderr, "Error reading indirect block\n");
                return -1;
            }

            if (diskimg_readsector(fs->dfd, indirect_block[sector2_offset], indirect_block) < 0) {
                fprintf(stderr, "Error reading indirect block\n");
                return -1;
            }

            diskBlockNum = indirect_block[address_offset];

        }
    }

    return diskBlockNum;
}


int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
