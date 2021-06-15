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



template<typename T>
bool assert_eq(
	std::vector<T> const &lhs,
	std::vector<T> const &rhs,
	char const *err = "Error",
	bool silent = false
)
{
	return assert_eq(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), err, silent);
}

template<typename T>
bool assert_eq(
	std::list<T> const &lhs,
	std::list<T> const &rhs,
	char const *err = "Error",
	bool silent = false
)
{
	return assert_eq(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), err, silent);
}

template<typename T>
bool assert_eq(
	std::set<T> const &lhs,
	std::set<T> const &rhs,
	char const *err = "Error",
	bool silent = false
)
{
	return assert_eq(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), err, silent);
}

template<typename T, typename U>
bool assert_eq(
	std::map<T, U> const &lhs,
	std::map<T, U> const &rhs,
	char const *err = "Error", bool silent = false
)
{
	return assert_eq(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), err, silent);
}

template<typename T>
bool assert_eq(
	std::multiset<T> const &lhs,
	std::multiset<T> const &rhs,
	char const *err = "Error",
	bool silent = false
)
{
	return assert_eq(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), err, silent);
}

template<typename T, typename U>
bool assert_eq(
	std::multimap<T, U> const &lhs,
	std::multimap<T, U> const &rhs,
	char const *err = "Error", bool silent = false
)
{
	return assert_eq(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), err, silent);
}

template<typename T>
bool assert_eq(
	std::unordered_set<T> const &lhs,
	std::unordered_set<T> const &rhs,
	char const *err = "Error", bool silent = false
)
{
	std::set<T> lhsset, rhsset;
	std::copy(lhs.begin(), lhs.end(), std::insert_iterator(lhsset, lhsset.begin()));
	std::copy(rhs.begin(), rhs.end(), std::insert_iterator(rhsset, rhsset.begin()));
	return assert_eq(lhsset, rhsset, err, silent);
}

template<typename T, typename U>
bool assert_eq(
	std::unordered_map<T, U> const &lhs,
	std::unordered_map<T, U> const &rhs,
	char const *err = "Error", bool silent = false
)
{
	std::map<T, U> lhsmap, rhsmap;
	std::copy(lhs.begin(), lhs.end(), std::insert_iterator(lhsmap, lhsmap.begin()));
	std::copy(rhs.begin(), rhs.end(), std::insert_iterator(rhsmap, rhsmap.begin()));
	return assert_eq(lhsmap, rhsmap, err, silent);
}

template<typename T>
bool assert_eq(
	std::unordered_multiset<T> const &lhs,
	std::unordered_multiset<T> const &rhs,
	char const *err = "Error", bool silent = false
)
{
	std::multiset<T> lhsmultiset, rhsmultiset;
	std::copy(lhs.begin(), lhs.end(), std::insert_iterator(lhsmultiset, lhsmultiset.begin()));
	std::copy(rhs.begin(), rhs.end(), std::insert_iterator(rhsmultiset, rhsmultiset.begin()));
	return assert_eq(lhsmultiset, rhsmultiset, err, silent);
}

template<typename T, typename U>
bool assert_eq(
	std::unordered_multimap<T, U> const &lhs,
	std::unordered_multimap<T, U> const &rhs,
	char const *err = "Error", bool silent = false
)
{
	std::multimap<T, U> lhsmultimap, rhsmultimap;
	std::copy(lhs.begin(), lhs.end(), std::insert_iterator(lhsmultimap, lhsmultimap.begin()));
	std::copy(rhs.begin(), rhs.end(), std::insert_iterator(rhsmultimap, rhsmultimap.begin()));
	return assert_eq(lhsmultimap, rhsmultimap, err, silent);
}







}





#endif
