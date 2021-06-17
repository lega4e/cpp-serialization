#ifndef NVX_RANDOM_HPP
#define NVX_RANDOM_HPP

/*
 * Этот файл содержит псевдонимы типов для получения
 * псевдослучайных чисел
 */

#include <chrono>
#include <iterator>
#include <random>
#include <type_traits>





namespace nvx
{
	typedef std::default_random_engine dre_t;
}

extern nvx::dre_t dre;

#define NVX_DRE nvx::dre_t dre = nvx::create_dre();



namespace nvx
{






template<typename T>
using uidis_t = std::uniform_int_distribution<T>;

template<typename T>
using urdis_t = std::uniform_real_distribution<T>;

typedef uidis_t<int>       uidisI, disI;
typedef uidis_t<long long> uidisLL, disLL;
typedef urdis_t<float>     urdisF, disF;
typedef urdis_t<double>    urdisD, disD;

/*
 * Почему-то первое значение, генерируемое dre
 * всегда одно и то же независимо от семени
 */
inline dre_t create_dre()
{
	dre_t dre(std::chrono::system_clock::now().time_since_epoch().count());
	dre();
	return dre;
}



/*
 * Случайное число в заданном промежутке
 */
template<typename T, bool IsInt, bool IsFloat>
struct _rnd_dispatcher;

template<
	typename T,
	typename TNOREF = typename std::remove_reference<T>::type,
	bool IsInt      = std::is_integral<TNOREF>::value,
	bool IsFloat    = std::is_floating_point<TNOREF>::value
>
inline TNOREF rnd(T min, T max)
{
	return _rnd_dispatcher<TNOREF, IsInt, IsFloat>::_rnd(min, max);
}

template<typename T>
struct _rnd_dispatcher<T, true, false>
{
	static T _rnd(T min, T max)
	{
		return uidis_t<T>(min, max)(dre);
	}
};

template<typename T>
struct _rnd_dispatcher<T, false, true>
{
	static T _rnd(T min, T max)
	{
		return urdis_t<T>(min, max)(dre);
	}
};

template<typename Iterator>
inline auto choice_it(Iterator b, Iterator e)
{
	std::advance(b, rnd(0, (int)std::distance(b, e)-1));
	return b;
}





}






#endif
