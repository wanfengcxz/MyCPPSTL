//
// Created by 晚风吹行舟 on 2023/4/9.
//

#ifndef MYCPPSTL_VECTOR_H
#define MYCPPSTL_VECTOR_H

// 这个头文件包含一个模板类 vector
// vector : 向量

// notes:
//
// 异常保证：
// mystl::vecotr<T> 满足基本异常保证，部分函数无异常保证，并对以下函数做强异常安全保证：
//   * emplace
//   * emplace_back
//   * push_back
// 当 std::is_nothrow_move_assignable<T>::value == true 时，以下函数也满足强异常保证：
//   * reserve
//   * resize
//   * insert

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "utils.h"
#include "exceptdef.h"
#include "algo.h"

namespace stl {

#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

    template<class T>
    class vector {
        // TODO:什么时候执行？
        static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in mystl");
    public:
        typedef stl::allocator<T> allocator_type;
        typedef stl::allocator<T> data_allocator;

        typedef typename allocator_type::value_type value_type;
        typedef typename allocator_type::pointer pointer;
        typedef typename allocator_type::const_pointer const_pointer;
        typedef typename allocator_type::reference reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type size_type;   // unsigned int64
        typedef typename allocator_type::difference_type difference_type;

        typedef value_type *iterator;
        typedef const value_type *const_iterator;
        typedef stl::reverse_iterator<iterator> reverse_iterator;
        typedef stl::reverse_iterator<const_iterator> const_reverse_iterator;

        allocator_type get_allocator() { return data_allocator(); }

    private:

        iterator begin_;
        iterator end_;
        iterator cap_;

    public:

        vector() noexcept { try_init(); }

        explicit vector(size_type n) {
            fill_init(n, value_type());
        }

        vector(size_t n, const value_type &value) {
            fill_init(n, value);
        }

        // first last是input_iterator或其子类，那么就可以读数据
        // 因为output_iterator不能读数据
        template<class Iter, typename std::enable_if<
                stl::is_input_iterator<Iter>::value, int>::type = 0>
        vector(Iter first, Iter last) {
            STL_DEBUG(!(last < first));
            range_init(first, last);
        }

        vector(const vector &rhs) {
            // private:只要同属一个类就可以不用区分同一个类的不同对象
            range_init(rhs.begin_, rhs.end_);
        }

        vector(vector &&rhs) noexcept: begin_(rhs.begin_),
                                       end_(rhs.end_), cap_(rhs.cap_) {
            rhs.begin_ = nullptr;
            rhs.end_ = nullptr;
            rhs.cap_ = nullptr;
        }

        vector(std::initializer_list<value_type> ilist) {
            range_init(ilist.begin(), ilist.end());
        }

        // 拷贝赋值
        vector &operator=(const vector &rhs);

        // 移动赋值
        vector &operator=(vector &&rhs) noexcept;

        // vector v = { 1, 2, 3, 4 }; initializer_list<T> lst = {1,2,3};
        vector &operator=(std::initializer_list<value_type> ilist) {
            vector tmp(ilist.begin(), ilist.end());
            swap(tmp);
            return *this;
        }

        ~vector() {
            destroy_and_recover(begin_, end_, cap_ - begin_);
            begin_ = end_ = cap_ = nullptr;
        }

    public:

        /// 迭代器相关操作
        iterator begin() noexcept { return begin_; }

        // 使用const重载成员方法 常成员函数
        // const定义的对象叫常对象，常对象只能调用常成员函数
        const_iterator begin() const noexcept { return begin_; }

        iterator end() noexcept { return end_; }

        const_iterator end() const noexcept { return end_; }

        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

        const_iterator cbegin() const noexcept { return begin(); }

        const_iterator cend() const noexcept { return end(); }

        const_reverse_iterator crbegin() const noexcept { return rbegin(); }

        const_reverse_iterator crend() const noexcept { return rend(); }

        /// 容量相关操作
        bool empty() const noexcept { return begin_ == end_; }

        size_type size() const noexcept {
            return static_cast<size_type>(end_ - begin_);
        }

        size_type max_size() const noexcept { return static_cast<size_type>(-1) / sizeof(T); }

        size_type capacity() const noexcept { return static_cast<size_type>(cap_ - begin_); }

        void reserve(size_type n);

        void shrink_to_fit();


        /// 访问元素操作
        reference operator[](size_type n) {
            STL_DEBUG(n < size());
            return *(begin_ + n);
        }

        const_reference operator[](size_type n) const {
            STL_DEBUG(n < size());
            return *(begin_ + n);
        }

        reference at(size_type n) {
            THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
            return (*this)[n];
        }

        const_reference at(size_type n) const {
            THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
            return (*this)[n];
        }

        reference front() {
            STL_DEBUG(!empty());
            return *begin_;
        }

        const reference front() const {
            STL_DEBUG(!empty());
            return *begin_;
        }

        reference back() {
            STL_DEBUG(!empty());
            return *(end_ - 1);
        }

        const_reference back() const {
            STL_DEBUG(!empty());
            return *(end_ - 1);
        }

        pointer data() noexcept { return begin_; };

        const_pointer data() const noexcept { return begin_; }

        /// 修改容器操作
        // assign
        void assign(size_type n, const T &value) {
            fill_assign(n, value);
        }

        template<class Iter, typename std::enable_if<
                stl::is_input_iterator<Iter>::value, int> = 0>
        void assign(Iter first, Iter last) {
            STL_DEBUG(!(last < first));
            copy_assign(first, last, iterator_category(first));
        }

        void assign(std::initializer_list<value_type> il) {
            copy_assign(il.begin(), il.end(), stl::forward_iterator_tag());
        }

        // emplace / emplace_back

        // 在 pos 位置就地构造元素，之后的元素往后移动一位，避免额外的复制和移动开销
        template<class ...Args>
        // 数量不定的模板参数 c++11
        iterator emplace(const_iterator pos, Args &&...args);

        // TODO:测试emplace_back和push_back

        // 在尾部原地构造元素
        template<class ...Args>
        void emplace_back(Args &&...args);

        // push_back / pop_back
        void push_back(const value_type &value);

        // TODO:理论上forward也行
        void push_back(value_type &&value) { emplace_back(stl::move(value)); }

        void pop_back();

        // insert

        iterator insert(const_iterator pos, const value_type &value);

        iterator insert(const_iterator pos, value_type &&value) {
            emplace_back(pos, stl::move(value));
        }

        iterator insert(const_iterator pos, size_type n, const value_type &value) {
            STL_DEBUG(pos >= begin() && pos <= end());
            return fill_insert(const_cast<iterator>(pos), n, value);
        }

        // 如果是input_iterator及其子类，那么可以读其数据
        template<class Iter, typename std::enable_if<
                stl::is_input_iterator<Iter>::value, int>::type = 0>
        void insert(const_iterator pos, Iter first, Iter last) {
            STL_DEBUG(pos >= begin() && pos <= end() && !(last < first));
            copy_insert(const_cast<iterator>(pos), first, last);
        }

        // erase / clear
        iterator erase(const_iterator pos);

        iterator erase(const_iterator first, const_iterator last);

        void clear() { erase(begin(), end()); }

        // resize / reverse
        void resize(size_type new_size) { return resize(new_size, value_type()); }

        void resize(size_type new_size, const value_type &value);

        void reverse() { stl::reverse(begin(), end()); }

        // swap
        void swap(vector &rhs) noexcept;

    private:
        /// helper functions

        // 初始化/销毁
        void try_init() noexcept;

        void init_space(size_type size, size_type cap);

        void fill_init(size_type n, const value_type &value);

        template<class Iter>
        void range_init(Iter first, Iter last);

        void destroy_and_recover(iterator first, iterator last, size_type n);


        // 计算增长的长度
        size_type get_new_cap(size_type add_size);


        // assign赋值
        void fill_assign(size_type n, const value_type &value);


        template<class IIter>
        void copy_assign(IIter first, IIter last, input_iterator_tag);

        template<class FIter>
        void copy_assign(FIter first, FIter last, forward_iterator_tag);

        // reallocate

        template<class... Args>
        void reallocate_emplace(iterator pos, Args &&...args);

        void reallocate_insert(iterator pos, const value_type &value);

        // insert

        iterator fill_insert(iterator pos, size_type n, const value_type &value);

        template<class IIter>
        void copy_insert(iterator pos, IIter first, IIter last);

        // shrink_to_fit

        void reinsert(size_type size);
    };

/*****************************************************************************************/

    template<class T>
    vector<T> &vector<T>::operator=(const vector &rhs) {
        if (this == &rhs) return *this;
        const auto len = rhs.size();
        if (len > capacity()) {
            vector tmp(rhs.begin(), rhs.end());
            swap(tmp);
        } else if (size() >= len) {
            auto i = stl::copy(rhs.begin(), rhs.end(), begin());
            data_allocator::destroy(i, end_);
            end_ = begin_ + len;
        } else {
            stl::copy(rhs.begin(), rhs.begin() + size(), begin_);
            stl::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
            // TODO：cap_应该保持不变？？
            cap_ = end_ = begin_ + len;
        }
    }

    template<class T>
    vector<T> &vector<T>::operator=(vector &&rhs) noexcept {
        destroy_and_recover(begin_, end_, capacity());
        // private:只要同属一个类就可以不用区分同一个类的不同对象
        begin_ = rhs.begin_;
        end_ = rhs.end_;
        cap_ = rhs.cap_;
        rhs.cap_ = rhs.end_ = rhs.begin_ = nullptr;
        return *this;
    }

    // 预留空间大小，当原容量小于要求大小时，才会重新分配
    template<class T>
    void vector<T>::reserve(size_type n) {
        if (capacity() < n) {
            THROW_LENGTH_ERROR_IF(n > max_size(), "n can not larger than max_size() in vector<T>::reserve(n)");
            const auto old_size = size();
            auto tmp = data_allocator::allocate(n);
            stl::uninitialized_move(begin_, end_, tmp);
            data_allocator::deallocate(begin_, capacity());
            begin_ = tmp;
            end_ = tmp + old_size;
            cap_ = tmp + n;
        }
    }

    // 放弃多余的容量
    template<class T>
    void vector<T>::shrink_to_fit() {
        if (end_ < cap_) {
            reinsert(size());
        }
    }


    template<class T>
    template<class... Args>
    typename vector<T>::iterator vector<T>::emplace(vector::const_iterator pos, Args &&... args) {
        // pos可以等于end()，意味着可以在末尾插入
        STL_DEBUG(pos >= begin() && pos <= end());
        /*
         * 一个变量在使用const_cast去掉指针或者引用的const限定符后，
         * “如果常量本身不是常量，获得的权限是合法的，
         * 如果本身是常量，使用const_cast再写的后果是未定义的。
         */
        iterator xpos = const_cast<iterator>(pos);
        const size_type n = xpos - begin_;
        if (end_ != cap_ && xpos == end_) {
            // xpos指向end_，但是容量还有
            data_allocator::construct(stl::address_of(*end_), stl::forward<Args>(args)...);
            ++end_;
        } else if (end_ != cap_) {
            // 容量还有，但是xpos指向中间位置
            auto new_end = end_;
            // 不能直接倒着拷贝，因为end_指向的内存块还没有初始化，需要先构造一个对象。
            data_allocator::construct(stl::address_of(*end_), *(end_ - 1));
            ++new_end;
            // 从尾部向前开始拷贝
            stl::copy_backward(xpos, end_ - 1, end_);
            // 右侧是个右值，调用移动赋值函数
            *xpos = value_type(stl::forward<Args>(args)...);
            end_ = new_end;
        } else {
            // xpos == end_ == cap_ 容量没有，xpos指向end_
            reallocate_emplace(xpos, stl::forward<Args>(args)...);
        }
        return begin_ + n;
    }

    template<class T>
    template<class... Args>
    void vector<T>::emplace_back(Args &&... args) {
        if (end_ < cap_) {
            data_allocator::construct(stl::address_of(*end_), stl::forward<Args>(args)...);
            ++end_;
        } else {
            reallocate_emplace(end_, stl::forward<Args>(args)...);
        }
    }

    template<class T>
    void vector<T>::push_back(const value_type &value) {
        if (end_ != cap_) {
            data_allocator::construct(stl::address_of(*end_), value);
            ++end_;
        } else {
            reallocate_insert(end_, value);
        }
    }

    template<class T>
    void vector<T>::pop_back() {
        STL_DEBUG(!empty());
        data_allocator::destroy(end_ - 1);
        --end_;
    }

    template<class T>
    typename vector<T>::iterator vector<T>::insert(vector::const_iterator pos, const value_type &value) {
        STL_DEBUG(pos >= begin() && pos <= end());
        iterator xpos = const_cast<iterator>(pos);
        const size_type n = xpos - begin_;
        if (end_ != cap_ && end_ == xpos) {
            data_allocator::allocate(stl::address_of(*end_), value);
            ++end_;
        } else if (end_ != cap_) {
            auto new_end = end_;
            // 不能直接倒着拷贝，因为end_指向的内存块还没有初始化，需要先构造一个对象。
            data_allocator::construct(stl::address_of(*end_), *(end_ - 1));
            ++new_end;
            auto value_copy = value;  // 避免元素因以下copy_backward操作而被改变
            stl::copy_backward(xpos, end_ - 1, end_);
            *xpos = stl::move(value_copy);
            end_ = new_end;
        } else {
            reallocate_insert(xpos, value);
        }
        return begin_ + n;
    }

    template<class T>
    typename vector<T>::iterator vector<T>::erase(vector::const_iterator pos) {
        STL_DEBUG(pos >= begin() && pos < end());
        iterator xpos = begin_ + (pos - begin());
        stl::move(xpos + 1, end_, xpos);
        data_allocator::destroy(end_ - 1);
        --end_;
        return xpos;
    }

    template<class T>
    typename vector<T>::iterator vector<T>::erase(vector::const_iterator first, vector::const_iterator last) {
        STL_DEBUG(first >= begin() && last <= end() && !(last < first));
        const auto n = first - begin();
        iterator r = begin_ + n;
        data_allocator::destroy(stl::move(r + (last - first), end_, r), end_);
        end_ = end_ - (last - first);
        return begin_ + n;
    }

    template<class T>
    void vector<T>::resize(vector::size_type new_size, const value_type &value) {
        if (new_size < size()) {
            erase(begin_ + new_size, end_);
        } else {
            // TODO:哪里的insert?
            insert(end_, new_size - size(), value);
        }
    }

    template<class T>
    void vector<T>::swap(vector<T> &rhs) noexcept {
        if (&rhs != this) {
            stl::swap(begin_, rhs.begin_);
            stl::swap(end_, rhs.end_);
            stl::swap(cap_, rhs.cap_);
        }
    }

/*****************************************************************************************/
    /// helper function

    // try_init函数，若分配失败则忽略，不抛出异常
    template<class T>
    void vector<T>::try_init() noexcept {
        try {
            begin_ = data_allocator::allocate(16);
            end_ = begin_;
            cap_ = begin_ + 16;
        } catch (...) {
            begin_ = end_ = cap_ = nullptr;
        }
    }

    template<class T>
    void vector<T>::init_space(vector::size_type size, vector::size_type cap) {
        try {
            begin_ = data_allocator::allocate(cap);
            end_ = begin_ + size;
            cap_ = begin_ + cap;
        } catch (...) {
            begin_ = nullptr;
            end_ = nullptr;
            cap_ = nullptr;
            throw;
        }
    }

    template<class T>
    void vector<T>::fill_init(size_type n, const value_type &value) {
        const size_type init_size = stl::max(static_cast<size_type>(16), n);
        // 先分配一块内存空间
        init_space(n, init_size);
        // 在这个未初始化的内存空间上通过构造函数/复制来逐个生成value_type对象
        stl::uninitialized_fill_n(begin_, n, value);
    }

    template<class T>
    template<class Iter>
    void vector<T>::range_init(Iter first, Iter last) {
        const size_type len = stl::distance(first, last);
        const size_type init_size = stl::max(static_cast<size_type>(16), len);
        init_space(len, init_size);
        stl::uninitialized_copy(first, last, begin_);
    }

    template<class T>
    void vector<T>::destroy_and_recover(vector::iterator first, vector::iterator last, vector::size_type n) {
        // 先摧毁每个位置上的数据，即摧毁value_type对象
        data_allocator::destroy(first, last);
        // 然后摧毁整个内存区间
        data_allocator::deallocate(first, n);
    }

    template<class T>
    typename vector<T>::size_type vector<T>::get_new_cap(vector::size_type add_size) {
        const auto old_size = capacity();
        THROW_LENGTH_ERROR_IF(old_size > max_size() - add_size, "vector<T>'s size too big");
        if (old_size > max_size() - old_size / 2) {
            // 当old_size>(2/3)*max_size()时
            return old_size + add_size > max_size() - 16 ?
                   old_size + add_size : old_size + add_size + 16;
        }
        const size_type new_size = old_size == 0 ?
                                   stl::max(add_size, static_cast<size_type>(16)) :
                                   stl::max(old_size + old_size / 2, old_size + add_size);
        return new_size;
    }

    template<class T>
    void vector<T>::fill_assign(vector::size_type n, const value_type &value) {
        if (n > capacity()) {
            vector tmp(n, value);
            swap(tmp);
        } else if (n > size()) {
            // TODO:
            // 已经初始化过的，可以使用fill填充
            stl::fill(begin(), end(), value);
            // 没有初始化过的内存块
            end_ = stl::uninitialized_fill_n(end_, n - size(), value);
        } else {
            erase(stl::fill_n(begin_, n, value), end_);
        }
    }

    // 用[first,last)为容器赋值
    template<class T>
    template<class IIter>
    void vector<T>::copy_assign(IIter first, IIter last, input_iterator_tag) {
        auto cur = begin_;
        for (; first != last && cur != end_; ++first, ++cur) {
            *cur = *first;
        }
        if (first == last) erase(cur, end_);
        else insert(end_, first, last);
    }

    template<class T>
    template<class FIter>
    void vector<T>::copy_assign(FIter first, FIter last, forward_iterator_tag) {
        const size_type len = stl::distance(first, last);
        if (len > capacity()) {
            vector tmp(first, last);
            swap(tmp);
        } else if (size() >= len) {
            auto new_end = stl::copy(first, last, begin_);
            data_allocator::destroy(new_end, end_);
            end_ = new_end;
        } else {
            auto mid = first;
            stl::advance(mid, size());
            stl::copy(first, mid, begin_);
            auto new_end = stl::uninitialized_copy(mid, last, end_);
            end_ = new_end;
        }
    }

    // 重新分配空间并在pos处原地构造元素
    template<class T>
    template<class ...Args>
    void vector<T>::reallocate_emplace(iterator pos, Args &&...args) {
        const auto new_size = get_new_cap(1);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try {
            new_end = stl::uninitialized_move(begin_, pos, new_begin);
            data_allocator::construct(stl::address_of(*new_end),
                                      stl::forward<Args>(args)...);
            ++new_end;
            new_end = stl::uninitialized_move(pos, end_, new_end);
        } catch (...) {
            data_allocator::deallocate(new_begin, new_size);
            throw;
        }

        destroy_and_recover(begin_, end_, capacity());
        begin_ = new_begin;
        end_ = new_end;
        cap_ = begin_ + new_size;
    }

    template<class T>
    void vector<T>::reallocate_insert(iterator pos, const value_type &value) {
        auto new_size = get_new_cap(1);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        const value_type value_copy = value;
        try {
            new_end = stl::uninitialized_move(begin_, pos, new_begin);
            data_allocator::construct(stl::address_of(*new_end), value_copy);
            ++new_end;
            new_end = stl::uninitialized_move(pos, end_, new_end);
        }
        catch (...) {
            data_allocator::deallocate(new_begin, new_size);
            throw;
        }
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = new_begin + new_size;
    }

    // fill_insert 函数
    template<class T>
    typename vector<T>::iterator
    vector<T>::
    fill_insert(iterator pos, size_type n, const value_type &value) {
        if (n == 0) return pos;
        const size_type xpos = pos - begin_;
        const value_type value_copy = value;
        if (static_cast<size_type>(cap_ - end_) >= n) { // 如果备用空间大于等于增加的空间
            const size_type after_elems = end_ - pos;
            auto old_end = end_;
            if (after_elems > n) {
                stl::uninitialized_copy(end_ - n, end_, end_);
                end_ += n;
                stl::move_backward(pos, old_end - n, old_end);
                stl::uninitialized_fill_n(pos, n, value_copy);
            } else {
                end_ = stl::uninitialized_fill_n(end_, n - after_elems, value_copy);
                end_ = stl::uninitialized_move(pos, old_end, end_);
                stl::uninitialized_fill_n(pos, after_elems, value_copy);
            }
        } else { // 如果备用空间不足
            const auto new_size = get_new_cap(n);
            auto new_begin = data_allocator::allocate(new_size);
            auto new_end = new_begin;
            try {
                new_end = stl::uninitialized_move(begin_, pos, new_begin);
                new_end = stl::uninitialized_fill_n(new_end, n, value);
                new_end = stl::uninitialized_move(pos, end_, new_end);
            }
            catch (...) {
                destroy_and_recover(new_begin, new_end, new_size);
                throw;
            }
            data_allocator::deallocate(begin_, cap_ - begin_);
            begin_ = new_begin;
            end_ = new_end;
            cap_ = begin_ + new_size;
        }
        return begin_ + xpos;
    }

    // copy_insert 函数
    template<class T>
    template<class IIter>
    void vector<T>::
    copy_insert(iterator pos, IIter first, IIter last) {
        if (first == last)
            return;
        const auto n = stl::distance(first, last);
        if ((cap_ - end_) >= n) { // 如果备用空间大小足够
            const auto after_elems = end_ - pos;
            auto old_end = end_;
            if (after_elems > n) {
                end_ = stl::uninitialized_copy(end_ - n, end_, end_);
                stl::move_backward(pos, old_end - n, old_end);
                stl::uninitialized_copy(first, last, pos);
            } else {
                auto mid = first;
                stl::advance(mid, after_elems);
                end_ = stl::uninitialized_copy(mid, last, end_);
                end_ = stl::uninitialized_move(pos, old_end, end_);
                stl::uninitialized_copy(first, mid, pos);
            }
        } else { // 备用空间不足
            const auto new_size = get_new_cap(n);
            auto new_begin = data_allocator::allocate(new_size);
            auto new_end = new_begin;
            try {
                new_end = stl::uninitialized_move(begin_, pos, new_begin);
                new_end = stl::uninitialized_copy(first, last, new_end);
                new_end = stl::uninitialized_move(pos, end_, new_end);
            }
            catch (...) {
                destroy_and_recover(new_begin, new_end, new_size);
                throw;
            }
            data_allocator::deallocate(begin_, cap_ - begin_);
            begin_ = new_begin;
            end_ = new_end;
            cap_ = begin_ + new_size;
        }
    }

    // reinsert 函数
    template<class T>
    void vector<T>::reinsert(size_type size) {
        auto new_begin = data_allocator::allocate(size);
        try {
            // TODO：不用检查长度？？？
            stl::uninitialized_move(begin_, end_, new_begin);
        } catch (...) {
            data_allocator::deallocate(new_begin, size);
            throw;
        }
        data_allocator::deallocate(begin_, capacity());
        begin_ = new_begin;
        end_ = begin_ + size;
        cap_ = begin_ + size;
    }

/*****************************************************************************************/
    /// 重载比较操作符
    template<class T>
    bool operator==(const vector<T> &lhs, const vector<T> &rhs) {
        return lhs.size() == rhs.size() &&
               stl::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<class T>
    bool operator<(const vector<T> &lhs, const vector<T> &rhs) {
        return stl::lexicographical_compare(lhs.begin(), lhs.end(),
                                            rhs.begin(), rhs.end());
    }

    template<class T>
    bool operator!=(const vector<T> &lhs, const vector<T> &rhs) {
        return !(lhs == rhs);
    }

    template<class T>
    bool operator>(const vector<T> &lhs, const vector<T> &rhs) {
        return rhs < lhs;
    }

    template<class T>
    bool operator<=(const vector<T> &lhs, const vector<T> &rhs) {
        return !(rhs < lhs);
    }

    template<class T>
    bool operator>=(const vector<T> &lhs, const vector<T> &rhs) {
        return !(lhs < rhs);
    }

    // 重载 mystl 的 swap
    template<class T>
    void swap(vector<T> &lhs, vector<T> &rhs) {
        lhs.swap(rhs);
    }
}


#endif //MYCPPSTL_VECTOR_H
