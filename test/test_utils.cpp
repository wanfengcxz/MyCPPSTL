//
// Created by 晚风吹行舟 on 2023/4/12.
//
#include <iostream>
#include <string>

#include "utils.h"

using std::cout;
using std::endl;

class A {
public:
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

void test_move() {
    // 引用折叠
    // T -> int
    cout << stl::move<int>(2) << endl;

    int x(4);
    int &y = x;
    // T -> int& && -> int &
    // 此处T必须为int & 因为int &&不能接受X
    cout << stl::move(x) << endl;

    int &&z = 3;
    // T -> int && && -> int&&
    cout << stl::move(x) << endl;

    std::string s = "123";
    std::string s2 = stl::move(s);
    cout << s << endl;
    cout << s2 << endl;

//    std::string s3 = static_cast<std::string&&>(s2);
    static_cast<std::string &&>(s2); // 这是一个右值
    cout << s2 << endl;         // s2中的内容并没有被移走

    A m = 2;
    A n = stl::move(m);
}

namespace test_forward {
    template<class T>
    // 完美转发
    T &&forward(typename std::remove_reference<T>::type &arg) noexcept {
        if (std::is_same<T, int>::value) cout << "type &arg: T is int" << endl;
        return static_cast<T &&>(arg);
    }

    template<class T>
    // 如果是个左值，肯定不会传入这个函数
    T &&forward(typename std::remove_reference<T>::type &&arg) noexcept {
        if (std::is_same<T, int>::value) cout << "type &&arg: T is int" << endl;
        static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
        return static_cast<T &&>(arg);
    }


    void test_forward() {
        int x = 3;

        // error 因为右边是右值
//        int &y = (std::forward<int>(x));
//        int *p1 = &(std::forward<int>(x));
//        int *p2 = &(std::forward<int>(2));
        int &&z = std::forward<int>(x);
        int &&n = std::forward<int>(3);

        // 解释如下：
        int a = 4;
        forward<int>(a);
        forward<int>(3);
        /*
         *  以上代码分别输出：
         *      type &arg: T is int
         *      type &&arg: T is int
         *  可见两个forward中T都被推导为了int，因此在return时，
         *  会通过static_cast<int&&>被转化为右值，因此无法被取地址
         */
    }
}

void test_swap() {
    const int N = 4;
    A arr2[N] = {1, 2, 3, 4};
    A arr3[N] = {5, 6, 7, 8};
    stl::swap<A, 4>(arr2, arr3); // 1 OK 2 OK
//    stl::swap<A,5>(arr2, arr3);      // 1 error 2 OK
    stl::swap(arr2, arr3);      // 1 OK 2 error
    // 分析原因,1中用的是Tp(&a)[N],推导时发现有N这个参数,因此,
    // 没有<>显式指定就可以,而且如果数组长度指定错误也会报错
    // 所以2这个版本更安全,而且还能省去显式指定,更方便.

    for (A i: arr2) cout << i.x << " ";
    cout << endl;
};

template<class = int>
void fun1(int x) {
    cout << x << endl;
}

void InputPair(stl::pair<int, double> x) {

}

void test_pair() {

    stl::pair<int, int> a1;
    stl::pair<A, A> a2;
    stl::pair<A, A> a21;

    int x1 = 1, y1 = 2;
    int &x2 = x1, &y2 = y1;
    stl::pair<int, int> a4 = {x1, y1};
    stl::pair<int, int> a5(x2, y2);

    stl::pair<int, int> a6(1, 2);
    stl::pair<int, int> a7(1U, 3);

    stl::pair<int, double> a8(a6);

    InputPair({1, 2});
    InputPair(stl::pair(1, 2));

//    a8 = a4;
//    a8 = stl::move(a4);

//    a5 = a4;
//    a5 = stl::move(a4);

//    a2 = a4;
    a2 = stl::move(a4);

//    a2 = stl::move(a21);

    bool flag = a4 == a5;

    stl::pair<int, int> x = stl::make_pair(2, 4);
}


void other() {
    int arr[2] = {1, 2};
    int (*a)[2] = &arr;
    int (&b)[2] = arr;
}

int main() {

//    A &&a = 2;  // A default constructor
//
//    test_move();

    test_forward::test_forward();

    return 0;
}