//
// Created by 晚风吹行舟 on 2023/4/11.
//

#ifndef MYCPPSTL_CONSTRUCT_H
#define MYCPPSTL_CONSTRUCT_H


// this head file contains two function: construct, destroy
// construct: 负责对象的构造
// destroy: 负责对象的析构

#include <new>

#include "type_traits.h"
#include "iterator.h"
#include "utils.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)  // unused parameter
#endif // _MSC_VER


namespace stl {

    // construct 构造对象
    template<class Ty>
    void construct(Ty *ptr) {
        // 在ptr指针指向的地址
        ::new((void *) ptr)Ty();
    }

    template<class Ty1, class Ty2>
    void construct(Ty1 *ptr, Ty2 &value) {
        ::new((void *) ptr)Ty1(value);
    }

    template<class Ty, class ... Args>
    void construct(Ty *ptr, Args &&...args) {
        ::new((void *) ptr)Ty(stl::forward<Args>(args)...);
    }

    // destroy 将对象析构
    template<class Ty>
    void destroy_one(Ty *, std::true_type) {
    }

    template<class Ty>
    void destroy_one(Ty *pointer, std::false_type) {
        if (pointer != nullptr) {
            pointer->~Ty();
        }
    }

    template<class ForwardIter>
    // 如果是平凡可析构 直接自动释放
    void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

    template<class ForwardIter>
    void destroy_cat(ForwardIter first, ForwardIter end, std::false_type) {
        // 为什么要加&*
        for (; first != end; first++) destroy_one(&*first);
    }

    template<class Ty>
    void destroy(Ty *pointer) {
        //         destroy_one(pointer, std::is_trivially_destructible<Ty>{});
        // 如果可以平凡析构，那么就不用调用析构函数
        destroy_one(pointer, std::is_trivially_destructible<Ty>());
    }

    template<class ForwardIter>
    void destroy(ForwardIter first, ForwardIter end) {
        destroy_cat(first, end, std::is_trivially_destructible<
                typename iterator_traits<ForwardIter>::value_type>{});
    }

}   // namespace stl

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif //MYCPPSTL_CONSTRUCT_H
