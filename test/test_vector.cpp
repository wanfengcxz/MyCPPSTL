//
// Created by 晚风吹行舟 on 2023/4/9.
//

#include <vector.h>
#include <iostream>

template <class T, T v>
struct m_integral_constant{
    static constexpr T value = v;
};

int main(){

    std::cout << m_integral_constant<int,10>::value << std::endl;


    return 0;
}