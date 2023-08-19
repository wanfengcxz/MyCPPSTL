//
// Created by 晚风吹行舟 on 2023/4/16.
//
#include <iostream>

#include "algobase.h"
#include "base_test.h"


using namespace std;

void test_copy_trivially_value() {
    int a[5] = {1, 2, 3, 4, 5};
    int b[5] = {0, 0, 0, 0, 0};
    int *b_end = stl::copy(a, a + 5, b);

    for (int i = 0; i < 5; i++) std::cout << a[i] << " ";
    cout << endl;
    for (int i = 0; i < 5; i++) std::cout << b[i] << " ";
    cout << endl;
}


int main() {

    test_copy_trivially_value();

    return 0;
}