#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <ctime>
#include <chrono>
#include <unistd.h>
//to compile: use command g++ predict.cpp -o predict.out 
//to run: ./predict.out
using  namespace  std;
enum class binarystatus{
    stronglynot,//strongly not taken
    weaklynot,//weakly not taken
    weaklytaken,//weakly taken
    stronglytaken,//strongly taken
};
class bipredictor{
public:
    binarystatus bst;
    bipredictor(){
        bst=binarystatus::stronglynot;
    }
    void iftaken(bool tk){
        if(tk){
            switch(bst){
                case binarystatus::stronglynot:
                    bst=binarystatus::weaklynot;
                    break;
                case binarystatus::weaklynot:
                    bst=binarystatus::weaklytaken;
                    break;
                case binarystatus::weaklytaken:
                    bst=binarystatus::stronglytaken;
                    break;
                case binarystatus::stronglytaken:
                    break;
            }
        }else{
            switch(bst){
                case binarystatus::stronglynot:
                    break;
                case binarystatus::weaklynot:
                    bst=binarystatus::stronglynot;
                    break;
                case binarystatus::weaklytaken:
                    bst=binarystatus::weaklynot;
                    break;
                case binarystatus::stronglytaken:
                    bst=binarystatus::weaklytaken;
                    break;
            }
        }
    }
};
int main(){
    vector<long long> address{};
    ifstream fin("itrace.out");
    string str{};
    string::size_type sz;
    sz = 0;
    long long tmp;
    while(getline(fin,str)){
        if(str!="#eof"){
            tmp=stoll(str,&sz,0);
            address.push_back(tmp);
        }
    }
    vector<bool> jump{};
    tmp=address[0];
    long long diff;
    for(int i=1;i<address.size();i++){
        diff=address[i]-tmp;
        if(diff<0||diff>15){
            jump.push_back(true);
        }
        else{
            jump.push_back(false);
        }
        tmp=address[i];
    }
    /*for(auto add:address){
        cout<<hex<<add<<endl;
        cin>>tmp;
    }*/
    /*for(auto j:jump){
        if(j)
        cout<<"taken"<<endl;
        else
          cout<<"not taken"<<endl;
        cin>>tmp;
    }*/
    //1bit predictor-do same prediction
    long cnt=0;
    long buffmiss=0;
    float corr;
    for(int i=0;i<jump.size()-1;i++){
        if(jump[i]) buffmiss++;
        if(jump[i]==jump[i+1])
            cnt++;
    }
    if(jump.back()) buffmiss++;
    corr=(float)cnt/(jump.size()-1);
    cout<<"buff misses:"<<buffmiss<<endl;
    cout<<"1-bit predictor:"<<corr*100<<"%"<<endl;
    cout<<"correct: "<<cnt<<" uncorrect: "<<jump.size()-cnt<<endl;
    bipredictor *bpd=new bipredictor;

    bool predict;
    cnt=0;
    for(int i=0;i<jump.size();i++){
        if(bpd->bst==binarystatus::stronglynot||bpd->bst==binarystatus::weaklynot){
            predict=false;
        }else
            predict=true;
        if(predict==jump[i])
            cnt++;
        bpd->iftaken(jump[i]);
    }
    corr=(float)cnt/jump.size();
    cout<<"2-bit predictor:"<<corr*100<<"%"<<endl;
    cout<<"correct: "<<cnt<<" uncorrect: "<<jump.size()-cnt<<endl;

    return 0;
}