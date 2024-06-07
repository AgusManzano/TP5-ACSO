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
        return -1;  // Error reading inode
    }

    int diskBlockNum = inode_indexlookup(fs, &in, blockNum);
    if (diskBlockNum < 0) {
        return -1;  // Error looking up block index
    }

    if (diskimg_readsector(fs->dfd, diskBlockNum, buf) < 0) {
        return -1;  // Error reading block
    }

    int fileSize = inode_getsize(&in);
    int completeBlocks = fileSize / DISKIMG_SECTOR_SIZE;
    int lastBlockSize = fileSize % DISKIMG_SECTOR_SIZE;

    if (blockNum < completeBlocks) {
        return DISKIMG_SECTOR_SIZE;
    } else if (blockNum == completeBlocks) {
        return lastBlockSize;
    } else {
        return 0;
    }
}

