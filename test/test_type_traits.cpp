//
// Created by 晚风吹行舟 on 2023/4/10.
//

#include <iostream>
#include "src/type_traits.h"

int fun1() {
    return 2;
}

float fun2() {
    return 2.0;
}

int main() {

    std::cout << stl::is_int<decltype(fun1())>::value << std::endl;
    std::cout << stl::is_int<decltype(fun2())>::value << std::endl;

    std::pair<int, double> v1;
    std::cout << stl::is_pair<stl::pair<int, double>>::value << std::endl;
    std::cout << stl::is_pair<decltype(v1)>::value << std::endl;

    std::cout << stl::is_same<int, int>::value << std::endl;
    std::cout << stl::is_same<int, double>::value << std::endl;


    return 0;
}