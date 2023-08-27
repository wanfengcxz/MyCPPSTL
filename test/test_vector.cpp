//
// Created by 晚风吹行舟 on 2023/4/9.
//

#include <vector>
#include <string>
#include <iostream>

#include "vector.h"
#include "iterator.h"
#include "gtest/gtest.h"
//#include "easylogging++.h"
//
//_INITIALIZE_EASYLOGGINGPP

using std::cout;
using std::endl;


class StlVectorIntTest : public testing::Test {
protected:
    virtual void SetUp() {
        for (int i = 0; i < 5; ++i) a[i] = i + 1;
        for (int i = 0; i < 20; ++i) b[i] = 20;
    }

    stl::vector<int> v1;
    int a[5];
    int b[20];
};


TEST_F(StlVectorIntTest, iterator) {
    using stl::vector;

    vector<int> vec1;
    using vec_int_iterator_category = typename stl::iterator_traits<
            typename vector<int>::iterator>::iterator_category;
    bool flag = stl::is_same<vec_int_iterator_category, stl::random_access_iterator_tag>::value;
    // vector<int>::iterator 类型为 random_access_iterator_tag
    // 在vector中，iterator定义 typedef value_type *iterator即指针，指针本身就是random access
    // 在iterator_traits中，指针被偏特化为 random_access_iterator_tag
    // 因此就不需要重载各种运算符：++ -- ->
    EXPECT_TRUE(flag);

    v1.assign(a, a + 5);
    EXPECT_EQ(v1.front(), 1);
    EXPECT_EQ(v1.back(), 5);
    EXPECT_EQ(*v1.begin(), 1);
    EXPECT_EQ(*v1.cbegin(), 1);
    EXPECT_EQ(*(v1.end() - 1), 5);
    EXPECT_EQ(*(v1.cend() - 1), 5);

    int *p = v1.data();
    EXPECT_EQ(*p, 1);
    ++p;
    EXPECT_EQ(*p, 2);
    *p = 10;
    EXPECT_EQ(*p, 10);
}

TEST_F(StlVectorIntTest, init) {

    int a[] = {1, 2, 3, 4, 5};

    stl::vector<int> v1;
    EXPECT_EQ(v1.size(), 0);
    EXPECT_EQ(v1.capacity(), 16);

    stl::vector<int> v2(10);
    EXPECT_EQ(v2.size(), 10);
    EXPECT_EQ(v2.capacity(), 16);
    EXPECT_EQ(v2[0], 0);


    stl::vector<int> v3(10, 1);
    EXPECT_EQ(v3.size(), 10);
    EXPECT_EQ(v3.capacity(), 16);
    EXPECT_EQ(v3[0], 1);

    stl::vector<int> v4(a, a + 5);
    EXPECT_EQ(v4.size(), 5);
    EXPECT_EQ(v4.capacity(), 16);
    EXPECT_EQ(v4[4], 5);

    stl::vector<int> v5(v2);
    EXPECT_EQ(v5.size(), 10);
    EXPECT_EQ(v5.capacity(), 16);
    EXPECT_EQ(v5[0], 0);

    stl::vector<int> v6(std::move(v2));
    EXPECT_EQ(v6.size(), 10);
    EXPECT_EQ(v6.capacity(), 16);
    EXPECT_EQ(v6[0], 0);
    EXPECT_EQ(v2.size(), 0);
    EXPECT_EQ(v2.capacity(), 0);
    EXPECT_EQ(v2.begin(), nullptr);
    EXPECT_EQ(v2.end(), nullptr);

    stl::vector<int> v7{1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(v7.size(), 9);
    EXPECT_EQ(v7.capacity(), 16);
    EXPECT_EQ(v7[7], 8);
    EXPECT_EQ(v7.front(), 1);
    EXPECT_EQ(v7.back(), 9);
    EXPECT_EQ(v7.at(1), 2);

    stl::vector<int> v8, v9, v10;
    EXPECT_EQ(v8.size(), 0);

    v8 = v3;
    EXPECT_EQ(v8.size(), 10);
    EXPECT_EQ(v8.capacity(), 16);
    EXPECT_EQ(v8[0], 1);

    v9 = std::move(v3);
    EXPECT_EQ(v9.size(), 10);
    EXPECT_EQ(v9.capacity(), 16);
    EXPECT_EQ(v9[0], 1);
    EXPECT_EQ(v3.size(), 0);
    EXPECT_EQ(v3.capacity(), 0);
    EXPECT_EQ(v3.begin(), nullptr);

    v10 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(v10.size(), 9);
    EXPECT_EQ(v10.capacity(), 16);
    EXPECT_EQ(v10.at(5), 6);
}

TEST_F(StlVectorIntTest, assign) {

    v1.assign(8, 8);
    EXPECT_EQ(v1.size(), 8);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1[2], 8);

    v1.assign(a, a + 5);
    EXPECT_EQ(v1.size(), 5);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1[0], 1);
    EXPECT_EQ(v1.back(), 5);

    stl::vector<int> v2;
    v2.assign(b, b + 20);
    EXPECT_EQ(v2.size(), 20);
    EXPECT_EQ(v2.capacity(), 20);
}

TEST_F(StlVectorIntTest, emplace) {
    v1.assign(a, a + 5);

    v1.emplace(v1.begin(), 0);
    EXPECT_EQ(v1.size(), 6);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1.front(), 0);
    EXPECT_EQ(v1[1], 1);
    EXPECT_EQ(v1.back(), 5);

    v1.emplace_back(6);
    EXPECT_EQ(v1.size(), 7);
    EXPECT_EQ(v1.back(), 6);

    v1.push_back(7);
    EXPECT_EQ(v1.size(), 8);
    EXPECT_EQ(v1.back(), 7);

    v1.pop_back();
    EXPECT_EQ(v1.size(), 7);
    EXPECT_EQ(v1.back(), 6);
    EXPECT_EQ(v1[0], 0);
}

TEST_F(StlVectorIntTest, insert) {
    v1.assign(a, a + 5);

    v1.insert(v1.end(), 6);
    EXPECT_EQ(v1.size(), 6);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1.back(), 6);

    v1.insert(v1.begin() + 2, 2, 10);
    EXPECT_EQ(v1.size(), 8);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1[0], 1);
    EXPECT_EQ(v1[1], 2);
    EXPECT_EQ(v1[2], 10);
    EXPECT_EQ(v1[3], 10);
    EXPECT_EQ(v1[4], 3);
    EXPECT_EQ(v1.back(), 6);

    v1.insert(v1.begin(), b, b + 20);
    EXPECT_EQ(v1.size(), 28);
    EXPECT_EQ(v1.capacity(), 36);
    EXPECT_EQ(v1[0], 20);
    EXPECT_EQ(v1[19], 20);
    EXPECT_EQ(v1[20], 1);
}

TEST_F(StlVectorIntTest, erase) {
    v1.assign(a, a + 5);

    v1.erase(v1.begin());
    EXPECT_EQ(v1.size(), 4);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1[0], 2);

    v1.erase(v1.begin() + 1, v1.begin() + 2);
    EXPECT_EQ(v1.size(), 3);
    EXPECT_EQ(v1[0], 2);
    EXPECT_EQ(v1[1], 4);
    EXPECT_EQ(v1[2], 5);
}

TEST_F(StlVectorIntTest, reverse) {
    v1.assign(a, a + 5);

    v1.reverse();
    EXPECT_EQ(v1.size(), 5);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1[0], 5);
    EXPECT_EQ(v1[4], 1);
}

TEST_F(StlVectorIntTest, swap) {
    v1.assign(a, a + 5);

    stl::vector<int> v2, v3{1, 2, 3};

    stl::swap(v1, v2);
    EXPECT_EQ(v1.size(), 0);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v2.size(), 5);
    EXPECT_EQ(v2.capacity(), 16);
    EXPECT_EQ(v2[0], 1);
    EXPECT_EQ(v2[4], 5);

    v2.swap(v3);
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2.capacity(), 16);
    EXPECT_EQ(v2[0], 1);
    EXPECT_EQ(v2[2], 3);
}

TEST_F(StlVectorIntTest, shrink_to_fit) {
    v1.assign(a, a + 5);

    v1.shrink_to_fit();
    EXPECT_EQ(v1.size(), 5);
    EXPECT_EQ(v1.capacity(), 5);
}

class A {
public:
    int data_{0};

    A() = default;

    A(int data) : data_(data) {}

    A(const A &a) {
        data_ = a.data_;
    }

    A(A &&a) {
        data_ = a.data_;
        a.data_ = 0;
    }

    A &operator=(A &&a) {
        data_ = a.data_;
        a.data_ = 0;
    }

    A &operator=(const A &a) {
        data_ = a.data_;
    }
};

class StlVectorClassATest : public testing::Test {
protected:
    virtual void SetUp() {
        for (int i = 0; i < 5; ++i) a[i] = i + 1;
        for (int i = 0; i < 20; ++i) b[i] = 20;
    }

    stl::vector<A> v1;
    int a[5];
    int b[20];
};

TEST_F(StlVectorClassATest, init) {
    stl::vector<A> v1;
    EXPECT_EQ(v1.size(), 0);
    EXPECT_EQ(v1.capacity(), 16);

    stl::vector<A> v2(10);
    EXPECT_EQ(v2.size(), 10);
    EXPECT_EQ(v2.capacity(), 16);
    EXPECT_EQ(v2[0].data_, 0);

    stl::vector<A> v3(10, A(1));
    EXPECT_EQ(v3.size(), 10);
    EXPECT_EQ(v3.capacity(), 16);
    EXPECT_EQ(v3[0].data_, 1);

    // 如果A的构造函数不使用explicit，此处就可以用{1,2,3}
    stl::vector<A> v4{A(1), A(2), A(3)};
    EXPECT_EQ(v4.size(), 3);
    EXPECT_EQ(v4.capacity(), 16);
    EXPECT_EQ(v4[1].data_, 2);
    EXPECT_EQ(v4.front().data_, 1);
    EXPECT_EQ(v4.back().data_, 3);

    stl::vector<A> v5(v2);
    EXPECT_EQ(v5.size(), 10);
    EXPECT_EQ(v5.capacity(), 16);
    EXPECT_EQ(v5[0].data_, 0);

    stl::vector<A> v6(std::move(v2));
    EXPECT_EQ(v6.size(), 10);
    EXPECT_EQ(v6.capacity(), 16);
    EXPECT_EQ(v6[0].data_, 0);
    EXPECT_EQ(v2.size(), 0);
    EXPECT_EQ(v2.capacity(), 0);
    EXPECT_EQ(v2.begin(), nullptr);
    EXPECT_EQ(v2.end(), nullptr);

    stl::vector<A> v8, v9, v10;
    EXPECT_EQ(v8.size(), 0);

    v8 = v3;
    EXPECT_EQ(v8.size(), 10);
    EXPECT_EQ(v8.capacity(), 16);
    EXPECT_EQ(v8[0].data_, 1);

    v9 = std::move(v3);
    EXPECT_EQ(v9.size(), 10);
    EXPECT_EQ(v9.capacity(), 16);
    EXPECT_EQ(v9[0].data_, 1);
    EXPECT_EQ(v3.size(), 0);
    EXPECT_EQ(v3.capacity(), 0);
    EXPECT_EQ(v3.begin(), nullptr);

    v10 = {A(1), A(2)};
    EXPECT_EQ(v10.size(), 2);
    EXPECT_EQ(v10.capacity(), 16);
    EXPECT_EQ(v10.at(0).data_, 1);
}

TEST_F(StlVectorClassATest, assign) {

    v1.assign(8, A(8));
    EXPECT_EQ(v1.size(), 8);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1[2].data_, 8);

    v1.assign(a, a + 5);
    EXPECT_EQ(v1.size(), 5);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1[0].data_, 1);
    EXPECT_EQ(v1.back().data_, 5);

    stl::vector<A> v2;
    v2.assign(b, b + 20);
    EXPECT_EQ(v2.size(), 20);
    EXPECT_EQ(v2.capacity(), 20);
}

TEST_F(StlVectorClassATest, emplace) {
    v1.assign(a, a + 5);

    v1.emplace(v1.begin(), 0);
    EXPECT_EQ(v1.size(), 6);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1.front().data_, 0);
    EXPECT_EQ(v1[1].data_, 1);
    EXPECT_EQ(v1.back().data_, 5);

    v1.emplace_back(6);
    EXPECT_EQ(v1.size(), 7);
    EXPECT_EQ(v1.back().data_, 6);

    v1.push_back(7);
    EXPECT_EQ(v1.size(), 8);
    EXPECT_EQ(v1.back().data_, 7);

    v1.pop_back();
    EXPECT_EQ(v1.size(), 7);
    EXPECT_EQ(v1.back().data_, 6);
    EXPECT_EQ(v1[0].data_, 0);
}

class StlVectorStringTest : public testing::Test {
protected:

    virtual void SetUp() {
        for (int i = 0; i < 5; ++i) a[i] = std::to_string(i + 1);
        for (int i = 0; i < 20; ++i) b[i] = std::to_string(20);
    }

    stl::vector<std::string> v1;
    std::string a[5];
    std::string b[20];
};


TEST_F(StlVectorStringTest, init) {
    using std::string;

    stl::vector<string> v1;
    EXPECT_EQ(v1.size(), 0);
    EXPECT_EQ(v1.capacity(), 16);

    stl::vector<string> v2(10);
    EXPECT_EQ(v2.size(), 10);
    EXPECT_EQ(v2.capacity(), 16);
    EXPECT_TRUE(v2[0].empty());

    EXPECT_STREQ(v2[0].c_str(), "");    // 比较c风格的字符串 即char*
    EXPECT_EQ(v2[0], "");               // 比较std::string的字符串 利用operator==

    stl::vector<string> v3(5, "123abc");
    EXPECT_EQ(v3.size(), 5);
    EXPECT_EQ(v3.capacity(), 16);
    EXPECT_EQ(v3[0], "123abc");

    // 如果A的构造函数不使用explicit，此处就可以用{1,2,3}
    stl::vector<string> v4{string("1"), string("2"), string("3")};
    EXPECT_EQ(v4.size(), 3);
    EXPECT_EQ(v4.capacity(), 16);
    EXPECT_EQ(v4[1], "2");
    EXPECT_EQ(v4.front(), "1");
    EXPECT_EQ(v4.back(), "3");

    stl::vector<string> v5(v2);
    EXPECT_EQ(v5.size(), 10);
    EXPECT_EQ(v5.capacity(), 16);
    EXPECT_EQ(v5[0], "");

    stl::vector<string> v6(std::move(v4));
    EXPECT_EQ(v6.size(), 3);
    EXPECT_EQ(v6.capacity(), 16);
    EXPECT_EQ(v6[0], "1");
    EXPECT_EQ(v6[1], "2");

    EXPECT_EQ(v4.size(), 0);
    EXPECT_EQ(v4.capacity(), 0);
    EXPECT_EQ(v4.begin(), nullptr);
    EXPECT_EQ(v4.end(), nullptr);

    stl::vector<string> v8, v9, v10;
    EXPECT_EQ(v8.size(), 0);

    v8 = v3;
    EXPECT_EQ(v8.size(), 5);
    EXPECT_EQ(v8.capacity(), 16);
    EXPECT_EQ(v8[0], "123abc");

    v9 = std::move(v3);
    EXPECT_EQ(v9.size(), 5);
    EXPECT_EQ(v9.capacity(), 16);
    EXPECT_EQ(v9[0], "123abc");
    EXPECT_EQ(v3.size(), 0);
    EXPECT_EQ(v3.capacity(), 0);
    EXPECT_EQ(v3.begin(), nullptr);

    v10 = {string("1"), string("2")};
    EXPECT_EQ(v10.size(), 2);
    EXPECT_EQ(v10.capacity(), 16);
    EXPECT_EQ(v10.at(0), "1");
}

TEST_F(StlVectorStringTest, assign) {
    using std::string;

    v1.assign(8, string("abcd"));
    EXPECT_EQ(v1.size(), 8);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1[2], "abcd");


    v1.assign(a, a + 5);
    EXPECT_EQ(v1.size(), 5);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1[0], "1");
    EXPECT_EQ(v1.back(), "5");

    stl::vector<string> v2;
    v2.assign(b, b + 20);
    EXPECT_EQ(v2.size(), 20);
    EXPECT_EQ(v2.capacity(), 20);
}

TEST_F(StlVectorStringTest, emplace) {
    using std::string;

    v1.assign(a, a + 5);

    v1.emplace(v1.begin(), "0");
    EXPECT_EQ(v1.size(), 6);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1.front(), "0");
    EXPECT_EQ(v1[1], "1");
    EXPECT_EQ(v1.back(), "5");

    v1.emplace_back("6");
    EXPECT_EQ(v1.size(), 7);
    EXPECT_EQ(v1.back(), "6");

    v1.push_back("7");
    EXPECT_EQ(v1.size(), 8);
    EXPECT_EQ(v1.back(), "7");

    v1.pop_back();
    EXPECT_EQ(v1.size(), 7);
    EXPECT_EQ(v1.back(), "6");
    EXPECT_EQ(v1[0], "0");
}

TEST_F(StlVectorStringTest, insert) {
    using std::string;

    v1.assign(a, a + 5);

    v1.insert(v1.end(), "6");
    EXPECT_EQ(v1.size(), 6);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1.back(), "6");

    v1.insert(v1.begin() + 2, 2, "10");
    EXPECT_EQ(v1.size(), 8);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1[0], "1");
    EXPECT_EQ(v1[1], "2");
    EXPECT_EQ(v1[2], "10");
    EXPECT_EQ(v1[3], "10");
    EXPECT_EQ(v1[4], "3");
    EXPECT_EQ(v1.back(), "6");

    v1.insert(v1.begin(), b, b + 20);
    EXPECT_EQ(v1.size(), 28);
    EXPECT_EQ(v1.capacity(), 36);
    EXPECT_EQ(v1[0], "20");
    EXPECT_EQ(v1[19], "20");
    EXPECT_EQ(v1[20], "1");
}

TEST_F(StlVectorStringTest, erase) {
    using std::string;

    v1.assign(a, a + 5);

    v1.erase(v1.begin());
    EXPECT_EQ(v1.size(), 4);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1[0], "2");

    v1.erase(v1.begin() + 1, v1.begin() + 2);
    EXPECT_EQ(v1.size(), 3);
    EXPECT_EQ(v1[0], "2");
    EXPECT_EQ(v1[1], "4");
    EXPECT_EQ(v1[2], "5");
}

TEST_F(StlVectorStringTest, reverse) {
    using std::string;

    v1.assign(a, a + 5);

    v1.reverse();
    EXPECT_EQ(v1.size(), 5);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v1[0], "5");
    EXPECT_EQ(v1[4], "1");
}

TEST_F(StlVectorStringTest, swap) {
    using std::string;

    v1.assign(a, a + 5);

    stl::vector<string> v2, v3{string("1"), string("2"), string("3")};

    stl::swap(v1, v2);
    EXPECT_EQ(v1.size(), 0);
    EXPECT_EQ(v1.capacity(), 16);
    EXPECT_EQ(v2.size(), 5);
    EXPECT_EQ(v2.capacity(), 16);
    EXPECT_EQ(v2[0], "1");
    EXPECT_EQ(v2[4], "5");

    v2.swap(v3);
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2.capacity(), 16);
    EXPECT_EQ(v2[0], "1");
    EXPECT_EQ(v2[2], "3");
}

TEST_F(StlVectorStringTest, shrink_to_fit) {
    v1.assign(a, a + 5);

    v1.shrink_to_fit();
    EXPECT_EQ(v1.size(), 5);
    EXPECT_EQ(v1.capacity(), 5);
}


TEST(vector, const_iterator) {
    // const iterator 和 const_iterator

    std::vector<int> vec = {1, 2, 3};

    // const iterator意味着iterator的内容不能改变，即其中的指针不能动，但可以通过指针修改它指向的内容
    const std::vector<int>::iterator it1 = vec.begin();
    *it1 = 3;
//    it1++;      // error

    // const_iterator 其中指针是可以改变的 但是不能通过指针改变其指向的内容 可以理解为const int*
    std::vector<int>::const_iterator it2 = vec.cbegin();
//    *it2 = 3; // error
    it2++;


    const std::vector<int> vec2 = {1, 2, 3};

    // const vec2返回的是const_iterator  begin()有两个版本
    std::vector<int>::const_iterator it3 = vec2.begin();
//    *it3 = 5;     // error vec2是const，不能改变
    it3++;
}


TEST(other, const_ref) {
    int a = 2;
    const int &b = a;
    a = 3;
    EXPECT_EQ(b, 3);
}

TEST(other, const_cast_) {
    int a = 4;
    const int &a_const_ref = a;
    // a必须是非const才能这样做 否则是未定义行为
    int &a_ref = const_cast<int &>(a_const_ref);
    a_ref = 5;
    EXPECT_EQ(a, 5);
}

TEST(other, int64_max) {
    // 以8位二进制来表示的话 -1的补码是11111111
    EXPECT_EQ(18446744073709551615, static_cast<size_t>(-1));
}

int main() {

    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

