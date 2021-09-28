#pragma once
#include <iostream>
#pragma pack(1)       //单字节对齐
#define BUF_SIZE 1 //每次读取文件内容的大小
#define NAME_SIZE 128 //文件名大小
#define OPR_SIZE 20   //输入的操作的数组的大小
#define FILE_SIZE 128 //文件完整路径名的大小
#define PASSWORD_SIZE 16   //输入的密码的数组的大小
using namespace std;
struct MyFile
{
    int size;           //本次传输大小
    char buf[BUF_SIZE]; //文件内容
};

int MySend(int iSock, char *pchBuf, size_t tLen){                             //把pchBuf里tLen长度数据发送出去，直到发送完为止
    int iThisSend;            //一次发送了多少数据
    unsigned int iSended = 0; //已经发送了多少
    if (tLen == 0)
        return 0;
    while (iSended < tLen)
    {
        do
        {
            iThisSend = send(iSock, pchBuf, tLen - iSended, 0);
        } while ((iThisSend < 0) && (errno == EINTR)); //发送的时候遇到了中断
        if (iThisSend < 0)
            return iSended;
        iSended += iThisSend;
        pchBuf += iThisSend;
    }
    return tLen;
}

int MyRecv(int iSock, char *pchBuf, size_t tLen){
    int iThisRead;
    unsigned int iReaded = 0;
    if (tLen == 0)
        return 0;
    while (iReaded < tLen)
    {
//        do
//        {
            iThisRead = read(iSock, pchBuf, tLen - iReaded);
//            if(iThisRead==0) break;
//        } while ((iThisRead < 0) && (errno == EINTR));
        if (iThisRead < 0)
            return iThisRead;
        else if (iThisRead == 0)
            return iReaded;
        iReaded += iThisRead;
        pchBuf += iThisRead;
    }
}

void getOper(char a[], char c[])
{ //将输入的要上传的文件路径分割出文件件名放在c数组中，例如 e://aaa.txt  c=aaa.txt
    int i = strlen(a) - 1;
    for (; i > 0; i--)
    {
        if ((a[i] == '/' && a[i - 1] != '/') || (a[i] == '\\' && a[i - 1] != '\\'))
            break;
        //else
        //   c[i] = a[i];
        //		cout<<i<<endl;
    }
    if (i == 0)
    {
        //如果文件名里没有\ 或者/
        c[i] = a[i];
        return;
    }
    ++i;
    int t = 0;
    for (; i < strlen(a); i++)
    {
        c[t] = a[i];
        ++t;
    }
    c[t] = '\0';
    //	cout<<"c:"<<c<<endl;
}
