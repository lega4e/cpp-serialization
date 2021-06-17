#ifndef NVX_TYPE_HPP
#define NVX_TYPE_HPP

#include <ctype.h>
#include <limits>
#include <memory>



namespace nvx
{
	/*
	 * Numbers
	 */
	typedef int8_t      byte;
	typedef uint8_t     ubyte;
	typedef uint16_t    ushort;

	typedef uint32_t    uint;
	typedef int64_t     llong;
	typedef uint64_t    ullong;

	typedef long double ldouble;


	constexpr char    const char_max     = std::numeric_limits<char>::max();
	constexpr char    const char_min     = std::numeric_limits<char>::min();

	constexpr short   const short_max    = std::numeric_limits<short>::max();
	constexpr short   const short_min    = std::numeric_limits<short>::min();

	constexpr int     const int_inf      = std::numeric_limits<int>::max()    / 3;
	constexpr int     const int_max      = std::numeric_limits<int>::max();
	constexpr int     const int_min      = std::numeric_limits<int>::min();

	constexpr long    const long_inf     = std::numeric_limits<long>::max()   / 3;
	constexpr long    const long_max     = std::numeric_limits<long>::max();
	constexpr long    const long_min     = std::numeric_limits<long>::min();

	constexpr llong   const llong_inf    = std::numeric_limits<llong>::max()  / 3;
	constexpr llong   const llong_max    = std::numeric_limits<llong>::max();
	constexpr llong   const llong_min    = std::numeric_limits<llong>::min();

	constexpr uint    const uint_inf     = std::numeric_limits<uint>::max()   / 3;
	constexpr uint    const uint_max     = std::numeric_limits<uint>::max();
	constexpr uint    const uint_min     = std::numeric_limits<uint>::min();

	constexpr ulong   const ulong_inf    = std::numeric_limits<ulong>::max()  / 3;
	constexpr ulong   const ulong_max    = std::numeric_limits<ulong>::max();
	constexpr ulong   const ulong_min    = std::numeric_limits<ulong>::min();

	constexpr ullong  const ullong_inf   = std::numeric_limits<ullong>::max() / 3;
	constexpr ullong  const ullong_max   = std::numeric_limits<ullong>::max();
	constexpr ullong  const ullong_min   = std::numeric_limits<ullong>::min();

	constexpr float   const float_inf    = std::numeric_limits<float>::infinity();
	constexpr float   const float_nan    = std::numeric_limits<float>::quiet_NaN();
	constexpr float   const float_snan   = std::numeric_limits<float>::signaling_NaN();

	constexpr double  const double_inf   = std::numeric_limits<double>::infinity();
	constexpr double  const double_nan   = std::numeric_limits<double>::quiet_NaN();
	constexpr double  const double_snan  = std::numeric_limits<double>::signaling_NaN();

	constexpr ldouble const ldouble_inf  = std::numeric_limits<ldouble>::infinity();
	constexpr ldouble const ldouble_nan  = std::numeric_limits<ldouble>::quiet_NaN();
	constexpr ldouble const ldouble_snan = std::numeric_limits<ldouble>::signaling_NaN();




	/*
	 * Pointers
	 */
	typedef std::shared_ptr<byte>    byte_p;
	typedef std::shared_ptr<ubyte>   ubyte_p;
	typedef std::shared_ptr<ushort>  ushort_p;
	typedef std::shared_ptr<int>     int_p;
	typedef std::shared_ptr<uint>    uint_p;
	typedef std::shared_ptr<llong>   llong_p;
	typedef std::shared_ptr<ullong>  ullong_p;
	typedef std::shared_ptr<float>   float_p;
	typedef std::shared_ptr<double>  double_p;
	typedef std::shared_ptr<ldouble> ldouble_p;
}





#endif
