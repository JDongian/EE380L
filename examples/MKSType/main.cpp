//
//  main.cpp
//  epl
//
//  Created by wangqi on 4/27/16.
//  Copyright © 2016 wangqi. All rights reserved.
//
#include <iostream>
#include "MKS2.h"


using std::cout;
using std::endl;

int main(){
    Meters<double> x(10.0);
    cout<<"x: "<<x<<endl;
	cout << typeid(x).name() << endl;
}




















