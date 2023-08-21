//
// Created by 晚风吹行舟 on 2023/8/20.
//

#ifndef MYCPPSTL_HEAD_ALGO_H
#define MYCPPSTL_HEAD_ALGO_H

// 这个头文件包含 heap 的四个算法 : push_heap, pop_heap,
// sort_heap, make_heap

#include "iterator.h"

namespace stl {

/*****************************************************************************************/
// push_heap
// 该函数接受两个迭代器，表示一个 heap 容器的首尾，并且新元素已经插入到底部容器的最尾端，调整 heap
/*****************************************************************************************/
    template<class RandomIter, class Distance, class T>
    void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value) {
        // [topIndex, holeIndex]可以索引堆的一部分（子树）
        auto parent = (holeIndex - 1) / 2;    // index从0开始
        // 大顶堆，如果当前节点holeIndex(value)大于其父节点，则交换它们两个
        // 重复这个操作直到堆顶或者value<其父节点
        while (holeIndex > topIndex && *(first + parent) < value) {
            *(first + holeIndex) = *(first + parent);
            holeIndex = parent;
            parent = (holeIndex - 1) / 2;
        }
        *(first + holeIndex) = value;
    }

    template<class RandomIter, class Distance>
    void push_heap_d(RandomIter first, RandomIter last, Distance *) {
        stl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1));
    }

    template<class RandomIter>
    void push_heap(RandomIter first, RandomIter last) {
        stl::push_heap_d(first, last, distance_type(first));
    }

    template<class RandomIter, class Distance, class T, class Compared>
    void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value, Compared comp) {
        auto parent = (holeIndex - 1) / 2;    // index从0开始
        // 大顶堆，如果当前节点holeIndex(value)大于其父节点，则交换它们两个
        // 重复这个操作直到堆顶或者value<其父节点
        while (holeIndex > topIndex && comp(*(first + parent), value)) {
            *(first + holeIndex) = *(first + parent);
            holeIndex = parent;
            parent = (holeIndex - 1) / 2;
        }
        *(first + holeIndex) = value;
    }

    template<class RandomIter, class Distance, class Compared>
    void push_heap_d(RandomIter first, RandomIter last, Distance *, Compared, Compared comp) {
        stl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1), comp);
    }

    template<class RandomIter, class Compared>
    void push_heap(RandomIter first, RandomIter last, Compared comp) {
        stl::push_heap_d(first, last, distance_type(first), comp);
    }

/*****************************************************************************************/
// pop_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，将 heap 的根节点取出放到容器尾部，调整 heap
/*****************************************************************************************/
    template<class RandomIter, class T, class Distance>
    void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value) {
        auto topIndex = holeIndex;      // 暂存堆顶下标

        // 先假设堆顶值最小，则一直下坠，将孩子节点中最大的和根交换
        auto rchild = 2 * holeIndex + 2;
        while (rchild < len) {
            if (*(first + rchild) < *(first + rchild - 1)) --rchild;
            *(first + holeIndex) = *(first + rchild);
            holeIndex = rchild;
            rchild = 2 * (rchild + 1);
        }
        if (rchild == len) {
            // 如果有左孩子，没有右孩子
            *(first + holeIndex) = *(first + rchild - 1);
            holeIndex = rchild - 1;
        }

        // 下坠的叶子节点，把value（原堆中最右侧的值）放进去
        // 此时这个value不一定合法，因为他不一定是最小的，因此还要上升
        stl::push_heap_aux(first, holeIndex, topIndex, value);
    }

    template<class RandomIter, class T, class Distance>
    void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, T value, Distance *) {
        // 将堆顶的值放到尾部，即堆排序的基本操作，反复执行即可完成堆排序
        // 然后调整[first, last-1)使之重新成为一个 max-heap
        *result = *first;
        // value里面存放堆中最右侧的值
        stl::adjust_heap(first, static_cast<Distance>(0), last - first, value);
    }

    template<class RandomIter>
    void pop_heap(RandomIter first, RandomIter last) {
        // 将堆中最右侧的值给value
        stl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first));
    }

    template<class RandomIter, class T, class Distance, class Compared>
    void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value,
                     Compared comp) {
        auto topIndex = holeIndex;
        auto rchild = 2 * holeIndex + 2;
        while (rchild < len) {
            if (comp(*(first + rchild), *(first + rchild - 1))) --rchild;
            *(first + holeIndex) = *(first + rchild);
            holeIndex = rchild;
            rchild = holeIndex * 2 + 2;
        }
        if (rchild == len) {
            *(first + rchild) = *(first + rchild - 1);
            holeIndex = rchild - 1;
        }

        stl::push_heap_aux(first, holeIndex, topIndex, value, comp);
    }

    template<class RandomIter, class T, class Distance, class Compared>
    void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result,
                      T value, Distance *, Compared comp) {
        *result = *first;  // 先将尾指设置成首值，即尾指为欲求结果
        // 尾值存在value中
        stl::adjust_heap(first, static_cast<Distance>(0), last - first, value, comp);
    }

    template<class RandomIter, class Compared>
    void pop_heap(RandomIter first, RandomIter last, Compared comp) {
        stl::pop_heap_aux(first, last - 1, last - 1, *(last - 1),
                          distance_type(first), comp);
    }

/*****************************************************************************************/
// sort_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，不断执行 pop_heap 操作，直到首尾最多相差1
/*****************************************************************************************/
    template<class RandomIter>
    void sort_heap(RandomIter first, RandomIter last) {
        while (last - first > 1) {
            stl::pop_heap(first, last);
            --last;
        }
    }

    template<class RandomIter, class Compared>
    void sort_heap(RandomIter first, RandomIter last, Compared comp) {
        while (last - first > 1) {
            stl::pop_heap(first, last, comp);
            --last;
        }
    }

/*****************************************************************************************/
// make_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，把容器内的数据变为一个 heap
/*****************************************************************************************/
    template<class RandomIter, class Distance>
    void make_heap_aux(RandomIter first, RandomIter last, Distance *) {
        if (last - first < 2) return;
        auto len = last - first;
        auto holeIndex = (len - 2) / 2;     // last指向最后一个的后一个 所以减二
        while (true) {
            //
            stl::adjust_heap(first, holeIndex, len, *(first + holeIndex));
            if (holeIndex == 0) return;
            holeIndex--;
        }
    }

    template<class RandomIter>
    void make_heap(RandomIter first, RandomIter last) {
        stl::make_heap_aux(first, last, distance_type(first));
    }

    template<class RandomIter, class Distance, class Compared>
    void make_heap_aux(RandomIter first, RandomIter last, Compared comp) {
        if (last - first < 2) return;
        auto len = last - first;
        auto holeIndex = (len - 2) / 2;
        while (true) {
            stl::adjust_heap(first, holeIndex, len, *(first + holeIndex), comp);
            if (holeIndex == 0) return;
            --holeIndex;
        }
    }

    template<class RandomIter, class Compared>
    void make_heap(RandomIter first, RandomIter last, Compared comp) {
        stl::make_heap_aux(first, last, distance_type(first), comp);
    }

}

#endif //MYCPPSTL_HEAD_ALGO_H
