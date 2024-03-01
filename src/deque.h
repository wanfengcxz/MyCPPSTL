//
// Created by 晚风吹行舟 on 2023/9/19.
//

#ifndef MYCPPSTL_DEQUE_H
#define MYCPPSTL_DEQUE_H

// 异常保证：
// mystl::deque<T> 满足基本异常保证，部分函数无异常保证，并对以下等函数做强异常安全保证：
//   * emplace_front
//   * emplace_back
//   * emplace
//   * push_front
//   * push_back
//   * insert

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "utils.h"
#include "exceptdef.h"

namespace stl {
    /// #Pragma设定编译器的状态或者指示编译器完成一些特定的动作。

#ifdef max
#pragma message("#undefing marco max")
#undef max      /// 取消已定义的宏
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min


#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

    template<class T>
    struct deque_buf_size {
        // 保证元素个数不少于16
        static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
    };

    // deque 的迭代器
    template<class T, class Ref, class Ptr>
    struct deque_iterator : public iterator<random_access_iterator_tag, T> {
        /**
         * deque_iterator继承了iterator
         * 在STL中，对于iterator类，最初本想所有的迭代器实现类均继承之，但是从一开始就没有使用过！
         * 仅仅成为了可选的实现。所以在C++17中将该类废弃了，所以在官方STL中看不到这种继承
         *
         * deque由多段连续缓冲区构成，缓冲区之间不连续，因此它的迭代器需要维护其整体连续的假象
         * 即能够知道自己是否处于某个缓冲区的末尾，并且往后移动时能够跳转到下一个缓冲区的头部
         */

        typedef deque_iterator<T, T &, T *> iterator;
        typedef deque_iterator<T, const T &, const T *> const_iterator;
        typedef deque_iterator self;

        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T *value_pointer;
        typedef T **map_pointer;

        /// 缓冲区大小

        static const size_type buffer_size = deque_buf_size<T>::value;

        value_pointer cur;      // 指向目前缓冲区的当前元素
        value_pointer first;    // 指向目前缓冲区头部
        value_pointer last;     // 指向目前缓冲区尾部
        map_pointer node;       // 指向map中的某个缓冲区

        /// constructor(copy move)

        deque_iterator() noexcept
                : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

        deque_iterator(value_pointer v, map_pointer n)
                : cur(v), first(*n), last(*n + buffer_size), node(n) {}

        // 直接把指针拷贝一份，迭代器只起到定位的作用，不回去释放内存
        explicit deque_iterator(const iterator &rhs)
                : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

        explicit deque_iterator(iterator &&rhs) noexcept
                : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {
            rhs.cur = rhs.first = rhs.last = rhs.node = nullptr;
        }

        explicit deque_iterator(const const_iterator &rhs) : cur(rhs.cur), first(rhs.first), last(rhs.last),
                                                             node(rhs.node) {}

        self &operator=(const iterator &rhs) {
            if (this != &rhs) {
                cur = rhs.cur;
                first = rhs.first;
                last = rhs.last;
                node = rhs.node;
            }
            return *this;
        }

        // 切换到另一个缓冲区
        void set_node(map_pointer new_node) {
            node = new_node;
            first = *new_node;
            last = first + buffer_size;
            // cur需要在这个函数外进行改变
            // set_node只是专注更新node的边界，即first和last
        }

        /// 重载运算符

        // 迭代器解引用就是某个元素
        reference operator*() const { return *cur; }

        // 如果一个类重载了指针运算符 obj->m等价于(obj.operator->())->m
        pointer operator->() const { return cur; }

        difference_type operator-(const self &x) const {
            return static_cast<difference_type >(buffer_size * (node - x.node) +
                                                 (cur - first) - (x.cur - x.first));
        }

        /// 迭代器只能通过重载的++,--,+=,-=进行移动
        // TODO:node万一越界呢？ 非常危险
        self &operator++() {
            ++cur;
            if (cur == last) {
                set_node(node + 1);
                cur = first;
            }
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            ++(*this);
            return tmp;
        }

        self &operator--() {
            if (cur == first) {
                set_node(node - 1);
                cur = last;
            }
            --cur;
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            --(*this);
            return tmp;
        }

        self &operator+=(difference_type n) {
            // n可能为负数 假设buffer_size为16
            // 注意：offset=[0,15]定位到当前缓冲区 offset=[-16,-1]定位到上一个缓冲区
            const auto offset = n + (cur - first);
            if (offset >= 0 && offset < static_cast<difference_type>(buffer_size)) {
                cur = first + offset;
            } else {
                const auto step = offset > 0 ?
                                  offset / static_cast<difference_type>(buffer_size)
                                             : -static_cast<difference_type>((-offset - 1) / buffer_size) - 1;
                set_node(node + step);
                cur = first + (offset - step * static_cast<difference_type>(buffer_size));
            }
            return *this;
        }

        self operator+(difference_type n) {
            self tmp = *this;
            return tmp += n;
        }

        self &operator-=(difference_type n) {
            return (*this) += (-n);
        }

        self operator-(difference_type n) {
            self tmp = *this;
            return tmp += (-n);
        }

        reference operator[](difference_type n) const {
            // 返回基于当前位置移动n步后cur指向的元素的引用
            return *(*this + n);
        }

        bool operator==(const self &rhs) const {
            return cur == rhs.cur;
        }

        bool operator!=(const self &rhs) const {
            return !(cur == rhs.cur);
        }

        bool operator<(const self &rhs) const {
            return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node);
        }

        bool operator>(const self &rhs) const {
            return rhs < (*this);
        }

        bool operator<=(const self &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const self &rhs) const {
            return !(*this < rhs);
        }
    };

    template<class T>
    class deque {
    public:
        typedef stl::allocator<T> allocator_type;
        typedef stl::allocator<T> data_allocator;   // 返回T *
        typedef stl::allocator<T *> map_allocator;  // 返回T **

        typedef typename allocator_type::value_type value_type;
        typedef typename allocator_type::pointer pointer;
        typedef typename allocator_type::const_pointer const_pointer;
        typedef typename allocator_type::reference reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type size_type;
        typedef typename allocator_type::difference_type difference_type;
        typedef pointer *map_pointer;
        typedef const_pointer *const_map_pointer;

        typedef deque_iterator<T, T &, T *> iterator;
        typedef deque_iterator<T, const T &, const T *> const_iterator;
        typedef stl::reverse_iterator<iterator> reverse_iterator;
        typedef stl::reverse_iterator<const_iterator> const_reverse_iterator;

        allocator_type get_allocator() { return allocator_type(); }

        // 缓冲区的大小
        static const size_type buffer_size = deque_buf_size<T>::value;

    private:
        /// 通过以下四个变量来构造一个deque
        iterator begin_;        // 指向第一个节点
        iterator end_;          // 指向最后一个节点的下一个节点
        map_pointer map_;       /* T** 指向一组连续的数据块，每个数据块类型为T*
                                 每个数据块指向一个长为buffer_size的缓冲区 */
        size_type map_size_;    // 数据块的个数

    public:

        /// constructor 构造，拷贝，移动，析构

        deque() {
            fill_init(0, value_type{});
        }

        explicit deque(size_type n) {
            fill_init(n, value_type{});
        }

        deque(size_type n, const value_type &value) {
            fill_init(n, value);
        }

        template<class IIter, typename std::enable_if<
                stl::is_input_iterator<IIter>::value, int>::type = 0>
        deque(IIter first, IIter last) {
            // TODO:测试，使用vector/数组来创建deque
            copy_init(first, last, stl::iterator_category(first));
        }

        deque(std::initializer_list<value_type> init_list) {
            copy_init(init_list.begin(), init_list.end(), stl::forward_iterator_tag());
        }

        deque(const deque &rhs) {
            copy_init(rhs.begin(), rhs.end(), stl::forward_iterator_tag());
        }

        deque(deque &&rhs) noexcept
                : begin_(stl::move(rhs.begin_)), end_(stl::move(rhs.end_)),
                  map_size_(rhs.map_size_), map_(rhs.map_) {
            rhs.map_ = nullptr;
            rhs.map_size_ = 0;
        }

        deque &operator=(const deque &rhs);

        deque &operator=(deque &&rhs) noexcept;

        deque &operator=(std::initializer_list<value_type> init_list) {
            deque tmp(init_list);
            swap(tmp);
            return *this;   // tmp会在函数结束时自动调用析构函数销毁
        }

        ~deque() {
            if (map_ != nullptr) {
                clear();
                for (auto cur = map_; cur < map_ + map_size_; ++cur) {
                    data_allocator::deallocate(*cur, buffer_size);
                    *cur = nullptr;
                }
                map_allocator::deallocate(map_, map_size_);
                map_ = nullptr;
            }
        }


    public:

        /// iterator

        iterator begin() noexcept { return begin_; }

        const_iterator begin() const noexcept { return begin_; }

        iterator end() noexcept { return end_; }

        const_iterator end() const noexcept { return end_; }


        reverse_iterator rbegin() noexcept { return stl::reverse_iterator(end_); }

        const_reverse_iterator rbegin() const noexcept { return stl::reverse_iterator(end_); }

        reverse_iterator rend() noexcept { return stl::reverse_iterator(begin_); }

        const_reverse_iterator rend() const noexcept { return stl::reverse_iterator(begin_); }


        const_iterator cbegin() const noexcept { return begin_; }

        const_iterator cend() const noexcept { return end_; }

        const_iterator crbegin() const noexcept { return stl::reverse_iterator(end_); }

        const_iterator crend() const noexcept { return stl::reverse_iterator(begin_); }


        /// 容量相关操作

        bool empty() const noexcept { return begin_ == end_; }

        size_type size() const noexcept { return end_ - begin_; }

        size_type max_size() const noexcept { return static_cast<size_type>(-1); }

        void resize(size_type new_size) { return resize(new_size, value_type{}); }

        void resize(size_type new_size, const value_type &value);

        void shrink_to_fit() noexcept;

        /// 访问元素相关操作
        reference operator[](size_type n) {
            STL_DEBUG(n < size());
            return begin_[n];
        }

        const_reference operator[](size_type n) const {
            STL_DEBUG(n < size());
            return begin_[n];
        }

        reference at(size_type n) {
            STL_DEBUG(n < size());
            return begin_[n];
        }

        const_reference at(size_type n) const {
            STL_DEBUG(n < size());
            return begin_[n];
        }

        reference front() {
            STL_DEBUG(!empty());
            return *begin();
        }

        const_reference front() const {
            STL_DEBUG(!empty());
            return *begin();
        }

        reference back() {
            STL_DEBUG(!empty());
            return *(end() - 1);
        }

        const_reference back() const {
            STL_DEBUG(!empty());
            return *(end() - 1);
        }

        /// 修改容器相关操作

        // assign
        void assign(size_type n, const value_type &value) {
            fill_assign(n, value);
        }

        /// TODO: input_iterator只能访问一次吗？
//        template<class IIter, typename std::enable_if<
//                stl::is_input_iterator<IIter>::value, int>::type = 0>
//        void assign(IIter first, IIter end) {
//            copy_assign(first, end, iterator_category(first));
//        }

        void assign(std::initializer_list<value_type> init_list) {
            copy_assign(init_list.begin(), init_list.end(), stl::forward_iterator_tag{});
        }

        // emplace_front / emplace_back / emplace
        template<class ...Args>
        void emplace_front(Args &&...args);

        template<class ...Args>
        void emplace_back(Args &&...args);

        template<class ...Args>
        iterator emplace(iterator pos, Args &&...args);

        // push_front / push_back
        void push_front(const value_type &value);

        void push_back(const value_type &value);

        // pop_back / pop_front
        void pop_front();

        void pop_back();

        // insert

        iterator insert(iterator pos, const value_type &value);

        iterator insert(iterator pos, value_type &&value);

        void insert(iterator pos, size_type n, const value_type &value);

        template<class IIter, typename std::enable_if<stl::is_input_iterator<
                IIter>::value, int>::type = 0>
        void insert(iterator pos, IIter first, IIter last) {
            insert_dispatch(pos, first, last, input_iterator_tag{});
        }

        // erase / clear
        iterator erase(iterator pos);

        // 删除[first, last)内的元素
        iterator erase(iterator first, iterator last);

        // 清空缓冲区内的数据，但并不会改变缓冲区大小
        void clear();

        // swap
        void swap(deque &rhs) noexcept;

    private:

        /// help function

        /// creator node / destroy node
        map_pointer create_map(size_type size);

        void create_buffer(map_pointer start, map_pointer finish);

        void destroy_buffer(map_pointer start, map_pointer finish);

        // initialize
        void map_init(size_type n_elem);

        void fill_init(size_type n, const value_type &value);

        template<class IIter>
        void copy_init(IIter first, IIter last, input_iterator_tag);

        template<class FIter>
        void copy_init(FIter first, FIter last, forward_iterator_tag);

        // assign
        void fill_assign(size_type n, const value_type &value);

        template<class IIter>
        void copy_assign(IIter first, IIter last, input_iterator_tag);

        template<class FIter>
        void copy_assign(FIter first, FIter last, forward_iterator_tag);

        // insert
        template<class... Args>
        iterator insert_aux(iterator pos, Args &&...args    );

        void fill_insert(iterator position, size_type n, const value_type &value);

        template<class FIter>
        void copy_insert(iterator, FIter, FIter, size_type);

        template<class IIter>
        void insert_dispatch(iterator pos, IIter first, IIter last, input_iterator_tag);

        template<class FIter>
        void insert_dispatch(iterator, FIter, FIter, forward_iterator_tag);

        // reallocate

        void require_capacity(size_type n, bool front);

        void reallocate_map_at_front(size_type need);

        void reallocate_map_at_back(size_type need);
    };


    template<class T>
    deque<T> &deque<T>::operator=(const deque<T> &rhs) {
        // 必须是this!=&rhs 不能是*this != rhs
        if (this != &rhs) {
            const auto len = size();
            if (len >= rhs.size()) {
                erase(stl::copy(rhs.begin_, rhs.end_, begin_), end_);
            } else {
                iterator mid = rhs.begin() + static_cast<difference_type>(len);
                stl::copy(rhs.begin_, mid, begin_);
                insert(end_, mid, rhs.end_);
            }
        }
        return *this;
    }

    template<class T>
    deque<T> &deque<T>::operator=(deque<T> &&rhs) noexcept {
        clear();
        begin_ = stl::move(rhs.begin_);
        end_ = stl::move(rhs.end_);
        map_ = rhs.map_;
        map_size_ = rhs.map_size_;

        rhs.map_ = nullptr;
        rhs.map_size_ = 0;
        return *this;
    }

    template<class T>
    void deque<T>::resize(deque::size_type new_size, const value_type &value) {
        const auto len = size();
        if (new_size < len) {
            erase(begin_ + new_size, end_);
        } else {
            insert(end_, new_size - len, value);
        }
    }


    // 减小容器容量
    template<class T>
    void deque<T>::shrink_to_fit() noexcept {
        // 完全为空的缓冲区(竖条)会被释放
        for (auto cur = map_; cur < begin_.node; ++cur) {
            data_allocator::deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
        for (auto cur = end_.node + 1; cur < map_ + map_size_; ++cur) {
            data_allocator::deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
    }

    template<class T>
    template<class ...Args>
    void deque<T>::emplace_front(Args &&...args) {
        if (begin_.cur != begin_.first) {
            data_allocator::construct(begin_.cur - 1, stl::forward<Args>(args)...);
            --begin_.cur;
        } else {
            require_capacity(1, true);
            try {
                --begin_;
                data_allocator::construct(begin_.cur, stl::forward<Args>(args)...);
            } catch (...) {
                ++begin_;
                throw;
            }
        }
    }

    template<class T>
    template<class ...Args>
    void deque<T>::emplace_back(Args &&...args) {
        // 注意是 end_.last-1
        if (end_.cur != end_.last - 1) {
            data_allocator::construct(end_.cur, stl::forward<Args>(args)...);
            ++end_.cur;
        } else {
            require_capacity(1, false);
            data_allocator::construct(end_.cur, stl::forward<Args>(args)...);
            ++end_;
        }
    }

    template<class T>
    template<class ...Args>
    typename deque<T>::iterator deque<T>::emplace(iterator pos, Args &&...args) {
        if (pos.cur == begin_.cur) {
            emplace_front(stl::forward<Args>(args)...);
            return begin_;
        } else if (pos.cur == end_.cur) {
            emplace_back(stl::forward<Args>(args)...);
            return end_ - 1;
        }
        return insert_aux(pos, stl::forward<Args>(args)...);
    }

    template<class T>
    void deque<T>::push_front(const value_type &value) {
        if (begin_.cur != begin_.first) {
            /// 此处对已存在的内存空间来构造对象，这种情况下如果抛出异常，不需要回滚begin_
            /// 因为传入的是临时变量，所以就不需要catch了，默认会往上抛出
            data_allocator::construct(begin_.cur - 1, value);
            --begin_;
        } else {
            require_capacity(1, true);
            try {
                --begin_;
                /// 此处如果构造失败，需要回滚begin_，因此需要先catch，回滚，然后再抛出
                data_allocator::construct(begin_.cur, value);
            } catch (...) {
                ++begin_;
                throw;
            }
        }
    }

    template<class T>
    void deque<T>::push_back(const value_type &value) {
        if (end_.cur != end_.last - 1) {
            data_allocator::construct(end_.cur, value);
            ++end_.cur;
        } else {
            require_capacity(1, false);
            data_allocator::construct(end_.cur, value);
            /// 此处不可以为++end_.cur，因为要往下一个缓冲区走，迭代器可以完成这一操作，
            /// 仅仅是指针自增不可以
            ++end_;
        }
    }

    template<class T>
    void deque<T>::pop_front() {
        STL_DEBUG(!empty());
        if (begin_.cur != begin_.last - 1) {
            data_allocator::destroy(begin_.cur);
            ++begin_.cur;
        } else {
            data_allocator::destroy(begin_.cur);
            // 要跨过缓冲区 所以需要用迭代器
            ++begin_;
            destroy_buffer(begin_.node - 1, begin_.node - 1);
        }
    }

    template<class T>
    void deque<T>::pop_back() {
        STL_DEBUG(!empty());
        if (end_.cur != end_.first) {
            data_allocator::destroy(end_.cur - 1);
            end_.cur--;
        } else {
            try {
                --end_;
                data_allocator::destroy(end_.cur);
                destroy_buffer(end_.node + 1, end_.node + 1);
            } catch (...) {
                ++end_;
                throw;
            }
        }
    }

    template<class T>
    typename deque<T>::iterator deque<T>::insert(iterator pos, const value_type &value) {
        if (pos.cur == begin_.cur) {
            push_front(value);
            return begin_;
        } else if (pos.cur == end_.cur) {
            push_back(value);
            auto tmp = end_;
            tmp--;
            return tmp;
        } else {
            return insert_aux(pos, value);
        }
    }

    template<typename T>
    typename deque<T>::iterator deque<T>::insert(iterator pos, value_type &&value) {
        if (pos.cur == begin_.cur) {
            emplace_front(value);
            return begin_;
        } else if (pos.cur == end_.cur) {
            emplace_back(value);
            auto tmp = end_;
            tmp--;
            return tmp;
        } else {
            // value一定是右值，因此使用move
            return insert_aux(pos, stl::move(value));
        }
    }

    template<typename T>
    void deque<T>::insert(iterator pos, size_type n, const value_type &value) {
        if (pos.cur == begin_.cur) {
            require_capacity(n, true);
            auto new_begin = begin_ - n;
            stl::uninitialized_fill_n(new_begin, n, value);
            begin_ = new_begin;
        } else if (pos.cur == end_.cur) {
            require_capacity(n, false);
            stl::uninitialized_fill_n(end_, n, value);
            end_ += n;
        } else {
            fill_insert(pos, n, value);
        }
    }

    template<typename T>
    typename deque<T>::iterator deque<T>::erase(iterator pos) {
        auto next = pos;
        ++next;
        const size_type elems_before = pos - begin_;
        if (elems_before < (size() / 2)) {
            stl::copy_backward(begin_, pos, next);
            pop_front();
        } else {
            stl::copy(next, end_, pos);
            pop_back();
        }
        return begin_ + elems_before;
    }

    template<class T>
    typename deque<T>::iterator deque<T>::erase(iterator first, iterator last) {
        if (first.cur == begin_.cur && last.cur == end_.cur) {
            clear();
            return end_;
        } else {
            const size_type len = last - first;
            const size_type elems_before = first - begin_;
            if (elems_before < (size() - len) / 2) {
                stl::copy_backward(begin_, first, last);
                auto new_begin = begin_ + len;
                // TODO:源项目传参是begin_.cur 是错误的
                data_allocator::destroy(begin_, new_begin);
                begin_ = new_begin;
            } else {
                stl::copy(last, end_, first);
                auto new_end = end_ - len;
                data_allocator::destroy(new_end, end_);
                end_ = new_end;
            }
            // 删除元素后 收缩节点数量
            shrink_to_fit();
            return begin_ + elems_before;
        }

    }


    template<class T>
    void deque<T>::clear() {
        /// 摧毁所有缓冲区的对象 将end_移动到begin_
        for (auto cur = begin_.node + 1; cur != end_.node; ++cur) {
            // 释放中间缓冲区的对象
            data_allocator::destroy(*cur, *cur + buffer_size);
        }
        if (begin_.node != end_.node) {
            /// 有两个以上的缓冲区
            // 释放第一个缓冲区和最后一个缓冲区内的对象
            // TODO:STL中使用stl::destroy
            data_allocator::destroy(begin_.cur, begin_.last);
            data_allocator::destroy(end_.first, end_.cur);
        } else {
            data_allocator::destroy(begin_.cur, end_.cur);
        }
        shrink_to_fit();
        end_ = begin_;
    }

    template<class T>
    void deque<T>::swap(deque<T> &rhs) noexcept {
        if (this != &rhs) {
            stl::swap(begin_, rhs.begin_);
            stl::swap(end_, rhs.end_);
            stl::swap(map_, rhs.map_);
            stl::swap(map_size_, rhs.map_size_);
        }
    }

/**************************************************************************/
    /// help function

    template<class T>
    typename deque<T>::map_pointer deque<T>::create_map(size_type size) {
        /// 创建map数据块 并将每个都置为空
        map_pointer mp = nullptr;
        mp = map_allocator::allocate(size);
        for (size_type i = 0; i < size; ++i)
            *(mp + i) = nullptr;
        return mp;
    }

    template<class T>
    void deque<T>::create_buffer(map_pointer node_start, map_pointer node_finish) {
        /// 为区间[node_start, node_finish]区间内的T**指针分配缓冲区

        map_pointer cur;
        try {
            for (cur = node_start; cur <= node_finish; ++cur) {
                *cur = data_allocator::allocate(buffer_size);
            }
        }
        catch (...) {
            while (cur != node_start) {
                --cur;
                data_allocator::deallocate(*cur, buffer_size);
                *cur = nullptr;
            }
            throw;
        }
    }

    template<class T>
    void deque<T>::destroy_buffer(map_pointer node_start, map_pointer node_finish) {
        map_pointer cur = node_start;
        while (cur <= node_finish) {
            data_allocator::deallocate(*cur, buffer_size);
            *cur = nullptr;
            ++cur;
        }
    }

    template<class T>
    void deque<T>::map_init(size_type n_elem) {
        /// 初始化map数据块，为中心的数据块分配缓冲区空间，两边分别预留出一些空的map数据块（没有分配缓冲区）

        const size_type n_node = n_elem / buffer_size + 1;
        map_size_ = stl::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), n_node + 2);
        try {
            map_ = create_map(map_size_);
        } catch (...) {
            map_ = nullptr;
            map_size_ = 0;
            throw;
        }

        // map_size_和n_node至少相差2 这样以来，左右至少都可以留出一个空的map 数据块
        map_pointer node_start = map_ + (map_size_ - n_node) / 2;
        map_pointer node_finish = node_start + n_node - 1;
        try {
            create_buffer(node_start, node_finish);
        } catch (...) {
            map_allocator::deallocate(map_, map_size_);
            map_ = nullptr;
            map_size_ = 0;
            throw;
        }
        begin_.set_node(node_start);
        end_.set_node(node_finish);
        begin_.cur = begin_.first;
        end_.cur = end_.first + (n_elem % buffer_size); // 指向最后一个元素的下一个
    }

    template<class T>
    void deque<T>::fill_init(deque::size_type n, const value_type &value) {
        map_init(n);
        if (n == 0) return;
        // TODO:为什么不能直接这样做？
//        uninitialized_fill(begin_, end_, value);

        // STL中是如下做法
        for (auto cur = begin_.node; cur < end_.node; ++cur)
            stl::uninitialized_fill(*cur, *cur + buffer_size, value);
        stl::uninitialized_fill(end_.first, end_.cur, value);
    }

    template<class T>
    template<class IIter>
    void deque<T>::copy_init(IIter first, IIter last, input_iterator_tag) {
        /**
         * 只能一次一个向前读取元素，按此顺序一个个传回元素值。Input迭代器只能读取元素一次，
         * 如果你复制Input迭代器，并使原Input迭代器与新产生的副本都向前读取，可能会遍历到不同的值。
         * 纯粹Input迭代器的一个典型例子就是“从标准输入装置（通常为键盘）读取数据”的迭代器
         */

        // size_type为uint64，ptrdiff_t为int64
        // 但一般last都会大于first，做差是正数，因此不会出错
        const size_type n = stl::distance(first, last);
        map_init(n);
        while (first != last) {
            emplace_back(*first);
            ++first;
        }
    }

    template<class T>
    template<class FIter>
    void deque<T>::copy_init(FIter first, FIter last, forward_iterator_tag) {
        /**
         * Forward迭代器能多次指向同一群集中的同一元素，并能多次处理同一元素。
         */

        const size_type n = stl::distance(first, last);
        map_init(n);
        for (auto cur = begin_.node; cur < end_.node; ++cur) {
            auto next = first;
            stl::advance(next, buffer_size);
            stl::uninitialized_copy(first, next, *cur);
            first = next;
        }
        stl::uninitialized_copy(first, last, end_.first);
    }

    template<class T>
    void deque<T>::fill_assign(deque::size_type n, const value_type &value) {
        if (n > size()) {
            stl::fill(begin_, end_, value);
            insert(end_, n - size(), value);
        } else {
            erase(begin_ + n, end_);
            stl::fill(begin_, end_, value);
        }
    }

    template<class T>
    template<class IIter>
    void deque<T>::copy_assign(IIter first, IIter last, input_iterator_tag) {
//        auto first1 = begin();
//        auto last1 = end();
//        for (; first != last && first1 != last1; ++first, ++first1) {
//            *first1 = *first;
//        }
//        if (first1 != last1) {
//            erase(first1, end_);
//        }
//        if (first != last) {
//            insert_dispatch(end_, first, last, input_iterator_tag{});
//        }
    }

    template<class T>
    template<class FIter>
    void deque<T>::copy_assign(FIter first, FIter last, forward_iterator_tag) {
        const size_type len1 = size();
        // input类型的iter只要遍历过一次就失效了，只适用于单次遍历算法
        // forward类型的iter适用于多次遍历算法，因此这里可以使用forward来取长度
        const size_type len2 = stl::distance(first, last);
        if (len1 >= len2) {
            erase(stl::copy(first, last, begin_), end_);
        } else {
            auto next = first;
            stl::advance(next, len1);
            stl::copy(first, next, begin_);
            insert_dispatch(end_, next, last, forward_iterator_tag{});
        }
    }

    template<class T>
    template<class... Args>
    typename deque<T>::iterator deque<T>::insert_aux(deque::iterator pos, Args &&... args) {
        const size_type elems_before = pos - begin_;
        value_type value_copy = value_type(stl::forward<Args>(args)...);
        if (elems_before < (size() / 2)) {
            emplace_front(front());
            auto front1 = begin_;
            ++front1;
            auto front2 = front1;
            ++front2;
            // TODO:和源码不一样
            pos = begin_ + elems_before;
            stl::copy(front2, pos + 1, front1);
        } else {
            emplace_back(back());
            auto back1 = end_;
            --back1;
            auto back2 = back1;
            --back2;
            pos = begin_ + elems_before;
            stl::copy_backward(pos, back2, back1);
        }
        *pos = stl::move(value_copy);
        return pos;
    }

    template<class T>
    void deque<T>::fill_insert(deque::iterator position, deque::size_type n, const value_type &value) {
        /// 在迭代器position指定位置插入长度为n，数据值为value_copy的数据段
        /// 此操作可能引发容器空间的扩充

        const size_type elems_before = position - begin_;
        const size_type len = size();
        auto value_copy = value;

        if (elems_before < (len / 2)) {
            // 在头部开辟空间
            require_capacity(n, true);
            auto old_begin = begin_;
            auto new_begin = begin_ - n;
            position = begin_ + elems_before;
            try {
                if (elems_before >= n) {
                    // 此时对于elems_before个数据需要分两次来往前移动n个位置
                    // 前n个需要使用uninitialized_copy，后elems_before-n个用copy
                    auto begin_n = begin_ + n;
                    stl::uninitialized_copy(begin_, begin_n, new_begin);
                    begin_ = new_begin;
                    stl::copy(begin_n, position, old_begin);
                    stl::fill(position - n, position, value_copy);
                } else {
                    // 此时对于elems_before个数据全都要使用uninitialized_copy
                    // 同时n-elems_before个数据使用uninitialized_fill来填充值
                    // elems_before个数据使用fill来填充值
                    stl::uninitialized_fill(stl::uninitialized_copy(begin_, position, new_begin),
                                            begin_, value_copy);
                    begin_ = new_begin;
                    stl::fill(old_begin, position, value_copy);
                }
            } catch (...) {
                if (new_begin.node != begin_.node)
                    destroy_buffer(new_begin.node, begin_.node - 1);
                throw;
            }
        } else {
            // 在尾部开辟空间
            require_capacity(n, false);
            auto old_end = end_;
            auto new_end = end_ + n;
            const size_type elems_after = len - elems_before;
            position = end_ - elems_after;
            try {
                if (elems_after >= n) {
                    auto end_n = end_ - n;
                    stl::uninitialized_copy(end_n, end_, end_);
                    end_ = new_end;
                    stl::copy_backward(position, end_n, old_end);
                    stl::fill(position, position + n, value_copy);
                } else {
                    stl::uninitialized_fill(end_, position + n, value_copy);
                    stl::uninitialized_copy(position, end_, position + n);
                    end_ = new_end;
                    stl::fill(position, old_end, value_copy);
                }
            } catch (...) {
                if (new_end.node != end_.node)
                    destroy_buffer(end_.node + 1, new_end.nod);
                throw;
            }
        }
    }

    template<class T>
    template<class FIter>
    void deque<T>::copy_insert(deque::iterator, FIter, FIter, deque::size_type) {

    }

    template<class T>
    template<class IIter>
    void deque<T>::insert_dispatch(deque::iterator pos, IIter first, IIter last, input_iterator_tag) {
        if (last <= first) return;
        const size_type n = stl::distance(first, last);
        const size_type elems_before = pos - begin_;
        if (elems_before < (size() / 2)) {
            require_capacity(n, true);
        } else {
            require_capacity(n, false);
        }
        pos = begin_ + elems_before;
        auto cur = --last;
        for (size_type i = 0; i < n; ++i, --cur) {
            insert(pos, *cur);
        }
    }

    template<class T>
    template<class FIter>
    void deque<T>::insert_dispatch(deque::iterator pos, FIter first, FIter last, forward_iterator_tag) {
        if (last <= first) return;
        const size_type n = stl::distance(first, last);
        if (pos.cur == begin_.cur) {
            // 在头部插入n个元素
            require_capacity(n, true);
            auto new_begin = begin_ - n;
            try {
                stl::uninitialized_copy(first, last, new_begin);
                begin_ = new_begin;
            }
            catch (...) {
                // 如果copy出现异常，他会把copy一半的元素释放掉 这里只需要释放空白的缓冲区即可
                if (new_begin.node != begin_.node)
                    destroy_buffer(new_begin.node, begin_.node - 1);
                throw;
            }
        } else if (pos.cur == end_.cur) {
            // 在尾部插入
            require_capacity(n, false);
            auto new_end = end_ + n;
            try {
                stl::uninitialized_copy(first, last, end_);
                end_ = new_end;
            } catch (...) {
                if (new_end.node != end_.node)
                    destroy_buffer(end_.node + 1, new_end.node);
                throw;
            }
        } else {
            copy_insert(pos, first, last, n);
        }
    }

    template<class T>
    void deque<T>::require_capacity(size_type n, bool front) {

        if (front && (static_cast<size_type>(begin_.cur - begin_.first) < n)) {
            // 在头部扩充 并且要扩充的数目大于begin_缓冲区中的余量

            // 计算需要增加缓冲区的个数
            const size_type need_buffer = (n - (begin_.cur - begin_.first)) / buffer_size + 1;
            if (need_buffer > static_cast<size_type>(begin_.node - map_)) {
                // 需要缓冲区的个数大于map_头部预留的数量
                reallocate_map_at_front(need_buffer);
                return;
            }
            create_buffer(begin_.node - need_buffer, begin_.node - 1);
        } else if (!front && (static_cast<size_type>(end_.last - end_.cur - 1) < n)) {   // 减1不能忘
            // 在尾部扩充 并且要扩充的数目大于end_缓冲区中的余量

            const size_type need_buffer = (n - (end_.last - end_.cur - 1)) / buffer_size + 1;
            if (need_buffer > static_cast<size_type>((map_ + map_size_) - end_.node - 1)) {
                // 需要缓冲区的个数大于map_尾部预留的数量
                reallocate_map_at_back(need_buffer);
                return;
            }
            create_buffer(end_.node + 1, end_.node + need_buffer);
        }
    }

    template<class T>
    void deque<T>::reallocate_map_at_front(deque::size_type need) {
        /// 重新分配map数据块，在头部预留出need个空的数据块

        // 至少扩大两倍的空间 避免多次移动
        const size_type new_map_size = stl::max(map_size_ << 1,
                                                map_size_ + need + DEQUE_MAP_INIT_SIZE);
        map_pointer new_map = create_map(new_map_size);
        const size_type old_buffer = end_.node - begin_.node + 1;
        const size_type new_buffer = old_buffer + need;     // 目前需要的总的数据块个数

        // 分配的空间要比要求的多一些，即余量。头部和尾部各留出一半的余量，这些数据块都是nullptr
        auto begin = new_map + (new_map_size - new_buffer) / 2;
        // [begin, mid)是要增大的空间 [mid, end)是原来的空间
        auto mid = begin + need;
        auto end = mid + old_buffer;

        // 为need空间分配缓冲区
        create_buffer(begin, mid - 1);
        // 将之前原有的缓冲区移动到新的map数据块上
        for (auto begin1 = mid, begin2 = begin_.node; begin1 != end; ++begin1, ++begin2)
            *begin1 = *begin2;

        map_allocator::deallocate(map_, map_size_); // 释放老的map数据块
        map_ = new_map;
        map_size_ = new_map_size;
        begin_ = iterator(*mid + (begin_.cur - begin_.first), mid);
        end_ = iterator(*(end - 1) + (end_.cur - end_.first), end - 1);
    }

    template<class T>
    void deque<T>::reallocate_map_at_back(deque::size_type need) {
        /// 重新分配map数据块，在尾部预留出need个空的数据块

        // 至少扩大两倍的空间 避免多次移动
        const size_type new_map_size = stl::max(map_size_ << 1,
                                                map_size_ + need + DEQUE_MAP_INIT_SIZE);
        map_pointer new_map = create_map(new_map_size);
        const size_type old_buffer = end_.node - begin_.node + 1;
        const size_type new_buffer = old_buffer + need;     // 目前需要的总的数据块个数

        // 分配的空间要比要求的多一些，即余量。头部和尾部各留出一半的余量，这些数据块都是nullptr
        auto begin = new_map + (new_map_size - new_buffer) / 2;
        // [begin, mid)是原来的空间 [mid, end)是要增大的空间
        auto mid = begin + old_buffer;
        auto end = mid + need;

        // 为need空间分配缓冲区
        create_buffer(mid, end - 1);
        // 将之前原有的缓冲区移动到新的map数据块上
        for (auto begin1 = begin, begin2 = begin_.node; begin1 != mid; ++begin1, ++begin2)
            *begin1 = *begin2;

        map_allocator::deallocate(map_, map_size_); // 释放老的map数据块
        map_ = new_map;
        map_size_ = new_map_size;
        begin_ = iterator(*begin + (begin_.cur - begin_.first), begin);
        end_ = iterator(*(mid - 1) + (end_.cur - end_.first), mid - 1);
    }

    template<class T>
    bool operator==(const deque<T> &lhs, const deque<T> &rhs) {
        return lhs.size() == rhs.size() &&
               stl::equal(lhs.begin(), rhs.end(), rhs.begin());
    }

    template<class T>
    bool operator<(const deque<T> &lhs, const deque<T> &rhs) {
        return stl::lexicographical_compare(lhs.begin(), lhs.end(),
                                            rhs.begin(), rhs.end());
    }

    template<class T>
    bool operator>(const deque<T> &lhs, const deque<T> &rhs) {
        return rhs < lhs;
    }

    template<typename T>
    bool operator!=(const deque<T> &lhs, const deque<T> &rhs) {
        return !(lhs == rhs);
    }

    template<class T>
    bool operator<=(const deque<T> &lhs, const deque<T> &rhs) {
        return !(rhs < lhs);
    }

    template<class T>
    bool operator>=(const deque<T> &lhs, const deque<T> &rhs) {
        return !(lhs < rhs);
    }

    // 重载 stl 的 swap
    template<class T>
    void swap(deque<T> &lhs, deque<T> &rhs) {
        lhs.swap(rhs);
    }

}   // namespace stl


#endif //MYCPPSTL_DEQUE_H
