#ifndef NVX_ASSERT_HPP_89732
#define NVX_ASSERT_HPP_89732

#include <algorithm>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <nvx/iostream.hpp>
#include <nvx/mescal.hpp>





namespace nvx
{





/********************* ASSERT FUNCTIONS *********************/
// main functions
template<typename T, typename U>
bool assert_eq(T const &lhs, U const &rhs, char const *err = "Error", bool silent = false)
{
	if (silent)
		return lhs == rhs;

	if (!(lhs == rhs))
	{
		std::cerr << "lhs: " << lhs << ", " << rhs << '\n';
		throw err;
	}
	return true;
}



// for wstring
inline bool assert_eq(
	std::wstring const &lhs,
	std::wstring const &rhs,
	char const *err = "Error",
	bool silent = false
)
{
	if (silent)
		return lhs == rhs;

	if (!(lhs == rhs))
	{
		// std::wcerr << "lhs: " << ws2s(lhs) << ", " << ws2s(rhs) << '\n';
		throw err;
	}
	return true;
}



// for sequence
template<typename IteratorLhs, typename IteratorRhs>
bool assert_eq(
	IteratorLhs lhsbeg, IteratorLhs lhsend,
	IteratorRhs rhsbeg, IteratorRhs rhsend,
	char const *err = "Error", bool silent = false
)
{
	auto lhsbegcp = lhsbeg;
	auto rhsbegcp = rhsbeg;

	while (lhsbeg != lhsend && rhsbeg != rhsend)
	{
		if (!assert_eq(*lhsbeg, *rhsbeg, err, true))
			goto fail_label;
		++lhsbeg, ++rhsbeg;
	}

	if (lhsbeg != lhsend || rhsbeg != rhsend)
		goto fail_label;

	return true;

fail_label:
	if (silent)
		return false;

	nvx::print(std::cerr << "lhs: ", lhsbegcp, lhsend, " ") << '\n';
	nvx::print(std::cerr << "rhs: ", rhsbegcp, rhsend, " ") << '\n';
	throw err;
}





/******************* STANDART CONTAINUERS *******************/
/*
 * .begin(), .end()
 */
#define _NVX_ASSERT_EQ_SEQUENCES(seqname) \
	template<typename...Types, typename...Types2> \
	bool assert_eq( \
		seqname<Types...>  const &lhs, \
		seqname<Types2...> const &rhs, \
		char const *err = "Error", \
		bool silent = false \
	) \
	{ \
		return assert_eq(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), err, silent); \
	}


/*
 * .begin(), .end(), .insert()
 */
#define _NVX_ASSERT_EQ_UNORDERED_SEQUENCE_ONE_TARG(unord_name, ord_name) \
	template<typename T, typename...Other> \
	bool assert_eq( \
		unord_name<T, Other...> const &lhs, \
		unord_name<T, Other...> const &rhs, \
		char const *err = "Error", \
		bool silent = false \
	) \
	{ \
		ord_name<T> lhsset, rhsset; \
		std::copy(lhs.begin(), lhs.end(), std::insert_iterator(lhsset, lhsset.begin())); \
		std::copy(rhs.begin(), rhs.end(), std::insert_iterator(rhsset, rhsset.begin())); \
		return assert_eq(lhsset, rhsset, err, silent); \
	}

#define _NVX_ASSERT_EQ_UNORDERED_SEQUENCE_TWO_TARGS(unord_name, ord_name) \
	template<typename T, typename U, typename...Other> \
	bool assert_eq( \
		unord_name<T, U, Other...> const &lhs, \
		unord_name<T, U, Other...> const &rhs, \
		char const *err = "Error", \
		bool silent = false \
	) \
	{ \
		ord_name<T, U> lhsset, rhsset; \
		std::copy(lhs.begin(), lhs.end(), std::insert_iterator(lhsset, lhsset.begin())); \
		std::copy(rhs.begin(), rhs.end(), std::insert_iterator(rhsset, rhsset.begin())); \
		return assert_eq(lhsset, rhsset, err, silent); \
	}



_NVX_ASSERT_EQ_SEQUENCES(std::vector);
_NVX_ASSERT_EQ_SEQUENCES(std::list);

_NVX_ASSERT_EQ_SEQUENCES(std::set);
_NVX_ASSERT_EQ_SEQUENCES(std::map);
_NVX_ASSERT_EQ_SEQUENCES(std::multiset);
_NVX_ASSERT_EQ_SEQUENCES(std::multimap);

_NVX_ASSERT_EQ_UNORDERED_SEQUENCE_ONE_TARG(std::unordered_set,       std::set);
_NVX_ASSERT_EQ_UNORDERED_SEQUENCE_ONE_TARG(std::unordered_multiset,  std::multiset);
_NVX_ASSERT_EQ_UNORDERED_SEQUENCE_TWO_TARGS(std::unordered_map,      std::map);
_NVX_ASSERT_EQ_UNORDERED_SEQUENCE_TWO_TARGS(std::unordered_multimap, std::multimap);





}





#endif
