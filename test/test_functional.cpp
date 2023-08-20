//
// Created by 晚风吹行舟 on 2023/8/20.
//

#include "functional.h"
#include "gtest/gtest.h"

using namespace std;

struct Num {
    int data;

    explicit Num(int x) : data(x) {}

    Num operator+(const Num &other) const {
        return Num(other.data + data);
    }

    bool operator==(const Num &other) const {
        return data == other.data;
    }
};

TEST(functional, add) {
    EXPECT_EQ(stl::plus<int>()(3, 5), 8);

    Num num1{3}, num2{5}, num3{8};
    EXPECT_EQ(stl::plus<Num>()(num1, num2), num3);
}

TEST(functional, hash){
    EXPECT_EQ(stl::hash<int>()(5), size_t(5));
    EXPECT_EQ(stl::hash<char>()('A'), size_t('A'));
}

int main() {

    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}