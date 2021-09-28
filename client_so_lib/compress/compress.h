#pragma once
#include "huffmanTree.h"
#include <fstream>

using namespace std;
using unchar = unsigned char;

// 压缩文件头部信息：用于解压缩
struct fileHead{
    char flag[4]; //压缩文件头部标志
    unchar characterNum; //字符种类数目
    unchar lastValidBit; //最后一个有效位
};

// 存储字符和出现频率：用于解压缩
struct characterFreq{
    unchar chara;
    int freq;
    characterFreq(){}
    characterFreq(const std::pair<char,int>& x):chara(x.first),freq(x.second){}
};

class huffmanEncode{
public:
    // 压缩译码函数
    bool encode(const char* srcFile,const char* destFile){
        if(!_getCharacterFreq(srcFile)) return false;
        huffmanTree hfTree(_hfMap);
        hfTree.huffmanCode(_codeMap);
        return _encode(srcFile,destFile);
    }
private:
    map<unchar,int> _hfMap;
    map<unchar,string> _codeMap;

    bool _getCharacterFreq(const char* filename){
        unchar ch;
        ifstream inFile(filename,ios::binary);
        if(!inFile.is_open()){
            cout<<"read file failed! filename:"<<filename<<endl;
            return false;
        }
        inFile.read((char*)&ch,sizeof(unchar));
        while(!inFile.eof()){
            _hfMap[ch]++;
            inFile.read((char*)&ch,sizeof(unchar));
        }
        inFile.close();
        return true;
    }

    int _getLastValidBit(){
        int sum=0;
        for(auto it:_codeMap){
            sum+=it.second.size()*_hfMap.at(it.first);
            sum &= 0xFF;
        }
        sum &= 0x7;
        return sum == 0 ? 8 : sum;
    }

    bool _encode(const char* srcFile,const char* destFile){
        unchar ch;
        unchar value;
        int count=0;
        fileHead fHead={'e','v','e','n'};
        fHead.characterNum=(unchar)_hfMap.size();
        fHead.lastValidBit=_getLastValidBit();

        ifstream inFile(srcFile,ios::binary);
        if(!inFile.is_open()){
            cout<<"read file failed! filename:"<<srcFile<<endl;
            return false;
        }
        ofstream outFile(destFile,ios::binary);
        if(!outFile.is_open()){
            cout<<"read file failed! filename:"<<destFile<<endl;
            return false;
        }

        outFile.write((char*)&fHead,sizeof(fileHead));
        for(auto i:_hfMap){
            characterFreq cf(i);
            outFile.write((char*)&cf,sizeof(characterFreq));
        }

        inFile.read((char*)&ch,sizeof(unchar));
        while(!inFile.eof()){
            std::string code=_codeMap.at(ch);
            for(auto c:code){
                value<<=1;
                if(c=='1'){
                    value|=1;
                }
                ++count;
                if(count==8){
                    outFile.write((char*)&value,sizeof(unchar));
                    count=0;
                    value=0;
                }
            }
            inFile.read((char*)&ch,sizeof(unchar));
        }
        if(count>0){
            value<<=(8-count);
            outFile.write((char*)&value,sizeof(unchar));
        }
        cout<<"compress file success！"<<endl;

        inFile.close();
        outFile.close();
        return true;
    }
};
