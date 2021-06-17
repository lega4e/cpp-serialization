#ifndef NVX_METATYPE_HPP
#define NVX_METATYPE_HPP

#include <nvx/type.hpp>





namespace nvx
{





// ------------- МЕТОПРОГРАММИРОВАНИЕ ---------------
template<typename T> struct Type
{
	typedef T value;
};



template<bool Cond, typename T, typename U>
struct IF
{
	typedef T type;
};

template<typename T, typename U>
struct IF<false, T, U>
{
	typedef U type;
};





// ----------------- КАСКАДЫ ------------------------
/*
 * bcascade находит f(a, f(b, f(c, ...)))
 * ucascade вызывает f(a), f(b), f(c)...
 * dcascade находит f(f2(a), f(f2(b), f(f2(c), ...)))
 */
template<typename F, typename T, typename U>
inline auto bcascade(F f, T t, U u)
{
	return f(t, u);
}

template<typename F, typename T, typename...Args>
inline auto bcascade(F f, T t, Args...args)
{
	return f(t, bcascade(f, args...));
}


template<typename F, typename T>
inline void ucascade(F f, T t)
{
	f(t);
	return;
}

template<typename F, typename T, typename...Args>
inline void ucascade(F f, T t, Args...args)
{
	f(t);
	ucascade(f, args...);
	return;
}


template<typename F, typename F2, typename T, typename U>
inline T dcascade(F f, F2 f2, T t, U u)
{
	return f(f2(t), f2(u));
}

template<typename F, typename F2, typename T, typename...Args>
inline T dcascade(F f, F2 f2, T t, Args...args)
{
	return f( f2(t), f2(dcascade(f, f2, args...)) );
}





// -------------------- СТРОКИ ------------------------
template<char...Chars>
struct String
{
	static constexpr char value[] = { Chars..., '\0' };
};

template< typename S, char ch >
struct PushChar;

template< char ch, char...Chars >
struct PushChar< nvx::String<Chars...>, ch >
{
	typedef nvx::String<Chars..., ch> string;
};

template<int N>
constexpr char CharAt(char const (&s)[N], int i)
{
	return s[i];
}



#define NVX_STR1(str) \
	typename nvx::PushChar< nvx::String<>, nvx::CharAt(str, 0) >::string

#define NVX_STR2(str) \
	typename nvx::PushChar< NVX_STR1(str), nvx::CharAt(str, 1) >::string

#define NVX_STR3(str) \
	typename nvx::PushChar< NVX_STR2(str), nvx::CharAt(str, 2) >::string

#define NVX_STR4(str) \
	typename nvx::PushChar< NVX_STR3(str), nvx::CharAt(str, 3) >::string

#define NVX_STR5(str) \
	typename nvx::PushChar< NVX_STR4(str), nvx::CharAt(str, 4) >::string

#define NVX_STR6(str) \
	typename nvx::PushChar< NVX_STR5(str), nvx::CharAt(str, 5) >::string

#define NVX_STR7(str) \
	typename nvx::PushChar< NVX_STR6(str), nvx::CharAt(str, 6) >::string

#define NVX_STR8(str) \
	typename nvx::PushChar< NVX_STR7(str), nvx::CharAt(str, 7) >::string

#define NVX_STR9(str) \
	typename nvx::PushChar< NVX_STR8(str), nvx::CharAt(str, 8) >::string

#define NVX_STR10(str) \
	typename nvx::PushChar< NVX_STR9(str), nvx::CharAt(str, 9) >::string

#define NVX_STR11(str) \
	typename nvx::PushChar< NVX_STR10(str), nvx::CharAt(str, 10) >::string

#define NVX_STR12(str) \
	typename nvx::PushChar< NVX_STR11(str), nvx::CharAt(str, 11) >::string

#define NVX_STR13(str) \
	typename nvx::PushChar< NVX_STR12(str), nvx::CharAt(str, 12) >::string

#define NVX_STR14(str) \
	typename nvx::PushChar< NVX_STR13(str), nvx::CharAt(str, 13) >::string

#define NVX_STR15(str) \
	typename nvx::PushChar< NVX_STR14(str), nvx::CharAt(str, 14) >::string

#define NVX_STR16(str) \
	typename nvx::PushChar< NVX_STR15(str), nvx::CharAt(str, 15) >::string

#define NVX_STR17(str) \
	typename nvx::PushChar< NVX_STR16(str), nvx::CharAt(str, 16) >::string

#define NVX_STR18(str) \
	typename nvx::PushChar< NVX_STR17(str), nvx::CharAt(str, 17) >::string

#define NVX_STR19(str) \
	typename nvx::PushChar< NVX_STR18(str), nvx::CharAt(str, 18) >::string

#define NVX_STR20(str) \
	typename nvx::PushChar< NVX_STR19(str), nvx::CharAt(str, 19) >::string

#define NVX_STR21(str) \
	typename nvx::PushChar< NVX_STR20(str), nvx::CharAt(str, 20) >::string

#define NVX_STR22(str) \
	typename nvx::PushChar< NVX_STR21(str), nvx::CharAt(str, 21) >::string

#define NVX_STR23(str) \
	typename nvx::PushChar< NVX_STR22(str), nvx::CharAt(str, 22) >::string

#define NVX_STR24(str) \
	typename nvx::PushChar< NVX_STR23(str), nvx::CharAt(str, 23) >::string

#define NVX_STR25(str) \
	typename nvx::PushChar< NVX_STR24(str), nvx::CharAt(str, 24) >::string

#define NVX_STR26(str) \
	typename nvx::PushChar< NVX_STR25(str), nvx::CharAt(str, 25) >::string

#define NVX_STR27(str) \
	typename nvx::PushChar< NVX_STR26(str), nvx::CharAt(str, 26) >::string

#define NVX_STR28(str) \
	typename nvx::PushChar< NVX_STR27(str), nvx::CharAt(str, 27) >::string

#define NVX_STR29(str) \
	typename nvx::PushChar< NVX_STR28(str), nvx::CharAt(str, 28) >::string

#define NVX_STR30(str) \
	typename nvx::PushChar< NVX_STR29(str), nvx::CharAt(str, 29) >::string

#define NVX_STR31(str) \
	typename nvx::PushChar< NVX_STR30(str), nvx::CharAt(str, 30) >::string

#define NVX_STR32(str) \
	typename nvx::PushChar< NVX_STR31(str), nvx::CharAt(str, 31) >::string

#define NVX_STR33(str) \
	typename nvx::PushChar< NVX_STR32(str), nvx::CharAt(str, 32) >::string

#define NVX_STR34(str) \
	typename nvx::PushChar< NVX_STR33(str), nvx::CharAt(str, 33) >::string

#define NVX_STR35(str) \
	typename nvx::PushChar< NVX_STR34(str), nvx::CharAt(str, 34) >::string

#define NVX_STR36(str) \
	typename nvx::PushChar< NVX_STR35(str), nvx::CharAt(str, 35) >::string

#define NVX_STR37(str) \
	typename nvx::PushChar< NVX_STR36(str), nvx::CharAt(str, 36) >::string

#define NVX_STR38(str) \
	typename nvx::PushChar< NVX_STR37(str), nvx::CharAt(str, 37) >::string

#define NVX_STR39(str) \
	typename nvx::PushChar< NVX_STR38(str), nvx::CharAt(str, 38) >::string

#define NVX_STR40(str) \
	typename nvx::PushChar< NVX_STR39(str), nvx::CharAt(str, 39) >::string

#define NVX_STR41(str) \
	typename nvx::PushChar< NVX_STR40(str), nvx::CharAt(str, 40) >::string

#define NVX_STR42(str) \
	typename nvx::PushChar< NVX_STR41(str), nvx::CharAt(str, 41) >::string

#define NVX_STR43(str) \
	typename nvx::PushChar< NVX_STR42(str), nvx::CharAt(str, 42) >::string

#define NVX_STR44(str) \
	typename nvx::PushChar< NVX_STR43(str), nvx::CharAt(str, 43) >::string

#define NVX_STR45(str) \
	typename nvx::PushChar< NVX_STR44(str), nvx::CharAt(str, 44) >::string

#define NVX_STR46(str) \
	typename nvx::PushChar< NVX_STR45(str), nvx::CharAt(str, 45) >::string

#define NVX_STR47(str) \
	typename nvx::PushChar< NVX_STR46(str), nvx::CharAt(str, 46) >::string

#define NVX_STR48(str) \
	typename nvx::PushChar< NVX_STR47(str), nvx::CharAt(str, 47) >::string

#define NVX_STR49(str) \
	typename nvx::PushChar< NVX_STR48(str), nvx::CharAt(str, 48) >::string

#define NVX_STR50(str) \
	typename nvx::PushChar< NVX_STR49(str), nvx::CharAt(str, 49) >::string

#define NVX_STR51(str) \
	typename nvx::PushChar< NVX_STR50(str), nvx::CharAt(str, 50) >::string

#define NVX_STR52(str) \
	typename nvx::PushChar< NVX_STR51(str), nvx::CharAt(str, 51) >::string

#define NVX_STR53(str) \
	typename nvx::PushChar< NVX_STR52(str), nvx::CharAt(str, 52) >::string

#define NVX_STR54(str) \
	typename nvx::PushChar< NVX_STR53(str), nvx::CharAt(str, 53) >::string

#define NVX_STR55(str) \
	typename nvx::PushChar< NVX_STR54(str), nvx::CharAt(str, 54) >::string

#define NVX_STR56(str) \
	typename nvx::PushChar< NVX_STR55(str), nvx::CharAt(str, 55) >::string

#define NVX_STR57(str) \
	typename nvx::PushChar< NVX_STR56(str), nvx::CharAt(str, 56) >::string

#define NVX_STR58(str) \
	typename nvx::PushChar< NVX_STR57(str), nvx::CharAt(str, 57) >::string

#define NVX_STR59(str) \
	typename nvx::PushChar< NVX_STR58(str), nvx::CharAt(str, 58) >::string

#define NVX_STR60(str) \
	typename nvx::PushChar< NVX_STR59(str), nvx::CharAt(str, 59) >::string

#define NVX_STR61(str) \
	typename nvx::PushChar< NVX_STR60(str), nvx::CharAt(str, 60) >::string

#define NVX_STR62(str) \
	typename nvx::PushChar< NVX_STR61(str), nvx::CharAt(str, 61) >::string

#define NVX_STR63(str) \
	typename nvx::PushChar< NVX_STR62(str), nvx::CharAt(str, 62) >::string

#define NVX_STR64(str) \
	typename nvx::PushChar< NVX_STR63(str), nvx::CharAt(str, 63) >::string





}





#endif
