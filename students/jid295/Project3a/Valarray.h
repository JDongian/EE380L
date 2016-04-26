// Valarray.h
#ifndef _Valarray_h
#define _Valarray_h

#include <iostream>
#include <cstdint>
#include <algorithm>
#include <complex>
#include <vector>

using std::vector;
// TODO
//using epl::vector<T>;
using std::cout;
using std::endl;
using std::complex;

template <typename> struct valarray;

/* type promotion */
template <typename> struct SRank { static constexpr int value = 0; };
template <> struct SRank<int> { static constexpr int value = 1; };
template <> struct SRank<float> { static constexpr int value = 2; };
template <> struct SRank<double> { static constexpr int value = 3; };

template <int> struct SType; //
template <> struct SType<1> { using type = int; };
template <> struct SType<2> { using type = float; };
template <> struct SType<3> { using type = double; };

template <typename T1, typename T2>
struct choose_type {
    static constexpr int rank1 = SRank<T1>::value;
    static constexpr int rank2 = SRank<T2>::value;
    static constexpr int rank_max = (rank1 > rank2) ? rank1 : rank2;

    using type = typename SType<rank_max>::type;
};
/* end of type promotion */

/* scalar determination */
template <typename T1, typename T2> struct get_type;
template <typename V1, typename V2> struct get_type<valarray<V1>, valarray<V2>> {
    using type = typename choose_type<V1, V2>::type;
};
template <typename V, typename T> struct get_type<valarray<V>, T> {
    using type = typename choose_type<V, T>::type;
};
template <typename T, typename V> struct get_type<T, valarray<V>> {
    using type = typename choose_type<T, V>::type;
};

template <bool p, typename T>
using Enabled = typename std::enable_if<p, T>::type;
/* end of scalar determination */


template <typename T> class valarray;
template <typename Op, typename T>
void apply_op(valarray<T>& lhs, valarray<T> const& x, valarray<T> const& y, Op op = Op{}) {
        uint64_t size = std::min(x.size(), y.size());
        size = std::min(size, lhs.size()); // probably not needed
        for (uint64_t k = 0; k < size; k += 1) {
                lhs[k] = op(x[k], y[k]);
        }
}

//template <typename T> class valarray;
//template <typename Op, typename T, typename S>
//void apply_op(valarray<T>& lhs, valarray<T> const& arr, const S& scalar, Op op = Op{}) {
//        for (uint64_t k = 0; k < arr.size(); k += 1) {
//                lhs[k] = op(arr[k], scalar);
//        }
//}

template <typename T>
class valarray : public vector<T> {
    using Same = valarray<T>;
public:
    using std::vector<T>::vector;
};

template <typename T>
valarray<T> operator+(valarray<T> const& lhs, valarray<T> const& rhs) {
        valarray<T> result(std::min(lhs.size(), rhs.size()));
        apply_op<std::plus<T>>(result, lhs, rhs);
        return result;
}
 
template <typename T>
inline std::ostream& operator<<(std::ostream& ost, const valarray<T>& arr) {
    for (auto val: arr) {
        ost << val << " ";
    }
    return ost;
}

#endif /* _Valarray_h */
