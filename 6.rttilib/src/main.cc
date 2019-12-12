
// main.cpp

#include <iostream>

#include "rttilib.hpp"

using namespace std;

// 定义好下面用于测试库的对象及其复杂的继承关系
class A: public RttiPort<A> {};
class B: public RttiPort<B> {};
class C: public RttiPort<C> {};
class D: public RttiPort<D> {};
class E: public RttiPort<E> {};
class AB: public A, public B, public RttiPort<AB> {};
class CD: public C, public D, public RttiPort<CD> {};
class AB_CD: public AB, public CD, public RttiPort<AB_CD> {};
class AB_CD_E: public AB, public CD, public E, public RttiPort<AB_CD_E> {};

void test_useage() {
    std::cout.setf(std::ios::boolalpha);

    // 定义对象 AB_CD_E, 该对象继承自 AB,CD,E, 其中 AB 和 CD 分别继承自 A,B 和 C,D
    AB_CD_E* obj1 = new AB_CD_E();

    // 使用 `isKindOf` 方法来判断 AB_CD_E 对象是否是 A 类型的对象, true
    cout << "Is object of A type? " << obj1->isKindOf(RttiPort<A>::getType()) << endl;
    // 提供的另一种使用方法 rttilib::is
    cout << "Is object of A type? " << is<A>(obj1) << endl;
    // 判断 AB_CD_E 对象是否是 E 类型的对象, true
    cout << "Is object of E type? " << obj1->isKindOf(RttiPort<E>::getType()) << endl;
    // 提供的另一种使用方法 rttilib::is
    cout << "Is object of E type? " << is<E>(obj1) << endl;
    delete obj1;
    cout << endl;

    A* obj2 = new AB_CD_E();
    // 判断 A 类指针指向的 AB_CD_E 对象是否是 A 类型的对象, true
    cout << "Is object of A type? " << obj2->isKindOf(RttiPort<A>::getType()) << endl;
    // 判断 A 类指针指向的 AB_CD_E 对象是否是 E 类型的对象, true
    cout << "Is object of E type? " << obj2->isKindOf(RttiPort<E>::getType()) << endl;
    delete obj2;
    cout << endl;


    AB* obj3 = new AB();
    // 判断 AB 类指针指向的 AB 对象是否是 A 类型的对象, true
    cout << "Is object of A type? " << obj3->isKindOf(RttiPort<A>::getType()) << endl;
    // 判断 AB 类指针指向的 AB 对象是否是 E 类型的对象, true
    cout << "Is object of E type? " << obj3->isKindOf(RttiPort<E>::getType()) << endl;
    delete obj3;
    cout << endl;

    CD* obj4 = new CD();
    // 判断 CD 类指针指向的 CD 对象是否是 A 类型的对象, true
    cout << "Is object of A type? " << obj4->isKindOf(RttiPort<A>::getType()) << endl;
    // 判断 CD 类指针指向的 CD 对象是否是 E 类型的对象, true
    cout << "Is object of E type? " << obj4->isKindOf(RttiPort<E>::getType()) << endl;
    delete obj4;
    cout << endl;
}

int main() {
    test_useage();
    return 0;
}