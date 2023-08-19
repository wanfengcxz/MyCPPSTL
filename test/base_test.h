//
// Created by 晚风吹行舟 on 2023/4/16.
//

#ifndef MYCPPSTL_BASE_TEST_H
#define MYCPPSTL_BASE_TEST_H

#include <iostream>
#include <iterator.h>

using namespace std;

// C++11的类，对右值有支持
class A {
public:

    typedef stl::bidirectional_iterator_tag iterator_category;
    typedef int value_type;
    typedef int *pointer;
    typedef int &reference;
    typedef int *difference_type;

    A() = default;

    A(int v) : x(v) { cout << "A default constructor" << endl; }

    A(const A &a) : x(a.x) { cout << "A copy constructor" << endl; }

    A &operator=(A &&a) noexcept {
        x = a.x;
        cout << "A move assign constructor" << endl;
    }

    A(A &&a) noexcept: x(a.x) { cout << "A move constructor" << endl; }

public:
    virtual ~A() {
        cout << "A de constructor" << endl;
    }

    int x;
};


class B {
public:

    typedef stl::bidirectional_iterator_tag iterator_category;
    typedef int value_type;
    typedef int *pointer;
    typedef int &reference;
    typedef int *difference_type;

    B() = default;

    explicit B(int v) : x(v) { cout << "B default constructor" << endl; }

    B(const B &a) : x(a.x) { cout << "B copy constructor" << endl; }

//    B &operator=(B &&a) noexcept {
//        x = a.x;
//        cout << "B move assign constructor" << endl;
//    }
//
//    B(B &&a) noexcept: x(a.x) { cout << "B move constructor" << endl; }

public:
    virtual ~B() {
        cout << "B de constructor" << endl;
    }

    int x;
};


class C : public A {

public:
    int c;

    explicit C(int x, int y) : c(x), A(y) {}
};

class D : public A {
public:
    explicit D(int x) : A(x) {}
};

#endif //MYCPPSTL_BASE_TEST_H
