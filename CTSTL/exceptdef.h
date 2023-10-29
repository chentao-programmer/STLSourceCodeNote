#ifndef CTSTL_EXCEPTDEF_H_
#define CTSTL_EXCEPTDEF_H_

/*
#include <stdexcept>：这个头文件定义了用于报告异常的多个标准类。这些类构成派生自exception类的所有派生层次结构，并包括两种常规类型的异常：逻辑错误和运行时错误。
#include <cassert>：这个头文件定义了一个宏函数assert()。当assert()中的表达式为假时，程序会终止并输出错误信息。它通常被用来在代码中进行调试和错误处理。
*/
#include <stdexcept>
#include <cassert>

namespace ctstl
{

#define CTSTL_DEBUG(expr) \
    assert(expr)

#define THROW_LENGTH_ERROR_IF(expr, what) \
    if ((expr)) throw std::length_error(what);

#define THROW_OUT_RANGE_IF(expr, what) \
    if ((expr)) throw std::out_of_range(what);

#define THROW_RUNTIME_ERROR_IF(expr, what) \
    if ((expr)) throw std::runtime_error(what)

} // namepsace ctstl
#endif // !CTSTL_EXCEPTDEF_H_