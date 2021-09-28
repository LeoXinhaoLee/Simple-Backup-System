#ifndef ZNFS_LIB_LIBRARY_H
#define ZNFS_LIB_LIBRARY_H
#ifdef __cplusplus
extern "C" {
#endif


int localPackUnpack(const wchar_t *src_path, const wchar_t *tgt_path);

const wchar_t *localCompareDiff(const wchar_t *src_path, const wchar_t *tgt_path);

int logIn(const wchar_t *password);

int upLoad(const wchar_t *src_path, const wchar_t *password, const wchar_t *key);

const wchar_t *showAll(const wchar_t *password);

int downLoad(const wchar_t *src_path, const wchar_t *tgt_path, const wchar_t *password, const wchar_t *key);

#ifdef __cplusplus
}
#endif
#endif //ZNFS_LIB_LIBRARY_H
