#include<iostream>
#include<functional>
using namespace std;

void print(string str,int a)
{
    cout << str << a<< endl;
}
int main(){
    //print("hello",10);
    auto func = std::bind(print,"bit",std::placeholders::_1);
    func(1);


    return 0;
}