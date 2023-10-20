#ifndef CTSTL_UTIL_H
#define CTSTL_UTIL_H

// 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 
#include <cstddef>

#include "iterator"

namespace ctstl
{
// move
/*
typename std::remove_reference<T>::type&&：这是函数的返回类型。std::remove_reference<T>::type会得到去除引用后的类型, 通过添加&&，得到该类型的右值引用。
参数arg是一个通用引用，可以接受左值或右值。关键字noexcept表示此函数不会抛出异常。
函数的主体：返回参数arg的右值引用。使用static_cast可以将左值转换为右值引用，使得原本不能移动的左值能够被移动。
*/
template <class T>
typename std::remove_reference<T>::type&& move(T&& arg) noexcept
{
    return static_cast<typename std::remove_reference<T>::type&&>(arg);
}

/*
这是第一个forward函数的声明。它接受一个左值引用参数，并返回一个右值引用。关键字noexcept表示此函数不会抛出异常。
这是第一个forward函数的主体。它返回参数arg的右值引用。使用static_cast可以将左值转换为右值引用。
*/
template <class T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept
{
    return static_cast<&&>(arg);
}

/*
它接受一个右值引用参数，并返回一个右值引用。关键字noexcept表示此函数不会抛出异常。
首先，它使用静态断言检查传递给函数的是否是左值引用。如果是，那么编译器将在编译时期报错。然后，它返回参数arg的右值引用。
*/
template <class T>
T&& forward(typename std::remove_reference<T>::type&& arg) noexcept
{
    static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
    return static_cast<&&>(arg);
}

//swap
template <class Tp>
void swap(Tp& lhs, Tp& rhs)
{
    auto tmp(ctstl::moce(lhs));
    lhs = ctstl::move(rhs);
    rhs = ctstl::moce(tmp);
}

template <class ForwardIter1, class Forwarditer2>
Forwarditer2 swap_range(ForwardIter1 first1, ForwardIter1 last1, Forwarditer2 first2)
{
    for (; first1 != last1; ++first1, (void)++first2) {
        ctstl::swap(*first1, *first2);
    }
    return first2;
}

template <class Tp, size_t N>
void swap(Tp(&a)[N], Tp(&b)[N])
{
    ctstl::swap_range(a, a + N, b);
}

//pair
// 结构体模板 : pair
// 两个模板参数分别表示两个数据的类型
// 用 first 和 second 来分别取出第一个数据和第二个数据
template <class Ty1, class Ty2>
struct pair
{
    typedef Ty1     first_type;
    typedef Ty2     second_type;

    first_type  first;
    second_type second;

    // default constructiable
    // std::enable_if是一个模板元编程技术，它用于在编译时根据条件启用或禁用函数或类模板。在这里，它用于确保Other1和Other2都是可默认构造的类型。
    template <class Other1 = Ty1, class Other2 = Ty2,
        typename = typename std::enable_if<
        std::is_default_constructible<Other1>::value &&
        std::is_default_constructible<Other2>::value, void>::type>
        constexpr pair()
        : first(), second()
    {
    }

    // implicit constructiable for this type
    // 它用于确保U1和U2都是可复制构造的类型，并且可以从U1和U2隐式转换为Ty1和Ty2。
    template <class U1 = Ty1, class U2 = Ty2,
        typename std::enable_if<
        std::is_copy_constructible<U1>::value &&
        std::is_copy_constructible<U2>::value &&
        std::is_convertible<const U1&, Ty1>::value &&
        std::is_convertible<const U2&, Ty2>::value, int>::type = 0>
        constexpr pair(const Ty1& a, const Ty2& b)
        : first(a), second(b)
        {            
        }
    
    // explicit constructible for this type
    // 它用于确保U1和U2都是可复制构造的类型，并且不能从U1和U2隐式转换为Ty1和Ty2。
    template <class U1 = Ty1, class U2 = Ty2,
        typename std::enable_if<
        std::is_copy_constructible<U1>::value &&
        std::is_copy_constructible<U2>::value &&
        (!std::is_convertible<const U1&, Ty1>::value ||
         !std::is_convertible<const U2&, Ty2>::value), int>::type = 0>
        explicit constexpr pair(const Ty1& a, const Ty2& b)
        : first(a), second(b)
    {
    }

    pair(const pair& rhs) = default;
    pair(pair&& rhs) = default;

    // implicit constructiable for other type
    // 它用于确保Other1和Other2都可以构造为Ty1和Ty2，并且可以从Other1和Other2隐式转换为Ty1和Ty2。
    template <class Other1, class Other2,
        typename std::enable_if<
        std::is_constructible<Ty1, Other1>::value &&
        std::is_constructible<Ty2, Other2>::value &&
        std::is_convertible<Other1&&, Ty1>::value &&
        std::is_convertible<Other2&&, Ty2>::value, int>::type = 0>
        constexpr pair(Other1&& a, Other2&& b)
        : first(mystl::forward<Other1>(a)),
        second(mystl::forward<Other2>(b))
    {
    }

    // explicit constructiable for other type
    template <class Other1, class Other2,
        typename std::enable_if<
        std::is_constructible<Ty1, Other1>::value &&
        std::is_constructible<Ty2, Other2>::value &&
        (!std::is_convertible<Other1, Ty1>::value ||
        !std::is_convertible<Other2, Ty2>::value), int>::type = 0>
        explicit constexpr pair(Other1&& a, Other2&& b)
        : first(mystl::forward<Other1>(a)),
        second(mystl::forward<Other2>(b))
    {
    }

    // implicit constructiable for other pair
    template <class Other1, class Other2,
        typename std::enable_if<
        std::is_constructible<Ty1, const Other1&>::value &&
        std::is_constructible<Ty2, const Other2&>::value &&
        std::is_convertible<const Other1&, Ty1>::value &&
        std::is_convertible<const Other2&, Ty2>::value, int>::type = 0>
        constexpr pair(const pair<Other1, Other2>& other)
        : first(other.first),
        second(other.second)
    {
    }

    // explicit constructiable for other pair
    template <class Other1, class Other2,
        typename std::enable_if<
        std::is_constructible<Ty1, const Other1&>::value &&
        std::is_constructible<Ty2, const Other2&>::value &&
        (!std::is_convertible<const Other1&, Ty1>::value ||
        !std::is_convertible<const Other2&, Ty2>::value), int>::type = 0>
        explicit constexpr pair(const pair<Other1, Other2>& other)
        : first(other.first),
        second(other.second)
    {
    }

    // implicit constructiable for other pair
    template <class Other1, class Other2,
        typename std::enable_if<
        std::is_constructible<Ty1, Other1>::value &&
        std::is_constructible<Ty2, Other2>::value &&
        std::is_convertible<Other1, Ty1>::value &&
        std::is_convertible<Other2, Ty2>::value, int>::type = 0>
        constexpr pair(pair<Other1, Other2>&& other)
        : first(mystl::forward<Other1>(other.first)),
        second(mystl::forward<Other2>(other.second))
    {
    }

    // explicit constructiable for other pair
    template <class Other1, class Other2,
        typename std::enable_if<
        std::is_constructible<Ty1, Other1>::value &&
        std::is_constructible<Ty2, Other2>::value &&
        (!std::is_convertible<Other1, Ty1>::value ||
        !std::is_convertible<Other2, Ty2>::value), int>::type = 0>
        explicit constexpr pair(pair<Other1, Other2>&& other)
        : first(mystl::forward<Other1>(other.first)),
        second(mystl::forward<Other2>(other.second))
    {
    }

    // copy assign for this pair
    pair& operator=(const pair& rhs)
    {
        if (this != &ths)
        {
            first = rhs.first;
            second = rhs.second;
        }
        return *this;
    }

    // move assign for this pair 
    pair& operator=(pair&& rhs)
    {
        if (this != & rhs)
        {
            first = ctstl::move(rhs.first);
            second = ctstl::move(rhs.second);
        }
        return *this;
    }

    // copy assign for other pair
    template <class Other1, class Other2>
    pair& operator=(const pair<Other1, Other2>& other)
    {
        first = other.first;
        second = other.second;
        return this;
    }

    // move assign for other pair
    tmeplate <class Other1, class Other2>
    pair& operator=(const Pair<Other1, Other2>&& other)
    {
        fisrt = ctstl::forward<Other1>(other.first);
        second = ctstl::forward<Other2>(Other.second);
        return *this;
    }

    ~pair() = default;

    void swap(pair& other)
    {
        if (this != other)
        {
            ctstl::swap(first, other.first);
            ctstl::swap(second, other.second);
        }
    }
};

// 重载比较操作符
template <class Ty1, class Ty2>
bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <class Ty1, class Ty2>
bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
    return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
}

template <class Ty1, class Ty2>
bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
    return !(lhs == rhs);
}

template <class Ty1, class Ty2>
bool operator>(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
    return rhs < lhs;
}

template <class Ty1, class Ty2>
bool operator<=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
    return !(rhs < lhs);
}

template <class Ty1, class Ty2>
bool operator>=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
    return !(lhs < rhs);
}

// 重载ctstl的swap
template <class Ty1, class Ty2>
void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs)
{
    lhs.swap(rhs);
}

// 全局函数，让两个数据成为一个pair
template <class Ty1, class Ty2>
pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second)
{
    return pair<Ty1, Ty2>(ctstl::forward<Ty1>(first), ctstl::forward<Ty2>(second));
}

}
#endif // !CTSTL_UTIL_H