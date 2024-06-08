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

    uint16_t diskBlockNum = inode_indexlookup(fs, &in, blockNum);
    if (diskBlockNum < 0) {
        return -1;  // Error looking up block index
    }

    if (diskimg_readsector(fs->dfd, diskBlockNum, buf) < 0) {
        return -1;  // Error reading block
    }

    uint64_t fileSize = inode_getsize(&in);
    uint64_t completeBlocks = fileSize / DISKIMG_SECTOR_SIZE;
    uint64_t lastBlockSize = fileSize % DISKIMG_SECTOR_SIZE;

    if (blockNum < completeBlocks) {
        return DISKIMG_SECTOR_SIZE;
    } else if (blockNum == completeBlocks) {
        return lastBlockSize;
    } else {
        return 0;
    }
}

