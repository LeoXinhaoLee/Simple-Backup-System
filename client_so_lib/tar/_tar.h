#include <iostream>
#include <string>
#include "tar.h"
using namespace std;

#define MAX(x, y) (((x) > (y)) ? (x) : (y))


int toTar(const char * filename, const char ** files, int move){
    char verbosity = 0;

    int rc = 0;
    struct tar_t * archive = NULL;
    int fd = -1;

    if ((fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR)) == -1){
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        return -1;
    }

    if (tar_write(fd, &archive, 1, files, verbosity, move) < 0){
        rc = -1;
    }

    return rc;
}

int toExtract(const char * filename, const char ** files, const char * extract_tgt_dir){
    char verbosity = 0;

    int rc = 0;
    struct tar_t * archive = NULL;
    int fd = -1;

    if ((fd = open(filename, O_RDWR)) < 0){
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        return -1;
    }

    // read in data from fd(tgt_pack) -> archive struct list
    if (tar_read(fd, &archive, verbosity) < 0){
        tar_free(archive);
        close(fd);
        return -1;
    }

    // write data from archive strut list -> extrat_tgt_dir
    if (tar_extract(fd, archive, 0, files, verbosity, extract_tgt_dir) < 0)
        rc = -1;

    return rc;
}


int test_tar(int flag) {
    const char * src_dir = "../test";
    const char * extract_tgt_dir = "../test/";  // need to add '/' at last
    const char * filename = "test.star";
    const char ** files = (const char **) &src_dir;

    int rc = 0;

    if (flag == 0) {
        int move = 0;
        size_t len = strlen(src_dir);
        size_t len_redundant = len;
        char * path = (char*) calloc(len + 1, sizeof(char));
        strncpy(path, src_dir, len);

        while (--len_redundant && (path[len_redundant] != '/'));
        move = len_redundant + 1;

        rc = toTar(filename, files, move);
    }
    else
        rc = toExtract(filename, files, extract_tgt_dir);

    return rc;
}
