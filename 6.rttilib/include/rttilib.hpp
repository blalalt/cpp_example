#ifndef RTTILIB_H
#define RTTILIB_H

#if !defined(RTTI_MAXCHAIN_SIZE)
#define RTTI_MAXCHAIN_SIZE 32
#endif // RTTI_MAXCHAIN_SIZE

#include <iostream>
#include <cstdint>
#include <array>

// rtti 类用于模拟运行时类型识别, 该类是 rttiport 模板类的基类
// 该类被设计成协议, 所以不能直接创建此类型的对象, 应该继承后使用
class Rtti {

public:
    virtual ~Rtti() {}
    // 检测示例是否为 type 所表示的类型
    // type: 对象类型的 ID, 使用 intptr_t 表示
    // return: 指向实例的 void* 指针, 若实例没有对应的 type 则返回 nullptr
    inline void* getKindOf(std::intptr_t type) {
        for (int i=0; i<=m_InherChainCounter; i++) {
            // 当在整个 inherChainID 中查找满足时，返回其指针
            if (m_InherChainId[i] == type) {
                return m_InherChainPtr[i];
            }
        }
        return nullptr;
    }
    inline bool isKindOf(std::intptr_t type) {
        return getKindOf(type) == nullptr ? false : true;
    }

protected:
    std::array<std::intptr_t, RTTI_MAXCHAIN_SIZE> m_InherChainId;
    std::array<void*, RTTI_MAXCHAIN_SIZE> m_InherChainPtr;
    int m_InherChainCounter {0};
    Rtti () {
        for (auto &d : m_InherChainId) d = -1;
        for (auto &ptr: m_InherChainPtr) ptr = 0;
    }
};

// 定义 rttiport 模板来作为实际的 rtti 特性接口，需要使用 rtti 特性的类都必须要继承自此类。
template<typename T>
class RttiPort: public Rtti {

private:
    // 使用私有对象 id 来表示类型
    static std::intptr_t id;

public:
    // 静态方法 type() 来获取对象的类型
    static std::intptr_t getType();

protected:
    RttiPort() {
        // 每继承一次 
        m_InherChainCounter ++;
        // 保存类型ID
        m_InherChainId[m_InherChainCounter] = getType();
        // 保存实例指针
        m_InherChainPtr[m_InherChainCounter] = static_cast<T*>(this); 
    }

    virtual ~RttiPort () {}
};

template<typename T>
std::intptr_t RttiPort<T>::id(0);

template<typename T>
std::intptr_t RttiPort<T>::getType() { // 它的类型通过其成员 id 进行表示获取
    // 此处我们要把 id 的地址的位模式进行重新解释，所以使用 reinterpret_cast
    return reinterpret_cast<std::intptr_t>(&id);
}

template<typename T>
static T* cast(Rtti *to) {
    if (to == nullptr) return nullptr;
    return static_cast<T*>(to->getKindOf(RttiPort<T>::type()))
}

// 提供另一种检查 obj 指向的对象是否为 T 类型的对象
template<typename T>
static bool is(Rtti *obj) {
    if (obj == nullptr) return false;
    return obj->getKindOf(RttiPort<T>::type()) != nullptr;
}

#endif // RTTILIB_H