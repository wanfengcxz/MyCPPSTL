//
// Created by 晚风吹行舟 on 2023/9/19.
//
#include <iostream>
#include <vector>

#include "gtest/gtest.h"
#include "easylogging++.h"

using namespace std;

struct Foo {
    int x{}, y{};
};

/// 如果一个类重载了指针运算符 obj->m等价于(obj.operator->())->m
struct Goo {
    int x{}, y{};
    Foo *p;

    Goo() : x(0), y(0), p(nullptr) {
        p = new Foo;
        p->x = 123;
    }

    Foo *operator->() const {
        return p;
    }

    ~Goo() {
        delete p;
    }
};

_INITIALIZE_EASYLOGGINGPP

TEST(test_operator_overload, test01) {

    Foo *p1 = new Foo();
    LOG(INFO) << "p1->x: " << p1->x;

    Goo *p2 = new Goo();
    LOG(INFO) << "p2->x: " << p2->x;

    Goo obj;
    LOG(INFO) << "obj->x: " << obj->x;
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}