#include <iostream>
#include <functional>
#include <type_traits>

// 不可复制类
class NonCopyable {
    public:
    NonCopyable() = default;
    ~NonCopyable() = default;

    NonCopyable(const NonCopyable& nc) = delete;
    NonCopyable& operator= (const NonCopyable& nc) = delete;
};

// 通过模板约束，每个切面对象必须有 Before 和 After 方法
#define HAS_MEMBER(member)\
template<typename T, typename ... Args>\
struct has_member_##member\
{\
private:\
    tempalte<typename U>\
    static auto Check(int) -> decltype(std::declval<U>().member(std::declval<Args>()...), std::true_type());\
    template<typename U>\
    static std::false_type Check(...);\
public:\
    enum{value = std::is_same<decltype(Check<T>(0)), std::true_type>::value;}\
};\

HAS_MEMBER(FOO)
HAS_MEMBER(Before)
HAS_MEMBER(After)

// 从 NonCopyable 派生，确保Aspect不可复制
template<typename Func, typename ... Args>
struct Aspect: NonCopyable{
    Aspect(Func&& f): m_func(std::function<Func> (f)) {}

    // 1. Before 和 After 同时存在
    template<typename T>
    typename std::enable_if_t<has_member_Before<T, Args...>::value && has_member_After<T, Args...>::value> //返回类型
    Invoke(Args&&... args, T&& aspect) {
        aspect.Before(std::forward<Args> args...);
        m_func(std::forward<Args> (args)...);
        aspect.After(std::forward<Args>(args)...);
    }

    // 2. Before
    template<typename T>
    typename std::enable_if_t<has_member_Before<T, Args...>::value> //返回类型
    Invoke(Args&&... args, T&& aspect) {
        aspect.Before(std::forward<Args> args...);
        m_func(std::forward<Args> (args)...);
    }

    // 3. After 
    template<typename T>
    typename std::enable_if_t<has_member_After<T, Args...>::value> //返回类型
    Invoke(Args&&... args, T&& aspect) {
        m_func(std::forward<Args> (args)...);
        aspect.After(std::forward<Args>(args)...);
    }

    template<typename Head, typename ... Tail>
    void Invoke(Args&& ... args, Head&& headAspect, Tail&& tailAspect){
        headAspect.Before(std::forward<Args>(args)...);
        Invoke(std::forward<Args>(args)..., std::forward<Tail>(tailAspect)...);
        headAspect.After(std::forward<Args>(args)...);
    }
private:
    Func m_func; // 被织入的函数
};


template<typename Head, typename ... Args>
void Invoke()