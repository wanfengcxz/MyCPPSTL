//
// Created by 晚风吹行舟 on 2023/8/24.
//

#include <iostream>
#include <initializer_list>
#include "gtest/gtest.h"

using std::cout;
using std::endl;

class ArrayInt {
public:
    int len_;
    int *p_;
public:

    explicit ArrayInt() : p_(nullptr), len_(0) {}

    explicit ArrayInt(int len) : p_(new int[len_]), len_(len) {
        for (int i = 0; i < len_; ++i) p_[i] = 0;
    }

    template<class ...Args>
    ArrayInt(std::initializer_list<int> il):len_(il.size()), p_(new int[len_]) {
        auto first = il.begin();
        for (int i = 0; i < len_; ++i, ++first) p_[i] = *first;
    }

    ArrayInt(const ArrayInt &arr) {
        p_ = nullptr;
        len_ = 0;
        if (arr.len_ <= 0) return;

        len_ = arr.len_;
        p_ = new int[len_];
        memcpy(p_, arr.p_, len_);
    }


    ArrayInt &operator=(const ArrayInt &arr) {
        if (this == &arr) return *this;
        delete[] p_;        // delete之后p不为nullptr
        p_ = nullptr;
        len_ = 0;
        if (arr.len_ <= 0) return *this;

        len_ = arr.len_;
        p_ = new int[len_];
        memcpy(p_, arr.p_, len_);
        return *this;
    }

};

class Base {
    int a;
};

class Base2 {
    int a{};

    Base2() { a = 3; };
};

TEST(Trivial, TriviallyConstructible) {
    EXPECT_FALSE(std::is_trivially_constructible<ArrayInt>::value);
    EXPECT_TRUE(std::is_trivially_constructible<Base>::value);
}

TEST(Trivial, TriviallyCopyable) {
    EXPECT_FALSE(std::is_trivially_copyable<ArrayInt>::value);
    EXPECT_TRUE(std::is_trivially_copyable<Base>::value);
}

TEST(Trivial, TriviallyCopyAssign) {
    EXPECT_FALSE(std::is_trivially_copy_assignable<ArrayInt>::value);
    EXPECT_TRUE(std::is_trivially_copy_assignable<Base>::value);
    EXPECT_TRUE(std::is_trivially_copy_assignable<Base>::value);
}

TEST(New, SizeOf) {
    int a[4] = {1, 2, 3, 4};
    ASSERT_EQ(sizeof(a), 16);   // 取这个数组的大小

    // 初始化列表
    int *p = new int[4]{1, 2, 3, 4};
    ASSERT_EQ(sizeof(p), 8);    // 取指针的大小  动态分配的内存无法统计
}

TEST(New, OperatorNew) {
    // 平时用的new就是全局new
    //创建并初始化拥有动态存储期的对象，这些对象的生存期不受它们创建时所在的作用域限制。
    int *p1 = ::new int[4]{1, 2, 3, 4};
    int *p2 = new int[4]{1, 2, 3, 4};

    int *p3 = static_cast<int *> (::operator new(2 * sizeof(int)));
    p3[0] = p3[1] = 0;
    char *p_ch1 = reinterpret_cast<char *>(p3);

    // 在p_ch1指向的地方初始化一个字符a
    ::new((void *) (p_ch1))char('a');
    ::new((void *) (p_ch1 + 1))char(0);
    ::new((void *) (p_ch1 + 2))char(0);
    ::new((void *) (p_ch1 + 3))char(0);

    EXPECT_EQ(*p_ch1, 97);
    EXPECT_EQ(*p3, 97);     // 小端模式
}

void test() {
    const int n = 3;
    auto *p = static_cast<ArrayInt *>(::operator new(n * sizeof(ArrayInt)));
    cout << "sizeof ArrayInt: " << sizeof(ArrayInt) << endl;

    // 到目前为止只是分配了字节  如果是平凡可拷贝，那么直接可以赋值
    // e.g.
    int *p3 = static_cast<int *> (::operator new(2 * sizeof(int)));
    p3[0] = p3[1] = 0;

    // 但是如果不是平凡可拷贝，需要初始化
    auto tmp = ArrayInt(2);
    *p = tmp;
    EXPECT_EQ((*p).len_, 2);
}


int main() {

    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();

    return 0;
}
