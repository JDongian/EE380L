// iterator.hpp -- header file for Vector iterator
#pragma once
#ifndef _iterator_hpp
#define _iterator_hpp
/*
#include "Vector.h"

namespace epl{
    //template <typename T>
    class invalid_iterator { 
        public:
        enum { SEVERE = 0, MODERATE = 1, MILD = 2 };
        int level;
    };
    template <typename T, class A = std::allocator<T>>
        class iterator { 
            private:
                vector<T, A> data;
                uint64_t index;

            public:
                typedef typename A::difference_type difference_type;
                typedef typename A::value_type value_type;
                typedef typename A::reference reference;
                typedef typename A::pointer pointer;
                typedef std::random_access_iterator_tag iterator_category; //or another tag

                iterator() {
                    printf("DEBUG ITERATOR CONSTRUCTOR DEFAULT \n");
                }
                iterator(const vector<T, A>& v, const uint64_t& i) {
                    // copy
                    data = v;
                    index = i;
                }
                iterator(const iterator&) {
                    printf("DEBUG ITERATOR COPY\n");
                }
                ~iterator() {
                    printf("DEBUG ITERATOR DESTRUCT\n");
                }

                iterator& operator=(const iterator& rhs) {
                    if (this != &rhs) {
                        //destroy_all();
                        //copy(rhs);
                    }
                    return *this;
                }
                // binary operators
                // who needs relops
                bool operator==(const iterator& rhs) const {
                    return false; // TODO
                }
                bool operator!=(const iterator& rhs) const {
                    return ! operator==(rhs);
                }
                bool operator<(const iterator& rhs) const {
                    return false;// TODO
                }
                bool operator<=(const iterator& rhs) const {
                    return operator<(rhs) || operator==(rhs);
                }
                bool operator>(const iterator& rhs) const {
                    return ! operator<=(rhs);
                }
                bool operator>=(const iterator& rhs) const {
                    return ! operator<(rhs);
                }

                iterator& operator++() {
                    return *this; // TODO
                }
                iterator& operator--() {
                    return *this; // TODO
                }

                //iterator& operator+=(size_type); //optional
                //iterator operator+(size_type) const; //optional
                //friend iterator operator+(size_type, const iterator&); //optional
                //iterator& operator-=(size_type); //optional            
                //iterator operator-(size_type) const; //optional
                //difference_type operator-(iterator) const; //optional

                reference operator*() const {
                    //return (T&) addr;
                    return data[index];
                }
                pointer operator->() const {
                    return data[index];
                    //throw std::out_of_range("not implemented");
                    //return nullptr;
                }
                //reference operator[](size_type) const; //optional
        };

    template <typename T, class A = std::allocator<T>>
        class const_iterator {
            public:
                typedef typename A::difference_type difference_type;
                typedef typename A::value_type value_type;
                typedef typename A::reference const_reference;
                typedef typename A::pointer const_pointer;
                typedef std::random_access_iterator_tag iterator_category; //or another tag


                const_iterator() {
                }
                const_iterator(const const_iterator&) {
                }
                const_iterator (const iterator<T, A>&) {
                }
                ~const_iterator() {
                }

                const_iterator& operator=(const const_iterator& rhs) {
                    if (this != &rhs) {
                        //destroy_all();
                        //copy(rhs);
                    }
                    return *this;
                }
                // binary operators
                // who needs relops
                bool operator==(const const_iterator& rhs) const {
                    return false; // TODO
                }
                bool operator!=(const const_iterator& rhs) const {
                    return ! operator==(rhs);
                }
                bool operator<(const const_iterator& rhs) const {
                    return false;// TODO
                }
                bool operator<=(const const_iterator& rhs) const {
                    return operator<(rhs) || operator==(rhs);
                }
                bool operator>(const const_iterator& rhs) const {
                    return ! operator<=(rhs);
                }
                bool operator>=(const const_iterator& rhs) const {
                    return ! operator<(rhs);
                }

                const_iterator& operator++() {
                    return *this;
                }
                const_iterator& operator--() {
                    return *this;
                }
                //const_iterator& operator+=(size_type); //optional
                //const_iterator operator+(size_type) const; //optional
                //friend const_iterator operator+(size_type, const const_iterator&); //optional
                //const_iterator& operator-=(size_type); //optional            
                //const_iterator operator-(size_type) const; //optional
                //difference_type operator-(const_iterator) const; //optional

                T& operator*() const {
                    throw std::out_of_range("not implemented");
                    //return (T&) addr;
                }
                T operator->() const {
                    throw std::out_of_range("not implemented");
                    //return nullptr;
                }
                //const_reference operator[](size_type) const; //optional
        };
}
*/
#endif /* _iterator_hpp */
