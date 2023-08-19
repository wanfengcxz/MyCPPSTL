//
// Created by 晚风吹行舟 on 2023/4/9.
//

#ifndef MYCPPSTL_ALLOCATOR_H
#define MYCPPSTL_ALLOCATOR_H

// 这个头文件包含一个模板类 allocator，用于管理内存的分配、释放，对象的构造、析构

#include "construct.h"
#include "utils.h"

namespace stl {

    template<class T>
    class allocator {
    public:
        typedef T value_type;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef T &reference;
        typedef const T &const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    public:
        static T *allocate();

        static T *allocate(size_type n);

        static void deallocate(T *ptr);

        static void deallocate(T *ptr, size_type n);

        static void construct(T *ptr);

        static void construct(T *ptr, const T &value);

        static void construct(T *ptr, T &&value);


        template<class ... Args>
        static void construct(T *ptr, Args &&...args);

        static void destroy(T *ptr);

        static void destroy(T *first, T *last);

    };

    template<class T>
    T *allocator<T>::allocate() {
        return static_cast<T *>(::operator new(sizeof(T)));
    }

    template<class T>
    T *allocator<T>::allocate(size_type n) {
        if (n == 0) return nullptr;
        return static_cast<T *>(::operator new(sizeof(T) * n));
    }

    template<class T>
    void allocator<T>::deallocate(T *ptr) {
        if (ptr == nullptr) return;
        ::operator delete(ptr);
    }

    template<class T>
    void allocator<T>::deallocate(T *ptr, size_type n) {
        if (ptr == nullptr) return;
        ::operator delete(ptr);
    }

    template<class T>
    void allocator<T>::construct(T *ptr) {
        if (ptr == nullptr) return ;
        stl::construct(ptr);
    }

    template<class T>
    void allocator<T>::construct(T *ptr, const T &value) {
        if (ptr == nullptr) return ;
        stl::construct(ptr, value);
    }

    template<class T>
    void allocator<T>::construct(T *ptr, T &&value) {
        if (ptr == nullptr) return ;
        // TODO: 他用的move
        stl::construct(ptr, stl::forward<T>(value));
    }

    template<class T>
    template<class ...Args>
    void allocator<T>::construct(T *ptr, Args &&...args) {
        // TODO: ...什么意思
        stl::construct(ptr, stl::forward<Args>(args)...);
    }

    template<class T>
    void allocator<T>::destroy(T *ptr) {
        stl::destroy(ptr);
    }

    template<class T>
    void allocator<T>::destroy(T *first, T *last) {
        stl::destroy(first, last);
    }
}


#endif //MYCPPSTL_ALLOCATOR_H
