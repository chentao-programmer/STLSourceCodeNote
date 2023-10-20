#ifndef CTSTL_TYPE_TRAITS_H_
#define CTSTL_TYPE_TRAITS_H_

// 这个头文件用于提取类型信息
#include <type_traits>

namespace ctstl {

template <class T, T v>
struct m_integral_constant
{
    // static关键字表示这个变量是静态的，也就是说它在程序的整个生命周期内都存在，并且在类的所有实例中共享
    // constexpr关键字表示这个变量是一个常量表达式，也就是说它的值在编译时就已经确定，并且在程序运行期间不会改变
    static constexpr T value = v;
};

template <bool b>
using m_bool_constant = m_integral_constant<bool, b>;

typedef m_bool_constant<true> m_true_type;
typedef m_bool_constant<false> m_false_type;

// is_pair

template <class T1, class T2>
struct pair;

template <class T>
struct is_pair : ctstl::m_false_type { };

template <class T1, class T2>
struct is_pair<ctstl::pair<T1, T2>> : ctstl::m_true_type {};
}
#endif