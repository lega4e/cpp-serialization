#ifndef NVX_RANDOM_VALUES_HPP_83782019
#define NVX_RANDOM_VALUES_HPP_83782019

#include <limits>
#include <list>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <nvx/random.hpp>





namespace nvx
{





constexpr int const MIN_COUNT = 2;
constexpr int const MAX_COUNT = 5;

template<typename T, bool IsInt, bool IsFloat, bool IsPoitner>
struct _random_dispatcher;





/*********************** MAIN FUNCTION **********************/
template<
	typename T,
	typename TNOREF = typename std::remove_reference<T>::type,
	bool IsInt      = std::is_integral<TNOREF>::value,
	bool IsFloat    = std::is_floating_point<TNOREF>::value,
	bool IsPointer  = std::is_pointer<TNOREF>::value
>
TNOREF random_value()
{
	return nvx::_random_dispatcher<
		TNOREF, IsInt, IsFloat, IsPointer
	>::_random();
}





/************************** STRINGS *************************/
template<>
inline std::string random_value<std::string>()
{
	std::string s;
	s.resize(nvx::disI(nvx::MIN_COUNT, nvx::MAX_COUNT)(dre));

	for (auto b = s.begin(), e = s.end(); b != e; ++b)
		*b = nvx::disI(0, 1)(dre) ? nvx::disI('a', 'z')(dre) : nvx::disI('A', 'Z')(dre);

	return s;
}

template<>
inline std::wstring random_value<std::wstring>()
{
	std::wstring s;
	s.resize(nvx::disI(nvx::MIN_COUNT, nvx::MAX_COUNT)(dre));

	for (auto b = s.begin(), e = s.end(); b != e; ++b)
		*b = nvx::disI(0, 1)(dre) ? nvx::disI(L'a', L'z')(dre) :
		     nvx::disI(0, 1)(dre) ? nvx::disI(L'A', L'Z')(dre) :
			 nvx::disI(0, 1)(dre) ? nvx::disI(L'а', L'я')(dre) :
			 nvx::disI(L'А', L'Я')(dre);

	return s;
}


template<typename Iterator>
void randomize(Iterator b, Iterator e);

template<typename T>
T &randomize(T &obj)
{
	return obj = random_value<T>();
}


template<
	typename T,
	typename TNOREF = typename std::remove_reference<T>::type,
	bool IsInt      = std::is_integral<TNOREF>::value,
	bool IsFloat    = std::is_floating_point<TNOREF>::value,
	bool IsPointer  = std::is_pointer<TNOREF>::value
>
TNOREF *random_array(int &size)
{
	size = disI(MIN_COUNT, MAX_COUNT)(dre);
	TNOREF *ar = new TNOREF[size];
	randomize(ar, ar+size);
	return ar;
}





/********************* FUNDAMENTAL TYEPS ********************/
template<typename T>
struct _random_dispatcher<T *, false, false, true>
{
	static T *_random()
	{
		return new T(random_value<T>());
	}
};

template<typename T>
struct _random_dispatcher<T, true, false, false>
{
	static T _random()
	{
		return uidis_t<T>(
			std::numeric_limits<T>::min(),
			std::numeric_limits<T>::max()
		)(dre);
	}
};

template<typename T>
struct _random_dispatcher<T, false, true, false>
{
	static T _random()
	{
		return urdis_t<T>()(dre);
	}
};

template<typename T, typename U>
struct _random_dispatcher<std::pair<T, U>, false, false, false>
{
	static std::pair<T, U> _random()
	{
		return std::pair<T, U>( random_value<T>(), random_value<U>() );
	}
};





/************************* SEQUENCE *************************/
template<typename Iterator>
void randomize(Iterator b, Iterator e)
{
	while (b != e)
	{
		*b = random_value<decltype(*b)>();
		++b;
	}

	return;
}





/********************** STD CONTAINERS **********************/
// help
template<typename T, typename Cont>
void insert_random_values(Cont &cont, int count)
{
	for (int i = 0; i < count; ++i)
		cont.insert(random_value<T>());
	return;
}

template<typename T, typename Cont>
void push_back_random_values(Cont &cont, int count)
{
	for (int i = 0; i < count; ++i)
		cont.push_back(random_value<T>());
	return;
}



template<typename T>
struct _random_dispatcher<std::vector<T>, false, false, false>
{
	static std::vector<T> _random()
	{
		std::vector<T> cont;
		push_back_random_values<T>(cont, disI(MIN_COUNT, MAX_COUNT)(dre));
		return cont;
	}
};

template<typename T>
struct _random_dispatcher<std::list<T>, false, false, false>
{
	static std::list<T> _random()
	{
		std::list<T> cont;
		push_back_random_values<T>(cont, disI(MIN_COUNT, MAX_COUNT)(dre));
		return cont;
	}
};



template<typename T>
struct _random_dispatcher<std::set<T>, false, false, false>
{
	static std::set<T> _random()
	{
		std::set<T> cont;
		insert_random_values<T>(cont, disI(MIN_COUNT, MAX_COUNT)(dre));
		return cont;
	}
};

template<typename T>
struct _random_dispatcher<std::unordered_set<T>, false, false, false>
{
	static std::unordered_set<T> _random()
	{
		std::unordered_set<T> cont;
		insert_random_values<T>(cont, disI(MIN_COUNT, MAX_COUNT)(dre));
		return cont;
	}
};


template<typename T, typename U>
struct _random_dispatcher<std::map<T, U>, false, false, false>
{
	static std::map<T, U> _random()
	{
		std::map<T, U> cont;
		insert_random_values<std::pair<T, U>>(cont, disI(MIN_COUNT, MAX_COUNT)(dre));
		return cont;
	}
};

template<typename T, typename U>
struct _random_dispatcher<std::unordered_map<T, U>, false, false, false>
{
	static std::unordered_map<T, U> _random()
	{
		std::unordered_map<T, U> cont;
		insert_random_values<std::pair<T, U>>(cont, disI(MIN_COUNT, MAX_COUNT)(dre));
		return cont;
	}
};



// multi*
template<typename T>
struct _random_dispatcher<std::multiset<T>, false, false, false>
{
	static std::multiset<T> _random()
	{
		std::multiset<T> cont;
		insert_random_values<T>(cont, disI(MIN_COUNT, MAX_COUNT)(dre));
		return cont;
	}
};

template<typename T>
struct _random_dispatcher<std::unordered_multiset<T>, false, false, false>
{
	static std::unordered_multiset<T> _random()
	{
		std::unordered_multiset<T> cont;
		insert_random_values<T>(cont, disI(MIN_COUNT, MAX_COUNT)(dre));
		return cont;
	}
};


template<typename T, typename U>
struct _random_dispatcher<std::multimap<T, U>, false, false, false>
{
	static std::multimap<T, U> _random()
	{
		std::multimap<T, U> cont;
		insert_random_values<std::pair<T, U>>(cont, disI(MIN_COUNT, MAX_COUNT)(dre));
		return cont;
	}
};

template<typename T, typename U>
struct _random_dispatcher<std::unordered_multimap<T, U>, false, false, false>
{
	static std::unordered_multimap<T, U> _random()
	{
		std::unordered_multimap<T, U> cont;
		insert_random_values<std::pair<T, U>>(cont, disI(MIN_COUNT, MAX_COUNT)(dre));
		return cont;
	}
};





}





#endif
