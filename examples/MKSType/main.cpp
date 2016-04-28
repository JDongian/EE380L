//
//  main.cpp
//  epl
//
//  Created by wangqi on 4/27/16.
//  Copyright © 2016 wangqi. All rights reserved.
//
#include <iostream>
#include "MKS.h"


using std::cout;
using std::endl;

int main(){
    Meter<double> x(10.0);
    cout<<"x: "<<x<<endl;
    
    Inch<double> y(20.0);
    cout<<"y: "<<y<<endl;
    
    cout<<"x+y: "<<x+y<<endl;
    
    cout<<"x/y: "<<x/y<<endl;
    cout<<"x*y: "<<x*y<<endl;
    
    x=y;
    cout<<"x after assignment: "<<x<<endl;
    
    Second<double> z(40.5);
    
    cout<<"x/z:"<<x/z<<endl;
}

//template<typename ...Args>
//void print(Args... args){
//    for(int i =0; i<num_of(args); ++i){
//        cout << args[i]<<endl;
//    }
//}

template<typename T>
void print(T const& t){
    cout<<t<<endl;
}

//template<typename T1, typename T2>
//void print(T1 const& t1, T2 const& t2){
//    cout << t1 << endl;
////    cout << t2 << endl;
//    print<T2> (t2);
//}

template<typename T, typename... Args>
void print(T const& t, Args const& ... args){
    cout << t << endl;
    print<Args...>(args...);
}

//int main(){
//    print(10.5, 42, std::string{"hello"}, "world");
//}

//template<int index, typename ... Args>
//struct retrieve_index{
//    using type = typename Args[index];
//};

template<int index, typename T, typename... Args>
struct retrieve_index{
    using type = typename retrieve_index<index-1, Args...>::type;
};

template<typename T, typename ... Args>
struct retrieve_index<0, T, Args...>{
    using type = T;
};

//template<typename T, typename R, typename ...Args>
//struct retrieve_index<1, T, R, Args...>{
////    using type = R;
//    using type = typename retrieve_index<1-1, R, Args...>::type;
//};

//int main(){
//    using rtype = retrieve_index<1, int ,double*, float, std::string, char*>::type;
//    cout << typeid(rtype).name()<<endl;
//}

























