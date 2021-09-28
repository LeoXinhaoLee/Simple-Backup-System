// diff
//
// Created by Phoenix Wang on 2021/9/20.
//
// This class compare 2 certain files, then reports difference
// Example:
// diff df("../test/text","../test/cp_text");
// df.dif();
//

#ifndef ZNFS_DIFF_H
#define ZNFS_DIFF_H

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <ctime>
#include <string>
#include <cstring>

class diff {
// private:
    int file_1, file_2;
    std::string path_1, path_2;
    const int buffer_size = 4096;  // 4KB
    char buffer1[4096], buffer2[4096];

public:
    diff(const char * path1, const char * path2);

    // compare two files,
    // if fails to open file 1, return -1;
    // if fails to open file 2, return -2;
    // if fails to read file 1, return -3;
    // if fails to read file 2, return -4;
    // if fails to close files, return -5;
    // if they are the same, return 0;
    // if they are different, return 1.
    int dif();
};

#endif //ZNFS_DIFF_H
