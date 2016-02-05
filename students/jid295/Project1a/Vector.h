// Vector.h -- header file for Vector data structure project

//#include <stdio.h>
#include <iostream>

#pragma once
#ifndef _Vector_h
#define _Vector_h

namespace epl{

    template <typename T>

        class vector {
            private:
                T* data;
                uint64_t length;
                uint64_t capacity;

                static constexpr uint64_t default_capacity = 8;

                void init() {
                    data = (T*) operator new (sizeof(T) * default_capacity);
                    length = 0;
                    capacity = default_capacity;
                }

                void copy(const T& from) {
                    length = from.length;
                    if (length == 0) {
                        data = nullptr;
                        return;
                    }

                    data = new T[length];
                    //memcpy(data, from.data, sizeof(T) * length);
                    std::copy(from.data, from.data + length, data);
                }

                //void swap(T& first, T& second) {
                //    std::swap(first.length, second.length);
                //    std::swap(first.data, second.data);
                //}

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
                        capacity = n;
                        length = n;
                        data = new T[n];
                    }
                }

                // copy (deep)
                vector(const vector& other) {
                    length = other.length;
                    capacity = other.capacity;
                    std::copy(other.data, other.data + other.length, data);
                }

                // move
                vector(vector<T>&& other) {
                    length = other.length;
                    capacity = other.capacity;
                    data = std::move(other.data);
                }

                // copy assignment
                vector<T>& operator=(const vector<T>& other) {
                    if (this != &other) {
                        T* new_data = new T[other.length];
                        std::copy(other.data, other.data + other.length, new_data);

                        delete[] data;

                        data = new_data;
                        length = other.length;
                    }
                    return *this;
                }

                // move assignment
                vector<T>& operator=(vector<T>&& rhs) {
                    length = rhs.length;
                    capacity = rhs.capacity;

                    delete[] data;
                    data = std::move(rhs.data);
                    return *this;
                }
                
                // destructor
                ~vector() {
                    delete[] data;
                }
                
                uint64_t size(void) const { return length; }
               
                /* If k is out of bounds (equal to or larger than length),
                 * then you must throw
                 * std::out_of_range(“subscript out of range”); or a similar
                 * error message (the string is arbitrary). If k is in bounds,
                 * then you must return a reference to the element at position
                 * k */
                T& operator [](uint64_t k) {
                    if (k >= length) {
                        throw std::out_of_range("subscript out of range");
                    } else {
                        return data[k];
                    }
                }
                
                const T& operator [](uint64_t k) const {
                    if (k >= length) {
                        throw std::out_of_range("subscript out of range");
                    } else {
                        return (const T&)&(data[k]);
                    }
                }

                /* add a new value to the end of the array,  using amortized
                 * doubling if the array has to be resized. Copy construct the
                 * argument */
                void push_back(const T& new_value) {
                    if(length == capacity) {
                        capacity = length*2;
                        data = (T*) realloc(data, capacity);
                    }
                    new (data+length) T{new_value};
                    length++;
                }
                
                // void push_back(T&&)
                
                void push_front(const T&) {
                }
                
                //void push_front(T&&) {
                //}

                void pop_back(void) {
                    data[length].~T();
                    length--;
                }

                void pop_front(void) {
                }
        };

} //namespace epl



#endif /* _Vector_h */
