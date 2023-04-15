//
// Created by 晚风吹行舟 on 2023/4/10.
//
#include <iostream>
#include "iterator.h"


using namespace std;


class A {
public:

    typedef stl::bidirectional_iterator_tag iterator_category;
    typedef int value_type;
    typedef int *pointer;
    typedef int &reference;
    typedef int *difference_type;

    A() = default;

    A(int v) : x(v) { cout << "A default constructor" << endl; }

    A(const A &a) : x(a.x) { cout << "copy constructor" << endl; }

    A &operator=(A &&a) noexcept {
        x = a.x;
        cout << "move assign constructor" << endl;
    }

    A(A &&a) noexcept: x(a.x) { cout << "move constructor" << endl; }

public:
    virtual ~A() {
        cout << "de constructor" << endl;
    }

    int x;
};


struct two {
    char a;
    char b;
    char c;
    int x;
};

static two test(...);

template<typename T, bool x>
int fun(T a) {
    return 3;
}

void test_has_iterator_cat() {
    std::cout << std::boolalpha;

//    auto t = stl::has_iterator_cat<int>::test<int>(0);  // t:two
//    auto t2 = stl::has_iterator_cat<A>::test<A>(0);     // t2:char

    std::cout << stl::has_iterator_cat<int>::value << std::endl;
    std::cout << stl::has_iterator_cat<A>::value << std::endl;

}


void test_iterator_traits() {
    std::cout << std::boolalpha;

    // 对于内置类型，会使用偏特化版本的推断，得到random_access_iterator_tag
    // 因为对于内置类型的指针，都是可以随机访问的
    typedef stl::iterator_traits<int *>::iterator_category random_access_t1;
    std::cout << stl::is_same<random_access_t1, stl::random_access_iterator_tag>::value << std::endl;

    typedef stl::iterator_traits<const double *>::iterator_category random_access_t2;
    std::cout << stl::is_same<random_access_t2, stl::random_access_iterator_tag>::value << std::endl;

//    typedef stl::iterator_traits<double>::iterator_category random_access_t3; // error double没有iterator_category属性
//    std::cout << stl::is_same<random_access_t3, stl::random_access_iterator_tag>::value << std::endl;

    /*
     * 如果A不是内置类型，那么只有当类A中有以下五个属性时：
     * iterator_category;
     * value_type;
     * pointer;
     * reference;
     * difference_type;
     * 才会推断成功
     */
    typedef stl::iterator_traits<A>::iterator_category random_access_t3;

}

int main() {

//    cout << sizeof(test(0)) << endl;
    test_has_iterator_cat();
//    test_iterator_traits();

    return 0;
}