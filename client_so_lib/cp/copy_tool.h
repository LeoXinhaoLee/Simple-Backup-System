// copy_tool
//
// Created by Phoenix Wang on 2021/9/11.
//
// This class copy a certain file to a certain address
// Example:
// copy_tool cp("../test/text","../test/cp_text");
// cp.copy();
//

#ifndef ZNFS_COPY_TOOL_H
#define ZNFS_COPY_TOOL_H

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <utime.h>
#include <string>

class copy_tool {
public:
    struct stat stat_buf_input;
// private:
    int file_input, file_output, stat_input;
    std::string path_input, path_output;
    const int buffer_size = 4096;  // 4KB
    unsigned char buffer[4096];

public:
    copy_tool(const char * open_path, const char * create_path);

    // copy normal file,
    // if fails to read stat, return < 100;
    // else if fails to copy the context, return < 200;
    // else if fails to copy the stat, return <300;
    // if unknown error appears, return 300;
    // if success, return 0.
    int copy();

    // checks and returns for stat_copy().
    // if fails to read the stat of the file, return -1;
    // if the stat is unable to process, return -2;
    // if the stat is 'd', return -3;
    // if the stat is '-', return 0;
    // if the stat is 'l', return 1;
    // if the stat is 'p', return 2.
    int check_stat();

    // copy context of a file,
    // if the file is not found, return -1;
    // if failed to create a new file, return -2;
    // if read error, return -3;
    // if write error, return -4;
    // if close error, return -5;
    // if success, return 0.
    int content_copy();

    // write stat into the created file by utime(),
    // if fails to modify uid and gid, return -1;
    // if fails to modify modes, return -2;
    // if fails to modify utime, return -3;
    // if success, return 0.
    int write_stat();

    // write stat into the created file by utimes(),
    // error same with write_stat();
    // not finished
    int stat();
    int stat_chown(const char* path, uid_t uid, gid_t gid);
    int stat_chmod(const char* path, mode_t mode);
    int stat_mtime(const char* path, time_t mtime);
};


#endif //ZNFS_COPY_TOOL_H
