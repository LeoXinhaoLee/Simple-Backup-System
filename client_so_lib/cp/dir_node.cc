//
// Created by Phoenix Wang on 2021/9/13.
//

#include "dir_node.h"

dir_node::dir_node(const char * open_path, const char * create_path) {
    root_input = open_path;
    root_output = create_path;
    this_node = new copy_tool(open_path, create_path);
    this_df = new diff(open_path, create_path);
    type = this_node->check_stat();
    copy_tool* cp2 = new copy_tool(create_path, open_path);
    type2 = cp2->check_stat();

}

int dir_node::build_tree() {
    if(type != -3) {
        return -1;
    }

    int rm_flag = rmdir(root_output.c_str());
    int mkdir_flag = mkdir(root_output.c_str(), this_node->stat_buf_input.st_mode);
    if (mkdir_flag == -1) {
        if (rm_flag == -1){
            return -2;
        }
        return -3;
    }

    DIR* open_flag = opendir(root_input.c_str());
    if (open_flag == NULL) {
        return -5;
    }

    std::vector<std::string> names;
    while (1) {
        struct dirent* read_dir = readdir(open_flag);
        if (read_dir == NULL) {
            break;
        }
        if ( strcmp(read_dir->d_name,".")==0 || strcmp(read_dir->d_name,"..")==0 ) {
            continue;
        }
        names.push_back(read_dir->d_name);
    }

    for (int i = 0; i < names.size(); i++) {
        std::string new_root_input, new_root_output;
        if (root_input[root_input.size() - 1] == '/') {
            new_root_input = root_input + names[i];
        }
        else {
            new_root_input = root_input + "/" + names[i];
        }
        if (root_output[root_output.size() - 1] == '/') {
            new_root_output = root_output + names[i];
        }
        else {
            new_root_output = root_output + "/" + names[i];
        }

        struct dir_node* new_node = new dir_node(new_root_input.c_str(), new_root_output.c_str());
        new_node->build_tree();
        children.push_back(new_node);

        int write_flag = this_node->write_stat();
        if (write_flag < 0) {
            return -4;
        }
    }

    return 0;
}

int dir_node::display() {
    for (int i = 0; i < children.size(); i++) {
        if (i == 0) {
            std::cout<<children.size()<<std::endl;
        }
        children[i]->display();
    }
    std::cout<<root_output<<std::endl;
    return 0;
}

int dir_node::copy_file() {
    if (type == 3) {
        return 0;
    }

    int copy_flag = this_node->copy();
    if (copy_flag != 0 && copy_flag != 3){
        return copy_flag;
    }

    for (int i = 0; i < children.size(); i++) {
        children[i]->copy_file();
    }

    return 0;
}

int dir_node::stat_tree() {
    if(type != -3) {
        return -1;
    }

    DIR* open_flag = opendir(root_input.c_str());
    if (open_flag == NULL) {
        return -2;
    }

    std::vector<std::string> names;
    while (1) {
        struct dirent* read_dir = readdir(open_flag);
        if (read_dir == NULL) {
            break;
        }
        if ( strcmp(read_dir->d_name,".")==0 || strcmp(read_dir->d_name,"..")==0 ) {
            continue;
        }
        names.push_back(read_dir->d_name);
    }

    for (int i = 0; i < names.size(); i++) {
        std::string new_root_input, new_root_output;
        if (root_input[root_input.size() - 1] == '/') {
            new_root_input = root_input + names[i];
        }
        else {
            new_root_input = root_input + "/" + names[i];
        }
        if (root_output[root_output.size() - 1] == '/') {
            new_root_output = root_output + names[i];
        }
        else {
            new_root_output = root_output + "/" + names[i];
        }

        struct dir_node* new_node = new dir_node(new_root_input.c_str(), new_root_output.c_str());
        new_node->stat_tree();
        children.push_back(new_node);

        int write_flag = this_node->write_stat();
        if (write_flag < 0) {
            return -3;
        }
    }

    return 0;
}

int dir_node::diff_tree(std::string &in_file_1, std::string &in_file_2, std::string &diff1, std::string &diff2) {
    if (type == 2) {
        return 0;
    }

    if(type != -3 && type2 != -3) {
        int ret = this_df->dif();
        if (ret == -1) {
            in_file_2 += root_output + "\n";
        }
        else if (ret == -2) {
            in_file_1 += root_input + "\n";
        }
        else if (ret == 1) {
            diff1 += root_input  + "\n";
            diff2 += root_output + "\n";
        }
        return 0;
    }

    DIR* open_flag1 = opendir(root_input.c_str());
    DIR* open_flag2 = opendir(root_output.c_str());
    if (open_flag1 == NULL) {
        in_file_2 += root_output + "\n";
    }
    else {
        std::vector<std::string> names1;
        while (1) {
            struct dirent* read_dir = readdir(open_flag1);
            if (read_dir == NULL) {
                break;
            }
            if ( strcmp(read_dir->d_name,".")==0 || strcmp(read_dir->d_name,"..")==0 ) {
                continue;
            }
            names1.push_back(read_dir->d_name);
        }

        for (int i = 0; i < names1.size(); i++) {
            std::string new_root_input, new_root_output;
            if (root_input[root_input.size() - 1] == '/') {
                new_root_input = root_input + names1[i];
            }
            else {
                new_root_input = root_input + "/" + names1[i];
            }
            if (root_output[root_output.size() - 1] == '/') {
                new_root_output = root_output + names1[i];
            }
            else {
                new_root_output = root_output + "/" + names1[i];
            }

            struct dir_node* new_node = new dir_node(new_root_input.c_str(), new_root_output.c_str());
            new_node->diff_tree(in_file_1, in_file_2, diff1, diff2);
            children.push_back(new_node);
        }
    }

    if (open_flag2 == NULL) {
        in_file_1 += root_input + "\n";
        return -2;
    }
    else {
        std::vector<std::string> names2;
        while (1) {
            struct dirent* read_dir = readdir(open_flag2);
            if (read_dir == NULL) {
                break;
            }
            if ( strcmp(read_dir->d_name,".")==0 || strcmp(read_dir->d_name,"..")==0 ) {
                continue;
            }
            names2.push_back(read_dir->d_name);
        }

        for (int i = 0; i < names2.size(); i++) {
            std::string new_root_input, new_root_output;
            if (root_input[root_input.size() - 1] == '/') {
                new_root_input = root_input + names2[i];
            }
            else {
                new_root_input = root_input + "/" + names2[i];
            }
            if (root_output[root_output.size() - 1] == '/') {
                new_root_output = root_output + names2[i];
            }
            else {
                new_root_output = root_output + "/" + names2[i];
            }

            struct dir_node* new_node = new dir_node(new_root_input.c_str(), new_root_output.c_str());
            if (new_node->type != -3 && new_node->type != 2) {
                if (new_node->this_df->dif() == 1){
                    continue;
                }
            }
            else if(new_node->type == -3) {
                DIR* open_dir_flag1 = opendir(root_input.c_str());
                DIR* open_dir_flag2 = opendir(root_output.c_str());
                if (open_dir_flag1 != NULL && open_dir_flag2 != NULL) {
                    continue;
                }
            }
            new_node->diff_tree(in_file_1, in_file_2, diff1, diff2);
            children.push_back(new_node);
        }
    }

    return 0;
}

int dir_node::copy() {
    int build_flag = build_tree();
    if (build_flag < -1) {
        return 400 - build_flag;
    }

    int copy_flag = copy_file();
    if (copy_flag != 0) {
        return copy_flag;
    }

    int stat_flag = stat_tree();
    if (stat_flag < -1) {
        return 410 - build_flag;
    }

    return 0;
}

std::string dir_node::dir_diff() {
    std::string in_file_1;
    std::string in_file_2;
    std::string diff1, diff2;

    diff_tree(in_file_1, in_file_2, diff1, diff2);

    if (in_file_1 == "") in_file_1 = "[EMP]";
    if (in_file_2 == "") in_file_2 = "[EMP]";
    if (diff1 == "") diff1 = "[EMP]";
    if (diff2 == "") diff2 = "[EMP]";

    std::string output = in_file_1 + "[SEP]" + in_file_2 + "[SEP]" + diff1 + "[SEP]" + diff2;

    return output;
}