#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <cmath>
#include <ctime>
#include <chrono>
#include <unistd.h>
//to compile: use command g++ mypredict.cpp -o mypredict.out
//to run: ./mypredict.out
using  namespace  std;

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
    long cnt=0;
    long buffmiss=0;
    float corr=0;
    for(int i=0;i<jump.size()-1;i++){
        if(jump[i]) buffmiss++;
    }
    if(jump.back()) buffmiss++;
    cout<<"buff misses:"<<buffmiss<<endl;
    set<long long> jumped{};

    bool prediction=false;
    for(int i=1;i<address.size();i++){
        if(prediction==jump[i-1])
            cnt++;
        if(jumped.count(address[i])){
            prediction=true;
        }else
            prediction=false;
        if(jump[i-1])
            jumped.insert(address[i-1]);
    }
    corr=(float)cnt/jump.size();
    cout<<"my predictor:"<<corr*100<<"%"<<endl;
    cout<<"correct: "<<cnt<<" uncorrect: "<<jump.size()-cnt<<endl;

    return 0;
}