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

template<typename T, bool IsNumber, bool IsPoitner, typename...Args>
struct _random_dispatcher;

template<typename T, bool IsNumber, bool IsPoitner, typename...Args>
struct _random_pointer_dispatcher;





/*********************** MAIN FUNCTION **********************/
template<
	typename T,
	typename TNOREF = typename std::remove_reference<T>::type,
	bool IsNumber   = std::is_integral<TNOREF>::value ||
	                  std::is_floating_point<TNOREF>::value,
	bool IsPointer  = std::is_pointer<TNOREF>::value,
	typename...Args
>
TNOREF random_value(Args... args)
{
	return nvx::_random_pointer_dispatcher<
		TNOREF, IsNumber, IsPointer, Args...
	>::_random(args...);
}





/********************* RANDOMIZE, ARRAY *********************/


template<typename Iterator, typename...Args>
void randomize(Iterator b, Iterator e, Args...args)
{
	while (b != e)
	{
		*b = random_value<decltype(*b)>(args...);
		++b;
	}

	return;
}

template<typename T, typename...Args>
T &randomize(T &obj, Args...args)
{
	return obj = random_value<T>(args...);
}


template<
	typename T,
	typename TNOREF = typename std::remove_reference<T>::type,
	bool IsNumber   = std::is_integral<TNOREF>::value ||
	                  std::is_floating_point<TNOREF>::value,
	bool IsPointer  = std::is_pointer<TNOREF>::value,
	typename...Args
>
TNOREF *random_array(int &size, Args...args)
{
	size = disI(MIN_COUNT, MAX_COUNT)(dre);
	TNOREF *ar = new TNOREF[size];
	randomize(ar, ar+size, args...);
	return ar;
}







/* DISPATCHER */

/********************* FUNDAMENTAL TYEPS ********************/
// pointer
template<typename T, typename...Args>
struct _random_dispatcher<T *, false, true, Args...>
{
	static T *_random(Args...args)
	{
		return new T(random_value<T>(args...));
	}
};



// number
template<typename T>
struct _random_dispatcher<T, true, false, T, T>
{
	static T _random(T min, T max)
	{
		return rnd<T>(min, max);
	}
};

template<typename T>
struct _random_dispatcher<T, true, false>
{
	static T _random()
	{
		return rnd<T>(
			std::numeric_limits<T>::min(),
			std::numeric_limits<T>::max()
		);
	}
};



// common
template<typename T, typename...Args>
struct _random_dispatcher<T, false, false, Args...>
{
	static T _random(Args...args)
	{
		return T::random_value(args...);
	}
};


template<typename T, bool IsNumber, bool IsPointer, typename...Args>
struct _random_pointer_dispatcher
{
	static T _random(Args...args)
	{
		return _random_dispatcher<T, IsNumber, IsPointer, Args...>::_random(args...);
	}
};





/**************************** STD ***************************/
template<typename T, typename U, typename...Args>
struct _random_dispatcher<std::pair<T, U>, false, false, Args...>
{
	static std::pair<T, U> _random(Args...args)
	{
		return std::pair<T, U>( random_value<T>(args...), random_value<U>(args...) );
	}
};

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





/********************** STD CONTAINERS **********************/
// help
template<typename T, typename Cont, typename...Args>
void insert_random_values(Cont &cont, int count, Args...args)
{
	for (int i = 0; i < count; ++i)
		cont.insert(random_value<T>(args...));
	return;
}

template<typename T, typename Cont, typename...Args>
void push_back_random_values(Cont &cont, int count, Args...args)
{
	for (int i = 0; i < count; ++i)
		cont.push_back(random_value<T>(args...));
	return;
}



// macros
#define _NVX_RAND_ARG(...) __VA_ARGS__

#define _NVX_RANDOM_DISPATCHER_CONTAINER_CORE( \
	contname, types1, types2, \
	funtype, addfunction, \
	min_count, max_count \
) \
	template<types1> \
	struct _random_dispatcher<contname<types2>, false, false> \
	{ \
		static contname<types2> _random() \
		{ \
			contname<types2> cont; \
			addfunction<funtype>(cont, rnd(min_count, max_count)); \
			return cont; \
		} \
	}; \
	\
	template<types1, typename...Args> \
	struct _random_dispatcher<contname<types2>, false, false, int, int, Args...> \
	{ \
		static contname<types2> _random(int min, int max, Args...args) \
		{ \
			contname<types2> cont; \
			addfunction<funtype>(cont, rnd(min, max), args...); \
			return cont; \
		} \
	};

#define _NVX_RANDOM_DISPATCHER_PUSH_BACK_CONTAINER(contname, min_count, max_count) \
	_NVX_RANDOM_DISPATCHER_CONTAINER_CORE( \
		contname, typename T, \
		T, T, push_back_random_values, \
		min_count, max_count \
	)

#define _NVX_RANDOM_DISPATCHER_INSERT_CONTAINER(contname, min_count, max_count) \
	_NVX_RANDOM_DISPATCHER_CONTAINER_CORE( \
		contname, typename T, \
		T, T, insert_random_values, \
		min_count, max_count \
	)

#define _NVX_RANDOM_DISPATCHER_INSERT_PAIR_CONTAINER(contname, min_count, max_count) \
	_NVX_RANDOM_DISPATCHER_CONTAINER_CORE( \
		contname, \
		_NVX_RAND_ARG(typename T, typename U), \
		_NVX_RAND_ARG(T, U), \
		_NVX_RAND_ARG(std::pair<T, U>), \
		insert_random_values, \
		min_count, max_count \
	)



// definitions random dispatchers
_NVX_RANDOM_DISPATCHER_PUSH_BACK_CONTAINER(   std::vector,             MIN_COUNT, MAX_COUNT );
_NVX_RANDOM_DISPATCHER_PUSH_BACK_CONTAINER(   std::list,               MIN_COUNT, MAX_COUNT );

_NVX_RANDOM_DISPATCHER_INSERT_CONTAINER(      std::set,                MIN_COUNT, MAX_COUNT );
_NVX_RANDOM_DISPATCHER_INSERT_CONTAINER(      std::unordered_set,      MIN_COUNT, MAX_COUNT );
_NVX_RANDOM_DISPATCHER_INSERT_CONTAINER(      std::multiset,           MIN_COUNT, MAX_COUNT );
_NVX_RANDOM_DISPATCHER_INSERT_CONTAINER(      std::unordered_multiset, MIN_COUNT, MAX_COUNT );

_NVX_RANDOM_DISPATCHER_INSERT_PAIR_CONTAINER( std::map,                MIN_COUNT, MAX_COUNT );
_NVX_RANDOM_DISPATCHER_INSERT_PAIR_CONTAINER( std::unordered_map,      MIN_COUNT, MAX_COUNT );
_NVX_RANDOM_DISPATCHER_INSERT_PAIR_CONTAINER( std::multimap,           MIN_COUNT, MAX_COUNT );
_NVX_RANDOM_DISPATCHER_INSERT_PAIR_CONTAINER( std::unordered_multimap, MIN_COUNT, MAX_COUNT );





}





#endif
