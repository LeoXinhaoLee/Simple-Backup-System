//client.cpp

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <fstream>
#include "net.h"
#include "../simple_tar.h"

using namespace std;
#define DEFAULT_PORT 6666                          //服务器端口
#define IPADDRESS "127.0.0.1"                      //IP地址
#define CLI_PATH "./download_package/" //客户端默认存放文件的路径名


void getOper(char a[], char c[]);          //将输入的要上传的文件路径分割出文件件名放在c数组中，例如 e://aaa.txt  c=aaa.txt
int update(char filename[], int connfd);   //客户端上传文件
int download(char filename[], int connfd); //客户端从服务器下载文件

int client(char passWord[],char ope[],char src[],char dest[])
{
    int connfd = 0;
    int cLen = 0;
    struct sockaddr_in client; //服务器socket地址
    client.sin_family = AF_INET;
    client.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, IPADDRESS, &client.sin_addr); //将IP地址从点分十进制转换为二进制整数
    connfd = socket(AF_INET, SOCK_STREAM, 0);        //客户端socket描述字
    if (connfd < 0)
    {
        printf("socket fail!\n");
        return -1;
    }
    if (connect(connfd, (struct sockaddr *)&client, sizeof(client)) < 0)
    {
        printf("connect error!\n");
        return -1;
    }
    char password[PASSWORD_SIZE];
    char pass[2];
    memset(password, 0, PASSWORD_SIZE);
    strcpy(password,passWord);

    MySend(connfd, password, PASSWORD_SIZE);
    MyRecv(connfd, pass, sizeof(pass));

    if(strcmp(pass, "a")==0){
        printf("illegal user\n");
        close(connfd);
        return 1;
    }

    char opera[OPR_SIZE]; //分别存放操作和完整的路径名（文件名）
    strcpy(opera,ope);
    char com_file[FILE_SIZE];
    //getOper(src,com_file);
    strcpy(com_file, src);
    //strcat(com_file,".pkg");

    MySend(connfd, opera, OPR_SIZE); //发送操作给服务器

    if (strcmp(opera, "update") == 0)  //update 要上传文件给服务器
    {
        //simple_tar st;
        //st.package(src, "./");
        MySend(connfd, com_file, FILE_SIZE); //发送文件名给服务器
        if (update(com_file, connfd) == 0)
            printf("update successfully!\n");
    }
    else if (strcmp(opera, "download") == 0)
    {                                        //download 要下载文件
        MySend(connfd, src, FILE_SIZE); //发送文件名给服务器

        if (download(src, connfd) == 0)
        {
            //simple_tar st;
            //char root[]=CLI_PATH;
            //strcat(root,src);
            //st.unpackage(root, dest);

            printf("download successfully\n");
        }
    }
    else if(strcmp(opera, "show") == 0){
        char fileList[256];
        cout<<"----------fiielist-----------"<<endl;
        while(true){
            MyRecv(connfd,fileList,sizeof(fileList));
            if(strcmp(fileList,"end")==0) break;
            cout<<fileList<<endl;
        }
    }
    else if(strcmp(opera, "login") == 0)
    {
        printf("Log in successfully!\n");
    }
    else
        printf("illegal operation!\n");

    close(connfd);
    return 0;
}

string clientShowALL(char passWord[],char ope[],char src[],char dest[]){
    string ret;

    int connfd = 0;
    int cLen = 0;
    struct sockaddr_in client; //服务器socket地址
    client.sin_family = AF_INET;
    client.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, IPADDRESS, &client.sin_addr); //将IP地址从点分十进制转换为二进制整数
    connfd = socket(AF_INET, SOCK_STREAM, 0);        //客户端socket描述字
    if (connfd < 0)
    {
        printf("socket fail!\n");
        ret = "Fail";
        return ret;
    }
    if (connect(connfd, (struct sockaddr *)&client, sizeof(client)) < 0)
    {
        printf("connect error!\n");
        ret = "Fail";
        return ret;
    }
    char password[PASSWORD_SIZE];
    char pass[2];
    memset(password, 0, PASSWORD_SIZE);
    strcpy(password,passWord);

    MySend(connfd, password, PASSWORD_SIZE);
    MyRecv(connfd, pass, sizeof(pass));

    if(strcmp(pass, "a")==0){
        printf("illegal user\n");
        close(connfd);
        ret = "Fail";
        return ret;
    }
    else
        printf("Log in Successfully!\n");

    char opera[OPR_SIZE]; //分别存放操作和完整的路径名（文件名）
    strcpy(opera,ope);
    char com_file[FILE_SIZE];
    getOper(src,com_file);
    strcat(com_file,".pkg");

    MySend(connfd, opera, OPR_SIZE); //发送操作给服务器

    char fileList[256];
    cout<<"----------fiielist-----------"<<endl;
    ret = "";
    while(true){
        MyRecv(connfd,fileList,sizeof(fileList));
        if(strcmp(fileList,"end")==0) break;
        cout<<fileList<<endl;
        ret += fileList;
        ret += "\n";
    }

    return ret;

}

int update(char filename[], int connfd)
{ //客户端上传文件
    MyFile myfile;
    FILE *fp;
    FILE *ppp;

    if ((fp = fopen(filename, "rb")) == NULL)
    {
        printf("cannot open file: %s\n", filename);
        return -1;
    }
    printf("openfile ok!\n");

    int length;
    while (!feof(fp))
    {
        length = fread(myfile.buf, sizeof(char), BUF_SIZE, fp);
        myfile.size = length;
        char *pBuff = new char[1500];
        int iLen = 0;

        *(int *)(pBuff + iLen) = (int)htonl(myfile.size); //host to net long
        iLen += sizeof(int);

        memcpy(pBuff + iLen, myfile.buf, myfile.size);
        iLen += sizeof(myfile.buf);

        ssize_t writeLen = MySend(connfd, pBuff, iLen);
        if (writeLen < 0)
        {
            printf("write failed\n");
            close(connfd);
            return -1;
        }
        delete[] pBuff;
    }

    fclose(fp);
    return 0;
}

int download(char filename[], int connfd)
{ //从服务器下载文件 返回0成功
    char *recvMsg = new char[1500];
    int readLen;
    FILE *fp;
    MyFile myfile;

    string path = CLI_PATH;
    path += filename;

    const char *p = path.c_str();
    if ((fp = fopen(p, "wb")) == NULL)
    {
        printf("cannot open file");
        return -1;
    }

    while ((readLen = MyRecv(connfd, recvMsg, sizeof(MyFile))) > 0)
    {
        int iLen = 0;

        memcpy(&myfile.size, recvMsg + iLen, sizeof(int));
        myfile.size = (int)ntohl(myfile.size);
        iLen += sizeof(int);

        memcpy(&myfile.buf, recvMsg + iLen, sizeof(myfile.buf));

        if (myfile.size > 0)
            fwrite(myfile.buf, sizeof(char), BUF_SIZE, fp);
    }

    fclose(fp);
    return 0;
}
