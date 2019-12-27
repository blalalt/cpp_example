#pragma once
#include "Iterator.hpp"

// 用于迭代器前移，增加迭代的位置。可用于定向访问到迭代器的某个变量。
template<typename InputIterator, typename Distance>
void _advance(InputIterator& it, Distance n, input_iterator_tag tag) {
    static_assert(n >= 0);
    while (n--) {
        ++it;
    }
}

template<typename InputIterator, typename Distance>
void advance(InputInterator& it, Distance n) {
    typedef typename iterator_traits<InputInterator>::iterator_category iterator_category;
    _advance(it, n, iterator_category())
}


// 用于计算迭代器间的距离
template<typename InputIterator>
typename iterator_traits<InputInterator>::difference_type
_distance(InputIterator& first, InputIterator& last, input_iterator_tag tag) {
    // 初始距离
    typename iterator_traits<InputInterator>::difference_type dist = 0;
    while (first++ != last) {
        ++dist;
    }
    return dist;
}

template<typename InputIterator>
typename iterator_traits<InputInterator>::difference_type
distance(InputInterator& first, InputIterator& last) {
    typedef typename iterator_traits<InputInterator>::iterator_category iterator_category;
    _distance(it, n, iterator_category())
}

