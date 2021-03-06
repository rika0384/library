/*
文字列集合Sを管理し、以下のクエリに O(|t|*log(X)) で答える (Xはアルファベットサイズ)
ただし、辞書順クエリ、辞書順前後クエリは O(|t|*X*log(X))
- 検索クエリ：文字列 t が含まれるか？
- 追加クエリ：文字列 t を追加する
- 削除クエリ：文字列 t を削除する
- prefixクエリ：文字列 t をprefixとしてもつものがいくつあるか？
- 辞書順前後クエリ：文字列 t の辞書順で次/前であるような文字列はなにか？
- LCPクエリ：文字列 t とのLCPが最大になるような文字列はなにか？
- 辞書順クエリ：文字列 t は辞書順で何番目か？
また、以下のクエリにも対応する
- 辞書順クエリ2：辞書順で N 番目の文字列はなにか？
*/

//g++ --std=c++11 trie.cpp

#include<iostream>
#include<vector>
#include<string>
#include<map>
using namespace std;

class TrieTree{
private:
    struct Node{
        map<char,Node*> child;
        Node* parent = nullptr; //親のノード
        int self = 0; //自身で終わる文字列がいくつあるか
        int children = 0; //子孫で終わる文字列がいくつあるか
        int ind = -1; //先頭から何文字目か
        char ch = '*';
    };

    Node* root;

    void print_dfs(Node* node,string s){
        for(int i = 0; i < node->ind; i++)cout << "-";
        cout << node->ch << "(" << node->self << "," << node->children << ")";
        if(node->self)cout << " " << s;
        cout << endl;
        for(auto itr = node->child.begin(); itr != node->child.end(); itr++){
            print_dfs(itr->second,s+itr->first);
        }
    }

public:
    TrieTree(){
        root = new Node;
        root->ind = 0;
    }
    TrieTree(vector<string> &vs){
        root = new Node;
        root->ind = 0;
        for(int i = 0; i < vs.size(); i++){
            insert(vs[i]);
        }
    }
    int insert(string t){//追加クエリ：文字列 t を追加する
        Node* node = root;
        node->children++;
        for(int i = 0; i < t.size(); i++){//nodeとt[i]が対応してる
            auto itr = node->child.find(t[i]);
            if(itr == node->child.end()){//ノードが存在しない
                node->child[t[i]] = new Node;
                itr = node->child.find(t[i]);
                itr->second->parent = node;
                node = itr->second;
                node->ind = i+1;
                node->ch = t[i];
            }else{//たどる
                node = itr->second;
            }
            node->children++;
        }
        return ++node->self;//追加後のtの数
    }
    int erase(string t){//削除クエリ：文字列 t を削除する
        Node* node = root;
        for(int i = 0; i < t.size(); i++){//nodeとt[i]が対応してる
            auto itr = node->child.find(t[i]);
            if(itr == node->child.end()){//ノードが存在しない
                return -1;
            }else{//たどる
                node = itr->second;
            }
        }
        if(node->self == 0)return -1;
        int ret = --node->self;
        while(true){
            node->children--;
            if(node->children == 0 && node != root){
                char c = node->ch;
                node = node->parent;
                delete(node->child[c]);//ノードの削除
                node->child.erase(c);//mapの削除
            }else{
                if(node == root)break;
                node = node->parent;
            }
        }
        return ret;//削除後のtの数(tが存在しない場合は-1を返す)
    }

    int find(string t){//検索クエリ：文字列 t が含まれるか？
        Node* node = root;
        for(int i = 0; i < t.size(); i++){//nodeとt[i]が対応してる
            auto itr = node->child.find(t[i]);
            if(itr == node->child.end()){//ノードが存在しない
                return 0;
            }else{//たどる
                node = itr->second;
            }
        }
        return node->self;//文字列tの個数
    }

    int prefix(string t){//prefixクエリ：文字列 t をprefixとしてもつものがいくつあるか？
        Node* node = root;
        for(int i = 0; i < t.size(); i++){//nodeとt[i]が対応してる
            auto itr = node->child.find(t[i]);
            if(itr == node->child.end()){//ノードが存在しない
                return 0;
            }else{//たどる
                node = itr->second;
            }
        }
        return node->children;//子孫の個数
    }

    string lcp(string t){//LCPクエリ：文字列 t とのLCPが最大になるような文字列はなにか？
        string ret = "";
        Node* node = root;
        for(int i = 0; i < t.size(); i++){//nodeとt[i]が対応してる
            auto itr = node->child.find(t[i]);
            if(itr == node->child.end()){//ノードが存在しない
                break;
            }else{//たどる
                node = itr->second;
                ret += itr->first;
            }
        }

        while(true){
            if(node->self){
                return ret;//LCPが最大になるような文字列のうち、辞書順最小
            }else{
                auto itr = node->child.begin();
                ret += itr->first;
                node = itr->second;
            }
        }
        /*
        while(true){
            if(node->children == node->self){
                return ret;//LCPが最大になるような文字列のうち、辞書順最大
            }else{
                auto itr = --node->child.end();
                ret += itr->first;
                node = itr->second;
            }
        }
        */
    }

    string NthString(int n){//辞書順クエリ2：辞書順で N 番目の文字列はなにか？
        string ret = "";
        if(n <= 0)return ret;//存在しない時は "" が返る
        Node* node = root;
        int tmp = 0;
        while(n <= tmp + node->children){
            if(tmp < n && n <= tmp + node->self){
                return ret; //辞書順で N 番目の文字列
            }
            tmp += node->self;
            for(auto itr = node->child.begin(); itr != node->child.end(); itr++){
                Node* ne = itr->second;
                if(tmp + ne->children < n){
                    tmp += ne->children;
                }else{
                    ret += itr->first;
                    node = ne;
                    break;
                }
            }
        }
        return ret;//存在しない時は "" が返る
    }

    int NthString(string t){//辞書順クエリ：文字列 t は辞書順で何番目か？
        if(find(t) == 0)return -1; //存在しない場合は -1
        int ret = 0;
        Node* node = root;
        for(int i = 0; i < t.size(); i++){//nodeとt[i]が対応してる
            ret += node->self;
            for(auto itr = node->child.begin(); itr != node->child.end(); itr++){
                Node* ne = itr->second;
                if(itr->first == t[i]){
                    node = ne;
                    break;
                }else{
                    ret += ne->children;
                }
            }
        }
        ret++;
        return ret;//辞書順の番号
    }

    string NextString(string t){//辞書順前後クエリ：文字列 t の辞書順で次であるような文字列はなにか？
        int num = find(t);
        if(num == 0)return "";
        int tmp = NthString(t);
        return NthString(tmp+num);
    }

    string PrevString(string t){//辞書順前後クエリ：文字列 t の辞書順で前であるような文字列はなにか？
        if(find(t) == 0)return "";
        int tmp = NthString(t);
        return NthString(tmp-1);
    }

    void print(void){
        print_dfs(root,"");
    }
};

int main(){
    TrieTree trie;
    trie.insert("ba");
    trie.insert("abcde");
    trie.insert("ab");
    trie.insert("ab");
    trie.insert("abe");
    trie.insert("abcx");
    trie.insert("aa");
    trie.print();
    cout << trie.NextString("abe") << endl;
    //trie.print();
}
