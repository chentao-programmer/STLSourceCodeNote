#ifndef CTSTL_ITERATOR_H
#define CTSTL_ITERATOR_H

// 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数，

// 这个头文件包含了如size_t，ptrdiff_t，NULL等类型和宏。
#include <cstddef>

#include "type_traits.h"

namespace ctstl {
// 五种迭代器类型
struct input_iterator_tag {}; // 输入迭代器，只读
struct output_iterator_tag {}; // 输出迭代器，只写
struct forward_iterator_tag : public input_iterator_tag {}; // 前向迭代器，可以读写，只能单向移动
struct bidirectional_iterator_tag : public forward_iterator_tag {}; // 双向迭代器，可以读写，可以双向移动
struct random_access_iterator_tag : public bidirectional_iterator_tag {};  // 随机访问迭代器，可以读写，可以任意跳转

// iterator 模板
template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
struct iterator
{
    typedef Category                    iterator_category;      // 迭代器的类型
    typedef T                           value_type;             // 元素的类型
    typedef Pointer                     pointer;                // 指向元素的指针的类型，它默认为T*1
    typedef Reference                   reference;              // 元素的引用的类型，它默认为T&
    typedef Distance                    difference_type;        // 两个迭代器之间的距离的类型，它默认为ptrdiff_t
};

// iterator traits
// has_iterator_cat：这个模板用于检查一个类型是否有iterator_category成员
/*
这个模板结构体has_iterator_cat是用来检查一个类型是否有iterator_category这个嵌套类型的。它使用了一种被称为SFINAE（Substitution Failure Is Not An Error）的技术。
这个结构体定义了两个test函数模板，一个接受任意参数，返回一个包含两个char的结构体two；另一个只接受指向类型U::iterator_category的指针作为参数，返回一个char。当尝试用某个类型T调用test<T>(0)时，如果T::iterator_category存在，那么第二个版本的test函数会被选中，因为它更特化；如果不存在，那么只能选择第一个版本。
然后通过比较调用结果的大小和单个字符的大小来确定是否存在嵌套类型。如果选择了第二个版本的函数，那么结果的大小就是一个字符的大小；如果选择了第一个版本，那么结果的大小就是两个字符的大小12。
所以，如果类型T有嵌套类型iterator_category，那么has_iterator_cat<T>::value就是真；否则就是假。
*/
template <class T>
struct has_iterator_cat
{
private:
    // 
    struct two { char a; char b; };
    // 这行代码定义了一个函数模板test，它接受任意数量和类型的参数，并返回一个two类型的对象
    template <class U> static two test(...);
    // 这行代码定义了另一个函数模板test，它接受一个指向U::iterator_category类型的指针，并返回一个字符。
    // 如果U::iterator_category不存在，那么这个函数模板就无法被实例化
    template <class U> static char test(typename U::iterator_category* = 0);
public:
    static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

// iterator_traits_impl模板结构体定义了一组类型别名，这些别名对应于迭代器的五个关联类型，这个模板只有在第二个模板参数为真时才会被实例化。
template <class Iterator, bool>
struct iterator_traits_impl {};

template <class Iterator>
struct iterator_traits_impl<Iterator, true>
{
  typedef typename Iterator::iterator_category iterator_category;
  typedef typename Iterator::value_type        value_type;
  typedef typename Iterator::pointer           pointer;
  typedef typename Iterator::reference         reference;
  typedef typename Iterator::difference_type   difference_type;
};

template <class Iterator, bool>
struct iterator_traits_helper {};

// 这个条件检查了类型的嵌套类型iterator_category是否可以转换为input_iterator_tag或者output_iterator_tag。
template <class Iterator>
struct iterator_traits_helper<Iterator, true>
  : public iterator_traits_impl<Iterator,
  std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
  std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>
{
};

// 萃取迭代器的特性
template <class Iterator>
struct iterator_traits : public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};

// 针对原生指针的偏特化版本
template <class T>
struct iterator_traits<T*>
{
    typedef random_access_iterator_tag          iterator_category;
    typedef T                                   value_type;
    typedef T*                                  pointer;
    typedef T&                                  reference;
    typedef ptrdiff_t                           difference_type;
};

template <class T>
struct iterator_traits<const T*>
{
  typedef random_access_iterator_tag           iterator_category;
  typedef T                                    value_type;
  typedef const T*                             pointer;
  typedef const T&                             reference;
  typedef ptrdiff_t                            difference_type;
};

// 用于检查一个类型的迭代器类别是否可以转换为另一个类别
template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
struct has_iterator_cat_of
  : public m_bool_constant<std::is_convertible<
  typename iterator_traits<T>::iterator_category, U>::value>
{
};

// 萃取某种迭代器
template <class T, class U>
struct has_iterator_cat_of<T, U, false> : public m_false_type {};

template <class Iter>
struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {};

template <class Iter>
struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {};

template <class Iter>
struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

template <class Iter>
struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

template <class Iter>
struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

// 检查一个类型是否是输入迭代器或输出迭代器
template <class Iterator>
struct is_iterator :
  public m_bool_constant<is_input_iterator<Iterator>::value ||
    is_output_iterator<Iterator>::value>
{
};

// 萃取某个迭代器的 category
template <class Iterator>
typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&)
{
    typedef typename iterator_traits<Iterator>::iterator_category Category;
    return Category();
}

// 萃取某个迭代器的 distance_type
// 并没有实际计算任何距离，而只是返回了一个类型为difference_type的空指针
template <class Iterator>
typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&)
{
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// 萃取某个迭代器的 value_type
template <class Iterator>
typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&)
{
  return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// 以下函数用于计算迭代器间的距离
// distance 的 input_iterator_tag 的版本
template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag)
{
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

// distance 的 random_access_iterator_tag 的版本
template <class RandomIter>
typename iterator_traits<RandomIter>::difference_type
distance_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag)
{
    return last - first;
}

template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
  return distance_dispatch(first, last, iterator_category(first));
}

// 以下函数用于让迭代器前进 n 个距离
// advance 的 input_iterator_tag 的版本
template <class InputIterator, class Distance>
void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag)
{
    while (n--) i++;
}

// advance 的 bidirectional_iterator_tag 的版本
template <class BidirectionalIterator, class Distance>
void advance_dispatch(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag)
{
  if (n >= 0)
    while (n--)  ++i;
  else
    while (n++)  --i;
}

// advance 的 random_access_iterator_tag 的版本
template <class RandomIter, class Distance>
void advance_dispatch(RandomIter& i, Distance n, random_access_iterator_tag)
{
  i += n;
}

template <class InputIterator, class Distance>
void advance(InputIterator& i, Distance n)
{
  advance_dispatch(i, n, iterator_category(i));
}

// 模板类 : reverse_iterator
// 代表反向迭代器，使前进为后退，后退为前进
template <class Iterator>
class reverse_iterator
{
private:
    Iterator current;  // 记录对应的正向迭代器

public:
    // 反向迭代器的五种相应型别
    typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
    typedef typename iterator_traits<Iterator>::value_type        value_type;
    typedef typename iterator_traits<Iterator>::difference_type   difference_type;
    typedef typename iterator_traits<Iterator>::pointer           pointer;
    typedef typename iterator_traits<Iterator>::reference         reference;

    typedef Iterator                                              iterator_type;
    typedef reverse_iterator<Iterator>                            self;

public:
    // 构造函数
    reverse_iterator() {}
    explicit reverse_iterator(iterator_type i) :current(i) {}
    reverse_iterator(const self& rhs) :current(rhs.current) {}

public:
    // 取出对应的正向迭代器
    iterator_type base() const
    {
        return current;
    }

    // 重载操作符
    reference operator*() const
    { // 实际对应正向迭代器的前一个位置
        auto tmp = current;
        return *--tmp;
    }

    pointer operator->() const
    {
        return &(operator*());
    }

    /*
    self& operator--(): 这个函数返回类型是self&，也就是对象本身的引用。这意味着这个函数返回的是调用它的对象本身，而不是对象的一个新副本。因此，对返回值进行的任何修改都会影响到原始对象。
    self operator--(int): 这个函数返回类型是self，也就是对象的副本。这意味着这个函数返回的是调用它的对象的一个新副本，而不是对象本身。因此，对返回值进行的任何修改都不会影响到原始对象。
    */
    // 前进(++)变为后退(--)
    // 前置++
    self& operator++()
    {
        --current;
        return *this;
    }

    // 后置++
    self operator++(int)
    {
        self tmp = *this;
        --current;
        return tmp;
    }

    self& operator--()
    {
        ++current;
        return *this;
    }

    self operator--(int)
    {
        self tmp = *this;
        ++current;
        return tmp;
    }

    self& operator+=(difference_type n)
    {
        current -= n;
        return *this;
    }

    self operator+(difference_type n) const
    {
        return self(current - n);
    }

    self& operator-=(difference_type n)
    {
        current += n;
        return *this;
    }

    self operator-(difference_type n) const
    {
        return self(current + n);
    }

    reference operator[](difference_type n) const
    {
        return *(*this + n);
    }
};

// 重载 operator-
template <class Iterator>
typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& lhs,
          const reverse_iterator<Iterator>& rhs)
{
  return rhs.base() - lhs.base();
}

// 重载比较操作符
template <class Iterator>
bool operator==(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
{
    return lhs.base() == rhs.base();
}

template <class Iterator>
bool operator!=(const reverse_iterator<Iterator> &lhs,
                const reverse_iterator<Iterator> &rhs)
{
    return !(lhs == rhs);
}

template <class Iterator>
bool operator<(const reverse_iterator<Iterator> &lhs,
               const reverse_iterator<Iterator> &rhs)
{
    return rhs.base() < lhs.base();
}

template <class Iterator>
bool operator>(const reverse_iterator<Iterator> &lhs,
               const reverse_iterator<Iterator> &rhs)
{
    return rhs < lhs;
}

template <class Iterator>
bool operator<=(const reverse_iterator<Iterator> &lhs,
                const reverse_iterator<Iterator> &rhs)
{
    return !(rhs < lhs);
}

template <class Iterator>
bool operator>=(const reverse_iterator<Iterator> &lhs,
                const reverse_iterator<Iterator> &rhs)
{
    return !(lhs < rhs);
}

} // namespace std;
#endif