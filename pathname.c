
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {

    if (pathname == NULL) {
        return -1;
    }

    //Si el pathname no empieza con '/', entonces devuelvo error
    if (pathname[0] != '/') {
        return -1;
    }

    //Si el pathname es solo '/', entonces devuelvo el inodo raiz
    if (strcmp(pathname, "/") == 0) {
        return ROOT_INUMBER;
    }

    //split del pathnam
    char *token = strtok(strdup(pathname), "/");
    int inumber = ROOT_INUMBER;

    //Mientras haya tokens
    while (token != NULL) {
        struct direntv6 dirEnt;
        if (directory_findname(fs, token, inumber, &dirEnt) < 0) {
            printf("Fallo en la busqueda del nombre %s en el inodo %d\n", token, inumber);
            return -1;
        }
        inumber = dirEnt.d_inumber;
        token = strtok(NULL, "/");
    }

    return inumber;
}


