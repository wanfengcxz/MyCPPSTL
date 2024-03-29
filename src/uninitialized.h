//
// Created by 晚风吹行舟 on 2023/4/16.
//

#ifndef MYCPPSTL_UNINITIALIZED_H
#define MYCPPSTL_UNINITIALIZED_H

// 这个头文件用于对未初始化空间构造元素

#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "utils.h"
#include "type_traits.h"

namespace stl {

/*****************************************************************************************/
// uninitialized_copy
// 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
/*****************************************************************************************/

    // 有拷贝构造函数
    template<class InputIter, class ForwardIter>
    ForwardIter
    unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::true_type) {
        return stl::copy(first, last, result);
    }

    // 没有拷贝构造函数，使用构造函数替代
    template<class InputIter, class ForwardIter>
    ForwardIter unchecked_uninit_copy(InputIter first, InputIter last,
                                      ForwardIter result, std::false_type) {
        auto cur = result;
        try {
            // 如果该类型没有拷贝构造函数 则需要逐一构造
            for (; first != last; ++first, ++cur) {
                stl::construct(&*cur, *first);
            }
        } catch (...) {
            // 如果发生异常，则撤销该操作
            for (; result != cur; --cur) {
                stl::destroy(&*cur);
            }
        }
        return cur;
    }

    template<class InputIter, class ForwardIter>
    ForwardIter uninitialized_copy(InputIter first, InputIter last,
                                   ForwardIter result) {
        // TODO: ????? 为什么第四个参数可以传给false_type/true_type
        return stl::unchecked_uninit_copy(first, last, result,
                                          std::is_trivially_copy_assignable<
                                                  typename iterator_traits<ForwardIter>::value_type>{});
    }

/*****************************************************************************************/
// uninitialized_copy_n
// 把 [first, first + n) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
/*****************************************************************************************/
    template<class InputIter, class Size, class ForwardIter>
    ForwardIter unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::true_type) {
        return stl::copy_n(first, n, result).second;
    }

    template<class InputIter, class Size, class ForwardIter>
    ForwardIter
    unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::false_type) {
        auto cur = result;
        try {
            for (; n > 0; --n, ++cur, ++first) {
                stl::construct(&*cur, *first);
            }
        }
        catch (...) {
            for (; result != cur; --cur)
                stl::destroy(&*cur);
        }
        return cur;
    }

    template<class InputIter, class Size, class ForwardIter>
    ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result) {
        return stl::unchecked_uninit_copy_n(first, n, result,
                                            std::is_trivially_copy_assignable<
                                                    typename iterator_traits<InputIter>::
                                                    value_type>{});
    }

/*****************************************************************************************/
// uninitialized_fill
// 在 [first, last) 区间内填充元素值
/*****************************************************************************************/
    template<class ForwardIter, class T>
    void unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T &value, std::true_type) {
        stl::fill(first, last, value);
    }

    template<class ForwardIter, class T>
    void
    unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T &value, std::false_type) {
        auto cur = first;
        try {
            for (; cur != last; ++cur) {
                stl::construct(&*cur, value);
            }
        }
        catch (...) {
            for (; first != cur; ++first)
                stl::destroy(&*first);
        }
    }

    template<class ForwardIter, class T>
    void uninitialized_fill(ForwardIter first, ForwardIter last, const T &value) {
        stl::unchecked_uninit_fill(first, last, value,
                                   std::is_trivially_copy_assignable<
                                           typename iterator_traits<ForwardIter>::
                                           value_type>{});
    }

/*****************************************************************************************/
// uninitialized_fill_n
// 从 first 位置开始，填充 n 个元素值，返回填充结束的位置
/*****************************************************************************************/
    template<class ForwardIter, class Size, class T>
    ForwardIter
    unchecked_uninit_fill_n(ForwardIter first, Size n, const T &value, std::true_type) {
        return stl::fill_n(first, n, value);
    }

    // value_type类型没有平凡拷贝构造函数，则需要在内存块上使用构造函数构造
    template<class ForwardIter, class Size, class T>
    ForwardIter
    unchecked_uninit_fill_n(ForwardIter first, Size n, const T &value, std::false_type) {
        auto cur = first;
        try {
            for (; n > 0; --n, ++cur) {
                stl::construct(&*cur, value);
            }
        }
        catch (...) {
            for (; first != cur; ++first)
                stl::destroy(&*first);
        }
        return cur;
    }

    template<class ForwardIter, class Size, class T>
    // 分为两种情况 1.可以平凡拷贝赋值，那么不需要初始化，这种平凡类型可以使用=来赋值
    // 2.不可以... 这种情况只能使用构造函数来进行初始化
    ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T &value) {
        return stl::unchecked_uninit_fill_n(first, n, value,
                                            std::is_trivially_copy_assignable<
                                                    typename iterator_traits<ForwardIter>::
                                                    value_type>{});
    }

/*****************************************************************************************/
// uninitialized_move
// 把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
/*****************************************************************************************/
    template<class InputIter, class ForwardIter>
    ForwardIter
    unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::true_type) {
        return stl::move(first, last, result);
    }

    template<class InputIter, class ForwardIter>
    ForwardIter
    unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::false_type) {
        ForwardIter cur = result;
        try {
            for (; first != last; ++first, ++cur) {
                stl::construct(&*cur, stl::move(*first));
            }
        }
        catch (...) {
            stl::destroy(result, cur);
        }
        return cur;
    }

    template<class InputIter, class ForwardIter>
    ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result) {
        return stl::unchecked_uninit_move(first, last, result,
                                          std::is_trivially_move_assignable<
                                                  typename iterator_traits<InputIter>::
                                                  value_type>{});
    }


/*****************************************************************************************/
// uninitialized_move_n
// 把[first, first + n)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
/*****************************************************************************************/
    template<class InputIter, class Size, class ForwardIter>
    ForwardIter
    unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::true_type) {
        return stl::move(first, first + n, result);
    }

    template<class InputIter, class Size, class ForwardIter>
    ForwardIter
    unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::false_type) {
        auto cur = result;
        try {
            for (; n > 0; --n, ++first, ++cur) {
                stl::construct(&*cur, stl::move(*first));
            }
        }
        catch (...) {
            for (; result != cur; ++result)
                stl::destroy(&*result);
            throw;
        }
        return cur;
    }

    template<class InputIter, class Size, class ForwardIter>
    ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result) {
        return stl::unchecked_uninit_move_n(first, n, result,
                                            std::is_trivially_move_assignable<
                                                    typename iterator_traits<InputIter>::
                                                    value_type>{});
    }

}

#endif //MYCPPSTL_UNINITIALIZED_H
