#ifndef ASSERT_HPP_89732
#define ASSERT_HPP_89732

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





/********************* ASSERT FUNCTIONS *********************/
template<typename T, typename U>
bool is_equal(T const &lhs, U const &rhs, char const *err, bool silent = false)
{
	if (silent)
		return lhs == rhs;

	if (lhs != rhs)
	{
		std::cerr << "lhs: " << lhs << ", " << rhs << '\n';
		throw err;
	}
	return true;
}

inline bool is_equal(
	std::wstring const &lhs,
	std::wstring const &rhs,
	char const *err,
	bool silent = false
)
{
	if (silent)
		return lhs == rhs;

	if (lhs != rhs)
	{
		// std::wcerr << "lhs: " << ws2s(lhs) << ", " << ws2s(rhs) << '\n';
		throw err;
	}
	return true;
}

// for sequence
template<typename IteratorLhs, typename IteratorRhs>
bool is_equal(
	IteratorLhs lhsbeg, IteratorLhs lhsend, 
	IteratorRhs rhsbeg, IteratorRhs rhsend, 
	char const *err, bool silent = false
)
{
	auto lhsbegcp = lhsbeg;
	auto rhsbegcp = rhsbeg;

	while (lhsbeg != lhsend && rhsbeg != rhsend)
	{
		if (!is_equal(*lhsbeg, *rhsbeg, err, true))
			goto fail_label;
		++lhsbeg, ++rhsbeg;
	}

	if (lhsbeg != lhsend || rhsbeg != rhsend)
		goto fail_label;

	return true;

fail_label:
	if (silent)
		return false;

	nvx::print(std::cerr << "lhs: ", lhsbegcp, lhsend) << '\n';
	nvx::print(std::cerr << "rhs: ", rhsbegcp, rhsend) << '\n';
	throw err;
}



template<typename T>
bool is_equal(
	std::vector<T> const &lhs,
	std::vector<T> const &rhs,
	char const *err,
	bool silent = false
)
{
	return is_equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), err, silent);
}

template<typename T>
bool is_equal(
	std::list<T> const &lhs,
	std::list<T> const &rhs,
	char const *err,
	bool silent = false
)
{
	return is_equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), err, silent);
}

template<typename T>
bool is_equal(
	std::set<T> const &lhs,
	std::set<T> const &rhs,
	char const *err,
	bool silent = false
)
{
	return is_equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), err, silent);
}

template<typename T, typename U>
bool is_equal(
	std::map<T, U> const &lhs,
	std::map<T, U> const &rhs,
	char const *err, bool silent = false
)
{
	return is_equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), err, silent);
}

template<typename T>
bool is_equal(
	std::multiset<T> const &lhs,
	std::multiset<T> const &rhs,
	char const *err,
	bool silent = false
)
{
	return is_equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), err, silent);
}

template<typename T, typename U>
bool is_equal(
	std::multimap<T, U> const &lhs,
	std::multimap<T, U> const &rhs,
	char const *err, bool silent = false
)
{
	return is_equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), err, silent);
}

template<typename T>
bool is_equal(
	std::unordered_set<T> const &lhs,
	std::unordered_set<T> const &rhs,
	char const *err, bool silent = false
)
{
	std::set<T> lhsset, rhsset;
	std::copy(lhs.begin(), lhs.end(), std::insert_iterator(lhsset, lhsset.begin()));
	std::copy(rhs.begin(), rhs.end(), std::insert_iterator(rhsset, rhsset.begin()));
	return is_equal(lhsset, rhsset, err, silent);
}

template<typename T, typename U>
bool is_equal(
	std::unordered_map<T, U> const &lhs,
	std::unordered_map<T, U> const &rhs,
	char const *err, bool silent = false
)
{
	std::map<T, U> lhsmap, rhsmap;
	std::copy(lhs.begin(), lhs.end(), std::insert_iterator(lhsmap, lhsmap.begin()));
	std::copy(rhs.begin(), rhs.end(), std::insert_iterator(rhsmap, rhsmap.begin()));
	return is_equal(lhsmap, rhsmap, err, silent);
}

template<typename T>
bool is_equal(
	std::unordered_multiset<T> const &lhs,
	std::unordered_multiset<T> const &rhs,
	char const *err, bool silent = false
)
{
	std::multiset<T> lhsmultiset, rhsmultiset;
	std::copy(lhs.begin(), lhs.end(), std::insert_iterator(lhsmultiset, lhsmultiset.begin()));
	std::copy(rhs.begin(), rhs.end(), std::insert_iterator(rhsmultiset, rhsmultiset.begin()));
	return is_equal(lhsmultiset, rhsmultiset, err, silent);
}

template<typename T, typename U>
bool is_equal(
	std::unordered_multimap<T, U> const &lhs,
	std::unordered_multimap<T, U> const &rhs,
	char const *err, bool silent = false
)
{
	std::multimap<T, U> lhsmultimap, rhsmultimap;
	std::copy(lhs.begin(), lhs.end(), std::insert_iterator(lhsmultimap, lhsmultimap.begin()));
	std::copy(rhs.begin(), rhs.end(), std::insert_iterator(rhsmultimap, rhsmultimap.begin()));
	return is_equal(lhsmultimap, rhsmultimap, err, silent);
}





#endif
