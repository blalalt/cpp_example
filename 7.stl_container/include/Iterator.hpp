#ifndef __ITERATOR_HPP
#define __ITERATOR_HPP

/*
迭代器类型： 定义五种迭代器
*/
struct input_iterator_tag {}; //输入迭代器
struct output_iterator_tag {}; // 输出迭代器
struct forward_iterator_tag: public input_iterator_tag {}; // 前向迭代器
struct bidirectional_iterator_tag: public forward_iterator_tag {}; // 双向迭代器
struct random_access_iterator_tag: public bidirectional_iterator_tag {}; // 随机迭代器


// 通过对输入迭代器解除引用，它将引用对象，而对象可能位于集合中。通常用于传递地址。
template<typename T, typename Distance>
struct input_iterator {
    using iterator_category = input_iterator_tag; // 返回类型
    using value_type = T; // 所指对象类型
    using difference_type = Distance; // 迭代器距离类型
    using pointer = T*; // 操作结果类型
    using reference = T&; //解引用操作结果类型
};

// 该类迭代器和输入迭代器极其相似，也只能单步向前迭代元素，不同的是该类迭代器对元素只有写的权力。通常用于返回地址。
struct out_iterator {
    using iterator_category  = output_iterator_tag; // 返回类型
    using value_type = void; // 所指对象类型
    using difference_type = void; // 迭代器距离类型
    using pointer = void; // 操作结果类型
    using reference = void; //解引用操作结果类型
};

// 前向迭代器可以在一个正确的区间中进行读写操作，它拥有输入迭代器的所有特性，和输出迭代器的部分特性，以及单步向前迭代元素的能力。通常用于遍历。
template<typename T, typename Distance>
struct forward_iterator {
    using iterator_category = forward_iterator_tag; // 返回类型
    using value_type = T; // 所指对象类型
    using difference_type = Distance; // 迭代器距离类型
    using pointer = T*; // 操作结果类型
    using reference = T&; //解引用操作结果类型
};

// 该类迭代器是在前向迭代器的基础上提供了单步向后迭代元素的能力，前向迭代器的高级版。
template<typename T, typename Distance>
struct bidirectional_iterator {
    using iterator_category  = bidirectional_iterator_tag; // 返回类型
    using value_type = T; // 所指对象类型
    using difference_type = Distance; // 迭代器距离类型
    using pointer = T*; // 操作结果类型
    using reference = T&; //解引用操作结果类型
};

template<typename T, typename Distance>
struct random_access_iterator {
    using iterator_category = random_access_iterator_tag; // 返回类型
    using value_type = T; // 所指对象类型
    using difference_type = Distance; // 迭代器距离类型
    using pointer = T*; // 操作结果类型
    using reference = T&; //解引用操作结果类型
};
#endif //  __ITERATOR_HPP