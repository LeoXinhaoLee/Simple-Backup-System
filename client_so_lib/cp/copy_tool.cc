//
// Created by Phoenix Wang on 2021/9/11.
//

#include "copy_tool.h"

copy_tool::copy_tool(const char * open_path, const char * create_path){
    path_input = open_path;
    stat_input = lstat(open_path, &stat_buf_input);
    path_output = create_path;
}

int copy_tool::content_copy() {
    file_input = open(path_input.c_str(), O_RDONLY);
    if (file_input == -1) {
        return -1;
    }

    file_output = creat(path_output.c_str(), 0644);
    if (file_output == -1) {
        return -2;
    }

    ssize_t read_size;
    while((read_size = read(file_input, buffer, buffer_size)) > 0) {
        if (write(file_output, buffer, read_size) != read_size) {
            return -4;
        }
    }

    if (read_size == -1) {
        return -3;
    }

    if (close(file_input) == -1 || close(file_output) == -1) {
        return -5;
    }

    return 0;
}

int copy_tool::check_stat() {
    if (stat_input == -1) {
        return -1;
    }

    switch (stat_buf_input.st_mode & S_IFMT) {
        case S_IFREG:
            return 0;
        case S_IFLNK:
            return 1;
        case S_IFIFO:
            return 2;
        case S_IFDIR:
            return -3;
    }

    return -2;
}

int copy_tool::write_stat() {
    int chown_flag = chown(path_output.c_str(),
                           stat_buf_input.st_uid,
                           stat_buf_input.st_gid);
    if (chown_flag == -1) {
        return -1;
    }

    int chmod_flag = chmod(path_output.c_str(), stat_buf_input.st_mode);
    if (chmod_flag == -1) {
        return -2;
    }

    struct timeval time_input[2];
    TIMESPEC_TO_TIMEVAL(&time_input[0],&stat_buf_input.st_atim);
    TIMESPEC_TO_TIMEVAL(&time_input[1],&stat_buf_input.st_mtim);

    int utime_flag = lutimes(path_output.c_str(), time_input);
    if (utime_flag == -1) {
        return -3;
    }

    return 0;
}
//
//int copy_tool::stat() {
//    int chown_flag = stat_chown(path_output.c_str(),
//                           stat_buf_input.st_uid,
//                           stat_buf_input.st_gid);
//    if (chown_flag < 0) {
//        return chown_flag;
//    }
//
//    int chmod_flag = stat_chmod(path_output.c_str(), stat_buf_input.st_mode);
//    if (chmod_flag < 0) {
//        return chmod_flag;
//    }
//
//
//    int utime_flag = stat_mtime(path_output.c_str(), stat_buf_input.st_mtime);
//    if (utime_flag < 0) {
//        return utime_flag;
//    }
//
//    return 0;
//}
//
//int copy_tool::stat_chown(const char* path, uid_t uid, gid_t gid) {
//    int chown_flag = chown(path, uid, gid);
//
//    if (chown_flag == -1) {
//        return -1;
//    }
//
//    return 0;
//}
//
//int copy_tool::stat_chmod(const char* path, mode_t mode) {
//    int chmod_flag = chmod(path, mode);
//
//    if (chmod_flag == -1) {
//        return -2;
//    }
//
//    return 0;
//}
//
//int copy_tool::stat_mtime(const char* path, time_t mtime) {
//    struct stat stat_buf;
//    int stat_flag = lstat(path, &stat_buf);
//    if (stat_flag == -1) {
//        return -4;
//    }
//
//    struct timeval time_input[2];
//    TIMESPEC_TO_TIMEVAL(&time_input[0],&stat_buf.st_atime);
//    TIMESPEC_TO_TIMEVAL(&time_input[1],&mtime);
//
//    int utime_flag = lutimes(path, time_input);
//    if (utime_flag == -1) {
//        return -3;
//    }
//
//    return 0;
//}

int copy_tool::copy() {
    int check = check_stat();
    if (check < 0) {
        return - check;
    }

    // normal file
    if (check == 0) {
        int context = content_copy();
        if (context < 0) {
            return 100 - context;
        }

        int write_stat_flag = write_stat();
        if (write_stat_flag < 0) {
            return  200 - write_stat_flag;
        }

        return 0;
    }

    // soft link
    if (check == 1) {
        char link_buffer[4096];
        int read_flag = readlink(path_input.c_str(), link_buffer, 4095);
        if (read_flag < 0) {
            // read link error
            return 130;
        }
        if (read_flag >= 4095) {
            // buffer overflow
            return 131;
        }

        std::string path_link;
        for (int i = 0; i < read_flag; i++) {
            path_link += link_buffer[i];
        }

        int symlink_flag = symlink(path_link.c_str(), path_output.c_str());
        if (symlink_flag == -1) {
            // fails to create symbol link
            return 132;
        }

        int write_stat_flag = write_stat();
        if (write_stat_flag < 0) {
            return  230;
        }

        return 0;
    }

    // pipe
    if (check == 2) {
        int fifo_flag = mkfifo(path_output.c_str(), stat_buf_input.st_mode);
        if (fifo_flag == -1) {
            // make fifo error
            return 150;
        }

        int write_stat_flag = write_stat();
        if (write_stat_flag < 0) {
            return  250;
        }

        return 0;
    }

    return 300;
}
