#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include <vector>         // std::vector
#include <string>         // std::string
#include <set>            // std::set
#include <fstream>        // std::fstream
#include <algorithm>      // std::algorithm
#include <unistd.h>       // unistd.h
//to compile: use command g++ swither_predictor.cpp -o swither_predictor.out
//to run: ./swither_predictor.out
using namespace std;
volatile int counter(0); // non-atomic counter
std::mutex mtx;           // locks access to counter
vector<bool> jump(0);      //the list of jump history
vector<long long> address(0);
void readfile(){
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
}
vector<float> taken_corr(0);         //the correction of taken predictor
void taken_predictor(){//predict taken
    long taken_cnt(0);
    for(int i=0;i<jump.size();i++) {
        if (jump[i]) {
            taken_cnt++;
        }
        taken_corr.push_back(((float) taken_cnt / (float) i)*100);
    }
        while(true){
            if (mtx.try_lock()) {   // counter finished
                ++counter;
                mtx.unlock();
                break;
            }
        }
}
vector<float> not_taken_corr(0);     //the correction of taken predictor
void not_taken_predictor(){
    long not_taken_cnt(0);
    for(int i=0;i<jump.size();i++){
        if(!jump[i]){
            not_taken_cnt++;
        }
        not_taken_corr.push_back((float)not_taken_cnt/(float)i*100);
    }
    while(true){
        if (mtx.try_lock()) {   // counter finished
            ++counter;
            mtx.unlock();
            break;
        }
    }
}
vector<float> backward_taken_forward_not_taken_corr(0);
vector<bool> backward_taken_forward_not_taken_predict(0);
void backward_taken_forward_not_taken_predictor(){
    long cnt=0;
    bool predict(false);
    for(int i=0;i<jump.size();i++){
        backward_taken_forward_not_taken_predict.push_back(predict);
        if(jump[i]==predict)
            cnt++;
        predict=!jump[i];
        backward_taken_forward_not_taken_corr.push_back((float)cnt/(float)i*100);
    }
    while(true){
        if (mtx.try_lock()) {   // counter finished
            ++counter;
            mtx.unlock();
            break;
        }
    }
}
vector<float> one_bit_corr(0);
vector<bool> one_bit_predict(0);
void one_bit_predictor(){
    long cnt=0;
    bool predict(false);
    for(int i=0;i<jump.size();i++){
        one_bit_predict.push_back(predict);
        if(predict==jump[i])
            cnt++;
        predict=jump[i];
        one_bit_corr.push_back((float)cnt/(float)i*100);
    }
    while(true){
        if (mtx.try_lock()) {   // counter finished
            ++counter;
            mtx.unlock();
            break;
        }
    }
}
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
vector<float> bimodal_corr(0);
vector<bool> bimodal_predict(0);
void bimodal_predictor(){
    auto *bpd=new bipredictor;
    long cnt=0;
    bool predict(false);
    for(int i=0;i<jump.size();i++){
        bimodal_predict.push_back(predict);
        if(predict==jump[i])
            cnt++;
        if(bpd->bst==binarystatus::stronglynot||bpd->bst==binarystatus::weaklynot){
            predict=false;
        }else
            predict=true;
        bpd->iftaken(jump[i]);
        bimodal_corr.push_back((float)cnt/(float )i*100);
    }
    while(true){
        if (mtx.try_lock()) {   // counter finished
            ++counter;
            mtx.unlock();
            break;
        }
    }
}
vector<float> two_level_adaptive_corr(0);
vector<bool>  two_level_adaptive_predict(0);
void handle(bipredictor *bpd,bool &predict,long &cnt,int i){
    if(bpd->bst==binarystatus::stronglynot||bpd->bst==binarystatus::weaklynot){
        predict=false;
    }else
        predict=true;
    if(predict==jump[i])
        cnt++;
    bpd->iftaken(jump[i]);
}
void two_level_adaptive_predictor(){
    auto *bpd00=new bipredictor;
    auto *bpd01=new bipredictor;
    auto *bpd10=new bipredictor;
    auto *bpd11=new bipredictor;
    bool predict(false);
    bool first(jump[0]);
    bool second(jump[1]);
    long cnt=2;
    two_level_adaptive_predict.push_back(first);
    two_level_adaptive_predict.push_back(second);
    two_level_adaptive_corr.push_back(100);
    two_level_adaptive_corr.push_back(100);
    for(int i=2;i<jump.size();i++){
        if(!first&&!second){
            handle(bpd00,predict,cnt,i);
        }else if(!first && second){
            handle(bpd01,predict,cnt,i);
        }else if(first && !second){
            handle(bpd10,predict,cnt,i);
        }else if(first&&second){
            handle(bpd11,predict,cnt,i);
        }
        first=jump[i-1];
        second=jump[i];
        two_level_adaptive_predict.push_back(predict);
        two_level_adaptive_corr.push_back((float)cnt/(float)i*100);
    }
    while(true){
        if (mtx.try_lock()) {   // counter finished
            ++counter;
            mtx.unlock();
            break;
        }
    }
}
vector<float> delayed_corr(0);
vector<bool> delayed_predict(0);
void delayed_predictor(){
    long cnt=2;
    delayed_corr.push_back(100);
    delayed_corr.push_back(100);
    delayed_predict.push_back(false);
    delayed_predict.push_back(false);
    for(int i=2;i<jump.size();i++){
        if(jump[i]==delayed_predict[i-2])
            cnt++;
        delayed_predict.push_back(jump[i]);
        delayed_corr.push_back((float)cnt/(float)i*100);
    }
    while(true){
        if (mtx.try_lock()) {   // counter finished
            ++counter;
            mtx.unlock();
            break;
        }
    }
}
vector<float> history_based_corr(0);
vector<bool> history_based_predict(0);
set<long long> jumped;
void history_based_predictor(){
    float cnt(1);
    bool prediction(false);
    for(int i=1;i<address.size();i++){
        history_based_corr.push_back(prediction);
        if(prediction==jump[i-1])
            cnt++;
        if(jumped.count(address[i])){
            prediction=true;
        }else
            prediction=false;
        if(jump[i-1])
            jumped.insert(address[i-1]);
        history_based_corr.push_back((float)cnt/(float )i);
    }
    while(true){
        if (mtx.try_lock()) {   // counter finished
            ++counter;
            mtx.unlock();
            break;
        }
    }
}
void do_prediction(bool &predict,int i){
    float maxvalue=max(taken_corr[i],not_taken_corr[i]);
    maxvalue=max(maxvalue,backward_taken_forward_not_taken_corr[i]);
    maxvalue=max(maxvalue,one_bit_corr[i]);
    maxvalue=max(maxvalue,bimodal_corr[i]);
    maxvalue=max(maxvalue,two_level_adaptive_corr[i]);
    maxvalue=max(maxvalue,delayed_corr[i]);
    maxvalue=max(maxvalue,history_based_corr[i]);
    if(maxvalue==taken_corr[i]) {
        predict = true;
        return;
    }
    if(maxvalue==not_taken_corr[i]){
       predict=false;
       return;
    }
    if(maxvalue==backward_taken_forward_not_taken_corr[i]){
        predict=backward_taken_forward_not_taken_predict[i];
        return ;
    }
    if(maxvalue==one_bit_corr[i]){
        predict=one_bit_predict[i];
        return ;
    }
    if(maxvalue==bimodal_corr[i]){
        predict=bimodal_predict[i];
        return ;
    }
    if(maxvalue==two_level_adaptive_corr[i]){
        predict=two_level_adaptive_predict[i];
        return ;
    }
    if(maxvalue==delayed_corr[i]){
        predict=delayed_predict[i];
        return ;
    }
    if(maxvalue==history_based_corr[i]){
        predict=history_based_predict[i];
        return ;
    }
}
void do_reduction(){
    bool predict(false);
    long cnt(0);
    for(int i=0;i<jump.size();i++){
        if(predict==jump[i])
            cnt++;
        do_prediction(predict,i);
    }
    cout<<"prediction correction: "<<(float)cnt/(float)jump.size()*100<<endl;
}
void reduction(){
    while(true){
        if (mtx.try_lock()) {   //read counter
            if (counter == 8) { //counter finished
                do_reduction();
                mtx.unlock();
                break;
            } else {
                mtx.unlock();
                sleep(5);//wait 5s for counter finished
            }
        }else
            sleep(5);
    }
}
int main () {
    readfile();
    std::thread threads[9];
    threads[0]=thread(reduction);
    threads[1]=thread(taken_predictor);
    threads[2]=thread(not_taken_predictor);
    threads[3]=thread(backward_taken_forward_not_taken_predictor);
    threads[4]=thread(one_bit_predictor);
    threads[5]=thread(bimodal_predictor);
    threads[6]=thread(two_level_adaptive_predictor);
    threads[7]=thread(delayed_predictor);
    threads[8]=thread(history_based_predictor);
    for (auto& th : threads) th.join();
    return 0;
}

