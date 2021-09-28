#pragma once
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <map>
#include <string>

using namespace std;
using unchar=unsigned char;

// huffmanTree的节点
struct Node{
    unchar ch; // ASCII码字符
    int freq; // 字符出现频率
    Node *left; // 左子树
    Node *right; // 右子树
    Node(unchar c,int f,Node *l=nullptr,Node *r=nullptr)
            :ch(c),freq(f),left(l),right(r){}
    bool operator<(const Node &no)const{
        return freq>no.freq; // 优先队列默认从大到小，修改成从小到大
    }
};

// huffman树
class huffmanTree{
public:
    explicit huffmanTree(const map<unchar,int>& hfMap){
        for(auto i:hfMap) {
            Node n(i.first, i.second);
            q.push(n);
        }
        _creatHfTree();
    }

    ~huffmanTree(){
        Node node = q.top();
        _destroyTree(node.left);
        _destroyTree(node.right);
    }

    // huffman压缩函数
    void huffmanCode(map<unchar,string>& codeMap){
        Node node = q.top();
        string hfCode;
        _huffmanCode(&node,hfCode,codeMap);
    }

    // 获取huffman树
    Node getHuffTree(){
        return q.top();
    }

private:
    priority_queue<Node> q;

    void _creatHfTree(){
        while(q.size()!=1){
            Node *left = new Node(q.top());
            q.pop();
            Node *right = new Node(q.top());
            q.pop();
            Node node('R',left->freq + right->freq,left,right);
            q.push(node);
        }
    }

    void _destroyTree(Node* n){
        if(!n) return ;
        _destroyTree(n->left);
        _destroyTree(n->right);
        delete n;
    }

    static bool _isLeaf(Node* node){
        return node->left==nullptr && node->right==nullptr;
    }

    void _huffmanCode(Node *root,string& hfCode,map<unchar,string>& codeMap){
        string tmp=hfCode;
        if(root->left!=nullptr){
            hfCode += '0';
            if(_isLeaf(root->left)){
                codeMap[root->left->ch] = hfCode;
            }else{
                _huffmanCode(root->left,hfCode,codeMap);
            }
        }
        if(root->right!=nullptr){
            hfCode =tmp;
            hfCode += '1';
            if(_isLeaf(root->right)){
                codeMap[root->right->ch] = hfCode;
            }else{
                _huffmanCode(root->right,hfCode,codeMap);
            }
        }
    }
};
