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





constexpr int const MIN_COUNT = 1;
constexpr int const MAX_COUNT = 100;



template<typename T, bool IsInt, bool IsFloat, bool IsPoitner>
struct _random_dispatcher;

template<
	typename T,
	bool IsInt = std::is_integral<T>::value,
	bool IsFloat = std::is_floating_point<T>::value,
	bool IsPointer = std::is_pointer<T>::value
>
T random()
{
	return _random_dispatcher<T, IsInt, IsFloat, IsPointer>::_random();
}





/********************* FUNDAMENTAL TYEPS ********************/
template<typename T>
struct _random_dispatcher<T *, false, false, true>
{
	static T *_random()
	{
		return new T(random<T>());
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
		return std::pair<T, U>( random<T>(), random<U>() );
	}
};





/************************** STRINGS *************************/
template<>
std::string random<std::string>()
{
	std::string s;
	s.resize(disI(MIN_COUNT, MAX_COUNT)(dre));

	for (auto b = s.begin(), e = s.end(); b != e; ++b)
		*b = disI(0, 1)(dre) ? disI('a', 'z')(dre) : disI('A', 'Z')(dre);

	return s;
}

template<>
std::wstring random<std::wstring>()
{
	std::wstring s;
	s.resize(disI(MIN_COUNT, MAX_COUNT)(dre));

	for (auto b = s.begin(), e = s.end(); b != e; ++b)
		*b = disI(0, 1)(dre) ? disI(L'a', L'z')(dre) :
		     disI(0, 1)(dre) ? disI(L'A', L'Z')(dre) :
			 disI(0, 1)(dre) ? disI(L'а', L'я')(dre) :
			 disI(L'А', L'Я')(dre);

	return s;
}





/********************** STD CONTAINERS **********************/
// help
template<typename T, typename Cont>
void insert_random_values(Cont &cont, int count)
{
	for (int i = 0; i < count; ++i)
		cont.insert(random<T>());
	return;
}

template<typename T, typename Cont>
void push_back_random_values(Cont &cont, int count)
{
	for (int i = 0; i < count; ++i)
		cont.push_back(random<T>());
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
