//
// Created by 晚风吹行舟 on 2023/4/11.
//

#include <iostream>

#include <construct.h>

using namespace std;

class X {
public:
    int data;

    X(int d) : data(d) {}


    X(const X &c) {
        data = c.data;
    }

};

class Y : public X {
public:
    int c;

    Y(int d, int c_) : c(c_), X(d) {}


};

class M{
public:
    typedef bool value_type;

    bool value = true;

//    constexpr operator value_type() const noexcept { return value; }

    constexpr value_type operator()() const noexcept { return value; }
};

#include "uninitialized.h"

int main() {

    cout << boolalpha;

    auto t = M();
    cout << t.value;

//    std::true_type c = std::is_trivially_copy_assignable<X>();
//    cout << c;

    return 0;
}