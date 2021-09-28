//
// Created by Phoenix Wang on 2021/9/25.
//

#include "simple_tar.h"


#define MAX(x, y) (((x) > (y)) ? (x) : (y))

std::vector<std::string> split(const std::string &str, const std::string &delim) {
    std::vector<std::string> result;
    if (str == "") {
        return result;
    }
    char* strs = new char[str.length() + 1];
    strcpy(strs, str.c_str());

    char* d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());

    char* p = strtok(strs, d);
    while(p) {
        std::string s = p;
        result.push_back(s);
        p = strtok(NULL, d);
    }

    return result;
}

int simple_tar::toTar(const char * filename, const char ** files, int move) {
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

int simple_tar::toExtract(const char * filename, const char * extract_tgt_dir) {
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
    if (tar_extract(fd, archive, verbosity, extract_tgt_dir) < 0)
        rc = -1;

    return rc;
}



int simple_tar::tar(std::string file_path, std::string package_name) {
    const char * src_dir = file_path.c_str();
    const char * filename = package_name.c_str();
    const char ** files = (const char **) &src_dir;

    int rc = 0;

    int move = 0;
    size_t len = strlen(src_dir);
    size_t len_redundant = len;
    char * path = (char*) calloc(len + 1, sizeof(char));
    strncpy(path, src_dir, len);

    while (--len_redundant && (path[len_redundant] != '/'));
    move = len_redundant + 1;

    rc = toTar(filename, files, move);

    return rc;
}

int simple_tar::untar(std::string file_path, std::string package_name) {
    const char * extract_tgt_dir = file_path.c_str();  // need to add '/' at last
    const char * filename = package_name.c_str();

    int rc = 0;

    rc = toExtract(filename, extract_tgt_dir);

    return rc;
}

// encrpt, decrypt are added by Li Xinhao
int simple_tar::encrypt(const char* file, long long key)
{
    char c;

    // Input stream
    fstream fin, fout;

    string ori_file(file);

    vector<string> path_vector = split(ori_file, "/");
    string ori_file_name = path_vector[path_vector.size() - 1];

    path_vector = split(ori_file_name, ".");
    ori_file_name = path_vector[0];

    // Open input file
    // ios::binary- reading file
    // character by character
    fin.open(file, fstream::in);

    fout.open(("./pack_temp/" + ori_file_name + ".enc").c_str(), fstream::out);

    // Reading original file till
    // end of file
    while (fin >> noskipws >> c) {
        int temp = (c + key);

        // Write temp as char in
        // output file
        fout << (char)temp;
    }

    // Closing both files
    fin.close();
    fout.close();

    return 0;

}

int simple_tar::decrypt(const char* file, long long key)
{
    char c;

    fstream fin;
    fstream fout;

    string enc_file(file);

    vector<string> path_vector = split(enc_file, "/");
    string enc_file_name = path_vector[path_vector.size() - 1];

    path_vector = split(enc_file_name, ".");
    enc_file_name = path_vector[0];

    fin.open(enc_file.c_str(), fstream::in);

    fout.open(("./unpack_temp/" + enc_file_name + ".dec").c_str(), fstream::out);

    while (fin >> noskipws >> c) {

        // Remove the key from the
        // character
        int temp = (c - key);
        fout << (char)temp;
    }

    fin.close();
    fout.close();

    return 0;
}

int simple_tar::package(std::string file_path, std::string package_path, long long key) {
    if (file_path.at(file_path.length() - 1) == '/') {
        file_path = file_path.substr(0, file_path.length() - 1);
    }
    if (package_path.at(package_path.length() - 1) != '/') {
        package_path += '/';
    }
    std::vector<std::string> paths = split(file_path, "/");
    std::string full_name = paths[paths.size() - 1];
    std::string name = split(full_name, ".")[0];

    // ?/xxx.xx -> ./xxx.tt
    int tar_flag = tar(file_path, "./pack_temp/" + name + ".tt");  // name --> .tt
    if (tar_flag < 0) {
        return -tar_flag;
    }

    huffmanEncode hfEncode; // huffman压缩类
    // huffman压缩函数： 参数1-要压缩的文件，参数2-压缩生成的文件
    // ./xxx.tt -> ./xxx.cps
    bool encode_flag = hfEncode.encode(("./pack_temp/" + name + ".tt").c_str(),
                                       ("./pack_temp/" + name + ".cps").c_str());  // .tt --> .cps
    if (encode_flag == false) {
        return 101;
    }

    encrypt(("./pack_temp/" + name + ".cps").c_str(), key);  // .cps --> .enc

    // ./xxx.cps -> ?/xxx.pkg
    copy_tool cp(("./pack_temp/" + name + ".enc").c_str(),
                 (package_path + name + ".pkg").c_str());  // .enc --> .pkg
    int cp_flag = cp.copy();
    if (cp_flag < 0) {
        return 200 + cp_flag;
    }

    return 0;
}

int simple_tar::unpackage(std::string package_path, std::string target_path, long long key) {
    if (package_path.at(package_path.length() - 1) == '/') {
        package_path = package_path.substr(0, package_path.length() - 1);
    }
    if (target_path.at(target_path.length() - 1) != '/') {
        target_path += '/';
    }

    std::vector<std::string> paths = split(package_path, "/");
    std::string full_name = paths[paths.size() - 1];
    std::vector<std::string> name_vec = split(full_name, ".");
    if (name_vec.size() < 0){
        return 102;
    }
    std::string name = name_vec[0];
    std::string type = name_vec[name_vec.size() - 1];

    // ?/xxx.pkg -> ./xxx.cps
    if (type == "pkg") {
        copy_tool cp(package_path.c_str(), ("./unpack_temp/" + name + "2.enc").c_str());  // pkg --> enc
        int cp_flag = cp.copy();
        if (cp_flag < 0) {
            return 200 + cp_flag;
        }
    }

    decrypt(("./unpack_temp/" + name + "2.enc").c_str(), key);  // enc --> dec

    huffmanDecode hfDecode; // huffman解压缩类
    // huffman解压缩函数： 参数1-要解压缩的文件，参数2-解压缩还原的文件
    // ./xxx.cps -> ./xxx.tt
    bool decode_flag = hfDecode.decode(("./unpack_temp/" + name + "2.dec").c_str(),
                                       ("./unpack_temp/" + name + "2.tt").c_str());  // dec --> tt
    if (decode_flag == false) {
        return 101;
    }

    // ./xxx.tt -> ?/xxx.xx
    int untar_flag = untar(target_path, "./unpack_temp/" + name + "2.tt");  // tt --> untar(how to recover .txt?)
    if (untar_flag < 0) {
        return - untar_flag;
    }

    return 0;
}
