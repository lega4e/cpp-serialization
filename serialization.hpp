#ifndef LIS_SERIALIZATION_HPP
#define LIS_SERIALIZATION_HPP

/*
 * autor:   lis
 * created: Oct  5 14:14:45 2020
 */

#include <any>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <unordered_set>





namespace lis
{





/* TYPES */
enum ArchiveMode
{
	/*
	 * Никакие указатели не сохраняются и не проверяются
	 */
	none_mode = 0,

	/*
	 * Обычные указатели сохраняются и, если встречаются при
	 * десериализации второй раз, то устанавливаются они
	 */
	determine_pointers_mode = 1 << 0,

	/*
	 * Shared-указатели сохраняются и, если встречаются
	 * при десериализации повторно, то устанавливаются они
	 * (режим по умолчанию)
	 */
	determine_shared_mode   = 1 << 1
};





/*
 * Указатели сохраняются только в пределах класса archive.
 * 
 * Stream — класс потока, использующийся для (де)сериализации,
 * например, ostream или istream, также stringstream; класс
 * должен иметь следующие методы:
 *   - operator bool()
 *
 *   (при сериализации)
 *   - write( char const *, size_t )
 *
 *   (при десериализации)
 *   - read( char *, size_t )
 */
template<class Stream>
class archive
{
public:


	archive(Stream *s, int mode = determine_shared_mode):
		s(s), mode(mode) {}



	archive &stream(Stream *s)
	{
		this->s = s;
		return *this;
	}

	Stream *stream() const
	{
		return s;
	}



	template<typename T>
	archive &operator<<(T t)
	{
		if(!s)
			return *this;
		serialize(*this, t);
		return *this;
	}

	template<typename T>
	archive &operator>>(T t)
	{
		if(!s)
			return *this;
		deserialize(*this, t);
		return *this;
	}

	operator bool() const
	{
		return s && (bool)*s;
	}



private:
	Stream *s;
	int mode;
	std::unordered_map<uint64_t, std::any> map;



	// pointers
	template<class Ostream, typename T>
	friend int _serialize_dispatcher(
		archive<Ostream> &os,
		T const *obj,
		std::true_type 
	);

	template<class Ostream, typename T>
	friend int _deserialize_dispatcher(
		archive<Ostream> &os,
		T *obj,
		std::true_type 
	);


	// shared prointers
	template<class Ostream, typename T>
	friend int serialize(
		archive<Ostream> &os,
		std::shared_ptr<T> const *obj 
	);

	template<class Istream, typename T>
	friend int deserialize(
		archive<Istream> &is,
		std::shared_ptr<T> *obj 
	);


	// final
	template<class Ostream, typename T>
	friend int _serialize_final(
		archive<Ostream> &os,
		T const *value,
		std::true_type isfundamental
	);

	template<class Istream, typename T>
	friend int _deserialize_final(
		archive<Istream> &is,
		T *value,
		std::true_type isfundamental
	);

	
	// plain
	template<class Ostream>
	friend int serialize_plain(
		archive<Ostream> &os,
		void const *obj,
		int size
	);

	template<class Istream>
	friend int deserialize_plain(
		archive<Istream> &is,
		void *obj,
		int size
	);
};





/* MACROS */
/*
 * Следующие структуры и функции обспечивают работу
 * макросов, которые включаются внутрь сериализуемых
 * классов и структур
 */

/*
 * Структура, представляющая сериализуемый динамический
 * массив; необходимо обратить внимание на то, что размер
 * всегда должен быть указателем на тип int
 */
template<typename T>
struct _serializable_dynamic_array_type
{
	T ptr;
	int *size;
};

/*
 * Структура, представляющая статический массив; при
 * сериализации статического массива в начало данных
 * не передаётся их размер
 */
template<typename T>
struct _serializable_static_array_type
{
	T ptr;
	int const SIZE;
};



// serialize cascade
/*
 * Сериазизация нескольких элементов разного типа
 * (в том числе динамических и статических массивов, но
 * дле этого их нужно обернуть в соответствующие структуры
 * или использовать для этого соответствующие макросы)
 */
template<class Ostream>
inline int serialize_elements(archive<Ostream> &os)
{
	return 0;
}

template<class Ostream, typename T, typename...Args>
inline int serialize_elements(
	archive<Ostream> &os,
	_serializable_dynamic_array_type<T> arr,
	Args...args
)
{
	return serialize(os, arr.ptr, arr.size) +
		   serialize_elements(os, args...);
}

template<class Ostream, typename T, typename...Args>
inline int serialize_elements(
	archive<Ostream> &os,
	_serializable_static_array_type<T> arr,
	Args...args
)
{
	return serialize_static(os, arr.ptr, arr.SIZE) +
		   serialize_elements(os, args...);
}

template<class Ostream, typename Head, typename...Args>
inline int serialize_elements(archive<Ostream> &os, Head head, Args...args)
{
	return serialize(os, head) + serialize_elements(os, args...);
}



// deserialize cascade
/*
 * То же самое, только для десериализации
 */
template<class Istream>
inline int deserialize_elements(archive<Istream> &is)
{
	return 0;
}

template<class Istream, typename T, typename...Args>
inline int deserialize_elements(
	archive<Istream> &is,
	_serializable_dynamic_array_type<T> arr,
	Args...args
)
{
	return deserialize(is, arr.ptr, arr.size) +
		   deserialize_elements(is, args...);
}

template<class Istream, typename T, typename...Args>
inline int deserialize_elements(
	archive<Istream> &is,
	_serializable_static_array_type<T> arr,
	Args...args
)
{
	return deserialize_static(is, arr.ptr, arr.SIZE) +
		   deserialize_elements(is, args...);
}

template<class Istream, typename Head, typename...Args>
inline int deserialize_elements(archive<Istream> &is, Head head, Args...args)
{
	return deserialize(is, head) + deserialize_elements(is, args...);
}





/*
 * Макросы для сериализации динамических и статических массив;
 * в макросы передаются два параметра: массив и размер; существуют
 * существенные отличия между динамическими и статическими массивами:
 *
 *   1. В качестве данных в динамеческом массиве выступает указатель
 *      на массив (то есть указатель на указатель на тип данных масива) —
 *      это используется для того, чтобы автоматически выделить необхо-
 *      димое количество памяти; в то время как для статического массива
 *      указывается сам массив без взятия адреса
 *
 *   2. При сериализации динамического массива указывается указатель на
 *      переменную содержащую размер (всегда int *), для того, чтобы
 *      при сериализации узнать число элементов и при десериализации
 *      записать его; в статическом массиве указывается непосредственно
 *      целое число (будет представлено как int)
 */
#define LIS_SERIALIZABLE_DINAMIC_ARRAY(ptr, sizeptr) \
	lis::_serializable_dynamic_array_type< \
		typename std::remove_reference<decltype(**ptr)>::type ** \
	> { \
		(typename std::remove_reference<decltype(**ptr)>::type **)ptr, \
		(typename std::remove_const< \
		 	typename std::remove_reference<decltype(*sizeptr) \
		>::type>::type *)sizeptr \
	}

#define LIS_SERIALIZABLE_STATIC_ARRAY(ptr, size) \
	lis::_serializable_static_array_type< \
		typename std::remove_reference<decltype(*ptr)>::type * \
	> { \
		ptr, size \
	}


/*
 * Эти макросы используется в теле класса или структуры, чтобы
 * сделать его сериализуемым; в качестве аргументов укызавыается:
 *   - Указатели на сериализуемые объекты (фундаментальные типы,
 *     строки, сериализуемые структуры, в том числе стандартные
 *     контейнеры)
 *   - Динамические массивы (используется макрос
 *     LIS_SERIALIZABLE_DINAMIC_ARRAY(ptr, sizeptr))
 *   - Статические массивы (используется макрос
 *     LIS_SERIALIZABLE_STATIC_ARRAY(ptr, size))
 *
 * Если необходимо совершить действия после сериализации или
 * десериализации, то объявляются методы класса или структуры с
 * именами after_serialization() и after_deserialization()
 * соответственно
 */

#define LIS_SERIALIZABLE(...) \
public: \
	template<typename Ostream> \
	int serialize(lis::archive<Ostream> &os) const \
	{ \
		int res = lis::serialize_elements( os, __VA_ARGS__ ); \
		after_serialization(); \
		return res; \
	} \
 \
	template<typename Istream> \
	int deserialize(lis::archive<Istream> &is) \
	{ \
		int res = lis::deserialize_elements( is, __VA_ARGS__ ); \
		after_deserialization(); \
		return res; \
	} 

/*
 * Используется этот макрос, если структура данных является 
 * плоской
 */
#define LIS_SERIALIZABLE_PLAIN() \
public: \
	template<class Ostream> \
	inline int serialize(lis::archive<Ostream> &os) const \
	{ \
		int res = lis::serialize_plain(os, (void const *)this, sizeof(*this)); \
		after_serialization(); \
		return res; \
	} \
 \
	template<class Istream> \
	inline int deserialize(lis::archive<Istream> &is) \
	{ \
		int res = lis::deserialize_plain(is, (void *)this, sizeof *this); \
		after_deserialization(); \
		return res; \
	}





/* DECLARATIONS */
/*
 * Основные функции сериализации; первым аргументом
 * всегда укызывается архив, в который (из которого)
 * будет происходить (де)сериализация; вторым аргументом
 * всегда идёт указатель на сериализуемый объект (если
 * необходимо сериализовать указатель, то передаётся указатель
 * на указатель; если необходимо сериализовать массив,
 * передаётся указатель на указатель на первый элемент массива),
 * однако есть иключения:
 *   - если сериализуется объект, на который указывает shared_ptr,
 *     то передаётся сам shared_ptr, а не указатель на него;
 *   - если сериализуется статический массив, то передаётся 
 *     указатель на первый элемент массива (то есть сам массив)
 *     (функция serialize_static)
 *   - если сериализуются плоские данные (serialize_plain), то
 *     передаётся указатель на первый байт данных 
 */
// main
template<class Ostream, typename T>
int serialize(archive<Ostream> &os, T const *value);

template<class Istream, typename T>
int deserialize(archive<Istream> &is, T *value);



// to string
/*
 * (Де)сериализация в(из) строку(и)
 */
template<typename T>
int serialize(
	std::string &src,
	T const *value,
	int mode = ArchiveMode::determine_shared_mode
);

template<typename T>
std::string serialize(
	T const *value,
	int mode = ArchiveMode::determine_shared_mode
);

template<typename T>
int deserialize(
	std::string const &src,
	T *value, 
	int mode = ArchiveMode::determine_shared_mode
);

template<typename T>
int deserialize(
	std::string &&src,
	T *value,
	int mode = ArchiveMode::determine_shared_mode
);


// to string dynamic array
/*
 * Не добавляет, а перезаписывает src
 */
template<typename T>
int serialize(
	std::string &src,
	T const * const *value,
	int *size,
	int mode = ArchiveMode::determine_shared_mode
);

template<typename T>
std::string serialize(
	T const * const *value,
	int *size,
	int mode = ArchiveMode::determine_shared_mode
);


template<typename T>
int deserialize(
	std::string const &src,
	T **value, 
	int *size,
	int mode = ArchiveMode::determine_shared_mode
);

template<typename T>
int deserialize(
	std::string &&src,
	T **value,
	int *size,
	int mode = ArchiveMode::determine_shared_mode
);


// to string static array
template<typename T>
int serialize_static(
	std::string &src,
	T const *value,
	int size,
	int mode = ArchiveMode::determine_shared_mode
);

template<typename T>
std::string serialize_static(
	T const *value,
	int size,
	int mode = ArchiveMode::determine_shared_mode
);


template<typename T>
int deserialize_static(
	std::string const &src,
	T *value, 
	int size,
	int mode = ArchiveMode::determine_shared_mode
);

template<typename T>
int deserialize_static(
	std::string &&src,
	T *value,
	int size,
	int mode = ArchiveMode::determine_shared_mode
);



// dynamic array
template<class Ostream, typename T>
int serialize(archive<Ostream> &os, T const * const *value, int const *size);

template<class Istream, typename T>
int deserialize(archive<Istream> &is, T **value, int *size);



// static array
template<class Ostream, typename T>
int serialize_static(archive<Ostream> &os, T const *value, int size);

template<class Istream, typename T>
int deserialize_static(archive<Istream> &is, T *value, int size);



// plain
template<class Ostream>
int serialize_plain(archive<Ostream> &os, void const *obj, int size);

template<class Istream>
int deserialize_plain(archive<Istream> &is, void *obj, int size);



// string
template<
	class Ostream, 
	typename CharType,
	class CharTraitsType,
	class Alloc
>
int serialize(
	archive<Ostream> &os,
	std::basic_string<CharType, CharTraitsType, Alloc> const *str
);

template<
	class Istream, 
	typename CharType,
	class CharTraitsType,
	class Alloc
>
int deserialize(
	archive<Istream> &is,
	std::basic_string<CharType, CharTraitsType, Alloc> *str
);



// final
template<class Ostream, typename T>
int _serialize_final(archive<Ostream> &os, T const *value);

template<class Ostream, typename T>
int _deserialize_final(archive<Ostream> &os, T *value);



// pointer
template<class Ostream, typename T>
int _serialize_dispatcher( archive<Ostream> &os, T const *obj, std::true_type );

template<class Ostream, typename T>
inline int _serialize_dispatcher( archive<Ostream> &os, T const *obj, std::false_type );


template<class Ostream, typename T>
int _deserialize_dispatcher( archive<Ostream> &os, T *obj, std::true_type );

template<class Ostream, typename T>
inline int _deserialize_dispatcher( archive<Ostream> &os, T *obj, std::false_type );



// shared pointers
template<class Ostream, typename T>
int serialize( archive<Ostream> &os, std::shared_ptr<T> const *obj );

template<class Istream, typename T>
int deserialize( archive<Istream> &is, std::shared_ptr<T> *obj );





/* STD-STRUCTS */
template<class Ostream, typename T, typename U>
int serialize( archive<Ostream> &os, std::pair<T, U> const *p );

template<class Istream, typename T, typename U>
int deserialize( archive<Istream> &is, std::pair<T, U> *p );





/* CONTAINERS */
// common
/*
 * class Container:
 *     size();  -> int32_t
 *     begin();
 *     end();
 */
template<class Ostream, class Container>
int serialize_container(archive<Ostream> &os, Container const *cont);


template<class Istream, class ResizableContainer>
int deserialize_resizable_container(archive<Istream> &is, ResizableContainer *cont);



// vector
template<class Ostream, typename T, typename Alloc>
int serialize(archive<Ostream> &os, std::vector<T, Alloc> const *cont);

template<class Istream, typename T, typename Alloc>
int deserialize(archive<Istream> &is, std::vector<T, Alloc> *cont);



// list
template<class Ostream, typename T, typename Alloc>
int serialize(archive<Ostream> &os, std::list<T, Alloc> const *list);

template<class Istream, typename T, typename Alloc>
int deserialize(archive<Istream> &is, std::list<T, Alloc> *list);



// set
template<
	class Ostream,
	typename Key,
	typename Compare,
	typename Alloc
>
int serialize(
	archive<Ostream> &os,
	std::set<Key, Compare, Alloc> const *set
);

template<
	class Istream,
	typename Key,
	typename Compare,
	typename Alloc
>
int deserialize(
	archive<Istream> &is,
	std::set<Key, Compare, Alloc> *set
);



// multiset
template<
	class Ostream,
	typename Key,
	typename Compare,
	typename Alloc
>
int serialize(
	archive<Ostream> &os,
	std::multiset<Key, Compare, Alloc> const *set
);

template<
	class Istream,
	typename Key,
	typename Compare,
	typename Alloc
>
int deserialize(
	archive<Istream> &is,
	std::multiset<Key, Compare, Alloc> *set
);



// unordered set
template<
	class Ostream,
	typename Value,
	typename Hash,
	typename Pred,
	typename Alloc
>
int serialize(
	archive<Ostream> &os,
	std::unordered_set<Value, Hash, Pred, Alloc> const *set
);

template<
	class Istream,
	typename Value,
	typename Hash,
	typename Pred,
	typename Alloc
>
int deserialize(
	archive<Istream> &is,
	std::unordered_set<Value, Hash, Pred, Alloc> *set
);



// map
template<
	class Ostream,
	typename Key,
	typename T,
	typename Compare,
	typename Alloc
>
int serialize(
	archive<Ostream> &os,
	std::map<Key, T, Compare, Alloc> const *map
);

template<
	class Istream,
	typename Key,
	typename T,
	typename Compare,
	typename Alloc
>
int deserialize(
	archive<Istream> &is,
	std::map<Key, T, Compare, Alloc> *map
);



// multimap
template<
	class Ostream,
	typename Key,
	typename T,
	typename Compare,
	typename Alloc
>
int serialize(
	archive<Ostream> &os,
	std::multimap<Key, T, Compare, Alloc> const *map
);

template<
	class Istream,
	typename Key,
	typename T,
	typename Compare,
	typename Alloc
>
int deserialize(
	archive<Istream> &is,
	std::multimap<Key, T, Compare, Alloc> *map
);



// unordered map
template<
	class Ostream,
	typename Key,
	typename T,
	typename Hash,
	typename Pred,
	typename Alloc
>
int serialize(
	archive<Ostream> &os,
	std::unordered_map<Key, T, Hash, Pred, Alloc> const *map
);

template<
	class Istream,
	typename Key,
	typename T,
	typename Hash,
	typename Pred,
	typename Alloc
>
int deserialize(
	archive<Istream> &is,
	std::unordered_map<Key, T, Hash, Pred, Alloc> *map
);



// unordered multimap
template<
	class Ostream,
	typename Key,
	typename T,
	typename Hash,
	typename Pred,
	typename Alloc
>
int serialize(
	archive<Ostream> &os,
	std::unordered_multimap<Key, T, Hash, Pred, Alloc> const *map
);

template<
	class Istream,
	typename Key,
	typename T,
	typename Hash,
	typename Pred,
	typename Alloc
>
int deserialize(
	archive<Istream> &is,
	std::unordered_multimap<Key, T, Hash, Pred, Alloc> *map
);





#include "serialization_implement.hpp"





}





inline void after_serialization() {}
inline void after_deserialization() {}





#endif // LIS_SERIALIZATION_HPP
