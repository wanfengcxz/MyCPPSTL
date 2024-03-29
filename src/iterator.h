//
// Created by 晚风吹行舟 on 2023/4/10.
//

#ifndef MYCPPSTL_ITERATOR_H
#define MYCPPSTL_ITERATOR_H

// some iterators, struct and global functions

#include <cstddef>
#include <iostream>

#include "type_traits.h"

namespace stl {

    // five types of iterators
    // 共性：
    // 1. 比较两个迭代器是否相等（==、!=）output迭代器不能比较
    // 2. 箭头运算符（->），解引用迭代器，并提取对象的成员。
    // 特性
    // 1. 只能递增，即单向移动，只能读数据，只能用于单遍扫描算法
    // 2. 只能递增，即单向移动，只能写数据，只能用于单遍扫描算法
    // 3. 只能递增，即单向移动，可读可写，可以用于多遍扫描算法
    // 4. 递增递减，即双向移动，可读可写，可以用于多遍扫描算法
    // 5. 比较两个迭代器相对位置的关系运算符（<、<=、>和>=）
    //  双向移动，可读可写，支持和一个整数值的加减运算（+、+=、-、-=）
    //  两个迭代器上的减法运算符（-），得到两个迭代器的距离。
    //  支持下标运算符（iter[n]）。可以用于多遍扫描算法

    // basic_ostream就是一个output_iterator
    // https://www.cnblogs.com/xuyd/p/17349731.html
    struct input_iterator_tag {
    };

    struct output_iterator_tag {
    };

    struct forward_iterator_tag : public input_iterator_tag {
    };

    struct bidirectional_iterator_tag : public forward_iterator_tag {
    };

    struct random_access_iterator_tag : public bidirectional_iterator_tag {
    };

    // iterator template
    template<class Category, class T, class Distance = ptrdiff_t,
            class Pointer = T *, class Reference = T &>
    struct iterator {
        typedef Category iterator_category;
        typedef T value_type;
        typedef Pointer pointer;
        typedef Reference reference;
        typedef Distance difference_type;
    };

    // iterator traits

    // 如果是内置类型，或者类中没有iterator_category则value为false
    // 否则value为true
    template<class T>
    struct has_iterator_cat {
    private:
        struct two {
            char a;
            char b;
        };

        template<class U>
        static two test(...) {
            std::cout << "two char version" << std::endl;
        };

        template<class U>
        static char test(typename U::iterator_category * = 0) {
            std::cout << "one char version" << std::endl;
        };

    public:

        static const bool value = sizeof(test<T>(0)) == sizeof(char);
    };

    template<class Iterator, bool>
    struct iterator_traits_impl {
    };

    // specializations
    template<class Iterator>
    struct iterator_traits_impl<Iterator, true> {
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type value_type;
        typedef typename Iterator::pointer pointer;
        typedef typename Iterator::reference reference;
        typedef typename Iterator::difference_type difference_type;
    };

    // 这里只有当传入bool为true时（执行下面的特化），才会有iterator_category这个属性
    // 否则推断错误
    template<class Iterator, bool>
            // 这个类内部什么也没有
    struct iterator_traits_helper {
    };

    // specializations
    template<class Iterator>
    struct iterator_traits_helper<Iterator, true> :
            // 1.如果Iterator中iterator_category是input/output_iterator_tag的子类，那么
            // 这个类就会继承iterator_traits_impl<Iterator,true>,而它里面有Iterator部分类型
            // 否则iterator_traits_impl<Iterator,false>,里面什么也没有
            public iterator_traits_impl<Iterator,
                    // C++ STL的std::is_convertible模板用于检查是否可以将任何数据类型A隐式转换为任何数据类型B。它返回布尔值true或false。
                    std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
                    std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value> {
    };

    // 萃取 迭代器的特性
    template<class Iterator>
    struct iterator_traits :
            // 根据Iterator这个类的特性来继承，如果有iterator_category并且确实是STL中合法的
            // 五种中任意一种，那么他里面就会包含Iterator的部分类型名,e.g. pointer reference...
            public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {
    };

    // 针对原生指针(int* double*)的偏特化版本
    template<class T>
    struct iterator_traits<T *> {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef ptrdiff_t difference_type;
    };

    template<class T>
    struct iterator_traits<const T *> {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;
        typedef ptrdiff_t difference_type;
    };

    // 判断当前类型T 到底是五种迭代器的哪一个
    template<class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
    struct has_iterator_cat_of :
            public m_bool_constant<std::is_convertible<
                    typename iterator_traits<T>::iterator_category, U>::value> {
    };

    // 萃取某种迭代器
    template<class T, class U>
    struct has_iterator_cat_of<T, U, false> : public m_false_type {
    };

    template<class Iter>
    struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {
    };

    template<class Iter>
    struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {
    };

    template<class Iter>
    struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {
    };

    template<class Iter>
    struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {
    };

    template<class Iter>
    struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {
    };

    template<class Iterator>
    struct is_iterator :
            public m_bool_constant<is_input_iterator<Iterator>::value ||
                                   is_output_iterator<Iterator>::value> {

    };


    // 萃取某个迭代器的 category
    template<class Iterator>
    typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator &) {
        typedef typename iterator_traits<Iterator>::iterator_category Category;
        return Category();
    }

    // 萃取某个迭代器的 distance_type
    template<class Iterator>
    typename iterator_traits<Iterator>::difference_type *
    distance_type(const Iterator &) {
        return static_cast<typename iterator_traits<Iterator>::difference_type *>(0);
    }

    // 萃取某个迭代器的 value_type
    template<class Iterator>
    typename iterator_traits<Iterator>::value_type *
    value_type(const Iterator &) {
        return static_cast<typename iterator_traits<Iterator>::value_type *>(0);
    }

    // 以下函数用于计算迭代器间的距离

    // distance 的 input_iterator_tag 的版本
    /// TODO:input_iterator只能访问一次，那么获取距离之后不就失效了吗？
    template<class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) {
            ++first;
            ++n;
        }
        return n;
    }

    // distance 的 random_access_iterator_tag 的版本
    template<class RandomIter>
    typename iterator_traits<RandomIter>::diference_type
    distance_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag) {
        return last - first;
    }

    template<class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last) {
        distance_dispatch<InputIterator>(first, last, iterator_category(first));
    }

    // 以下函数用于让迭代器前进 n 个距离

    // TODO:测试output_iterator和forward_iterator该怎么办
    // advance 的 input_iterator_tag 的版本
    template<class InputIterator, class Distance>
    void advance_dispatch(InputIterator &i, Distance n, input_iterator_tag) {
        while (n--) ++i;
    }

    // advance 的 bidirectional_iterator_tag 的版本
    template<class BidirectionalIterator, class Distance>
    void advance_dispatch(BidirectionalIterator &i, Distance n, bidirectional_iterator_tag) {
        if (n >= 0) while (n--) ++i;
        else while (n++) --i;
    }

    // advance 的 random_access_iterator_tag 的版本
    template<class RandomIterator, class Distance>
    void advance_dispatch(RandomIterator &i, Distance n, random_access_iterator_tag) {
        i += n;
    }

    template<class InputIterator, class Distance>
    void advance(InputIterator &i, Distance n) {
        return advance_dispatch(i, n, iterator_category(i));
    }

    /*******************************************************************************************/

    // 模板类: reverse_iterator
    // 反向迭代器，使前进为后退，后退为前进
    template<class Iterator>
    class reverse_iterator {
    private:
        Iterator current;   // 记录对应的正向迭代器

    public:
        // 反向迭代器的五种相应类型
        typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
        typedef typename iterator_traits<Iterator>::value_type value_type;
        typedef typename iterator_traits<Iterator>::difference_type difference_type;
        typedef typename iterator_traits<Iterator>::pointer pointer;
        typedef typename iterator_traits<Iterator>::reference reference;

        typedef Iterator iterator_type;
        typedef reverse_iterator<Iterator> self;

    public:
        // 构造函数
        reverse_iterator() {}

        explicit reverse_iterator(iterator_type i) : current(i) {}

        reverse_iterator(const self &rhs) : current(rhs.current) {}

    public:
        // 取出对应的正向迭代器
        iterator_type base() const {
            return current;
        }

        // 重载操作符
        reference operator*() const {
            // 实际对应正向迭代器的前一个位置
            auto tmp = current;
            return *--tmp;
        }

        pointer operator->() const {
            return &(operator*());
        }

        // 前进++变为后退--
        self &operator++() {
            --current;
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            --current;
            return tmp;
        }

        // 后退(--)变为前进(++)
        self &operator--() {
            ++current;
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            ++current;
            return tmp;
        }

        self operator+=(difference_type n) const {
            current -= n;
            return *this;
        }

        self operator+(difference_type n) const {
            return self(current - n);
        }

        self &operator-=(difference_type n) {
            current += n;
            return *this;
        }

        self operator-(difference_type n) const {
            return self(current + n);
        }

        reference operator[](difference_type n) const {
            return *(*this + n);
        }

    };


// 重载 operator-
    template<class Iterator>
    typename reverse_iterator<Iterator>::difference_type
    operator-(const reverse_iterator<Iterator> &lhs,
              const reverse_iterator<Iterator> &rhs) {
        return rhs.base() - lhs.base();
    }

// 重载比较操作符
    template<class Iterator>
    bool operator==(const reverse_iterator<Iterator> &lhs,
                    const reverse_iterator<Iterator> &rhs) {
        return lhs.base() == rhs.base();
    }

    template<class Iterator>
    bool operator<(const reverse_iterator<Iterator> &lhs,
                   const reverse_iterator<Iterator> &rhs) {
        return rhs.base() < lhs.base();
    }

    template<class Iterator>
    bool operator!=(const reverse_iterator<Iterator> &lhs,
                    const reverse_iterator<Iterator> &rhs) {
        return !(lhs == rhs);
    }

    template<class Iterator>
    bool operator>(const reverse_iterator<Iterator> &lhs,
                   const reverse_iterator<Iterator> &rhs) {
        return rhs < lhs;
    }

    template<class Iterator>
    bool operator<=(const reverse_iterator<Iterator> &lhs,
                    const reverse_iterator<Iterator> &rhs) {
        return !(rhs < lhs);
    }

    template<class Iterator>
    bool operator>=(const reverse_iterator<Iterator> &lhs,
                    const reverse_iterator<Iterator> &rhs) {
        return !(lhs < rhs);
    }
}

#endif //MYCPPSTL_ITERATOR_H
