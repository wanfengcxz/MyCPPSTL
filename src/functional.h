//
// Created by 晚风吹行舟 on 2023/5/5.
//

#ifndef MYCPPSTL_FUNCTIONAL_H
#define MYCPPSTL_FUNCTIONAL_H

// 这个头文件包含了 mystl 的函数对象与哈希函数
// 仿函数（functors）是早期的命名，C++标准规则定案后所采用的的新名称是函数对象（function objects

#include <cstddef>

namespace stl {

    // 定义一元函数的参数型别和返回值型别
    template<class Arg, class Result>
    struct unary_function {
        typedef Arg argument_type;
        typedef Result result_type;
    };

    // 定义二元函数的参数型别的返回值型别
    template<class Arg1, class Arg2, class Result>
    struct binary_function {
        typedef Arg1 first_argument_type;
        typedef Arg2 second_argument_type;
        typedef Result result_type;
    };

    // 函数对象 加法
    template<class T>
    struct plus : public binary_function<T, T, T> {
        T operator()(const T &x, const T &y) const { return x + y; }
    };

    // 函数对象 减法
    template<class T>
    struct minus : public binary_function<T, T, T> {
        T operator()(const T &x, const T &y) const { return x - y; }
    };

    // 函数对象 乘法
    template<class T>
    struct multiplies : public binary_function<T, T, T> {
        T operator()(const T &x, const T &y) const { return x * y; }
    };

    // 函数对象 除法
    template<class T>
    struct divides : public binary_function<T, T, T> {
        T operator()(const T &x, const T &y) const { return x / y; }
    };

    // 函数对象 取模
    template<class T>
    struct modulus : public binary_function<T, T, T> {
        T operator()(const T &x, const T &y) const { return x % y; }
    };

    // 函数对象 否定
    template<class T>
    struct negate : public unary_function<T, T> {
        T operator()(const T &x) const { return -x; }
    };

    // 加法的证同元素
    // 所谓“运算op的证同元素”，意思是说数值A若与该元素做op运算，会得到A自己
    template<class T>
    T identity_element(plus<T>) { return T(0); }

    // 乘法的证同元素
    template<class T>
    T identity_element(multiplies<T>) { return T(1); }

    // 函数对象 等于
    template<class T>
    struct equal_to : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x == y; }
    };

    // 函数对象 不等于
    template<class T>
    struct not_equal_to : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x != y; }
    };

    // 函数对象 大于
    template<class T>
    struct greater : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x > y; }
    };

    // 函数对象 小于
    template<class T>
    struct less : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x < y; }
    };

    // 函数对象 大于等于
    template<class T>
    struct greater_equal : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x >= y; }
    };

    // 函数对象 小于等于
    template<class T>
    struct less_equal : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x <= y; }
    };

    // 函数对象 逻辑与
    template<class T>
    struct logical_and : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x && y; }
    };

    // 函数对象 逻辑或
    template<class T>
    struct logical_or : public binary_function<T, T, bool> {
        bool operator()(const T &x, const T &y) const { return x || y; }
    };

    // 函数对象 逻辑非
    template<class T>
    struct logical_not : public unary_function<T, bool> {
        bool operator()(const T &x) const { return !x; }
    };

    // 证同函数 不改变元素，返回本身
    template<class T>
    // TODO: ??? 为什么是bool 应该是const T&
    struct identity : public unary_function<T, bool> {
        const T &operator()(const T &x) const { return x; }
    };

    // 选择函数 接受一个pair，返回第一个元素
    template<class Pair>
    struct selectfirst : public unary_function<Pair, typename Pair::first_type> {
        const typename Pair::first_type &operator()(const Pair &x) const {
            return x.first;
        }
    };

    // 选择函数 接受一个 pair，返回第二个元素
    template<class Pair>
    struct selectsecond : public unary_function<Pair, typename Pair::second_type> {
        const typename Pair::second_type &operator()(const Pair &x) const {
            return x.second;
        }
    };

    // 投射函数
    // z返回第一参数
    template<class Arg1, class Arg2>
    struct projectfirst : public binary_function<Arg1, Arg2, Arg1> {
        Arg1 operator()(const Arg1 &x, const Arg2 &) const { return x; }
    };

    // 投射函数 返回第二参数
    template<class Arg1, class Arg2>
    struct projectsecond : public binary_function<Arg1, Arg2, Arg1> {
        Arg2 operator()(const Arg1 &, const Arg2 &y) const { return y; }
    };

/*****************************************************************************************/
// 哈希函数对象

    template<class Key>
    struct hash {
    };

    // 针对指针的偏特化版本
    template<class T>
    struct hash<T *> {
        size_t operator()(T *p) const noexcept {
            return reinterpret_cast<size_t>(p);
        }
    };

// 对于整数类型，返回原值
// 全特化
#define STL_TRIVIAL_HASH_FCN(Type) \
template <> struct hash<Type>                \
{                                            \
  size_t operator()(Type val) const noexcept \
  { return static_cast<size_t>(val); }       \
};

    STL_TRIVIAL_HASH_FCN(bool)

    STL_TRIVIAL_HASH_FCN(char)

    STL_TRIVIAL_HASH_FCN(signed char)

    STL_TRIVIAL_HASH_FCN(unsigned char)

    STL_TRIVIAL_HASH_FCN(wchar_t)

    STL_TRIVIAL_HASH_FCN(char16_t)

    STL_TRIVIAL_HASH_FCN(char32_t)

    STL_TRIVIAL_HASH_FCN(short)

    STL_TRIVIAL_HASH_FCN(unsigned short)

    STL_TRIVIAL_HASH_FCN(int)

    STL_TRIVIAL_HASH_FCN(unsigned int)

    STL_TRIVIAL_HASH_FCN(long)

    STL_TRIVIAL_HASH_FCN(unsigned long)

    STL_TRIVIAL_HASH_FCN(long long)

    STL_TRIVIAL_HASH_FCN(unsigned long long)

#undef  STL_TRIVIAL_HASH_FCN

    // 对于浮点数，逐位哈希
    inline size_t bitwise_hash(const unsigned char *first, size_t count) {
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) && __SIZEOF_POINTER__ == 8)
        const size_t fnv_offset = 14695981039346656037ull;
        const size_t fnv_prime = 1099511628211ull;
#else
        const size_t fnv_offset = 2166136261u;
        const size_t fnv_prime = 16777619u;
#endif
        size_t result = fnv_offset;
        // TODO:在做什么？
        for (size_t i = 0; i < count; ++i) {
            result ^= (size_t) first[i];
            result *= fnv_prime;
        }
        return result;
    }

    template<>
    struct hash<float> {
        size_t operator()(const float &val) {
            // TODO:为什么static_cast不行？
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char *) (&val), sizeof(float));
        }
    };

    template<>
    struct hash<double> {
        size_t operator()(const double &val) {
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char *) (&val), sizeof(double));
        }
    };

    template<>
    struct hash<long double> {
        size_t operator()(const long double &val) {
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char *) &val, sizeof(long double));
        }
    };

}

#endif //MYCPPSTL_FUNCTIONAL_H
