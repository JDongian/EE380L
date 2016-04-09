// Vector.h -- header file for Vector data structure project
#pragma once
#ifndef _Vector_h
#define _Vector_h

#include <iostream>
#include <cstring>
#include "iterator.hpp"


namespace epl{
    class invalid_iterator { 
        public:
            enum Level { SEVERE = 0, MODERATE = 1, MILD = 2 };
            Level level = MILD;

            invalid_iterator(Level severity) {
                level = severity;
            }
    };

    template <typename T, class A = std::allocator<T>>
        class vector {
            private:
                T* head_data;
                uint64_t head_length;
                uint64_t head_capacity;
                T* tail_data;
                uint64_t tail_length;
                uint64_t tail_capacity;

                uint64_t version = 0;

                static constexpr uint64_t default_capacity = 8;

                void init() {
                    head_data = (T*) std::malloc(sizeof(T) * (default_capacity / 2));
                    head_length = 0;
                    head_capacity = default_capacity / 2;

                    tail_data = (T*) std::malloc(sizeof(T) * (default_capacity / 2));
                    tail_length = 0;
                    tail_capacity = default_capacity / 2;
                    ////printf("DEBUG: INIT(void) (%d/%d) @%lu\n",
                    ////        (int) head_length, (int) tail_length, (long) this);
                }

                void destroy(T* data, uint64_t length) {
                    for (int i = 0; i < length; ++i) {
                        //printf("DEBUG DELETE[%d] (%d/%d) @%lu\n", i, (int)length, (int) size(),
                        //        (long) data+i);
                        data[i].~T();
                    }
                    std::free(data);
                }

                void destroy_all(void) {
                    destroy(head_data, head_length);
                    destroy(tail_data, tail_length);
                }

                uint64_t get_head_index(uint64_t i) const {
                    return head_length - i - 1;
                }

                uint64_t get_tail_index(uint64_t i) const {
                    return i - head_length;
                }

                T& get_data(uint64_t i) const {
                    if(i < head_length) {
                        return head_data[get_head_index(i)];
                    } else {
                        return tail_data[get_tail_index(i)];
                    }
                }

                void move(vector<T>&& other) {
                    ++(other.version);

                    head_data = other.head_data;
                    head_length = other.head_length;
                    head_capacity = other.head_capacity;
                    tail_data = other.tail_data;
                    tail_length = other.tail_length;
                    tail_capacity = other.tail_capacity;

                    other.wipe();
                    ////printf("DEBUG MOVE (%d/%d)\n", (int) head_length, (int) tail_length);
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

                void push(const T& new_value,
                        T*& data, uint64_t& length, uint64_t& capacity) {
                    ++version;
                    ////printf("DEBUG PUSH (%d/%d) {\n", (int) head_length, (int) tail_length);
                    // deep copy
                    auto insert_value = T{new_value};
                    // amortized doubling
                    if (length == capacity) {
                        capacity += 1;
                        capacity *= 3;
                        // same as malloc if nullptr
                        data = (T*) std::realloc(data, sizeof(T) * capacity);
                        ////printf("DEBUG capacity doubled.\n");
                    }
                    // insert new element using copy constructor,
                    // moving from saved value
                    new (data + length) T{std::move(insert_value)};
                    length++;

                    ////printf("DEBUG PUSH (%d/%d) }\n", (int) head_length, (int) tail_length);
                }

                void push(T&& new_value,
                        T*& data, uint64_t& length, uint64_t& capacity) {
                    ++version;
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

                    ////printf("DEBUG PUSHr (%d/%d)\n", (int) head_length, (int) tail_length);
                }

                void rotate_one(T*& data, uint64_t& len) {
                    data = (T*) std::memmove(data, data + 1,
                            sizeof(T) * (len - 1));
                }

                void pop(T*& data, uint64_t& data_len, T*& other, uint64_t& other_len) {
                    ++version;
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

                    ////printf("DEBUG POP (%d/%d)\n", (int) head_length, (int) tail_length);
                }

            public:
                // Credits http://stackoverflow.com/a/7759622/2534876
                typedef A allocator_type;
                typedef typename A::value_type value_type; 
                typedef typename A::reference reference;
                typedef typename A::const_reference const_reference;
                typedef typename A::difference_type difference_type;
                typedef typename A::size_type size_type;

                class iterator { 
                    private:
                        vector<T, A>* data;
                        uint64_t index;
                        uint64_t data_version;

                        void bounds_check(void) const {
                            if (index < 0 || data->size() < index) {
                                throw invalid_iterator(invalid_iterator::SEVERE);
                            }
                        }

                        void version_check(void) const {
                            if (data->version != data_version) {
                                throw invalid_iterator(invalid_iterator::MODERATE);
                            }
                        }

                        void check(void) const {
                            ////printf("DEBUG CHECK %lu (%lu)\n", index, data->size());
                            bounds_check();
                            version_check();
                        }

                    public:
                        typedef typename A::difference_type difference_type;
                        typedef typename A::value_type value_type;
                        typedef typename A::reference reference;
                        typedef typename A::pointer pointer;
                        typedef std::random_access_iterator_tag iterator_category;

                        iterator() {
                            ////printf("DEBUG ITERATOR CONSTRUCTOR DEFAULT (!)\n");
                        }
                        iterator(vector<T, A>* v, uint64_t i) {
                            // copy
                            data = v;
                            index = i;
                            data_version = data->version;
                            ////printf("DEBUG ITERATOR CONSTRUCTOR (%lu) v%lu\n", i, data_version);
                        }
                        iterator(const iterator& other) {
                            index = other.index;
                            data = other.data;
                            data_version = data->version;
                            ////printf("DEBUG ITERATOR COPY\n");
                        }
                        ~iterator() {
                            ////printf("DEBUG ITERATOR DESTRUCT\n");
                        }

                        iterator& operator=(const iterator& rhs) {
                            rhs->check();
                            ////printf("DEBUG ITERATOR ASSIGN (!)\n");
                            if (this != &rhs) {
                                //destroy_all();
                                //copy(rhs);
                                //TODO
                            }
                            return *this;
                        }
                        // who needs relops
                        bool operator==(const iterator& rhs) const {
                            check();
                            //printf("DEBUG %lu == %lu\n", index, rhs.index);
                            return index == rhs.index;
                        }
                        bool operator!=(const iterator& rhs) const {
                            check();
                            return ! operator==(rhs);
                        }
                        bool operator<(const iterator& rhs) const {
                            check();
                            return index < rhs.index;
                        }
                        bool operator<=(const iterator& rhs) const {
                            check();
                            //return index <= rhs.index;
                            return operator<(rhs) || operator==(rhs);
                        }
                        bool operator>(const iterator& rhs) const {
                            check();
                            return ! operator<=(rhs);
                        }
                        bool operator>=(const iterator& rhs) const {
                            check();
                            return ! operator<(rhs);
                        }

                        iterator operator++() {
                            check();
                            auto previous = *this;
                            ++index;
                            ////printf("DEBUG (%lu)++\n", index);
                            return previous;
                        }
                        iterator operator++(int) {
                            check();
                            ++index;
                            ////printf("DEBUG ++(%lu)\n", index);
                            return *this;
                        }
                        iterator operator--() {
                            check();
                            auto previous = *this;
                            --index;
                            ////printf("DEBUG (%lu)--\n", index);
                            return previous;
                        }
                        iterator operator--(int) {
                            check();
                            --index;
                            ////printf("DEBUG --(%lu)\n", index);
                            return *this;
                        }

                        //iterator& operator+=(size_type); //optional
                        //iterator operator+(size_type) const; //optional
                        //friend iterator operator+(size_type, const iterator&); //optional
                        //iterator& operator-=(size_type); //optional            
                        //iterator operator-(size_type) const; //optional
                        //difference_type operator-(iterator) const; //optional

                        reference operator*() const {
                            check();
                            ////printf("DEBUG *(%lu) v%lu\n", index, data->version);
                            return (T&) (data->operator[](index));
                        }
                        pointer operator->() const {
                            check();
                            //return data[index];
                            throw std::out_of_range("op-> not implemented");
                            //return nullptr;
                        }
                        //reference operator[](size_type) const; //optional
                };

                class const_iterator {
                    private:
                        const vector<T, A>* data;
                        uint64_t index;
                        uint64_t data_version;

                        void bounds_check(void) const {
                            if (index < 0 || data->size() < index) {
                                throw invalid_iterator(invalid_iterator::SEVERE);
                            }
                        }

                        void version_check(void) const {
                            if (data->version != data_version) {
                                throw invalid_iterator(invalid_iterator::MODERATE);
                            }
                        }

                        void check(void) const {
                            ////printf("DEBUG CHECK %lu (%lu)\n", index, data->size());
                            bounds_check();
                            version_check();
                        }

                    public:
                        typedef typename A::difference_type difference_type;
                        typedef typename A::value_type value_type;
                        typedef typename A::reference const_reference;
                        typedef typename A::pointer const_pointer;
                        typedef std::random_access_iterator_tag iterator_category; //or another tag


                        const_iterator() {
                        }
                        const_iterator (const iterator&) {
                        }
                        const_iterator(const vector<T, A>* v, uint64_t i) {
                            // copy
                            data = v;
                            index = i;
                            data_version = data->version;
                            ////printf("DEBUG const_iterator CONSTRUCTOR (%lu) v%lu\n", i, data_version);
                        }
                        const_iterator(const const_iterator& other) {
                            index = other.index;
                            data = other.data;
                            data_version = data->version;
                            ////printf("DEBUG const_iterator COPY\n");
                        }
                        ~const_iterator() {
                            ////printf("DEBUG const_iterator DESTRUCT\n");
                        }

                        const_iterator& operator=(const const_iterator& rhs) {
                            rhs->check();
                            ////printf("DEBUG const_iterator ASSIGN (!)\n");
                            if (this != &rhs) {
                                //destroy_all();
                                //copy(rhs);
                                //TODO
                            }
                            return *this;
                        }
                        // who needs relops
                        bool operator==(const const_iterator& rhs) const {
                            check();
                            //printf("DEBUG %lu == %lu\n", index, rhs.index);
                            return index == rhs.index;
                        }
                        bool operator!=(const const_iterator& rhs) const {
                            check();
                            return ! operator==(rhs);
                        }
                        bool operator<(const const_iterator& rhs) const {
                            check();
                            return index < rhs.index;
                        }
                        bool operator<=(const const_iterator& rhs) const {
                            check();
                            //return index <= rhs.index;
                            return operator<(rhs) || operator==(rhs);
                        }
                        bool operator>(const const_iterator& rhs) const {
                            check();
                            return ! operator<=(rhs);
                        }
                        bool operator>=(const const_iterator& rhs) const {
                            check();
                            return ! operator<(rhs);
                        }

                        const_iterator operator++() {
                            check();
                            auto previous = *this;
                            ++index;
                            ////printf("DEBUG (%lu)++\n", index);
                            return previous;
                        }
                        const_iterator operator++(int) {
                            check();
                            ++index;
                            ////printf("DEBUG ++(%lu)\n", index);
                            return *this;
                        }
                        const_iterator operator--() {
                            check();
                            auto previous = *this;
                            --index;
                            ////printf("DEBUG (%lu)--\n", index);
                            return previous;
                        }
                        const_iterator operator--(int) {
                            check();
                            --index;
                            ////printf("DEBUG --(%lu)\n", index);
                            return *this;
                        }

                        //const_iterator& operator+=(size_type); //optional
                        //const_iterator operator+(size_type) const; //optional
                        //friend const_iterator operator+(size_type, const const_iterator&); //optional
                        //const_iterator& operator-=(size_type); //optional            
                        //const_iterator operator-(size_type) const; //optional
                        //difference_type operator-(const_iterator) const; //optional

                        reference operator*() const {
                            check();
                            ////printf("DEBUG *(%lu) v%lu\n", index, data->version);
                            return (T&) (data->operator[](index));
                        }
                        T* operator->() const {
                            check();
                            //return data[index];
                            throw std::out_of_range("op-> not implemented");
                            //return nullptr;
                        }
                        //reference operator[](size_type) const; //optional
                };

                // constructors and destructors
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
                        tail_data = (T*) std::malloc(sizeof(T) * n);
                        //tail_data = new T[n];

                        for (int i = 0; i < tail_length; ++i) {
                            new (tail_data + i) T;
                        }

                        ////printf("DEBUG INIT(%d): (%d/%d) @%lu\n",
                        ////        (int) n, (int) head_length, (int) tail_length, (long) this);
                    }
                }
                // deep copy
                vector(const vector& other) {
                    //printf("DEBUG COPY CONSTRUCT {\n");
                    copy(other);
                    //std::copy(other.data, other.data + other.length, data);
                    ////printf("DEBUG COPY CONSTRUCT (%d/%d)\n", (int) head_length, (int) tail_length);
                }
                vector(vector&& v) {
                    ////printf("DEBUG vector(move)...\t");
                    *this = std::move(v);
                }
                vector(std::initializer_list<T> values) {
                    init();
                    for (auto value: values) {
                        push_back(value);
                    }
                }
                ~vector() {
                    destroy_all();
                }

                // move assignment
                vector& operator=(vector&& v) {
                    move(std::move(v));
                    return *this;
                }
                // copy assignment
                vector<T>& operator=(const vector& rhs) {
                    if (this != &rhs) {
                        destroy_all();
                        copy(rhs);
                    }
                    return *this;
                }
                bool operator==(const vector& rhs) const {
                    if (size() == rhs.size()) {
                        for (int i = 0; i < size(); ++i) {
                            if (operator[](i) != rhs[i]) {
                                return false;
                            }
                        }
                        return true;
                    }
                    return false;
                }
                bool operator!=(const vector& rhs) const {
                    return ! operator==(rhs);
                }

                iterator begin() {
                    return iterator(this, 0);
                }
                const_iterator begin() const {
                    return const_iterator(this, 0);
                }
                const_iterator cbegin() const { //TODO: ????
                    return const_iterator(this, 0);
                }
                iterator end() {
                    return iterator(this, size());
                }
                const_iterator end() const {
                    return const_iterator(this, size());
                }
                const_iterator cend() const {
                    return const_iterator(this, size());
                }

                uint64_t size(void) const { return head_length + tail_length; }

                /* If k is out of bounds (equal to or larger than length),
                 * then you must throw
                 * std::out_of_range(“subscript out of range”); or a similar
                 * error message (the string is arbitrary). If k is in bounds,
                 * then you must return a reference to the element at position
                 * k */
                T& operator[](uint64_t i) {
                    if (i < 0 || size() <= i) {
                        throw std::out_of_range("subscript out of range");
                    } else {
                        return get_data(i);
                    }
                }

                const T& operator[](uint64_t i) const {
                    if (i < 0 || size() <= i) {
                        throw std::out_of_range("subscript out of range");
                    } else {
                        return (const T&) get_data(i);
                    }
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

                void pop_back(void) {
                    pop(head_data, head_length, tail_data, tail_length);
                }

                void pop_front(void) {
                    pop(tail_data, tail_length, head_data, head_length);
                }
        };

    template <typename T>
        using Vector = vector<T>;
} //namespace epl



#endif /* _Vector_h */
