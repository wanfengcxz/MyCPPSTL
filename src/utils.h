//
// Created by 晚风吹行舟 on 2023/4/12.
//

#ifndef MYCPPSTL_UTILS_H
#define MYCPPSTL_UTILS_H

// 这个文件包含一些通用工具，包括move,forward,swap,pair等函数

#include <iostream>
#include <cstddef>
#include "type_traits.h"


namespace stl {

    // move
    template<class T>
    // T&&是一个指向模板类型参数的右值引用，通过引用折叠，此参数可以与任何类型的实参匹配
    // (可以传递左值或右值)
    typename std::remove_reference<T>::type &&move(T &&arg) noexcept {
        return static_cast<typename std::remove_reference<T>::type &&>(arg);
    }

    // forward
    template<class T>
    // 完美转发
    T &&forward(typename std::remove_reference<T>::type &arg) noexcept {
        std::cout << "T& forward" << std::endl;
        return static_cast<T &&>(arg);
    }

    template<class T>
    T &&forward(typename std::remove_reference<T>::type &&arg) noexcept {
        std::cout << "T&& forward" << std::endl;
        static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
        return static_cast<T &&>(arg);
    }

    // swap
    template<class Tp>
    void swap(Tp &lhs, Tp &rhs) {
        auto tmp(stl::move(lhs));
        lhs = stl::move(rhs);
        rhs = stl::move(tmp);
    }

    template<class ForwardIter1, class ForwardIter2>
    ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2) {
        // TODO: ????? (void)
        for (; first1 != last1; ++first1, (void) ++first2)
            stl::swap(*first1, *first2);
        return first2;
    }

    template<class Tp, size_t N>
    void swap(Tp(&a)[N], Tp(&b)[N]) {
        stl::swap_range(a, a + N, b);
    }

//    // 和上面那个同时存在会报错 Call to 'swap' is ambiguous
//    template<class Tp, size_t N>
//    void swap(Tp* a, Tp* b) {
//        stl::swap_range(a, a + N, b);
//    }


/*********************************************************************/

    // pair

    template<class Ty1, class Ty2>
    struct pair {

        typedef Ty1 first_type;
        typedef Ty2 second_type;

        first_type first;
        second_type second;

        // default constructiable
        template<class Other1=Ty1, class Other2=Ty2,
                // 下面的typename是一个匿名的模板参数，并无真的用处，只是看当前传进来
                // 的模板实参能否替换成功

                // 如果类型Ty1,Ty2都有构造函数，则返回true，因此enable_if能够推断得到type
                // 否则，enable_if里面没有type，则推断失败，
                // 由于替换失败并非错误（SFINAE），则编译器会尝试替换其他的构造函数
                typename = typename std::enable_if<
                        std::is_default_constructible<Other1>::value &&
                        std::is_default_constructible<Other2>::value, void>::type>
        constexpr pair():first(), second() {
            std::cout << "pair default construct" << std::endl;
        }

        // implicit constructiable for this type
        template<class U1 = Ty1, class U2 = Ty2,
                typename std::enable_if<
                        std::is_copy_constructible<U1>::value &&
                        std::is_copy_constructible<U2>::value &&
                        std::is_convertible<const U1 &, U1>::value &&
                        std::is_convertible<const U2 &, Ty2>::value, int>::type = 0>
//                        std::is_convertible<const U2 &, U2>::value, void>::type>
        constexpr pair(const Ty1 &a, const Ty2 &b):first(a), second(b) {
            std::cout << "implicit constructiable for this type" << std::endl;
        }

        // explicit constructible for this type
        template<class U1 = Ty1, class U2 = Ty2,
                typename std::enable_if<
                        std::is_copy_constructible<U1>::value &&
                        std::is_copy_constructible<U2>::value &&
                        (!std::is_convertible<const U1 &, Ty1>::value ||
                         !std::is_convertible<const U2 &, Ty2>::value), int>::type = 0>
        explicit constexpr pair(const Ty1 &a, const Ty2 &b)
                : first(a), second(b) {
            std::cout << "explicit constructible for this type" << std::endl;
        }

        pair(const pair &rhs) = default;

        pair(pair &&rhs) noexcept = default;

        // implicit constructiable for other type
        template<class Other1, class Other2,
                typename std::enable_if<
                        std::is_constructible<Ty1, Other1>::value &&
                        std::is_constructible<Ty2, Other2>::value &&
                        std::is_convertible<Other1 &&, Ty1>::value &&
                        std::is_convertible<Other2 &&, Ty2>::value, int>::type = 0>
        constexpr pair(Other1 &&a, Other2 &&b)
                : first(stl::forward<Other1>(a)),
                  second(stl::forward<Other2>(b)) {
            std::cout << "implicit constructiable for other type" << std::endl;
        }

        // implicit constructiable for other pair
        template<class Other1, class Other2,
                typename std::enable_if<
                        std::is_constructible<Ty1, const Other1 &>::value &&
                        std::is_constructible<Ty2, const Other2 &>::value &&
                        std::is_convertible<const Other1 &, Ty1>::value &&
                        std::is_convertible<const Other2 &, Ty2>::value, int>::type = 0>
        constexpr pair(const pair<Other1, Other2> &other)
                : first(other.first),
                  second(other.second) {
            std::cout << "implicit constructiable for other pair" << std::endl;
        }

        // explicit constructiable for other pair
        template<class Other1, class Other2,
                typename std::enable_if<
                        std::is_constructible<Ty1, Other1>::value &&
                        std::is_constructible<Ty2, Other2>::value &&
                        (!std::is_convertible<Other1, Ty1>::value ||
                         !std::is_convertible<Other2, Ty2>::value), int>::type = 0>
        explicit constexpr pair(pair<Other1, Other2> &&other)
                : first(stl::forward<Other1>(other.first)),
                  second(stl::forward<Other2>(other.second)) {
        }

        // copy assign constructor for this type
        pair &operator=(pair &rhs) {
            if (this != &rhs) {
                first = rhs.first;
                second = rhs.second;
            }
            return *this;
        }

        // move assign constructor for this type
        pair &operator=(pair &&rhs) noexcept {
            if (this != &rhs) {
                first = stl::move(rhs.first);
                second = stl::move(rhs.second);
            }
            return *this;
        }

        // copy assign constructor for other type
        template<class Other1, class Other2>
        pair &operator=(const pair<Other1, Other2> &other) {
            first = other.first;
            second = other.second;
            return *this;
        }

        // move assign constructor for other type
        template<class Other1, class Other2>
        pair &operator=(pair<Other1, Other2> &&other) {
            // other.first已经是一个左值了，经过forward变成一个右值，
            // 这个右值需要调用构造函数构造出来，然后调用移动赋值函数
            // 将他的内容给到first，然后调用析构函数释放掉。
            first = stl::forward<Other1>(other.first);
            second = stl::forward<Other2>(other.second);
            // 为什么不用move
//            first = stl::move(other.first);
//            second = stl::move(other.second);
            return *this;
        }

        ~pair() = default;

        void swap(pair &other) {
            if (this != &other) {
                stl::swap(first, other.first);
                stl::swap(second, other.second);
            }
        }

    };

    // 重载比较运算符
    template<class Ty1, class Ty2>
    bool operator==(const pair<Ty1, Ty2> &lhs, const pair<Ty1, Ty2> &rhs) {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }

    template <class Ty1, class Ty2>
    bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
    }

    template <class Ty1, class Ty2>
    bool operator!=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class Ty1, class Ty2>
    bool operator>(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return rhs < lhs;
    }

    template <class Ty1, class Ty2>
    bool operator>=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return !(lhs < rhs);
    }

    // 重载 stl 的 swap
    template <class Ty1, class Ty2>
    void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs)
    {
        lhs.swap(rhs);
    }

    // 全局函数，让两个数据成为一个 pair
    template <class Ty1, class Ty2>
    pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second)
    {
        return pair<Ty1, Ty2>(stl::forward<Ty1>(first), stl::forward<Ty2>(second));
    }

}


#endif //MYCPPSTL_UTILS_H
