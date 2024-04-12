#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include <vector>         // std::vector
#include <string>         // std::string
#include <set>            // std::set
#include <fstream>        // std::fstream
#include <algorithm>      // std::algorithm
#include <unistd.h>       // unistd.h
//to compile: use command g++ 1_bit_predictor.cpp -o 1_bit_predictor.out
//to run: ./d1_bit_predictor.out
using  namespace  std;

int main(){
    vector<long long> address;
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
    return 0;
}
