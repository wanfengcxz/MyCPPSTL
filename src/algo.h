//
// Created by 晚风吹行舟 on 2023/8/20.
//

#ifndef MYCPPSTL_ALGO_H
#define MYCPPSTL_ALGO_H

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif

#include <cstddef>
#include <ctime>

#include "algobase.h"
#include "memory.h"
#include "heap_algo.h"
#include "functional.h"

namespace stl {

/*****************************************************************************************/
// all_of
// 检查[first, last)内是否全部元素都满足一元操作 unary_pred 为 true 的情况，满足则返回 true
/*****************************************************************************************/
    template<class InputIter, class UnaryPredicate>
    bool all_of(InputIter first, InputIter last, UnaryPredicate unary_pred) {
        for (; first != last; ++first) if (!unary_pred(*first)) return false;
        return true;
    }

/*****************************************************************************************/
// any_of
// 检查[first, last)内是否存在某个元素满足一元操作 unary_pred 为 true 的情况，满足则返回 true
/*****************************************************************************************/
    template<class InputIter, class UnaryPredicate>
    bool any_of(InputIter first, InputIter last, UnaryPredicate unary_pred) {
        for (; first != last; ++first) if (unary_pred(*first)) return true;
        return false;
    }

/*****************************************************************************************/
// none_of
// 检查[first, last)内是否全部元素都不满足一元操作 unary_pred 为 true 的情况，满足则返回 true
/*****************************************************************************************/
    template<class InputIter, class UnaryPredicate>
    bool none_of(InputIter first, InputIter last, UnaryPredicate unary_pred) {
        for (; first != last; ++first) if (unary_pred(*first)) return false;
        return true;
    }

/*****************************************************************************************/
// count
// 对[first, last)区间内的元素与给定值进行比较，缺省使用 operator==，返回元素相等的个数
/*****************************************************************************************/
    template<class InputIter, class T>
    size_t count(InputIter first, InputIter last, const T &value) {
        size_t n = 0;
        for (; first != last; ++first) if (*first == value) ++n;
        return n;
    }

/*****************************************************************************************/
// count_if
// 对[first, last)区间内的每个元素都进行一元 unary_pred 操作，返回结果为 true 的个数
/*****************************************************************************************/
    template<class InputIter, class UnaryPredicate>
    size_t count_if(InputIter first, InputIter last, UnaryPredicate unary_pred) {
        size_t n = 0;
        for (; first != last; ++first) if (unary_pred(*first))++n;
        return n;
    }

/*****************************************************************************************/
// find
// 在[first, last)区间内找到等于 value 的元素，返回指向该元素的迭代器
/*****************************************************************************************/
    template<class InputIter, class T>
    InputIter find(InputIter first, InputIter last, const T &value) {
        for (; first != last; ++first) if (*first == value) return first;
        return first;
    }


/*****************************************************************************************/
// find_if
// 在[first, last)区间内找到第一个令一元操作 unary_pred 为 true 的元素并返回指向该元素的迭代器
/*****************************************************************************************/

    template<class InputIter, class UnaryPredicate>
    InputIter find_if(InputIter first, InputIter last, UnaryPredicate unary_pred) {
        for (; first != last; ++first) if (unary_pred(*first)) return first;
        return first;
    }

/*****************************************************************************************/
// find_if_not
// 在[first, last)区间内找到第一个令一元操作 unary_pred 为 false 的元素并返回指向该元素的迭代器
/*****************************************************************************************/
    template<class InputIter, class UnaryPredicate>
    InputIter find_if_not(InputIter first, InputIter last, UnaryPredicate unary_pred) {
        for (; first != last; ++first) if (!unary_pred(*first)) return first;
        return first;
    }

/*****************************************************************************************/
// search
// 在[first1, last1)中查找[first2, last2)的首次出现点
/*****************************************************************************************/
    // TODO:没看懂干什么
    template<class ForwardIter1, class ForwardIter2>
    ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
                        ForwardIter2 first2, ForwardIter2 last2) {
        auto d1 = stl::distance(first1, last1);
        auto d2 = stl::distance(first2, last2);
        if (d1 < d2) return last1;
        auto curr1 = first1, curr2 = first2;
        while (curr2 != last2) {
            if (*curr1 == *curr2) {
                ++curr1;
                ++curr2;
            } else {
                if (d1 == d2) {
                    return last1;
                } else {
                    curr1 = ++first1;
                    curr2 = first2;
                    --d1;
                }
            }
        }
        return first1;
    }

    template<class ForwardIter1, class ForwardIter2, class Compared>
    ForwardIter1
    search(ForwardIter1 first1, ForwardIter1 last1,
           ForwardIter2 first2, ForwardIter2 last2, Compared comp) {
        auto d1 = stl::distance(first1, last1);
        auto d2 = stl::distance(first2, last2);
        if (d1 < d2)
            return last1;
        auto current1 = first1;
        auto current2 = first2;
        while (current2 != last2) {
            if (comp(*current1, *current2)) {
                ++current1;
                ++current2;
            } else {
                if (d1 == d2) {
                    return last1;
                } else {
                    current1 = ++first1;
                    current2 = first2;
                    --d1;
                }
            }
        }
        return first1;
    }

/*****************************************************************************************/
// search_n
// 在[first, last)中查找连续 n 个 value 所形成的子序列，返回一个迭代器指向该子序列的起始处
/*****************************************************************************************/
    template<class ForwardIter, class Size, class T>
    ForwardIter search_n(ForwardIter first, ForwardIter last, Size n, T value) {
        if (n <= 0) return first;
        first = stl::find(first, last, value);
        while (first != last) {
            auto m = n - 1, i = first;
            ++i;
            while (i != last && m != 0 && *i == value) --m, ++i;
            if (m == 0) return first;
            else first = stl::find(i, last, value);
        }
        return last;
    }

    template<class ForwardIter, class Size, class T, class Compared>
    ForwardIter search_n(ForwardIter first, ForwardIter last, Size n, T value, Compared comp) {
        if (n <= 0) return first;
        while (first != last) {
            // TODO: 为什么是comp
            if (comp(*first, value)) break;
            ++first;
        }
        while (first != last) {
            auto m = n - 1, i = first;
            ++i;
            while (i != last && m != 0 && comp(*i, value)) ++i, --m;
            if (m == 0)return first;
            else {
                while (i != last) {
                    if (comp(*i, last))break;
                    ++i;
                }
                first = i;
            }
        }
        return last;
    }

/*****************************************************************************************/
// find_end
// 在[first1, last1)区间中查找[first2, last2)最后一次出现的地方，若不存在返回 last1
/*****************************************************************************************/
// find_end_dispatch 的 forward_iterator_tag 版本
    template<class ForwardIter1, class ForwardIter2>
    ForwardIter1
    find_end_dispatch(ForwardIter1 first1, ForwardIter1 last1,
                      ForwardIter2 first2, ForwardIter2 last2,
                      forward_iterator_tag, forward_iterator_tag) {


    }

/*****************************************************************************************/
// find_first_of
// 在[first1, last1)中查找[first2, last2)中的某些元素，返回指向第一次出现的元素的迭代器
/*****************************************************************************************/
    template<class InputIter, class ForwardIter>
    InputIter find_first_of(InputIter first1, InputIter last1,
                            ForwardIter first2, ForwardIter last2) {
        for (; first1 != last1; ++first1) {
            for (auto iter = first2; iter != last2; ++iter) {
                if (*first1 == *iter) return first1;
            }
        }
        return last1;
    }

    template<class InputIter, class ForwardIter, class Compared>
    InputIter find_first_of(InputIter first1, InputIter last1,
                            ForwardIter first2, ForwardIter last2,
                            Compared comp) {
        for (; first1 != last1; ++first1) {
            for (auto iter = first2; iter != last2; ++iter) {
                if (comp(*first1, *iter)) return first1;
            }
        }
        return last1;
    }

/*****************************************************************************************/
// for_each
// 使用一个函数对象 f 对[first, last)区间内的每个元素执行一个 operator() 操作，但不能改变元素内容
// f() 可返回一个值，但该值会被忽略
/*****************************************************************************************/
    template<class InputIter, class Function>
    Function for_each(InputIter first, InputIter last, Function f) {
        for (; first != last; ++first) f(*first);
        return f;
    }

/*****************************************************************************************/
// adjacent_find
// 找出第一对匹配的相邻元素，缺省使用 operator== 比较，如果找到返回一个迭代器，指向这对元素的第一个元素
/*****************************************************************************************/
    template<class ForwardIter>
    ForwardIter adjacent_find(ForwardIter first, ForwardIter last) {
        if (first == last) return last;
        auto next = first;
        while (++next != last) {
            if (*first == *next) return first;
            first = next;
        }
        return last;
    }


    template<class ForwardIter, class Compared>
    ForwardIter adjacent_find(ForwardIter first, ForwardIter last, Compared comp) {
        if (first == last) return last;
        auto next = first;
        while (++next != last) {
            if (comp(*first, *next)) return first;
            first = next;
        }
        return last;
    }


/*****************************************************************************************/
// lower_bound
// 在[first, last)中查找第一个不小于 value 的元素，并返回指向它的迭代器，若没有则返回 last
/*****************************************************************************************/
    // lbound_dispatch 的 forward_iterator_tag 版本
    template<class ForwardIter, class T>
    ForwardIter lbound_dispatch(ForwardIter first, ForwardIter last, const T &value,
                                forward_iterator_tag) {
        auto len = stl::distance(first, last);
        auto half = len;
        ForwardIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first;
            stl::advance(middle, half);     // middle前进一半距离
            if (*middle < value) {
                first = middle;
                ++first;
                len = len - half - 1;
            } else {
                len = half;
            }
        }
        return first;
    }

    // lbound_dispatch 的 random_access_iterator_tag 版本
    template<class RandomIter, class T>
    RandomIter lbound_dispatch(RandomIter first, RandomIter last, const T &value,
                               random_access_iterator_tag) {
        auto len = last - first;
        auto half = len;
        RandomIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first + half;
            if (*middle < value) {
                first = middle + 1;
                len = len - half - 1;
            } else {
                len = half;
            }
        }
        return first;
    }

    template<class ForwardIter, class T>
    ForwardIter lower_bound(ForwardIter first, ForwardIter last, const T &value) {
        return stl::lbound_dispatch(first, last, value, stl::iterator_category(first));
    }

    // 重载版本使用函数对象 comp 代替比较操作
    // lbound_dispatch 的 forward_iterator_tag 版本
    template<class ForwardIter, class T, class Compared>
    ForwardIter
    lbound_dispatch(ForwardIter first, ForwardIter last,
                    const T &value, forward_iterator_tag, Compared comp) {
        auto len = stl::distance(first, last);
        auto half = len;
        ForwardIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first;
            stl::advance(middle, half);
            if (comp(*middle, value)) {
                first = middle;
                ++first;
                len = len - half - 1;
            } else {
                len = half;
            }
        }
        return first;
    }

    // lbound_dispatch 的 random_access_iterator_tag 版本
    template<class RandomIter, class T, class Compared>
    RandomIter
    lbound_dispatch(RandomIter first, RandomIter last,
                    const T &value, random_access_iterator_tag, Compared comp) {
        auto len = last - first;
        auto half = len;
        RandomIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first + half;
            if (comp(*middle, value)) {
                first = middle + 1;
                len = len - half - 1;
            } else {
                len = half;
            }
        }
        return first;
    }

    template<class ForwardIter, class T, class Compared>
    ForwardIter
    lower_bound(ForwardIter first, ForwardIter last, const T &value, Compared comp) {
        return stl::lbound_dispatch(first, last, value, iterator_category(first), comp);
    }

/*****************************************************************************************/
// upper_bound
// 在[first, last)中查找第一个大于value 的元素，并返回指向它的迭代器，若没有则返回 last
/*****************************************************************************************/
    // ubound_dispatch 的 forward_iterator_tag 版本
    template<class ForwardIter, class T>
    ForwardIter
    ubound_dispatch(ForwardIter first, ForwardIter last,
                    const T &value, forward_iterator_tag) {
        auto len = stl::distance(first, last);
        auto half = len;
        ForwardIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first;
            stl::advance(middle, half);
            if (value < *middle) {
                len = half;
            } else {
                first = middle;
                ++first;
                len = len - half - 1;
            }
        }
        return first;
    }

    // ubound_dispatch 的 random_access_iterator_tag 版本
    template<class RandomIter, class T>
    RandomIter
    ubound_dispatch(RandomIter first, RandomIter last,
                    const T &value, random_access_iterator_tag) {
        auto len = last - first;
        auto half = len;
        RandomIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first + half;
            if (value < *middle) {
                len = half;
            } else {
                first = middle + 1;
                len = len - half - 1;
            }
        }
        return first;
    }

    template<class ForwardIter, class T>
    ForwardIter
    upper_bound(ForwardIter first, ForwardIter last, const T &value) {
        return stl::ubound_dispatch(first, last, value, iterator_category(first));
    }

// 重载版本使用函数对象 comp 代替比较操作
// ubound_dispatch 的 forward_iterator_tag 版本
    template<class ForwardIter, class T, class Compared>
    ForwardIter
    ubound_dispatch(ForwardIter first, ForwardIter last,
                    const T &value, forward_iterator_tag, Compared comp) {
        auto len = stl::distance(first, last);
        auto half = len;
        ForwardIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first;
            stl::advance(middle, half);
            if (comp(value, *middle)) {
                len = half;
            } else {
                first = middle;
                ++first;
                len = len - half - 1;
            }
        }
        return first;
    }

// ubound_dispatch 的 random_access_iterator_tag 版本
    template<class RandomIter, class T, class Compared>
    RandomIter
    ubound_dispatch(RandomIter first, RandomIter last,
                    const T &value, random_access_iterator_tag, Compared comp) {
        auto len = last - first;
        auto half = len;
        RandomIter middle;
        while (len > 0) {
            half = len >> 1;
            middle = first + half;
            if (comp(value, *middle)) {
                len = half;
            } else {
                first = middle + 1;
                len = len - half - 1;
            }
        }
        return first;
    }

    template<class ForwardIter, class T, class Compared>
    ForwardIter
    upper_bound(ForwardIter first, ForwardIter last, const T &value, Compared comp) {
        return stl::ubound_dispatch(first, last, value, iterator_category(first), comp);
    }

/*****************************************************************************************/
// binary_search
// 二分查找，如果在[first, last)内有等同于 value 的元素，返回 true，否则返回 false
/*****************************************************************************************/
    template<class ForwardIter, class T>
    bool binary_search(ForwardIter first, ForwardIter last, const T &value) {
        auto tmp = stl::lower_bound(first, last, value);
        // TODO：(*tmp == value)应该也可以

        // low_bound查找第一个大于等于value的数
        // 查找结果：1.如果全都小于value，则返回last， tmp!=last为false
        // 2.如果全都大于，则返回大于，则!(value<*tmp)为false
        // 2.如果有大于，有小于，没有等于，则返回大于，则!(value<*tmp)为false
        // 3.如果有大于，有小于，有等于，则返回大于，则!(value<*tmp)为true
        return tmp != last && !(value < *tmp);
    }

    template<class ForwardIter, class T, class Compared>
    bool binary_search(ForwardIter first, ForwardIter last, const T &value, Compared comp) {
        // TODO:源码中没有comp
        auto tmp = stl::lower_bound(first, last, value, comp);

        return tmp != last && !comp(value, *tmp);
    }

/*****************************************************************************************/
// equal_range
// 查找[first,last)区间中与 value 相等的元素所形成的区间，返回一对迭代器指向区间首尾
// 第一个迭代器指向第一个不小于 value 的元素，第二个迭代器指向第一个大于 value 的元素
/*****************************************************************************************/
    // erange_dispatch 的 forward_iterator_tag 版本
    template<class ForwardIter, class T>
    stl::pair<ForwardIter, ForwardIter>
    erange_dispatch(ForwardIter first, ForwardIter last,
                    const T &value, forward_iterator_tag) {
        auto len = stl::distance(first, last);
        auto half = len;
        ForwardIter middle, left, right;
        while (len > 0) {
            half = len >> 1;
            middle = first;
            stl:
            advance(middle, half);
            if (*middle < value) {
                first = middle;
                ++first;
                len = len - half - 1;
            } else if (value < *middle) {
                len = half;
            } else {
                left = stl::lower_bound(first, last, value);
                stl::advance(first, len);
                right = stl::upper_bound(++middle, first, value);
                return stl::pair<ForwardIter, ForwardIter>(left, right);
            }
        }
        return stl::pair<ForwardIter, ForwardIter>(last, last);
    }

    // erange_dispatch 的 random_access_iterator_tag 版本
    template<class RandomIter, class T>
    stl::pair<RandomIter, RandomIter>
    erange_dispatch(RandomIter first, RandomIter last,
                    const T &value, random_access_iterator_tag) {
        auto len = last - first;
        auto half = len;
        RandomIter middle, left, right;
        while (len > 0) {
            half = len >> 1;
            middle = first + half;
            if (*middle < value) {
                first = middle + 1;
                len = len - half - 1;
            } else if (value < *middle) {
                len = half;
            } else {
                left = stl::lower_bound(first, middle, value);
                right = stl::upper_bound(++middle, first + len, value);
                return stl::pair<RandomIter, RandomIter>(left, right);
            }
        }
        return stl::pair<RandomIter, RandomIter>(last, last);
    }

    template<class ForwardIter, class T>
    stl::pair<ForwardIter, ForwardIter>
    equal_range(ForwardIter first, ForwardIter last, const T &value) {
        return stl::erange_dispatch(first, last, value, iterator_category(first));
    }

    // 重载版本使用函数对象 comp 代替比较操作
    // erange_dispatch 的 forward iterator 版本
    template<class ForwardIter, class T, class Compared>
    stl::pair<ForwardIter, ForwardIter>
    erange_dispatch(ForwardIter first, ForwardIter last,
                    const T &value, forward_iterator_tag, Compared comp) {
        auto len = stl::distance(first, last);
        auto half = len;
        ForwardIter middle, left, right;
        while (len > 0) {
            half = len >> 1;
            middle = first;
            stl::advance(middle, half);
            if (comp(*middle, value)) {
                first = middle;
                ++first;
                len = len - half - 1;
            } else if (comp(value, *middle)) {
                len = half;
            } else {
                left = stl::lower_bound(first, last, value, comp);
                stl::advance(first, len);
                right = stl::upper_bound(++middle, first, value, comp);
                return stl::pair<ForwardIter, ForwardIter>(left, right);
            }
        }
        return stl::pair<ForwardIter, ForwardIter>(last, last);
    }

    // erange_dispatch 的 random access iterator 版本
    template<class RandomIter, class T, class Compared>
    stl::pair<RandomIter, RandomIter>
    erange_dispatch(RandomIter first, RandomIter last,
                    const T &value, random_access_iterator_tag, Compared comp) {
        auto len = last - first;
        auto half = len;
        RandomIter middle, left, right;
        while (len > 0) {
            half = len >> 1;
            middle = first + half;
            if (comp(*middle, value)) {
                first = middle + 1;
                len = len - half - 1;
            } else if (comp(value, *middle)) {
                len = half;
            } else {
                left = stl::lower_bound(first, middle, value, comp);
                right = stl::upper_bound(++middle, first + len, value, comp);
                return stl::pair<RandomIter, RandomIter>(left, right);
            }
        }
        return stl::pair<RandomIter, RandomIter>(last, last);
    }

    template<class ForwardIter, class T, class Compared>
    stl::pair<ForwardIter, ForwardIter>
    equal_range(ForwardIter first, ForwardIter last, const T &value, Compared comp) {
        return stl::erange_dispatch(first, last, value, iterator_category(first), comp);
    }

/*****************************************************************************************/
// generate
// 将函数对象 gen 的运算结果对[first, last)内的每个元素赋值
/*****************************************************************************************/
    template<class ForwardIter, class Generator>
    void generate(ForwardIter first, ForwardIter last, Generator generator) {
        for (; first != last; ++first) *first = generator();
    }

/*****************************************************************************************/
// generate_n
// 用函数对象 gen 连续对 n 个元素赋值
/*****************************************************************************************/
    template<class ForwardIter, class Size, class Generator>
    void generate_n(ForwardIter first, Size n, Generator generator) {
        for (; n > 0; --n, ++first) *first = generator();
    }

/*****************************************************************************************/
// includes
// 判断序列一S1 是否包含序列二S2  S1,S2有序
/*****************************************************************************************/
    template<class InputIter1, class InputIter2>
    bool includes(InputIter1 first1, InputIter1 last1,
                  InputIter2 first2, InputIter2 last2) {
        while (first1 != last1 && first2 != last2) {
            if (*first1 < *first2) ++first1;
            else if (*first2 < *first1) return false;
            else ++first1, ++first2;
        }
        return first2 == last2;
    }

    template<class InputIter1, class InputIter2, class Compared>
    bool includes(InputIter1 first1, InputIter1 last1,
                  InputIter2 first2, InputIter2 last2, Compared comp) {
        while (first1 != last1 && first2 != last2) {
            if (comp(*first1, *first2)) ++first1;
            else if (comp(*first2, *first1)) return false;
            else ++first1, ++first2;
        }
        return first2 == last2;
    }

/*****************************************************************************************/
// is_heap
// 检查[first, last)内的元素是否为一个堆，如果是，则返回 true (大顶堆)
/*****************************************************************************************/
    template<class RandomIter>
    bool is_heap(RandomIter first, RandomIter last) {
        auto n = stl::distance(first, last);
        auto parent = 0;
        for (auto child = 1; child < n; ++child) {
            if (first[parent] < first[child]) return false;
            if ((child & 1) == 0) ++parent; // 如果当前为右孩子，则check下一棵子树
        }
        return true;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template<class RandomIter, class Compared>
    bool is_heap(RandomIter first, RandomIter last, Compared comp) {
        auto n = stl::distance(first, last);
        auto parent = 0;
        for (auto child = 1; child < n; ++child) {
            if (comp(first[parent], first[child]))
                return false;
            if ((child & 1) == 0)
                ++parent;
        }
        return true;
    }

/*****************************************************************************************/
// is_sorted
// 检查[first, last)内的元素是否升序，如果是升序，则返回 true
/*****************************************************************************************/
    template<class ForwardIter>
    bool is_sorted(ForwardIter first, ForwardIter last) {
        if (first == last) return true;
        auto next = first;
        ++next;
        for (; next != last; ++next, ++first) {
            if (*next < *first) return false;
        }
        return true;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template<class ForwardIter, class Compared>
    bool is_sorted(ForwardIter first, ForwardIter last, Compared comp) {
        if (first == last)
            return true;
        auto next = first;
        ++next;
        for (; next != last; first = next, ++next) {
            if (comp(*next, *first))
                return false;
        }
        return true;
    }

/*****************************************************************************************/
// median
// 找出三个值的中间值
/*****************************************************************************************/
    template<class T>
    const T &median(const T &left, const T &mid, const T &right) {
        if (left < mid)
            if (mid < right)        // left < mid < right
                return mid;
            else if (left < right)  // left < right <= mid
                return right;
            else                    // right <= left < mid
                return left;
        else if (left < right)      // mid <= left < right
            return left;
        else if (mid < right)       // mid < right <= left
            return right;
        else                        // right <= mid <= left
            return mid;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template<class T, class Compared>
    const T &median(const T &left, const T &mid, const T &right, Compared comp) {
        if (comp(left, mid))
            if (comp(mid, right))
                return mid;
            else if (comp(left, right))
                return right;
            else
                return left;
        else if (comp(left, right))
            return left;
        else if (comp(mid, right))
            return right;
        else
            return mid;
    }

/*****************************************************************************************/
// max_element
// 返回一个迭代器，指向序列中最大的元素
/*****************************************************************************************/
    template<class ForwardIter>
    ForwardIter max_element(ForwardIter first, ForwardIter last) {
        if (first == last) return last;
        auto result = first;
        while (++first != last) {
            if (*result < *first) result = first;
        }
        return result;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template<class ForwardIter, class Compared>
    ForwardIter max_element(ForwardIter first, ForwardIter last, Compared comp) {
        if (first == last)
            return first;
        auto result = first;
        while (++first != last) {
            if (comp(*result, *first))
                result = first;
        }
        return result;
    }

/*****************************************************************************************/
// min_element
// 返回一个迭代器，指向序列中最小的元素
/*****************************************************************************************/
    template<class ForwardIter>
    ForwardIter min_element(ForwardIter first, ForwardIter last) {
        if (first == last)
            return first;
        auto result = first;
        while (++first != last) {
            if (*first < *result)
                result = first;
        }
        return result;
    }

    // 重载版本使用函数对象 comp 代替比较操作
    template<class ForwardIter, class Compared>
    ForwardIter min_element(ForwardIter first, ForwardIter last, Compared comp) {
        if (first == last)
            return first;
        auto result = first;
        while (++first != last) {
            if (comp(*first, *result))
                result = first;
        }
        return result;
    }

/*****************************************************************************************/
// swap_ranges
// 将[first1, last1)从 first2 开始，交换相同个数元素
// 交换的区间长度必须相同，两个序列不能互相重叠，返回一个迭代器指向序列二最后一个被交换元素的下一位置
/*****************************************************************************************/
    template<class ForwardIter1, class ForwardIter2>
    ForwardIter2 swap_ranges(ForwardIter1 first1, ForwardIter1 last1,
                             ForwardIter2 first2) {
        for (; first1 != last1; ++first1, ++first2) {
            stl::iter_swap(first1, first2);
        }
        return first2;
    }

/*****************************************************************************************/
// transform
// 第一个版本以函数对象 unary_op 作用于[first, last)中的每个元素并将结果保存至 result 中
// 第二个版本以函数对象 binary_op 作用于两个序列[first1, last1)、[first2, last2)的相同位置
/*****************************************************************************************/
    template<class InputIter, class OutputIter, class UnaryOperation>
    OutputIter transform(InputIter first, InputIter last, OutputIter result,
                         UnaryOperation unary_op) {
        for (; first != last; ++first, ++result) {
            *result = unary_op(*first);
        }
        return result;
    }

    template<class InputIter1, class InputIter2, class OutputIter, class BinaryOperation>
    OutputIter
    transform(InputIter1 first1, InputIter1 last1,
              InputIter2 first2, OutputIter result, BinaryOperation binary_op) {
        for (; first1 != last1; ++first1, ++first2, ++result) {
            *result = binary_op(*first1, *first2);
        }
        return result;
    }

/*****************************************************************************************/
// remove_copy
// 移除区间内与指定 value 相等的元素，并将结果复制到以 result 标示起始位置的容器上
/*****************************************************************************************/
    template<class InputIter, class OutputIter, class T>
    OutputIter remove_copy(InputIter first, InputIter last, const T &value, OutputIter result) {
        for (; first != last; ++first) {
            if (*first != value) *result = *first, ++result;
        }
        return result;
    }

/*****************************************************************************************/
// remove
// 移除所有与指定 value 相等的元素
// 并不从容器中删除这些元素，所以 remove 和 remove_if 不适用于 array
/*****************************************************************************************/
    // TODO:没搞懂
    template<class ForwardIter, class T>
    ForwardIter remove(ForwardIter first, ForwardIter last, const T &value) {
        first = stl::find(first, last, value);
        auto next = first;
        return first == last ? first : stl::remove_copy(++next, last, value, first);
    }

/*****************************************************************************************/
// remove_copy_if
// 移除区间内所有令一元操作 unary_pred 为 true 的元素，并将结果复制到以 result 为起始位置的容器上
/*****************************************************************************************/
    template<class InputIter, class OutputIter, class UnaryPredicate>
    OutputIter remove_copy_if(InputIter first, InputIter last,
                              OutputIter result, UnaryPredicate unary_pred) {
        for (; first != last; ++first) {
            if (!unary_pred(*first)) {
                *result = *first;
                ++result;
            }
        }
        return result;
    }


/*****************************************************************************************/
// remove_if
// 移除区间内所有令一元操作 unary_pred 为 true 的元素
/*****************************************************************************************/
    template<class ForwardIter, class UnaryPredicate>
    ForwardIter
    remove_if(ForwardIter first, ForwardIter last, UnaryPredicate unary_pred) {
        first = stl::find_if(first, last, unary_pred);  // 利用 find_if 找出第一个匹配的地方
        auto next = first;
        return first == last ? first : stl::remove_copy_if(++next, last, first, unary_pred);
    }

/*****************************************************************************************/
// replace
// 将区间内所有的 old_value 都以 new_value 替代
/*****************************************************************************************/
    template<class ForwardIter, class T>
    void replace(ForwardIter first, ForwardIter last,
                 const T &old_value, const T &new_value) {
        for (; first != last; ++first) {
            if (*first == old_value) *first = new_value;
        }
    }

/*****************************************************************************************/
// replace_copy
// 行为与 replace 类似，不同的是将结果复制到 result 所指的容器中，原序列没有改变
/*****************************************************************************************/
    template<class InputIter, class OutputIter, class T>
    OutputIter
    replace_copy(InputIter first, InputIter last,
                 OutputIter result, const T &old_value, const T &new_value) {
        for (; first != last; ++first, ++result) {
            *result = *first == old_value ? new_value : *first;
        }
        return result;
    }

/*****************************************************************************************/
// replace_if
// 将区间内所有令一元操作 unary_pred 为 true 的元素都用 new_value 替代
/*****************************************************************************************/
    template<class ForwardIter, class UnaryPredicate, class T>
    // unary_pred(*first)涉及到读，*first = new_value涉及到写，所以只能用ForwardIter
    void replace_if(ForwardIter first, ForwardIter last,
                    UnaryPredicate unary_pred, const T &new_value) {
        for (; first != last; ++first) {
            if (unary_pred(*first)) *first = new_value;
        }
    }

/*****************************************************************************************/
// replace_copy_if
// 行为与 replace_if 类似，不同的是将结果复制到 result 所指的容器中，原序列没有改变
/*****************************************************************************************/
    template<class InputIter, class OutputIter, class UnaryPredicate, class T>
    OutputIter replace_copy_if(InputIter first, InputIter last, OutputIter result,
                               UnaryPredicate unary_pred, const T &new_value) {
        for (; first != last; ++first) {
            if (unary_pred(*first)) *result = new_value;
            else *result = *first;
        }
        return result;
    }

/*****************************************************************************************/
// reverse
// 将[first, last)区间内的元素反转
/*****************************************************************************************/
    // reverse_dispatch 的 bidirectional_iterator_tag 版本
    // TODO:真的有必要针对这两个迭代器写两种代码吗？加重了程序员负担
    // 但是感觉性能没有很高的提升，甚至由于封装reverse，多加了一层函数
    template<class BidirectionalIter>
    void reverse_dispatch(BidirectionalIter first, BidirectionalIter last,
                          bidirectional_iterator_tag) {
        while (true) {
            // 只能判断== !=，不能判断< > <=
            if (first != last && first != --last) stl::iter_swap(first++, last);
            else return;
        }
    }

    // reverse_dispatch 的 random_access_iterator_tag 版本
    template<class RandomIter>
    void reverse_dispatch(RandomIter first, RandomIter last,
                          random_access_iterator_tag) {

        while (first < last) stl::iter_swap(first++, --last);
    }

    template<class BidirectionalIter>
    void reverse(BidirectionalIter first, BidirectionalIter last) {
        stl::reverse_dispatch(first, last, stl::iterator_category(first));
    }

/*****************************************************************************************/
// reverse_copy
// 行为与 reverse 类似，不同的是将结果复制到 result 所指容器中
/*****************************************************************************************/
    template<class BidirectionalIter, class OutputIter>
    OutputIter reverse_copy(BidirectionalIter first, BidirectionalIter last,
                            OutputIter result) {
        while (first != last) {
            --last;
            *result = *last;
            ++result;
        }
        return result;
    }

/*****************************************************************************************/
// random_shuffle
// 将[first, last)内的元素次序随机重排
// 重载版本使用一个产生随机数的函数对象 rand
/*****************************************************************************************/
    template<class RandomIter>
    void random_shuffle(RandomIter first, RandomIter last) {
        if (first == last) return;
        srand((unsigned) time(0));
        for (auto i = first + 1; i != last; ++i) {
            stl::iter_swap(i, first + (rand() % (i - first + 1)));
        }
    }

    template<class RandomIter, class RandomNumberGenerator>
    void random_shuffle(RandomIter first, RandomIter last,
                        RandomNumberGenerator &rand) {
        if (first == last) return;
        auto len = last - first;
        for (auto i = first + 1; i != last; ++i) {
            // TODO：为什么和上面实现不一样
            stl::iter_swap(i, first + (rand(i - first + 1) % len));
        }
    }

/*****************************************************************************************/
// rotate
// 将[first, middle)内的元素和 [middle, last)内的元素互换，可以交换两个长度不同的区间
// 返回交换后 middle 的位置
/*****************************************************************************************/
    // rotate_dispatch 的 forward_iterator_tag 版本
    template<class ForwardIter>
    ForwardIter rotate_dispatch(ForwardIter first, ForwardIter middle,
                                ForwardIter last, forward_iterator_tag) {
        auto first2 = middle;
        do {
            stl::swap(*first++, *first2++);
            if (first == middle)
                middle = first2;
        } while (first2 != last);  // 后半段移到前面

        auto new_middle = first;   // 迭代器返回的位置
        first2 = middle;
        while (first2 != last) {   // 调整剩余元素
            stl::swap(*first++, *first2++);
            if (first == middle) {
                middle = first2;
            } else if (first2 == last) {
                first2 = middle;
            }
        }
    }

    // rotate_dispatch 的 bidirectional_iterator_tag 版本
    template<class BidirectionalIter>
    BidirectionalIter
    rotate_dispatch(BidirectionalIter first, BidirectionalIter middle,
                    BidirectionalIter last, bidirectional_iterator_tag) {
        stl::reverse_dispatch(first, middle, bidirectional_iterator_tag());
        stl::reverse_dispatch(middle, last, bidirectional_iterator_tag());
        while (first != middle && middle != last)
            stl::swap(*first++, *--last);
        if (first == middle) {
            stl::reverse_dispatch(middle, last, bidirectional_iterator_tag());
            return last;
        } else {
            stl::reverse_dispatch(first, middle, bidirectional_iterator_tag());
            return first;
        }
    }

    // 求最大公因子
    template<class EuclideanRingElement>
    EuclideanRingElement rgcd(EuclideanRingElement m, EuclideanRingElement n) {
        while (n != 0) {
            auto t = m % n;
            m = n;
            n = t;
        }
        return m;
    }

    // rotate_dispatch 的 random_access_iterator_tag 版本
    template<class RandomIter>
    RandomIter
    rotate_dispatch(RandomIter first, RandomIter middle,
                    RandomIter last, random_access_iterator_tag) {
        auto n = last - first;
        auto l = middle - first;
        auto r = n - l;
        auto result = first + (last - middle);
        if (l == r) {
            stl::swap_ranges(first, middle, middle);
            return result;
        }
        auto cycle_times = rgcd(n, l);
        for (auto i = 0; i < cycle_times; ++i) {
            auto tmp = *first;
            auto p = first;
            if (l < r) {
                for (auto j = 0; j < r / cycle_times; ++j) {
                    if (p > first + r) {
                        *p = *(p - r);
                        p -= r;
                    }
                    *p = *(p + l);
                    p += l;
                }
            } else {
                for (auto j = 0; j < l / cycle_times - 1; ++j) {
                    if (p < last - l) {
                        *p = *(p + l);
                        p += l;
                    }
                    *p = *(p - r);
                    p -= r;
                }
            }
            *p = tmp;
            ++first;
        }
        return result;
    }

    template<class ForwardIter>
    ForwardIter
    rotate(ForwardIter first, ForwardIter middle, ForwardIter last) {
        if (first == middle)
            return last;
        if (middle == last)
            return first;
        return stl::rotate_dispatch(first, middle, last, iterator_category(first));
    }

}


#endif //MYCPPSTL_ALGO_H
