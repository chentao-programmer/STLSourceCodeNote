// file: 3autoptr.cpp
// C++中的智能指针(auto_ptr) 可以实现堆内存对象的自动释放
template <class T>
class auto_ptr {
public:
    explicit auto_ptr(T *p = 0) : pointee(p) {}
    
    template <class U>
    auto_ptr(auto_ptr<U>& rhs): pointee(rhs.release()) {}

    ~auto_ptr() {
        delete pointee;
    }
    // 这是赋值操作符，它接受一个其他类型的auto_ptr对象，并从该对象中获取所有权。
    /*
    release()函数的作用是将当前对象内的裸指针置空，并返回裸指针的地址。在这个函数中，rhs.release()会释放rhs所拥有的对象的所有权，并返回这个对象的指针。
    reset()函数的作用是更新当前对象内的裸指针，如果当前裸指针与即将被复制的裸指针不同，则将当前裸指针释放之后再更新。在这个函数中，reset(rhs.release())会先释放当前对象所拥有的对象（如果有的话），然后获取rhs所拥有的对象的所有权。
    */
    template <class U>
    auto_ptr<T>& operator=(auto_ptr<U>& rhs) {
        if (this != &rhs)
            reset(rhs.release());
        return *this;
    }
    //这两个操作符允许你像使用普通指针一样使用auto_ptr对象。
    T& operator*() const {
        return *pointee;
    }
    T* operator->() const {
        return pointee;
    }
    T* get() const {
        return pointee;
    }

private:
    T *pointee;
};