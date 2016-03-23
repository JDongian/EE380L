#include <iostream>
#include <cstdint>

/* 
 * The partition function is used by the quicksort algorithm 
 * the purpose of the function is to move larger values towards
 * the end of the container and smaller values towards the front
 *
 * In this version of partition, FI must satisfy the forward iterator concept
 * that means FI::operator++, FI::operator*, FI::operator== must exist and
 * have O(1) time complexity and provide the expected semantics
 * comp must be a function object. The function must 
 * provide a strict weak ordering for elements stored in the container
 * i.e., if x and y are elements in the container then exactly one of the following must hold
 *   either {
          comp(x, y) is true -- signifying x is less than y
		  comp(y, x) is true -- signifying y is less than x
		  neither comp(x, y) nor comp(y, x) is true -- signifying x is equivalent to y
 *
 *
 * NOTE: the convention in writing generic code is to pass the iterators by value, not by
 * constant reference. In the parameters below [b, e) is a container (or sub-range of a container)
 * and comp is a comparison function object as described above. Note that in most cases, comp
 * is state-less, and the only real information in contained in the type Comp. However, it's 
 * possible that comp is an actual pointer to a function (yuck -- no inlining).
 */
template <typename FI, typename Comp>
FI partition(FI b, FI e, Comp comp) {
	/* this implementation assumes only forward iteration
	 * to keep the implementation simple, I will use *b as the pivot value */

	 /* loop invariant: the range [b, piv_pos) consists of elements strictly less than the pivot
	  * the range [piv_pos, next) is not empty and consists of elements not less than the pivot
	  * the pivot value is *piv_pos 
	  * thus, when next == e, we're done with partition */

	if (b == e) { return b; } // probably invalid case, not sure the returned value is meaningful

//	using I = std::remove_reference<FI>::type; // our iterator type
	FI piv_pos{ b };
	FI next{ b };
	++next; 

	/* invariant established [b, piv_pos) is empty (contains no elements)
	 * [piv_pos, next) contains one element (the pivot),
	 *              and that element is not less than the piv value ('cause it IS the piv value)
	 * (note that the piv value is always at *piv_pos) */
	while (!(next == e)) {
		if (comp(*next, *piv_pos)) { // *next is a small value, needs to be moved to the front
			std::swap(*next, *piv_pos); // small value moved to the front (it's now the last of the smalls)
			++piv_pos; // number of small values increases by one: [b, piv_pos) increased by one elem.
			std::swap(*next, *piv_pos); // put pivot value back where it belongs at *piv_pos
		}
		/* at this point [b, piv_pos) is less than *piv_pos
		 * the pivot value is stored at *piv_pos
		 * all values in the range [piv_pos, next] are not less than piv_value
		 * NOTE: [piv_pos, next) may be empty, so increment next to restore invariant and
		 * make progress towards our goal */
		++next;
	}

	return piv_pos;
}


/* see comments for partition regarding the requiremetns for FI and Comp */
template <typename FI, typename Comp>
void quickSort(FI b, FI e, Comp comp) {
	if (b == e) { return; } // empty container, already sorted
	FI piv_pos = partition(b, e, comp);
	quickSort(b, piv_pos, comp);
	++piv_pos;
	quickSort(piv_pos, e, comp);
}