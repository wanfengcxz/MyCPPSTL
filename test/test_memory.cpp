//
// Created by 晚风吹行舟 on 2023/5/5.
//

#include <iostream>

#include "memory.h"
#include "base_test.h"

using std::cout;
using std::cin;

using namespace stl;

void test_auto_ptr() {
    A *pA1 = new A(2);
    A *pA2 = new A(3);

    C *pC1 = new C(4, 5);
    C *pC2 = new C(6, 7);

    auto_ptr<C> ptrC1(pC1);
    auto_ptr<A> ptrA1, ptrA2(pA1), ptrA3(ptrC1);

//    cout << ptrA1->x << endl; // nullptr 无法访问
    cout << ptrA2->x << endl;
    cout << ptrA3->x << endl;

    cout << (*ptrA2).x << endl;
    cout << (*ptrA3).x << endl;

    pA2 = pC1;  // 和 ptrA3(ptrC1) 意思一样
    // 基类指针可以指向派生类的对象

    // 赋值也是一样 不再举例子
}

int main() {


    return 0;
}