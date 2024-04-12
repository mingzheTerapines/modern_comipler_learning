#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include <vector>         // std::vector
#include <string>         // std::string
#include <set>            // std::set
#include <fstream>        // std::fstream
#include <algorithm>      // std::algorithm
#include <unistd.h>       // unistd.h
//to compile: use command g++ two_level_adaptive_predictor.cpp -o two_level_adaptive_predictor.out
//to run: ./two_level_adaptive_predictor.out
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
void handle(bipredictor *bpd,bool &predict,vector<bool> &jump,long &cnt,int i){
    if(bpd->bst==binarystatus::stronglynot||bpd->bst==binarystatus::weaklynot){
        predict=false;
    }else
        predict=true;
    if(predict==jump[i])
        cnt++;
    bpd->iftaken(jump[i]);
}
int main(){
    vector<long long> address{};
    ifstream fin("itrace.out");
    string str="";
    string::size_type sz;
    sz = 0;
    long long tmp;
    while(getline(fin,str)){
        if(str!="#eof"){
            tmp=stoll(str,&sz,0);
            address.push_back(tmp);
        }
    }
    vector<bool> jump;
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

    long cnt=0;
    long buffmiss=0;
    float corr;
    for(int i=0;i<jump.size()-1;i++){
        if(jump[i]) buffmiss++;
    }
    if(jump.back()) buffmiss++;
    cout<<"buff misses:"<<buffmiss<<endl;
    bipredictor *bpd00=new bipredictor;
    bipredictor *bpd01=new bipredictor;
    bipredictor *bpd10=new bipredictor;
    bipredictor *bpd11=new bipredictor;

    bool predict;
    cnt=0;
    bool first=jump[0];
    bool second=jump[1];
    for(int i=3;i<jump.size();i++){
        if(!first&&!second){
            handle(bpd00,predict,jump,cnt,i);
        }else if(!first && second){
            handle(bpd01,predict,jump,cnt,i);
        }else if(first && !second){
            handle(bpd10,predict,jump,cnt,i);
        }else if(first&&second){
            handle(bpd11,predict,jump,cnt,i);
        }
        first=jump[i-1];
        second=jump[i];
    }
    corr=(float)cnt/jump.size();
    cout<<" two_level_adaptive_predictor:"<<corr*100<<"%"<<endl;
    cout<<"correct: "<<cnt<<" uncorrect: "<<jump.size()-cnt<<endl;

    return 0;
}
