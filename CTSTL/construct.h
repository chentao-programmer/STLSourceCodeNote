#ifndef CTSTL_CONSTRUCT_H
#define CTSTL_CONSTRUCT_H

// 这个头文件包含两个函数 construct，destroy
// construct : 负责对象的构造
// destroy   : 负责对象的析构

#include <new>

#include "type_traits.h"
#include "iterator.h"

/*
用于控制 Microsoft Visual C++ 编译器的警告行为的。具体来说：
#ifdef _MSC_VER：这是一个预处理器指令，用于检查 _MSC_VER 宏是否被定义。_MSC_VER 是一个预定义的宏，用于表示 Microsoft Visual C++ 编译器的版本。如果代码是在 Microsoft Visual C++ 编译器下编译，那么这个宏就会被定义。
#pragma warning(push)：这个指令会保存当前的警告状态。这样，你可以随后修改警告状态，然后使用 #pragma warning(pop) 恢复到之前保存的状态。
#pragma warning(disable : 4100)：这个指令会禁用编号为 4100 的警告。编号 4100 的警告是关于未使用的参数的警告。
#endif：这个指令结束了 #ifdef 的代码块。
在 STL 的 construct 中，这段代码可能被用于禁用关于未使用参数的警告。这是因为在某些情况下，construct 可能会接收到一些并未实际使用的参数。为了避免编译器因此产生警告，可以使用上述代码来禁用这类警告。
*/
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)  // unused parameter
#endif // _MSC_VER

namespace ctstl
{

// construct 构造对象

// 就是在 ptr 所指向的内存中构造一个 Ty 类型的对象。
// 这是operator new的一个重载版本、允许你在一个已经分配好的内存中（栈或堆中）构造一个新的对象，其中的void*p实际上就是指向一个已经分配好的内存缓冲区的的首地址。
template <class Ty>
void construct(Ty* ptr)
{
    ::new ((void*)ptr) Ty();
}

template <class Ty1, class Ty2>
void construct(Ty1* ptr, const Ty2& value)
{
    ::new ((void*)ptr) Ty1(value);
}

/*
- `template <class Ty, class... Args>`：这是一个模板声明，表示这个函数可以接受任意类型的参数。`Ty` 是第一个模板参数，表示对象的类型，`Args` 是一个模板参数包，表示一组参数。
- `void construct(Ty* ptr, Args&&... args)`：这是函数的声明。`construct` 是函数名，`Ty* ptr` 是第一个参数，表示指向已分配内存的指针，`Args&&... args` 是第二个参数，表示一组用于初始化新对象的参数。
- `::new ((void*)ptr) Ty(mystl::forward<Args>(args)...);`：这是函数体中的一行代码。它使用了所谓的 "placement new" 来在 `ptr` 所指向的内存中构造一个 `Ty` 类型的对象，并使用 `args` 进行初始化。
*/
template <class Ty1, class... Args>
void construct(Ty1* ptr, Args&&... args)
{
    ::new ((void*)ptr) Ty1(value);
}

// destroy 将对象析构

// 这个函数是当 Ty 类型的对象是平凡可析构的时候使用的。平凡可析构意味着这个类型的析构函数不会执行任何操作，因此这个函数体是空的
template <class Ty>
void destroy_one(Ty*, std::true_type) {}

// 这个函数是当 Ty 类型的对象不是平凡可析构的时候使用的。在这种情况下，我们需要显式地调用析构函数来销毁对象。这个函数首先检查指针是否为 nullptr，如果不是，就调用该类型的析构函数1
template <class Ty>
void destroy_one(Ty* pointer, std::false_type) 
{
    if (pointer != nullptr)
    {
        pointer->~Ty();
    }
}

template <class ForwardIter>
void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

template <class ForwardIter>
void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
{
    for (; first != last; ++first)
    {
        destroy(&*first);
    }
}

template <class Ty>
void destroy(Ty* pointer)
{
    destroy_one(pointer, std::is_trivially_destructible<Ty>{});
}

template <class ForwardIter>
void destroy(ForwardIter first, ForwardIter last)
{
    destroy_cat(first, last, std::is_trivially_destructible<
                typename iterator_traits<ForwardIter>::value_type>{});
}

} //// !MYTINYSTL_CONSTRUCT_H_

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER
#endif

