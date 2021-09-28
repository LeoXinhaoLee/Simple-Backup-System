#include <iostream>
#include <vector>
#include "library.h"
#include "simple_tar.h"
#include "cp/dir_node.h"
#include "net/client.h"
using namespace std;

int localPackUnpack(const wchar_t *src_path, const wchar_t *tgt_path) {
    int rt;
    wstring ws_s(src_path);
    wstring ws_t(tgt_path);

    string str_src(ws_s.begin(), ws_s.end());
    string str_tgt(ws_t.begin(), ws_t.end());

    vector<string> paths = split(str_src, "/");
    string src_name = paths[paths.size() - 1];

    paths = split(src_name, ".");
    src_name = paths[0];

    simple_tar st;

    rt = st.package(str_src, "./package");
    if (rt != 0)
        return rt;
    rt = st.unpackage("./package/"+src_name+".pkg", str_tgt);

    return rt;
}

const wchar_t *localCompareDiff(const wchar_t *src_path, const wchar_t *tgt_path){
    wstring ws_s(src_path);
    wstring ws_t(tgt_path);

    string str_src(ws_s.begin(), ws_s.end());
    string str_tgt(ws_t.begin(), ws_t.end());

    dir_node dn(str_src.c_str(), str_tgt.c_str());
    //string diff_str = dn.diff_tree(1);
    string diff_str = dn.dir_diff();

    const size_t cSize = strlen(diff_str.c_str())+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, diff_str.c_str(), cSize);

    return wc;
}

int logIn(const wchar_t *password){
    wstring ws_password(password);
    string str_password(ws_password.begin(), ws_password.end());

    char pass[PASSWORD_SIZE];
    memset(pass, 0, PASSWORD_SIZE);
    strcpy(pass, str_password.c_str());

    char opera[] = "login";
    char src[] = "./package";
    char dest[] = "";

    int success= client(pass, opera, src, dest);

    return success;
}

int upLoad(const wchar_t *src_path, const wchar_t *password, const wchar_t *key){
    int success = 0;
    wstring ws_s(src_path);
    string str_src(ws_s.begin(), ws_s.end());

    wstring ws_key(key);
    string str_key(ws_key.begin(), ws_key.end());
    long long my_key = stoll(str_key, nullptr, 10);

    vector<string> paths = split(str_src, "/");
    string src_name = paths[paths.size() - 1];

    paths = split(src_name, ".");
    src_name = paths[0];

    simple_tar st;
    success = st.package(str_src, "./upload_package", my_key);
    if (success != 0)
        return success;

    // Upload to server
    wstring ws_password(password);
    string str_password(ws_password.begin(), ws_password.end());

    char pass[PASSWORD_SIZE];
    memset(pass, 0, PASSWORD_SIZE);
    strcpy(pass, str_password.c_str());

    char opera[] = "update";
    char src[FILE_SIZE];
    memset(src, 0, FILE_SIZE);
    strcpy(src, ("./upload_package/"+src_name+".pkg").c_str());    // encrpted package file to be uploaded
    cout << src << endl;

    char dest[] = "";

    success = client(pass, opera, src, dest);

    return success;

}

const wchar_t *showAll(const wchar_t *password){
    string ret;

    wstring ws_password(password);
    string str_password(ws_password.begin(), ws_password.end());

    char pass[PASSWORD_SIZE];
    memset(pass, 0, PASSWORD_SIZE);
    strcpy(pass, str_password.c_str());

    char opera[] = "show";
    char src[] = "./package";
    char dest[] = "";

    ret = clientShowALL(pass, opera, src, dest);

    const size_t cSize = strlen(ret.c_str())+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, ret.c_str(), cSize);

    return wc;

}


int downLoad(const wchar_t *src_path, const wchar_t *tgt_path, const wchar_t *password, const wchar_t *key){
    int success = 0;

    wstring ws_s(src_path);
    string str_src(ws_s.begin(), ws_s.end());

    wstring ws_t(tgt_path);
    string str_tgt(ws_t.begin(), ws_t.end());

    wstring ws_key(key);
    string str_key(ws_key.begin(), ws_key.end());
    long long my_key = stoll(str_key, nullptr, 10);

    wstring ws_password(password);
    string str_password(ws_password.begin(), ws_password.end());


    char pass[PASSWORD_SIZE];
    memset(pass, 0, PASSWORD_SIZE);
    strcpy(pass, str_password.c_str());

    char opera[] = "download";

    char src[FILE_SIZE];
    memset(src, 0, FILE_SIZE);
    strcpy(src, str_src.c_str());    // encrpted package file to be downloaded

    char dest[] = "./download_package";

    cout << src << endl << dest << endl;
    success = client(pass, opera, src, dest);

    if (success != 0)
        return success;

    simple_tar st;

    success = st.unpackage("./download_package/"+str_src, str_tgt, my_key);


    return success;
}

