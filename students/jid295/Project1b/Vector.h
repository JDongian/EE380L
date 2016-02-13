// Vector.h -- header file for Vector data structure project

//#include <stdio.h>
//#include <iostream>

#pragma once
#ifndef _Vector_h
#define _Vector_h

namespace epl{

    template <typename T>

        class vector {
            private:
                uint64_t length;
                uint64_t capacity;
                uint64_t head_length;
                T* data = nullptr;

                static constexpr uint64_t default_capacity = 8;

                // Allocate memory but do not call constructors
                void init() {
                    length = 0;
                    head_length = 0;
                    capacity = default_capacity;
                    data = (T*) operator new (sizeof(T) * capacity);
                }
    
                void destroy(void) {
                    delete[] data;
                }

                // deep cope
                void copy(const vector<T>& src) {
                    if (this != &src) {
                        destroy();

                        length = src.length;
                        capacity = src.capacity;
                        head_length = src.head_length;
                        data = (T*) operator new (sizeof(T) * capacity);
                        memmove(data, src.data, sizeof(T) * capacity); // check this
                    }
                }

                void move(vector<T>&& src) { // check we need this
                    length = src.length;
                    capacity = src.capacity;
                    data = src.data;
                }

                T& fetch(uint64_t i) {
                    if (0 <= i && i < length) {
                        return data[head_length + i];
                    } else {
                        throw std::out_of_range("subscript out of range");
                    }
                }
                
                // this resizing scheme isn't memory-efficient with unbalanced inserts
                void resize(void) {
                    T* old = data;
                    head_length = 1 + capacity;
                    capacity = 3 + capacity * 3; // make sure this is needed

                    data = (T*) operator new (sizeof(T) * capacity);
                    // what is the right way to do this?
                    for (auto i = 0; i < length; ++i) {
                        new (data + head_length + i) T(std::move(fetch(i)));
                    }

                    operator delete (old);
                }

            public:

                /* creates an array with some minimum capacity and length
                 * equal to zero. Must not use T::T(void). In fact, as long as
                 * Vector::Vector(int) is not called, you should never use
                 * T::T(void) */
                vector(void) {
                    init();
                }

                explicit vector(uint64_t n) {
                    if (n == 0) {
                        init();
                    } else {
                        length = n;
                        capacity = n;
                        head_length = 0;

                        data = new T[n]; // confirm this ok
                    }
                }

                // copy (deep)
                vector(const vector<T>& src) {
                    copy(src);
                }

                // move
                vector(vector<T>&& src) {
                    *this = std::move(src); // check this works
                }

                // copy assignment
                vector<T>& operator=(const vector<T>& rhs) {
                    if (this != &rhs) {
                        destroy();
                        copy(rhs);
                    }
                    return *this;
                }

                // move assignment
                vector<T>& operator=(vector<T>&& rhs) {
                    if (this != &rhs) {
                        destroy();
                        *this = std::move(rhs); // check this works
                    }
                    return *this;
                }
                
                // destructor
                ~vector() {
                    destroy();
                }
                
                uint64_t size(void) const {
                    return length;
                }
               
                /* If k is out of bounds (equal to or larger than length),
                 * then you must throw
                 * std::out_of_range(“subscript out of range”); or a similar
                 * error message (the string is arbitrary). If k is in bounds,
                 * then you must return a reference to the element at position
                 * k */
                T& operator [](uint64_t k) {
                    return fetch(k);
                }
                
                const T& operator [](uint64_t k) const {
                    return fetch(k);
                }

                /* add a new value to the end of the array,  using amortized
                 * doubling if the array has to be resized. Copy construct the
                 * argument */
                void push_back(const T& e) {
                    if (head_length == 0 || head_length + length == capacity) {
                        resize();
                    }
                    new (data + head_length + length) T(e);
                    length++;
                }
                
                void push_back(T&& e) {
                    if (head_length == 0 || head_length + length == capacity) {
                        resize();
                    }
                    new (data + head_length + length) T(std::move(e));
                    length++;
                }
                
                void push_front(const T& e) {
                    if (head_length == 0 || head_length + length == capacity) {
                        resize();
                    }
                    new (data + head_length - 1) T(e);
                    length++;
                    head_length--;
                }

                void push_front(T&& e) {
                    if (head_length == 0 || head_length + length == capacity) {
                        resize();
                    }
                    new (data + head_length - 1) T(std::move(e));
                    length++;
                    head_length--;
                }

                void pop_back(void) {
                    if (length) {
                        length--;
                        data[length].~T();
                    } else {
                        throw std::out_of_range("nothing to pop");
                    }
                }

                void pop_front(void) {
                    if (length) {
                        length--;
                        data[length].~T();
                        data++;
                        head_length++;
                    } else {
                        throw std::out_of_range("nothing to pop");
                    }
                }
        };

} //namespace epl



#endif /* _Vector_h */
