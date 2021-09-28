// dir node
//
// Created by Phoenix Wang on 2021/9/13.
//
// This class copy a certain file or a dir to a certain address
// Example:
// dir_node dn("../test/text","../test/cp_text");
// dn.copy();
//

#ifndef ZNFS_DIR_NODE_H
#define ZNFS_DIR_NODE_H

#include "copy_tool.h"
#include "diff.h"
#include <dirent.h>
#include <vector>

class dir_node {
private:
    copy_tool* this_node;
    diff* this_df;
    int type;
    std::string root_input, root_output;
    std::vector<dir_node*> children;

public:
    dir_node(const char * open_path, const char * create_path);

    // if fails to build tree, return > 400;
    // other error is same with copy_tool.copy();
    // if success, return 0.
    int copy();

    // compares 2 dir,
    // if cannot open dir 1, return -1;
    // if cannot open dir 2, return -2;
    // if success, prints results and return 0.
    std::string diff_tree(int depth);

private:
    // build the tree of chosen dir，
    // if the file is not a dir, return -1;
    // if new dir exist and not replaceable, return -2;
    // if fails to make new dir, return -3;
    // if fails to write stat, return -4;
    // if fails to open chosen dir, return -5;
    // id success, return 0.
    int build_tree();

    // writen for test,
    // display the tree built.
    int display();

    // a recurrence method to copy files in the dir,
    // if fails, return error according to copy_tool.copy();
    // if success, return 0.
    int copy_file();

    // write the stat to the tree of chosen dir，
    // if the file is not a dir, return -1;
    // if fails to open chosen dir, return -2;
    // if fails to write stat, return -3;
    // id success, return 0.
    int stat_tree();
};

#endif //ZNFS_DIR_NODE_H
