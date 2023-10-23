#ifndef CTSTL_ALGOBASE_H
#define CTSTL_ALGOBASE_H

// 这个头文件包含了 mystl 的基本算法
#include <cstring>

#include "iterator.h"
#include "util.h"

namespace ctstl
{
    
#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

/*****************************************************************************************/
// max
// 取二者中的较大值，语义相等时保证返回第一个参数
/*****************************************************************************************/
template <class T>
const T& max(const T& lhs, const T& rhs)
{
    return lhs < rhs ? rhs : lhs;
}

// 重载版本使用函数对象 comp 代替比较操作
template <class T, class Compare>
const T& max(const T& lhs, const T& rhs, Compare compare)
{
    return compare(lhs, rhs) ? rhs : lhs;
}

/*****************************************************************************************/
// min 
// 取二者中的较小值，语义相等时保证返回第一个参数
/*****************************************************************************************/
template <class T>
const T& min(const T& lhs, const T& rhs)
{
    return rhs < lhs ? rhs : lhs;
}

// 重载版本使用函数对象 comp 代替比较操作
template <class T, class Compare>
const T& min(const T& lhs, const T& rhs, Compare compare)
{
    return compare(lhs, rhs) ? rhs : lhs;
}

/*****************************************************************************************/
// iter_swap
// 将两个迭代器所指对象对调
/*****************************************************************************************/
template <class FIter1, class FIter2>
void iter_swap(FIter1 lhs, FIter2 rhs)
{
    ctstl::swap(*lhs, *rhs);
}

/*****************************************************************************************/
// copy
// 把 [first, last)区间内的元素拷贝到 [result, result + (last - first))内
/*****************************************************************************************/
// input_iterator_tag 版本
template <class InputIter, class OutputIter>
OutputIter
unchecked_copy_cat(InputIter first, InputIter last, OutputIter result,
                   ctstl::input_iterator_tag)
{
    for (; first != last; ++first, ++result)
    {
        *result = *first;
    }
    return result;
}

// ramdom_access_iterator_tag 版本
template <class RandomIter, class OutputIter>
OutputIter
unchecked_copy_cat(RandomIter first, RandomIter last, OutputIter result,
                   ctstl::random_access_iterator_tag)
{
    for (auto n = last - first; n > 0; --n, ++first, ++result)
    {
        *result = *first;
    }
    return result;
}

template <class InputIter, class OutputIter>
OutputIter
unchecked_copy(InputIter first, InputIter last, OutputIter result)
{
    return unchecked_copy_cat(first, last, result, iterator_category(first));
}

// 为 trivially_copy_assignable 类型提供特化版本
/*
这段代码定义了一个模板函数 unchecked_copy，它用于在满足特定条件的情况下，将一段内存复制到另一段内存。
typename std::enable_if<...>::type：这是返回类型。std::enable_if 是一个模板，它根据条件选择是否定义 type 类型。如果条件为真，则 type 被定义为第二个模板参数的类型（默认为 void）；如果条件为假，则 type 不被定义。这里的条件是 std::is_same<typename std::remove_const<Tp>::type, Up>::value && std::is_trivially_copy_assignable<Up>::value，表示 Tp 去掉 const 后的类型必须与 Up 相同，并且 Up 必须是可平凡复制赋值的。
函数体中，首先计算要复制的元素数量 n，然后使用 std::memmove 函数将 [first, last) 范围内的元素复制到以 result 开始的内存中. 最后返回复制结束位置的指针
*/
template <class Tp, class Up>
typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
    std::is_trivially_copy_assignable<Up>::value,
    Up*>::type
unchecked_copy(Tp* first, Tp* last, Up* result)
{
    const auto n = static_cast<size_t>(last - first);
    if (n != 0)
        std:: memmove(result, first, n * sizeof(Up));
    return result + n;
}

template <class InputIter, class OutputIter>
OutputIter
copy(InputIter first, InputIter last, OutputIter result)
{
    return unchecked_copy(first, last, result);
}

/*****************************************************************************************/
// copy_backward
// 将 [first, last)区间内的元素拷贝到 [result - (last - first), result)内
/*****************************************************************************************/
// unchecked_copy_backward_cat 的 bidirectional_iterator_tag 版本
template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2
unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                            BidirectionalIter2 result, ctstl::bidirectional_iterator_tag)
{
    while (first != last)
        *--result = *--last;
    return result;
}

// unchecked_copy_backward_cat 的 random_access_iterator_tag 版本
template <class RandomIter1, class BidirectionalIter2>
BidirectionalIter2
unchecked_copy_backward_cat(RandomIter1 first, RandomIter1 last,
                            BidirectionalIter2 result, ctstl::random_access_iterator_tag)
{
    for (auto n = last - first; n > 0; --n)
    {
        *--result = *--last;
    }
    return result;
}

template <class BidirectionalIter1, class BidirecionalIter2>
BidirecionalIter2
unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
                        BidirecionalIter2 result)
{
    return unchecked_copy_backward_cat(first, last, result, iterator_category(first));
}

// 为 trivially_copy_assignable 类型提供特化版本
template <class Tp, class Up>
typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
    std::is_trivially_copy_assignable<Up>::value,
    Up*>::type
unchecked_copy_backward(Tp* first, Tp* last, Up* result)
{
    const auto n = static_cast<size_t>(last - first);
    if (n != 0)
    {
        result -= n;
        std::memmove(result, first, n * sizeof(Up));
    }
    return result;
}

template <class BidirectionalIter1, class BidirecitonalIter2>
BidirecitonalIter2
copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirecitonalIter2 result)
{
    return unchecked_copy_backward(first, last, result);
}

/*****************************************************************************************/
// copy_if
// 把[first, last)内满足一元操作 unary_pred 的元素拷贝到以 result 为起始的位置上
/*****************************************************************************************/
template <class InputIter, class OutputIter, class UnaryPredicate>
OutputIter
copy_if(InputIter first, InputIter last, OutputIter result, UnaryPredicate unary_pred)
{
    for (; first != last, ++first)
    {
        if (unary_pred(*(first)))
            *result++ = *first;
    }
    return result;
}

/*****************************************************************************************/
// copy_n
// 把 [first, first + n)区间上的元素拷贝到 [result, result + n)上
// 返回一个 pair 分别指向拷贝结束的尾部
/*****************************************************************************************/
template <class InputIter, class Size, class OutputIter>
ctstl::pair<InputIter, OutputIter>
unchecked_copy_n(InputIter first, Size n, OutputIter result, ctstl::input_iterator_tag)
{
    for (; n > 0; --n, ++first, ++result)
    {
        *result = *first;
    }
    return ctstl::make_pair<InputIter, OutputIter>(first, result);
}

template <class RandomIter, class Size, class OutputIter>
ctstl::pair<RandomIter, OutputIter>
unchecked_copy_n(RandomIter first, Size n, OutputIter result,
                ctstl::random_access_iterator_tag)
{
    auto last = first + n;
    return ctstl::pair<RandomIter, OutputIter>(last, ctstl::copy(first, last, result));
}

template <class InputIter, class Size, class OutputIter>
ctstl::pair<InputIter, OutputIter>
copy_n(InputIter first, Size n, OutputIter result)
{
    return unchecked_copy_n(first, n, last, iterator_category(first));
}

/*****************************************************************************************/
// move
// 把 [first, last)区间内的元素移动到 [result, result + (last - first))内
/*****************************************************************************************/
// input_iterator_tag 版本
template <class InputIter, class OutputIter>
OutputIter
unchecked_move_cat(InputIter first, InputIter last, OutputIter result,
                   ctstl::input_iterator_tag)
{
    for (; first != last; ++first, ++last)
    {
        *result = ctstl::move(first);
    }
    return resutl;
}

// ramdom_access_iterator_tag 版本
template <class InputIter, class OutputIter>
OutputIter
unchecked_move_cat(InputIter first, InputIter last, OutputIter result,
                   ctstl::random_access_iterator_tag)
{
    for (auto n = last - first; n > 0; --n, ++first, ++result)
    {
        *result = ctstl::move(*first);
    }
    return result;
}

template <class InputIter, class OutputIter>
OutputIter
unchecked_move(InputIter first, InputIter last, OutputIter result)
{
    return unchecked_move_cat(first, last, result, ctstl::iterator_category(first));
}

// 为 trivially_copy_assignable 类型提供特化版本
template <class Tp, class Up>
typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
    std::is_trivially_copy_assignable<Up>::value,
    Up*>::type
unchecked_move(Tp* first, Tp* last, Up* result)
{
    const size_t n = static_cast<size_t>(last - first);
    if (n != 0)
        std::memmove(result, first, n * sizeof(Up));
    return result + n;
}

template <class InputIter, class OutputIter>
OutputIter move(InputIter first, InputIter last, OutputIter result)
{
    return unchecked_move(first, last, result);
}

/*****************************************************************************************/
// move_backward
// 将 [first, last)区间内的元素移动到 [result - (last - first), result)内
/*****************************************************************************************/
// bidirectional_iterator_tag 版本
template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2
unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                            BidirectionalIter2 result, ctstl::bidirectional_iterator_tag)
{
    while (first != last)
        *--result = ctstl::move(*--last);
    return result;
}

// random_access_iterator_tag 版本
template <class RandomIter1, class RandomIter2>
RandomIter2
unchecked_move_backward_cat(RandomIter1 first, RandomIter1 last,
                            RandomIter2 result, ctstl::random_access_iterator_tag)
{
    for (auto n = last - first; n > 0; --n)
        *--result = ctstl::move(*--last);
    return result;
}

template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2
unchecked_move_backward(BidirectionalIter1 first, BidirectionalIter1 last,
                        BidirectionalIter2 result)
{
    return unchecked_move_backward_cat(first, last, result, ctstl::iterator_category(first));
}

// 为 trivially_copy_assignable 类型提供特化版本
template <class Tp, class Up>
typename std::enable_if<
  std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
  std::is_trivially_move_assignable<Up>::value,
  Up*>::type
unchecked_move_backward(Tp* first, Tp* last, Up* result)
{
  const size_t n = static_cast<size_t>(last - first);
  if (n != 0)
  {
    result -= n;
    std::memmove(result, first, n * sizeof(Up));
  }
  return result;
}

template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2
move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
{
  return unchecked_move_backward(first, last, result);
}

/*****************************************************************************************/
// equal
// 比较第一序列在 [first, last)区间上的元素值是否和第二序列相等
/*****************************************************************************************/
template <class InputIter1, class InputIter2>
bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2)
{
    for (; first1 != last1; ++first1, ++first2)
    {
        if (*first1 != *first2)
            return false;
    }
    return true;
}

// 重载版本使用函数对象 comp 代替比较操作
template <class InputIter1, class InputIter2, class Compared>
bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, Compared comp)
{
    for (; first1 != last1; ++first1, ++first2)
    {
        if (!comp(*first1, *first2))
            return false;
    }
    return true;
}

/*****************************************************************************************/
// fill_n
// 从 first 位置开始填充 n 个值
/*****************************************************************************************/
template <class OutputIter, class Size, class T>
OutputIter unchecked_fill_n(OutputIter first, Size n, const T& value)
{
    for (; n > 0; --n, ++first)
    {
        *first = value;
    }
    return first;
}

// 为 one-byte 类型提供特化版本
// std::enable_if<...>判断表示 Tp 和 Up 必须是整数类型，且它们的大小必须为 1 字节，并且 Tp 不能是布尔类型
template <class Tp, class Size, class Up>
typename std::enable_if<
    std::is_integral<Tp>::value && sizeof(Tp) == 1 &&
    !std::is_same<Tp, bool>::value &&
    std::is_integral<Up>::value && sizeof(Up) == 1,
    Tp*>::type
unchecked_fill_n(Tp* first, Size n, Up value)
{
    if (n > 0)
    {
        std::memset(first, (unsigned char)value, (size_t)(n));
    }
    return first + n;
}

template <class OutputIter, class Size, class T>
OutputIter fill_n(OutputIter first, Size n, const T& value)
{
    return unchecked_fill_n(first, n, value);
}

/*****************************************************************************************/
// fill
// 为 [first, last)区间内的所有元素填充新值
/*****************************************************************************************/
template <class ForwardIter, class T>
void fill_cat(ForwardIter first, ForwardIter last, const T& value,
              ctstl::forward_iterator_tag)
{
  for (; first != last; ++first)
  {
    *first = value;
  }
}

template <class RandomIter, class T>
void fill_cat(RandomIter first, RandomIter last, const T& value,
              ctstl::random_access_iterator_tag)
{
  fill_n(first, last - first, value);
}

template <class ForwardIter, class T>
void fill(ForwardIter first, ForwardIter last, const T& value)
{
  fill_cat(first, last, value, iterator_category(first));
}

/*****************************************************************************************/
// lexicographical_compare
// 以字典序排列对两个序列进行比较，当在某个位置发现第一组不相等元素时，有下列几种情况：
// (1)如果第一序列的元素较小，返回 true ，否则返回 false
// (2)如果到达 last1 而尚未到达 last2 返回 true
// (3)如果到达 last2 而尚未到达 last1 返回 false
// (4)如果同时到达 last1 和 last2 返回 false
/*****************************************************************************************/
template <class InputIter1, class InputIter2>
bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
                             InputIter2 first2, InputIter2 last2)
{
    for (; firs1 != last1 && first2 != last2; ++first1, ++last2)
    {
        if (*first1 < *first2)
            return true;
        if (*first2 < *first1)
            return false;
    }
    return first1 == last1 && first2 != last2;
}

// 重载版本使用函数对象 comp 代替比较操作
template <class InputIter1, class InputIter2, class Compred>
bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
                             InputIter2 first2, InputIter2 last2, Compred comp)
{
  for (; first1 != last1 && first2 != last2; ++first1, ++first2)
  {
    if (comp(*first1, *first2))
      return true;
    if (comp(*first2, *first1))
      return false;
  }
  return first1 == last1 && first2 != last2;
}

// 针对 const unsigned char* 的特化版本
bool lexicographical_compare(const unsigned char* first1,
                             const unsigned char* last1,
                             const unsigned char* first2,
                             const unsigned char* last2)
{
    const auto len1 = last1 - first1;
    const auto len2 = last2 - first2;
    // 先比较相同长度的部分
    const auto result = std::memcmp(first1, first2, ctstl::min(len1, len2));
    // 若相等，长度较长的比较大
    return result != 0 ? result < 0 : len1 < len2;
}

/*****************************************************************************************/
// mismatch
// 平行比较两个序列，找到第一处失配的元素，返回一对迭代器，分别指向两个序列中失配的元素
/*****************************************************************************************/
template <class InputIter1, class InputIter2>
ctstl::pair<InputIter1, InputIter2>
mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2)
{
    while (first1 != last1 && *first1 == *first2)
    {
        ++first1;
        ++first2;
    }
    return ctstl::make_pair<InputIter1, InputIter2>(first1, first2);
}

// 重载版本使用函数对象 comp 代替比较操作
template <class InputIter1, class InputIter2, class Compred>
ctstl::pair<InputIter1, InputIter2> 
mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compred comp)
{
  while (first1 != last1 && comp(*first1, *first2))
  {
    ++first1;
    ++first2;
  }
  return mystl::pair<InputIter1, InputIter2>(first1, first2);
}

} // namespace ctstl
#endif // !CTSTL_ALGOBASE_H_