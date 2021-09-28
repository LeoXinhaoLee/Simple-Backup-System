//
// Created by Phoenix Wang on 2021/9/20.
//

#include "diff.h"

diff::diff(const char * path1, const char * path2) {
    path_1 = path1;
    path_2 = path2;
}

int diff::dif() {
    file_1 = open(path_1.c_str(), O_RDONLY);
    if (file_1 == -1) {
        return -1;
    }

    file_2 = open(path_2.c_str(), O_RDONLY);
    if (file_2 == -1) {
        return -2;
    }

    ssize_t read_size1, read_size2;
    memset(buffer1, 0, sizeof(buffer1));
    memset(buffer2, 0, sizeof(buffer2));
    while(1) {
        read_size1 = read(file_1, buffer1, buffer_size);
        read_size2 = read(file_2, buffer2, buffer_size);

        if (read_size1 == read_size2 && read_size1 <= 0) {
            break;
        }

        int same_flag = strcmp(buffer1, buffer2);
        if (same_flag != 0) {
            return 1;
        }
    }

    if (read_size1 == -1) {
        return -3;
    }

    if (read_size2 == -1) {
        return -4;
    }

    if (close(file_1) == -1 || close(file_2) == -1) {
        return -5;
    }

    return 0;
}