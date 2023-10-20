template <class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value) {
    // 修改*first!=value
    while (first!=last && *first!=value)
    // while (first!=last && (*first).value()!=value)
        ++first;
    return first;
}

// 接受三个参数：两个迭代器first和last，以及一个函数对象f。
template <class InputIterator, class Function>
InputIterator for_each(InputIterator first, InputIterator last, Function f) {
    for (; first != last; ++first)
        f(*first);
    return first;
}