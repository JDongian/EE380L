// Vector.h -- header file for Vector data structure project

//#include <stdio.h>
#include <iostream>
#include <cstring>

#pragma once
#ifndef _Vector_h
#define _Vector_h

namespace epl{

    template <typename T>

        class vector {
            private:
                T* head_data;
                uint64_t head_length;
                uint64_t head_capacity;
                T* tail_data;
                uint64_t tail_length;
                uint64_t tail_capacity;

                static constexpr uint64_t default_capacity = 8;

                void init() {
                    head_data = (T*) std::malloc(sizeof(T) * (default_capacity / 2));
                    head_length = 0;
                    head_capacity = default_capacity / 2;
                
                    tail_data = (T*) std::malloc(sizeof(T) * (default_capacity / 2));
                    tail_length = 0;
                    tail_capacity = default_capacity / 2;
                    printf("DEBUG: INIT(void) (%d/%d) @%lu\n",
                            (int) head_length, (int) tail_length, (long) this);
                }

                void destroy(T* data, uint64_t length) {
                    for (int i = 0; i < length; ++i) {
                        printf("DEBUG DELETE[%d] (%d/%d) @%lu\n", i, (int)length, (int) size(),
                                (long) data+i);
                        data[i].~T();
                    }
                    std::free(data);
                }

                uint64_t get_head_index(uint64_t i) {
                    return head_length - i - 1;
                }

                uint64_t get_tail_index(uint64_t i) {
                    return i - head_length;
                }

                T& get_data(uint64_t i) {
                    if(i < head_length) {
                        return head_data[get_head_index(i)];
                    } else {
                        return tail_data[get_tail_index(i)];
                    }
                }

                void move(vector<T>&& other) {
                    head_data = other.head_data;
                    head_length = other.head_length;
                    head_capacity = other.head_capacity;
                    tail_data = other.tail_data;
                    tail_length = other.tail_length;
                    tail_capacity = other.tail_capacity;
                    
                    other.wipe();
                    printf("DEBUG MOVE (%d/%d)\n", (int) head_length, (int) tail_length);
                }

                void copy(const vector& other) {
                    //printf("DEBUG COPY FROM (%d/%d)\n", (int) other.head_length, (int) other.tail_length);
                    head_length = other.head_length;
                    head_capacity = other.head_length;
                    if (head_length == 0) {
                        head_data = nullptr;
                    } else {
                        head_data = (T*) std::malloc(sizeof(T) * other.head_length);
                        for (int i = 0; i < other.head_length; ++i) {
                            // copy construct data
                            new (head_data + i) T{other.head_data[i]};
                        }
                    }
                    tail_length = other.tail_length;
                    tail_capacity = other.tail_length;
                    if (tail_length == 0) {
                        tail_data = nullptr;
                    } else {
                        tail_data = (T*) std::malloc(sizeof(T) * other.tail_length);
                        for (int i = 0; i < other.tail_length; ++i) {
                            // copy construct data
                            new (tail_data + i) T{other.tail_data[i]};
                        }
                    }
                }

//                void swap(T& first, T& second) {
//                    // std::swap(first.length, second.length);
//                    // std::swap(first.data, second.data);
//                }

                void wipe(void) {
                    head_data = nullptr;
                    head_length = 0;
                    head_capacity = 0;
                    tail_data = nullptr;
                    tail_length = 0;
                    tail_capacity = 0;
                }

            public:

                /* creates an array with some minimum capacity and length
                 * equal to zero. Must not use T::T(void). In fact, as long as
                 * Vector::Vector(int) is not called, you should never use
                 * T::T(void) */
                vector(void) {
                    init();
                }

                vector(uint64_t n) {
                    if (n == 0) {
                        init();
                    } else {
                        head_capacity = 0;
                        head_length = 0;
                        head_data = nullptr;

                        tail_capacity = n;
                        tail_length = n;
                        // allocate T(void) n times
                        //tail_data = new T[n];
                        tail_data = (T*) std::malloc(sizeof(T) * n);

                        for (int i = 0; i < tail_length; ++i) {
                            new (tail_data + i) T;
                        }

                        printf("DEBUG INIT(%d): (%d/%d) @%lu\n",
                                (int) n, (int) head_length, (int) tail_length, (long) this);
                    }
                }

                // deep copy
                vector(const vector& other) {
                    //printf("DEBUG COPY CONSTRUCT {\n");
                    copy(other);
                    //std::copy(other.data, other.data + other.length, data);
                    printf("DEBUG COPY CONSTRUCT (%d/%d)\n", (int) head_length, (int) tail_length);
                }

                vector(vector&& v) {
                    printf("DEBUG vector(move)...\t");
                    *this = std::move(v);
                }

                vector& operator=(vector&& v) {
                    move(std::move(v));
                    return *this;
                }

//                // copy assignment
//                vector<T>& operator=(const vector<T>& other) {
//                //    if (this != &other) {
//                //        T* new_data =
//                //        std::copy(other.data, other.data + other.length, new_data);
//                //        delete[] data;
//                //        data = new_data;
//                //    }
//                //    return *this;
//                }

//                // move assignment
//                vector<T>& operator=(vector<T>&& rhs) {
//                //    length = rhs.length;
//                //    capacity = rhs.capacity;
//                //    delete data;
//                //    data = std::move(rhs.data);
//                //    return *this;
//                }

                ~vector() {
                    destroy(head_data, head_length);
                    destroy(tail_data, tail_length);
                    /*
                    for (int i = 0; i < head_length; ++i) {
                        printf("DEBUG DELETE HEAD: %d (%d/%d) @%lu\n", i, (int)head_length, (int) size(),
                                (long) head_data+i);
                        head_data[i].~T();
                    }
                    std::free(head_data);

                    for (int i = 0; i < tail_length; ++i) {
                        printf("DEBUG DELETE TAIL: %d (%d/%d) @%lu\n", i, (int)tail_length, (int) size(),
                                (long) tail_data+i);
                        tail_data[i].~T();
                    }
                    std::free(tail_data);
                    */
                }

                uint64_t size(void) const { return head_length + tail_length; }

                /* If k is out of bounds (equal to or larger than length),
                 * then you must throw
                 * std::out_of_range(“subscript out of range”); or a similar
                 * error message (the string is arbitrary). If k is in bounds,
                 * then you must return a reference to the element at position
                 * k */
                T& operator [](uint64_t i) {
                    if (i < 0 || size() <= i) {
                        throw std::out_of_range("subscript out of range");
                    } else {
                        return get_data(i);
                    }
                }

                const T& operator [](uint64_t i) const {
                    if (i < 0 || size() <= i) {
                      throw std::out_of_range("subscript out of range");
                    } else {
                        return (const T&)&(get_data(i));
                    }
                }

                void push(const T& new_value,
                        T*& data, uint64_t& length, uint64_t& capacity) {
                    printf("DEBUG PUSH (%d/%d) {\n", (int) head_length, (int) tail_length);
                    // deep copy
                    auto insert_value = T{new_value};
                    // amortized doubling
                    if (length == capacity) {
                        capacity += 1;
                        capacity *= 2;
                        // same as malloc if nullptr
                        data = (T*) std::realloc(data, sizeof(T) * capacity);
                        printf("DEBUG capacity doubled.\n");
                    }
                    // insert new element using copy constructor,
                    // moving from saved value
                    new (data + length) T{std::move(insert_value)};
                    length++;

                    printf("DEBUG PUSH (%d/%d) }\n", (int) head_length, (int) tail_length);
                }

                void push(T&& new_value,
                        T*& data, uint64_t& length, uint64_t& capacity) {
                    // amortized doubling
                    if (length == capacity) {
                        capacity += 1;
                        capacity *= 2;
                        // same as malloc if nullptr
                        data = (T*) std::realloc(data, sizeof(T) * capacity);
                    }
                    // insert new element using move constructor
                    new (data + length) T{std::move(new_value)};
                    length++;

                    printf("DEBUG PUSHr (%d/%d)\n", (int) head_length, (int) tail_length);
                }

                /* add a new value to the end of the array,  using amortized
                 * doubling if the array has to be resized. Copy construct the
                 * argument */
                void push_back(const T& new_value) {
                    push(new_value, tail_data, tail_length, tail_capacity);
                }

                void push_front(const T& new_value) {
                    push(new_value, head_data, head_length, head_capacity);
                }

                void push_back(T&& new_value) {
                    push(std::forward<T>(new_value),
                            tail_data, tail_length, tail_capacity);
                }

                void push_front(T&& new_value) {
                    push(std::forward<T>(new_value),
                            head_data, head_length, head_capacity);
                }

                void rotate_one(T*& data, uint64_t& len) {
                    data = (T*) std::memmove(data, data + 1,
                            sizeof(T) * (len - 1));
                }

                void pop(T*& data, uint64_t& data_len, T*& other, uint64_t& other_len) {
                    if (size() == 0) {
                        throw std::out_of_range("subscript out of range");
                    }
                    if (other_len > 0) {
                        // pop last element
                        other[other_len - 1].~T();
                        other_len--;
                    } else {
                        // pop first element and rotate
                        data[0].~T();
                        rotate_one(data, data_len);
                        data_len--;
                    }

                    printf("DEBUG POP (%d/%d)\n", (int) head_length, (int) tail_length);
                }

                void pop_back(void) {
                    pop(head_data, head_length, tail_data, tail_length);
                }

                void pop_front(void) {
                    pop(tail_data, tail_length, head_data, head_length);
                }
        };

} //namespace epl



#endif /* _Vector_h */
