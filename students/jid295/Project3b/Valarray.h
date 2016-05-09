// Valarray.h
#ifndef _Valarray_h
#define _Valarray_h

#include <iostream>
#include <cstdint>
#include <algorithm>
#include <complex>
#include <vector>
#include <math.h>
#include "Vector.h"

//using std::vector;
using epl::vector;
using std::cout;
using std::endl;
using std::complex;


template <typename V> class VExpression;
template <typename T> using valarray = VExpression<vector<T>>;


//// type choosing
    template <typename> struct SRank { static constexpr int value = 0; };
    template <> struct SRank<int> { static constexpr int value = 1; };
    template <> struct SRank<float> { static constexpr int value = 2; };
    template <> struct SRank<double> { static constexpr int value = 3; };
    template <typename T> struct SRank<std::complex<T>> { static constexpr int value = SRank<T>::value; };


    template <int> struct SType;
    template <> struct SType<1> { using type = int; };
    template <> struct SType<2> { using type = float; };
    template <> struct SType<3> { using type = double; };


    template <typename T> struct SComplex : public std::false_type {};
    template <typename T> struct SComplex<std::complex<T>> : public std::true_type {};

    template <bool, typename T> struct Complex;
    template <typename T> struct Complex<false, T> { using type = T; };
    template <typename T> struct Complex<true, T> { using type = std::complex<T>; };


    template <typename T1, typename T2>
    struct ctype {
        static constexpr bool is_complex = SComplex<T1>::value || SComplex<T2>::value;
        static constexpr int rank1 = SRank<T1>::value;
        static constexpr int rank2 = SRank<T2>::value;
        static constexpr int rank_max = (rank1 > rank2) ? rank1 : rank2;

        using type = typename Complex<is_complex, typename SType<rank_max>::type>::type;
    };

    template <typename T> struct wrap {
        using type = T;
    };
    template <typename T> struct wrap<vector<T>> {
        using type = vector<T> const&;
    };


    template <typename T1, typename T2>
    using Type = typename ctype<T1, T2>::type;

    template <typename T> using Wrap = typename wrap<T>::type;
////

// Exprs
template <typename Op, typename T1, typename T2>
class BinExpression {
private:
    Op op;
    Wrap<T1> left;
    Wrap<T2> right;

public:
    // choose with the priority
    using value_type = Type<typename T1::value_type, typename T2::value_type>;

    BinExpression(Op y, Wrap<T1> a, Wrap<T2> b) :
        op{ y }, left{ a }, right{ b } {}
    BinExpression(BinExpression const& other) :
        op{ other.op }, left{ other.left }, right{ other.right } {}

    auto operator[](uint64_t k) const {
        return op(static_cast<value_type>(left[k]), static_cast<value_type>(right[k]));
    }

    uint64_t size(void) const {
        return std::min(left.size(), right.size());
    }

};

// unary husk
template <typename Op, typename V>
class UnaryExpression {
private:
    Op op;
    Wrap<V> x;

public:
    using value_type = typename V::value_type;

    UnaryExpression(Op a, Wrap<V> b) : op{ a }, x{ b } {}
    UnaryExpression(UnaryExpression const& other) : op{ other.op }, x{ other.x } {}

    auto operator[](uint64_t k) const {
        return op(x[k]);
    }

    auto size(void) const {
        return x.size();
    }
};


// scalar husk
template <typename T>
class SExpression {
private:
    T val;

public:
    using value_type = T;

    SExpression(T const& x) : val{ x } {}

    auto operator[](uint64_t) const {
        return val;
    }

    auto size(void) const {
        return std::numeric_limits<uint64_t>::max();
    }
};

template <typename T>
struct squareRoot {
    constexpr auto operator() (T const& x) const {
        return sqrt(x);
    }
};

// vector expr wrapper
template <typename V>
class VExpression : public V {
using V::V;
using T_V = typename V::value_type;

public:
    VExpression(void) : V{} {}
    VExpression(int) : V{} {}
    VExpression(double) : V{} {}
    VExpression(V const& other) : V{ other } {}
    VExpression(VExpression const& other) : V{ other } {}
    

    template <typename T>
    VExpression(VExpression<T> const& other) : V(other.size()) {
        for (auto i = 0; i < this->size(); ++i) {
            (*this)[i] = static_cast<T_V>(other[i]);
        }
    }

    template <typename T, typename _ = typename SType<SRank<T>::value>::type>
    auto operator=(T const& other) {
        this->operator=(VExpression<SExpression<T>>(other));
    }

    template <typename T>
    auto operator=(VExpression<T> const& other) { // lol op
        auto size = std::min(this->size(), other.size());

        for (auto i = 0; i < size; ++i) {
            (*this)[i] = static_cast<T_V>(other[i]);
        }

        return *this;
    }

    template <typename Op>
    auto apply(Op o) const {
        V const& self{ *this };
        UnaryExpression<Op, V> result{ o, self };

        return VExpression<UnaryExpression<Op, V>>{ result };
    }

    auto operator-(void) const { // needed to add this
        return apply(std::negate<void>{});
    }

    class iterator {
    public:
        using value_type = typename VExpression::value_type;
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using reference = value_type;
        using pointer = value_type*;

        VExpression v;
        uint64_t index;

        iterator(VExpression b, uint64_t n) : v(b), index(n) {}
        iterator(const iterator& it) : v(it.v), index(it.index) {}
        iterator& operator=(const iterator& it) { v = it.v; index = it.index; return *this; }
        bool operator==(const iterator& it) const { return index == it.index; }
        bool operator!=(const iterator& it) const { return ! this->operator==(it); }
        bool operator<(const iterator& it)  const { return index < it.index; }
        bool operator>(const iterator& it)  const { return index > it.index; }
        bool operator<=(const iterator& it) const { return index <= it.index; }
        bool operator>=(const iterator& it) const { return index >= it.index; }
        iterator& operator++() { index++; return *this; }
        iterator& operator--() { index--; return *this; }
        iterator operator++(int) { iterator t{*this}; this->operator++(); return t; }
        iterator operator--(int) { iterator t{*this}; this->operator--(); return t; }
        iterator& operator+=(uint64_t d) { index += d; return *this; }
        iterator& operator-=(uint64_t d) { index -= d; return *this; }
        iterator operator+(uint64_t d) const { iterator t{*this}; t.index += d; return t; }
        iterator operator-(uint64_t d) const { iterator t{*this}; t.index -= d; return t; }
        int64_t operator-(iterator it) const { return index - it.index; }
        value_type operator*() const { return v.operator[](index); }
        value_type operator[](size_t n) const { return v.operator[](index + n); }
    };

    iterator begin() {
        return iterator(VExpression {}, 0);
    }

    iterator end() {
        auto x = VExpression {};
        return iterator(x, x.size());
    }
    
    template <typename Op>
    auto accumulate(Op op) const {
        if (!this->size()) {
            return 0;
        } else {
            typename V::value_type result{ (*this)[0] };
            for (auto i = 1; i < this->size(); ++i) {
                result = op(result, (*this)[i]);
            }
            return result;
        }
    }

    auto sum(void) const {
        return accumulate(std::plus<void>{});
    }

    auto sqrt(void) const {
        return apply(squareRoot<typename V::value_type>{});
    }
};
//


//// expr sub functions
    template <typename Op, typename V1, typename V2>
    auto apply(Op o, VExpression<V1> const& left, VExpression<V2> const& right) {
            V1 const& a{ left };
            V2 const& b{ right };
            BinExpression<Op, V1, V2> result{ o, a, b };
            return VExpression<BinExpression<Op, V1, V2>>{ result };
    };

    // how to make these symmetrical?
    template <typename Op, typename V, typename T, typename _ = typename SType<SRank<T>::value>::type>
    auto apply(Op o, VExpression<V> const& left, T const& right) {
            V const& a{ left };
            SExpression<T> b{ right };
            BinExpression<Op, V, SExpression<T>> result{ o, a, b };
            return VExpression<BinExpression<Op, V, SExpression<T>>>{ result };
    };
    template <typename Op, typename T, typename V, typename _ = typename SType<SRank<T>::value>::type>
    auto apply(Op o, T const& left, VExpression<V> const& right) {
            SExpression<T> a{ left };
            V const& b{ right };
            BinExpression<Op, SExpression<T>, V> result{ o, a, b };
            return VExpression<BinExpression<Op, SExpression<T>, V>>{ result };
    };
////


//// operators
    template <typename T1, typename T2>
    auto operator+(T1 const& a, T2 const& b) {
        return apply(std::plus<void>{}, a, b);
    };

    template <typename T1, typename T2>
    auto operator-(T1 const& a, T2 const& b) {
        return apply(std::minus<void>{}, a, b);
    };

    template <typename T1, typename T2>
    auto operator*(T1 const& a, T2 const& b) {
        return apply(std::multiplies<void>{}, a, b);
    };

    template <typename T1, typename T2>
    auto operator/(T1 const& a, T2 const& b) {
        return apply(std::divides<void>{}, a, b);
    };
////

template <typename T>
inline std::ostream& operator<<(std::ostream& ost, VExpression<T> const& arr) {
    //for (auto val: arr) {
    //    ost << val << " ";
    //}
    ost << "derp ";
    return ost;
}
#endif /* _Valarray_h */
