#pragma once
#include "huffmanTree.h"
#include "compress.h"
#include "fstream"

using namespace std;
using unchar = unsigned char;
#define GET_BYTE(hValue,index) (((hValue)&(1<<((index)^7)))!=0) // hValue第index位为1返回1，否则返回0

class huffmanDecode{
public:
    huffmanDecode():_fHead(nullptr),_hTree(nullptr){
        _fHead=new fileHead();
    }
    ~huffmanDecode(){
        if(!_fHead) delete _fHead;
        if(!_hTree) delete _hTree;
    }
    // 解压缩函数
    bool decode(const char* srcFile,const char* destFile){
        if(!_getCharacterFreq(srcFile)) return false;

        _hTree=new huffmanTree(_hfMap);

        _hTree->huffmanCode(_codeMap);

        for(auto it:_codeMap){
            _decodeMap.insert(std::pair<std::string,unchar>(it.second,it.first));
        }

        return _decode(srcFile,destFile);
    }
private:
    fileHead *_fHead;
    huffmanTree *_hTree;
    map<unchar,int> _hfMap;
    map<unchar,std::string> _codeMap;
    map<std::string,unchar> _decodeMap;

    bool _getCharacterFreq(const char* filename){
        ifstream inFile(filename,ios::binary);
        if(!inFile){
            cout<<"read file failed! filename:"<<filename<<endl;
            return false;
        }

        inFile.read((char*)_fHead,sizeof(fileHead));
        if(!(_fHead->flag[0]=='e'&&_fHead->flag[1]=='v'
        &&_fHead->flag[2]=='e'&&_fHead->flag[3]=='n')){
            cout<<"not support this file format! filename:"<<filename<<endl;
            return false;
        }

        int chNum=static_cast<int>(_fHead->characterNum);
        if(chNum==0) chNum=256;
        for(int i=0;i < chNum;++i){
            characterFreq cf;
            inFile.read((char*)&cf,sizeof(characterFreq));
            _hfMap.insert(std::pair<char,int>(cf.chara,cf.freq));
        }
        inFile.close();
        return true;
    }

    long _getFileSize(const char* filename){
        ifstream inFile(filename);
        if(!inFile.is_open()) return 0;

        inFile.seekg(0,ios_base::end);
        streampos sp=inFile.tellg();
        inFile.close();
        return sp;
    }

    static bool _isLeaf(Node* n){
        return n->left==nullptr && n->right==nullptr;
    }

    bool _decode(const char* srcFile,const char* destFile){
        long fileSize = _getFileSize(srcFile);

        ifstream inFile(srcFile,ios::binary);
        if(!inFile){
            cout<<"read file failed! filename:"<<srcFile<<endl;
            return false;
        }
        int chNum=static_cast<int>(_fHead->characterNum);
        if(chNum==0) chNum=256;
        inFile.seekg(sizeof(fileHead)+sizeof(characterFreq)*chNum);

        Node node = _hTree->getHuffTree();
        Node* pNode=&node;

        ofstream outFile(destFile,ios::binary);
        if(!outFile){
            cout<<"create file failed! filename::"<<destFile<<endl;
            return false;
        }

        unchar value;
        string code;
        int index=0;
        long curPos=inFile.tellg();
        inFile.read((char*)&value,sizeof(unchar));
        while(true){
            if(_isLeaf(pNode)){
                unchar chara=_decodeMap[code];
                outFile.write((char*)&chara,sizeof(unchar));
                if(curPos>=fileSize && index>=_fHead->lastValidBit){
                    break;
                }
                code.clear();
                pNode=&node;
            }

            if(GET_BYTE(value,index)){
                code+='1';
                pNode=pNode->right;
            }else{
                pNode=pNode->left;
                code+='0';
            }
            if(++index>=8){
                index=0;
                inFile.read((char*)&value,sizeof(unchar));
                curPos=inFile.tellg();
            }
        }

        inFile.close();
        outFile.close();
        cout<<"decompress file success!"<<endl;
        return true;
    }

};
