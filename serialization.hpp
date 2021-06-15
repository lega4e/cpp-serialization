#ifndef NVX_SERIALIZATION_HPP
#define NVX_SERIALIZATION_HPP

/**
 * \file Файл, в котором хранятся средства сериализации
 * \autor nvx
 * \date Oct  5 14:14:45 2020
 */

#include <algorithm>
#include <any>
#include <fstream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <nvx/type.hpp>





/// Пустышка для переопределения
/*!
 * Функция-пустышка, переопределив которую
 * внутри сериализуемого класса, можно задать
 * действие перед сериализацией
 */
inline void before_serialization() {}


/// Пустышка для переопределения
/*!
 * Функция-пустышка, переопределив которую
 * внутри сериализуемого класса, можно задать
 * действие перед десериализацией
 */
inline void before_deserialization() {}


/// Пустышка для переопределения
/*!
 * Функция-пустышка, переопределив которую
 * внутри сериализуемого класса, можно задать
 * действие после сериализации
 */
inline void after_serialization() {}


/// Пустышка для переопределения
/*!
 * Функция-пустышка, переопределив которую
 * внутри сериализуемого класса, можно задать
 * действие после десериализации
 */
inline void after_deserialization() {}





/// Основное пространство имён
namespace nvx
{





/*!
 * \defgroup help_functions Вспомогательные функции
 * @{
 */

/// Возвращает указатель на временный объект
template<typename T> T *R(T t)
{
	static T val;
	val = t;
	return &val;
}

/// Возвращает указатель на строковый литерал
inline std::string *operator""_R(char const *s, unsigned long n)
{
	static std::string val;
	val = s;
	return &val;
}

/// Возвращает указатель на литерал вещественного числа
inline ldouble *operator""_R(ldouble t)
{
	static ldouble val;
	val = t;
	return &val;
}

/// Возвращает указатель на литерал целого числа
inline llong *operator""_R(unsigned long long t)
{
	static llong val;
	val = t;
	return &val;
}

inline void open_io_file(std::fstream *stream, char const *filename)
{
	stream->open(filename, std::ios_base::in | std::ios_base::out);
	if(stream->is_open())
		return;

	stream->open(
		filename,
		std::ios_base::in |
		  std::ios_base::out |
		  std::ios_base::trunc
	);

	return;
}

inline void open_io_file(std::fstream *stream, std::string const &filename)
{
	return open_io_file(stream, filename.c_str());
}

/*! @} */










/* ARCHIVE */
typedef int32_t id_t;
constexpr id_t NULL_ID = std::numeric_limits<id_t>::min();

/// Перечисление, которое отвечает за режим работы архива;
/// режими можно комбинировать с помощью оператора |
enum ArchiveMode
{
	/// Ничего не сохранять, не проверять
	/*!
	 * Никакие указатели не сохраняются и не проверяются
	 */
	none_mode = 0,

	/// Режим поддержки разделяемых обычных указателей
	/*!
	 * Обычные указатели сохраняются и ассоциируются с
	 * уникальными идентификаторами. Если в дальнейшем потребуется
	 * десериализовать указатель, который уже встречался,
	 * то возвратится сохранённый указатель
	 */
	determine_pointers_mode = 1 << 0,

	/// Режим поддержки разделяемых (std::shared_ptr) указателей
	/*!
	 * Указатели std::shared_ptr сохраняются и ассоциируются с
	 * уникальными идентификаторами. Если в дальнейшем потребуется
	 * десериализовать указатель, который уже встречался,
	 * то возвратится сохранённый указатель. (Режим по умолчанию)
	 */
	determine_shared_mode   = 1 << 1
};



template<typename T>
class Lira;

/// Класс архива, необходимый для (де)сериализации
/*!
 * Указатели сохраняются только в пределах класса archive.
 *
 * \param Stream — класс потока, использующийся для
 * (де)сериализации, например, ostream или istream,
 * также stringstream; класс должен иметь следующие методы:
 * - operator bool()
 * - write( char const *, size_t )
 * - read( char *, size_t )
 * - tellp, seekp, tellg, seekg
 *
 * \param Meta — в случае если используется Lira,
 * соответствует типу её метаобъектов; иначе не
 * имеет смысла
 *
 *
 */
template<class Stream, typename Meta = void>
class archive
{
public:

	/// Конструктор по умолчанию
	archive(Stream *s, int mode = determine_shared_mode):
		s(s), mode(mode) {}



	/// Получения потока
	Stream *stream() const
	{
		return s;
	}



	/// Сериализация с опомщью оператора <<
	template<typename T>
	archive &operator<<(T t)
	{
		if(!s)
			return *this;
		serialize(*this, t);
		return *this;
	}

	/// Десериализация с помощью оператора >>
	template<typename T>
	archive &operator>>(T t)
	{
		if(!s)
			return *this;
		deserialize(*this, t);
		return *this;
	}

	/// Проверка потока на работоспособность
	operator bool() const
	{
		return s && (bool)*s;
	}



private:
	friend class nvx::Lira<Meta>;

	Stream *s;
	int  mode;

	// Сопоставление каждому идентификатору объекту в ОП
	std::unordered_map<id_t, std::pair<void *, std::any>> idns;

	// Сопоставление каждому объекту в ОП идентификатора
	// и числу указывающих на него объектов
	std::unordered_map<void *, std::pair<id_t, int>> objs;

	Lira<Meta> *lira = nullptr;

	int freshness = 0;
	int curid     = 0;
	int incid     = 0;

	id_t newid()
	{
		if(lira)
			return lira->next_shared_id();
		return incid++;
	}



	// pointers
	template<class Ostream, typename M, typename T>
	friend int _serialize_dispatcher(
		archive<Ostream, M> &os,
		T const *obj,
		std::true_type,
		bool write
	);

	template<class Ostream, typename M, typename T>
	friend int _deserialize_dispatcher(
		archive<Ostream, M> &os,
		T *obj,
		std::true_type
	);


	// shared prointers
	template<class Ostream, typename M, typename T>
	friend int serialize(
		archive<Ostream, M> &os,
		std::shared_ptr<T> const *obj,
		bool write
	);

	template<class Ostream, typename M, typename T>
	friend int serialize(
		archive<Ostream, M> &os,
		std::shared_ptr<T> const *obj,
		bool write
	);

	template<class Istream, typename M, typename T>
	friend int deserialize(
		archive<Istream, M> &is,
		std::shared_ptr<T> *obj
	);


	// final
	template<class Ostream, typename M, typename T>
	friend int _serialize_final(
		archive<Ostream, M> &os,
		T const *value,
		std::true_type isfundamental,
		bool write
	);

	template<class Istream, typename M, typename T>
	friend int _deserialize_final(
		archive<Istream, M> &is,
		T *value,
		std::true_type isfundamental
	);


	// plain
	template<class Ostream, typename M>
	friend int serialize_plain(
		archive<Ostream, M> &os,
		void const *obj,
		int size,
		bool write
	);

	template<class Istream, typename M>
	friend int deserialize_plain(
		archive<Istream, M> &is,
		void *obj,
		int size
	);
};










/* MACROS */
/********************** HELP FOR MACROS *********************/
/*
 * Следующие структуры и функции обспечивают работу
 * макросов, которые включаются внутрь сериализуемых
 * классов и структур
 */

/*!
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

/*!
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
/*!
 * Сериазизация нескольких элементов разного типа
 * (в том числе динамических и статических массивов, но
 * дле этого их нужно обернуть в соответствующие структуры
 * или использовать для этого соответствующие макросы)
 */
template<class Ostream>
inline int serialize_elements(
	archive<Ostream> &os,
	bool write = true
)
{
	return 0;
}

template<class Ostream, typename T, typename...Args>
inline int serialize_elements(
	archive<Ostream> &os, bool write,
	_serializable_dynamic_array_type<T> arr,
	Args...args
)
{
	return serialize_array(os, arr.ptr, arr.size, write) +
		   serialize_elements(os, write, args...);
}

template<class Ostream, typename T, typename...Args>
inline int serialize_elements(
	archive<Ostream> &os, bool write,
	_serializable_static_array_type<T> arr,
	Args...args
)
{
	return serialize_static(os, arr.ptr, arr.SIZE, write) +
		   serialize_elements(os, write, args...);
}

template<class Ostream, typename Head, typename...Args>
inline int serialize_elements(
	archive<Ostream> &os,
	bool write,
	Head head,
	Args...args
)
{
	return serialize(os, head, write) +
		serialize_elements(os, write, args...);
}



// deserialize cascade
/*!
 * Десериализация нескольких элементов разного типа
 * (в том числе динамических и статических массивов, но
 * дле этого их нужно обернуть в соответствующие структуры
 * или использовать для этого соответствующие макросы)
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
	return deserialize_array(is, arr.ptr, arr.size) +
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





/************************** ARRAYS **************************/
/*!
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
#define NVX_SERIALIZABLE_DYNAMIC_ARRAY(ptr, sizeptr) \
	nvx::_serializable_dynamic_array_type< \
		typename std::remove_reference<decltype(**ptr)>::type ** \
	> { \
		(typename std::remove_reference<decltype(**ptr)>::type **)ptr, \
		(typename std::remove_const< \
		 	typename std::remove_reference<decltype(*sizeptr) \
		>::type>::type *)sizeptr \
	}

#define NVX_SERIALIZABLE_STATIC_ARRAY(ptr, size) \
	nvx::_serializable_static_array_type< \
		typename std::remove_reference<decltype(*ptr)>::type * \
	> { \
		ptr, size \
	}



/************************* IN-STRUCT ************************/
/*!
 * Эти макросы используется в теле класса или структуры, чтобы
 * сделать его сериализуемым; в качестве аргументов укызавыается:
 *
 *   - Указатели на сериализуемые объекты (фундаментальные типы,
 *     строки, сериализуемые структуры, в том числе стандартные
 *     контейнеры)
 *
 *   - Динамические массивы (используется макрос
 *     NVX_SERIALIZABLE_DYNAMIC_ARRAY(ptr, sizeptr))
 *
 *   - Статические массивы (используется макрос
 *     NVX_SERIALIZABLE_STATIC_ARRAY(ptr, size))
 *
 * Если необходимо совершить действия после сериализации или
 * десериализации, то объявляются методы класса или структуры с
 * именами after_serialization() и after_deserialization()
 * соответственно
 */

#define NVX_SERIALIZABLE(...) \
public: \
	template<typename Ostream> \
	int serialize(nvx::archive<Ostream> &os, bool write = true) const \
	{ \
		int res = nvx::serialize_elements( os, write, __VA_ARGS__ ); \
		after_serialization(); \
		return res; \
	} \
 \
	template<typename Istream> \
	int deserialize(nvx::archive<Istream> &is) \
	{ \
		int res = nvx::deserialize_elements( is, __VA_ARGS__ ); \
		after_deserialization(); \
		return res; \
	}

/*!
 * Используется этот макрос, если структура данных является
 * плоской
 */
#define NVX_SERIALIZABLE_PLAIN() \
public: \
	template<class Ostream> \
	inline int serialize(nvx::archive<Ostream> &os, bool write = true) const \
	{ \
		int res = nvx::serialize_plain(os, (void const *)this, sizeof(*this), write); \
		after_serialization(); \
		return res; \
	} \
 \
	template<class Istream> \
	inline int deserialize(nvx::archive<Istream> &is) \
	{ \
		int res = nvx::deserialize_plain(is, (void *)this, sizeof *this); \
		after_deserialization(); \
		return res; \
	}





/************************ CONTAINERS ************************/
// DECLARATIONS
#define _NVX_SERIALIZE_CONTAINER_DECLARE_CORE(contname) \
	template< \
		class Ostream, \
		typename Meta, \
		typename...Other \
	> \
	int serialize( \
		archive<Ostream, Meta> &os, \
		contname<Other...> const *cont, \

#define _NVX_SERIALIZE_CONTAINER_DECLARE(contname) \
	_NVX_SERIALIZE_CONTAINER_DECLARE_CORE(contname) \
		bool write = true \
	)

#define _NVX_SERIALIZE_CONTAINER_DECLARE_NODARG(contname) \
	_NVX_SERIALIZE_CONTAINER_DECLARE_CORE(contname) \
		bool write \
	)


#define _NVX_DESERIALIZE_RESIZABLE_CONTAINER_DECLARE(contname) \
	template< \
		class Istream, \
		typename Meta, \
		typename...Other \
	> \
	int deserialize( \
		archive<Istream, Meta> &is, \
		contname<Other...> *cont  \
	)

#define _NVX_DESERIALIZE_INSERTED_CONTAINER_DECLARE(contname) \
	template< \
		class Istream, \
		typename Meta, \
		typename...Other \
	> \
	int deserialize( \
		archive<Istream, Meta> &is, \
		contname<Other...> *cont  \
	) \



// DEFINITIONS
#define _NVX_SERIALIZE_CONTAINER_DEFINE(contname) \
	_NVX_SERIALIZE_CONTAINER_DECLARE_NODARG(contname) \
	{ return nvx::serialize_container(os, cont, write); }

#define _NVX_DESERIALIZE_RESIZABLE_CONTAINER_DEFINE(contname) \
	_NVX_DESERIALIZE_RESIZABLE_CONTAINER_DECLARE(contname) \
	{ return nvx::deserialize_resizable_container(is, cont); }

#define _NVX_DESERIALIZE_INSERTED_CONTAINER_DEFINE(contname) \
	_NVX_DESERIALIZE_INSERTED_CONTAINER_DECLARE(contname) \
	{ return nvx::deserialize_inserted_container(is, cont); }



// SERIALIZATION + DESERIALIZATION
#define _NVX_SERIALIZABLE_RESIZABLE_CONTANER_DECLARE(contname) \
	_NVX_SERIALIZE_CONTAINER_DECLARE(contname); \
	_NVX_DESERIALIZE_RESIZABLE_CONTAINER_DECLARE(contname);

#define _NVX_SERIALIZABLE_INSERTED_CONTANER_DECLARE(contname) \
	_NVX_SERIALIZE_CONTAINER_DECLARE(contname); \
	_NVX_DESERIALIZE_INSERTED_CONTAINER_DECLARE(contname);


#define _NVX_SERIALIZABLE_RESIZABLE_CONTANER_DEFINE(contname) \
	_NVX_SERIALIZE_CONTAINER_DEFINE(contname); \
	_NVX_DESERIALIZE_RESIZABLE_CONTAINER_DEFINE(contname);

#define _NVX_SERIALIZABLE_INSERTED_CONTANER_DEFINE(contname) \
	_NVX_SERIALIZE_CONTAINER_DEFINE(contname); \
	_NVX_DESERIALIZE_INSERTED_CONTAINER_DEFINE(contname);










/* DECLARATIONS */
// main
/*!
 *
 * \defgroup serialization_functions Функции сериализации
 *
 * Основные функции сериализации делятся на две группы:
 * архивные и строковые; архивные (де)сериализуют объекты
 * в архив, а строковые, соответственно, в строку.
 *
 * При использовании архивных функций первым первым аргументом
 * всегда укызывается архив, в который (из которого)
 * будет проводиться (де)сериализация; вторым аргументом
 * всегда идёт указатель на сериализуемый объект (если
 * необходимо сериализовать указатель, то передаётся указатель
 * на указатель; если необходимо сериализовать массив,
 * передаётся указатель на указатель на первый элемент массива),
 * однако есть иключения:
 * - если сериализуется статический массив (т.е. размер которого не
 *   меняется во время выполнения программы), то передаётся
 *   указатель на первый элемент массива (то есть сам массив)
 *   (функция serialize_static)
 * - если сериализуются плоские данные (serialize_plain), то
 *   передаётся указатель на их первый байт
 *
 * Помимо одиночных объектов можно сериализовывать массивы, (TODO:
 * c-strings) которые разделяются на два типа: динамические и
 * статические. Отличие состоит в том, что при (де)сериализации
 * динамического массива, помимо записи самих элементов, также
 * добавляется размер массива; соответственно, пользователь при
 * десериализации получает не только сами элементы, но и их
 * количество — в отдельную (TODO: совместимую с int) переменную типа
 * int; при (де)сериализации статического массива его размер всегда
 * задаются извне пользователем. Есть ещё одно существенное отличие в
 * этих функциях, а точнее в их синтаксисе: при сериализации
 * динамического массива дополнительным аргументом передаётся
 * указатель на int, из которого будет будет узнан размер массива при
 * сериализации и, соответственно, куда будет помещён размер массива
 * при десериализации. В случае статического массива передаётся сам
 * размер, т.е. просто переменная типа int, а не указатель.
 *
 * В конце всех архивных функций сериализации находится
 * необязательный аргумент `write`, который указывает,
 * необходимо ли производить запись (по умолчанию
 * запись, конечно, производится); если `write = false`,
 * то объект не сериализуется и архив остаётся не тронутым,
 * но возвращается количество байт, которое будет занимать
 * записанный объект.
 *
 * Строковые функции могут использоваться с явным указанием
 * объекта строки, в которую (из которой) должно происходить
 * (де)сериализация, либо без оной. В первом случае в
 * качестве первого аргумента выступает ссылка на строку,
 * а уже затем идёт указатель на (де)сериализуемый объект и
 * остальные аргументы; в самом конце всегда есть параметр
 * режима архивации (ArchiveMode); во втором случае — то же
 * самое, но из начала убирается ссылка на строку
 *
 * \warning При сериализации std::shared_ptr передаётся
 * указатель на него, а не он сам
 */


/*!
 * \defgroup archive_serialization Архивные функции сериализации
 * \ingroup serialization_functions
 *
 * (Де)сериализуют единичные объекты, а также массивы или плоские
 * данные в архив.
 *
 * @{
 */

/// Архивная функция сериализации единичного объекта
/*!
 * Сериализует единичный объект в архив.
 *
 * \param os — архив, в который производится сериализация
 * \param value — указатель на сериализуемый объект
 * \param write — указание, действительно ли производить сериализацию
 *        или только подсчитать требуемое для оной число байт
 *
 * \return Число байт, которое было записано (или требуемое для этого)
 */
template<class Ostream, typename Meta, typename T>
int serialize(archive<Ostream, Meta> &os, T const *value, bool write = true);

/// Архивная функция десериализации единичного объекта
/*!
 * Десериализует единичный объект из архива.
 *
 * \param os — архив, из которого производится десериализация
 * \param value — указатель на десериализуемый объект
 *
 * \return Число считанных байт
 */
template<class Istream, typename Meta, typename T>
int deserialize(archive<Istream, Meta> &is, T *value);





/*************** ARRAY AND PLAIN SERIALIZATION **************/
/// Архивная функция сериализации динамического массива
/*!
 * Сериализует динамический массив в архив.
 *
 * \param os — архив, в который производится сериализация
 * \param value — указатель на указатель на первый элемент
 *        сериализуемого массива
 * \param size — указатель на переменную, содержащую размер массива
 * \param write — указание, действительно ли производить сериализацию
 *        или только подсчитать требуемое для оной число байт
 *
 * \return Число байт, которое было записано (или требуемое для этого)
 */
template<class Ostream, typename Meta, typename T>
int serialize_array(
	archive<Ostream, Meta> &os,
	T const * const *value,
	int const *size,
	bool write = true
);

/// Архивная функция десериализации динамического массива
/*!
 * Десериализует динамический массив в архив.
 *
 * \param os — архив, в который производится сериализация
 * \param value — указатель на указатель на первый элемент
 *        сериализуемого массива
 * \param size — указатель на переменную, содержащую размер массива
 * \param write — указание, действительно ли производить сериализацию
 *        или только подсчитать требуемое для оной число байт
 *
 * \return Число байт, которое было записано (или требуемое для этого)
 */
template<class Istream, typename Meta, typename T>
int deserialize_array(
	archive<Istream, Meta> &is,
	T **value,
	int *size
);

/* @} */



// static array
template<class Ostream, typename Meta, typename T>
int serialize_static(
	archive<Ostream, Meta> &os,
	T const *value,
	int size,
	bool write = true
);

template<class Istream, typename Meta, typename T>
int deserialize_static(
	archive<Istream, Meta> &is,
	T *value,
	int size
);



// plain
template<class Ostream, typename Meta>
int serialize_plain(
	archive<Ostream, Meta> &os,
	void const *obj,
	int size,
	bool write = true
);

template<class Istream, typename Meta>
int deserialize_plain(
	archive<Istream, Meta> &is,
	void *obj,
	int size
);





/****************** SERIALIZATION TO STRING *****************/
/*!
 * \defgroup string_serialization Строковые функции
 * \ingroup serialization_functions
 *
 * Функции, которые не требуют архива, чтобы (де)сериализовать
 * объект; (де)сериализация происходит в(из) строки
 *
 * @{
 */

/// Основная функция сериализации для одиночных объектов
/// с явным указанием строки
/*!
 * Сериализация объекта value происходит в строку src
 * (TODO: перезапись и добавление); дополнительно можно
 * указать режим архива. Возвращает число сериализованных
 * байт.
 */
template<typename T>
int serialize(
	std::string &src,
	T const *value,
	int mode = ArchiveMode::determine_shared_mode
);

/// Основная функция сериализации для одиночных объектов
/// без указания строки
/*!
 * Сериализация объекта value происходит в строку, а
 * затем возвращается (TODO: перезапись и добавление);
 * дополнительно можно указать режим архива
 */
template<typename T>
std::string serialize(
	T const *value,
	int mode = ArchiveMode::determine_shared_mode
);

/// Основная функция (де)сериализации для одиночных объектов
/// с явным указанием строки
/*!
 * Десериализация объекта value происходит из строки src
 * (TODO: перезапись и добавление); дополнительно можно
 * указать режим архива. Возвращает число десериализованных
 * байт.
 */
template<typename T>
int deserialize(
	std::string const &src,
	T *value,
	int mode = ArchiveMode::determine_shared_mode
);

/// Основная функция (де)сериализации для одиночных объектов
/// с явным указанием строки (перегрузка для rvalue)
/*!
 * Десериализация объекта value происходит из строки src
 * (TODO: перезапись и добавление); дополнительно можно
 * указать режим архива. Возвращает число десериализованных
 * байт.
 */
template<typename T>
int deserialize(
	std::string &&src,
	T *value,
	int mode = ArchiveMode::determine_shared_mode
);



// to string dynamic array
/// Функция сериализации динамического массива в строку
/// с явным указанием строки
/*!
 * Сериализует динамический массив в строку.
 *
 * \param src — строка, куда производится сериализация
 * \param value — указатель на указатель на первый элемент массива
 * \param size — указатель на переменную, хранящую размер массива
 * \param mode — режим архива (см. ArchiveMode)
 *
 * \return Число сериализованных байт
 */
template<typename T>
int serialize_array(
	std::string &src,
	T const * const *value,
	int const *size,
	int mode = ArchiveMode::determine_shared_mode
);

/// Функция сериализации динамического массива в строку
/// без явного указание строки
/*!
 * Сериализует динамический массив в строку.
 *
 * \param value — указатель на указатель на первый элемент массива
 * \param size — указатель на переменную, хранящую размер массива
 * \param mode — режим архива (см. ArchiveMode)
 *
 * \return Строку, в которую произвелась сериализация массива
 */
template<typename T>
std::string serialize_array(
	T const * const *value,
	int const *size,
	int mode = ArchiveMode::determine_shared_mode
);


/// Функция десериализации динамического массива из строки
/*!
 * Десериализует динамический массив из строки.
 *
 * \param src — строка, из которой производится сериализация
 * \param value — указатель на указатель на первый элемент массива
 * \param size — указатель на переменную, хранящую размер массива
 * \param mode — режим архива (см. ArchiveMode)
 *
 * \return Число десериализованных байт
 */
template<typename T>
int deserialize_array(
	std::string const &src,
	T **value,
	int *size,
	int mode = ArchiveMode::determine_shared_mode
);

/// Функция десериализации динамического массива из строки
/// (перегрузка для rvalue)
/*!
 * Десериализует динамический массив из строки.
 *
 * \param src — строка, из которой производится сериализация
 * \param value — указатель на указатель на первый элемент массива
 * \param size — указатель на переменную, хранящую размер массива
 * \param mode — режим архива (см. ArchiveMode)
 *
 * \return Число десериализованных байт
 */
template<typename T>
int deserialize_array(
	std::string &&src,
	T **value,
	int *size,
	int mode = ArchiveMode::determine_shared_mode
);


// to string static array
/// Функция сериализации статического массива в строку
/// с явным указанием строки
/*!
 * Сериализует статический массив в строку.
 *
 * \param src — строка, куда производится сериализация
 * \param value — указатель на указатель на первый элемент массива
 * \param size — размер статического массива
 * \param mode — режим архива (см. ArchiveMode)
 *
 * \return Число сериализованных байт
 */
template<typename T>
int serialize_static(
	std::string &src,
	T const *value,
	int size,
	int mode = ArchiveMode::determine_shared_mode
);

/// Функция сериализации статического массива в строку
/// без явного указание строки
/*!
 * Сериализует статический массив в строку.
 *
 * \param value — указатель на указатель на первый элемент массива
 * \param size — размер статического массива
 * \param mode — режим архива (см. ArchiveMode)
 *
 * \return Строку, в которую произвелась сериализация массива
 */
template<typename T>
std::string serialize_static(
	T const *value,
	int size,
	int mode = ArchiveMode::determine_shared_mode
);


/// Функция десериализации статического массива из строки
/*!
 * Десериализует статический массив из строки.
 *
 * \param src — строка, из которой производится сериализация
 * \param value — указатель на указатель на первый элемент массива
 * \param size — размер массива
 * \param mode — режим архива (см. ArchiveMode)
 *
 * \return Число десериализованных байт
 */
template<typename T>
int deserialize_static(
	std::string const &src,
	T *value,
	int size,
	int mode = ArchiveMode::determine_shared_mode
);

/// Функция десериализации статического массива из строки
/// (перегрузка для rvalue)
/*!
 * Десериализует статический массив из строки.
 *
 * \param src — строка, из которой производится сериализация
 * \param value — указатель на указатель на первый элемент массива
 * \param size — размер массива
 * \param mode — режим архива (см. ArchiveMode)
 *
 * \return Число десериализованных байт
 */
template<typename T>
int deserialize_static(
	std::string &&src,
	T *value,
	int size,
	int mode = ArchiveMode::determine_shared_mode
);

/* @} */





// final
template<class Ostream, typename Meta, typename T>
int _serialize_final(
	archive<Ostream, Meta> &os,
	T const *value,
	bool write = true
);

template<class Ostream, typename Meta, typename T>
int _deserialize_final(
	archive<Ostream, Meta> &os,
	T *value
);





/****************** POINTERS SERIALIZATION ******************/
/*!
 * \defgroup pointers_serialization Сериализация указателей
 * \ingroup serialization_functions
 *
 * Эта группа содержит функции, отвечающие за сериализацию
 * указателей, как обычных так и shared; работу этих
 * функций контролирует режим архива ArchiveMode
 *
 * @{
 */

/// Вспомогательная функция для сериализации обычных указателей
/*!
 * Вспомогательная функция для сериализации,
 * которая определяет, является ли obj
 * указателем на указатель; эта перегрузка
 * вызывается, если является; если включено
 * разпознавание разделяемых обычных указателей
 * (т.е. установлен флаг determine_pointers_mode),
 * то функция ведёт себя особым образом
 */
template<class Ostream, typename Meta, typename T>
int _serialize_dispatcher(
	archive<Ostream, Meta> &os,
	T const *obj,
	std::true_type,
	bool write = true
);

/// Вспомогательная функция для сериализации обычных указателей
/*!
 * Вспомогательная функция для сериализации,
 * которая определяет, является ли obj
 * указателем на указатель; эта перегрузка
 * вызывается, если не является
 */
template<class Ostream, typename Meta, typename T>
inline int _serialize_dispatcher(
	archive<Ostream, Meta> &os,
	T const *obj,
	std::false_type,
	bool write = true
);


/// Вспомогательная функция для десериализации обычных указателей
/*!
 * Вспомогательная функция для десериализации,
 * которая определяет, является ли obj
 * указателем на указатель; эта перегрузка
 * вызывается, если является; если включено
 * разпознавание разделяемых обычных указателей
 * (т.е. установлен флаг determine_pointers_mode),
 * то функция ведёт себя особым образом
 */
template<class Ostream, typename Meta, typename T>
int _deserialize_dispatcher(
	archive<Ostream, Meta> &os,
	T *obj,
	std::true_type
);

/// Вспомогательная функция для десериализации обычных указателей
/*!
 * Вспомогательная функция для десериализации,
 * которая определяет, является ли obj
 * указателем на указатель; эта перегрузка
 * вызывается, если не является
 */
template<class Ostream, typename Meta, typename T>
inline int _deserialize_dispatcher(
	archive<Ostream, Meta> &os,
	T *obj,
	std::false_type
);



// shared pointers
/// Вспомогательная функция для сериализации std::shared_ptr
/*!
 * Вспомогательная функция для сериализации,
 * если включено разпознавание разделяемых
 * shared_pointer (т.е. установлен флаг
 * determine_shared_mode), то функция ведёт
 * себя особым образом
 */
template<class Ostream, typename Meta, typename T>
int serialize(
	archive<Ostream, Meta> &os,
	std::shared_ptr<T> const *obj,
	bool write = true
);

/// Вспомогательная функция для десериализации std::shared_ptr
/*!
 * Вспомогательная функция для десериализации,
 * если включено разпознавание разделяемых
 * shared_pointer (т.е. установлен флаг
 * determine_shared_mode), то функция ведёт
 * себя особым образом
 */
template<class Istream, typename Meta, typename T>
int deserialize(
	archive<Istream, Meta> &is,
	std::shared_ptr<T> *obj
);



// unique pointers
/// Вспомогательная функция для сериализации std::unique_ptr
template<class Ostream, typename Meta, typename T>
int serialize(
	archive<Ostream, Meta> &os,
	std::unique_ptr<T> const *obj,
	bool write = true
);

/// Вспомогательная функция для десериализации std::unique_ptr
template<class Istream, typename Meta, typename T>
int deserialize(
	archive<Istream, Meta> &is,
	std::unique_ptr<T> *obj
);

/*! @} */





/************************ STD-STRUCTS ***********************/
/* STD-STRUCTS */
/*!
 * \defgroup std_serialization Стандартные контейнеры и структуры
 * \ingroup serialization_functions
 *
 * Эти функции предназначены для сериализации стандартных
 * контейнеров и структур, а именно:
 *
 * - `pair`
 * - `vector`
 * - `list`
 * - `set`
 * - `multiset`
 * - `unordered_set`
 * - `unordered_multiset`
 * - `map`
 * - `multimap`
 * - `unordered_map`
 * - `unordered_multimap`
 *
 * Каждая функция (де)сериализации устроена по единому образцу:
 *
 * \param os — архив, с помощью которого будет производится
 * (де)сериализация
 *
 * \param obj — объект, который необходимо (де)сериализовать
 *
 * \param [write] — аргумент только для функций сериализации;
 * говорит о том, действительно ли нужно сеарилизовать объект
 * или же только возвратить необходимый для сериализации
 * размер
 * @{
 */

/// Сериализация пары значений std::pair
template<class Ostream, typename Meta, typename T, typename U>
int serialize(
	archive<Ostream, Meta> &os,
	std::pair<T, U> const *p,
	bool write = true
);

/// Десериализация пары значений std::pair
template<class Istream, typename Meta, typename T, typename U>
int deserialize(
	archive<Istream, Meta> &is,
	std::pair<T, U> *p
);





/* CONTAINERS */

/// Вспомогательная функция для сериализации контейнеров
/*!
 * Функция принимает указатель на контейнер, который
 * должен предоставлять следующие методы:
 *
 * - size() -> int32_t — размер контейнера; возвращаемое значение
 *   должно быть преобразуемо к int32_t
 * - begin() — доступ к итератору, установленному на начало
 * - end() — доступ к итератору, установленному на конец
 *
 * Соответственно, итераторы, получаемые с помощью методов
 * begin и end, должны предоставлять следующие возможности:
 *
 * - T &operator*() — получение элемента
 * - operator++() — переход на следующих элемент
 * - bool operator!=(Iterator rhs) — сравнение с другим итератором
 */
template<class Ostream, typename Meta, class Container>
int serialize_container(
	archive<Ostream, Meta> &os,
	Container const *cont,
	bool write = true
);


/// Вспомогательная функция для десериализации контейнеров,
/// способных изменять размер с помощью функции resize()
/*!
 * Контейнер должен реализовывать следующие функции:
 *
 * - resize(int32_t) — задание размера контейнера
 * - begin() — доступ к итератору, установленному на начало
 * - end() — доступ к итератору, установленному на конец
 *
 * Соответственно, итераторы, получаемые с помощью методов
 * begin и end, должны предоставлять следующие возможности:
 *
 * - T &operator*() — получение элемента
 * - operator++() — переход на следующих элемент
 * - bool operator!=(Iterator rhs) — сравнение с другим итератором
 */
template<class Istream, typename Meta, class ResizableContainer>
int deserialize_resizable_container(
	archive<Istream, Meta> &is,
	ResizableContainer *cont
);



/// Вспомогательная функция для десериализации контейнеров,
/// в которые необходимо вставлять элементы

/*!
 * Контейнер должен реализовывать следующие функции:
 *
 * - clear() — удаление всех элементов контейнера
 * - insert(T) — добавление элемента в контейнер
 * - begin() — доступ к итератору, установленному на начало
 * - end() — доступ к итератору, установленному на конец
 *
 * Соответственно, итераторы, получаемые с помощью методов
 * begin и end, должны предоставлять следующие возможности:
 *
 * - T &operator*() — получение элемента
 * - operator++() — переход на следующих элемент
 * - bool operator!=(Iterator rhs) — сравнение с другим итератором
 */

template<
	typename T,
	class Istream,
	typename Meta,
	class Cont
>
int deserialize_inserted_container(
	archive<Istream, Meta> &is,
	Cont *cont
);





_NVX_SERIALIZABLE_RESIZABLE_CONTANER_DECLARE(std::basic_string);

_NVX_SERIALIZABLE_RESIZABLE_CONTANER_DECLARE(std::vector);
_NVX_SERIALIZABLE_RESIZABLE_CONTANER_DECLARE(std::list);

_NVX_SERIALIZABLE_INSERTED_CONTANER_DECLARE(std::set);
_NVX_SERIALIZABLE_INSERTED_CONTANER_DECLARE(std::map);
_NVX_SERIALIZABLE_INSERTED_CONTANER_DECLARE(std::multiset);
_NVX_SERIALIZABLE_INSERTED_CONTANER_DECLARE(std::multimap);

_NVX_SERIALIZABLE_INSERTED_CONTANER_DECLARE(std::unordered_set);
_NVX_SERIALIZABLE_INSERTED_CONTANER_DECLARE(std::unordered_map);
_NVX_SERIALIZABLE_INSERTED_CONTANER_DECLARE(std::unordered_multiset);
_NVX_SERIALIZABLE_INSERTED_CONTANER_DECLARE(std::unordered_multimap);










/* DEFINITIONS */
// main
template<class Ostream, typename Meta, typename T>
int serialize(
	archive<Ostream, Meta> &os,
	T const *value,
	bool write
)
{
	return _serialize_dispatcher(
		os, value,
		typename std::is_pointer<T>::type(),
		write
	);
}

template<class Istream, typename Meta, typename T>
int deserialize(
	archive<Istream, Meta> &is,
	T *value
)
{
	return _deserialize_dispatcher(
		is, value,
		typename std::is_pointer<T>::type()
	);
}





/****************** SERIALIZATION TO STRING *****************/
template<typename T>
int serialize(std::string &src, T const *value, int mode)
{
	std::stringstream ss;
	archive<decltype(ss)> arch(&ss, mode);

	int res = serialize(arch, value);
	src = std::move(ss.str());
	return res;
}

template<typename T>
std::string serialize(T const *value, int mode)
{
	std::stringstream ss;
	archive<decltype(ss)> arch(&ss, mode);

	serialize(arch, value);
	return ss.str();
}

template<typename T>
int deserialize(std::string const &src, T *value, int mode)
{
	std::stringstream ss(src);
	archive<decltype(ss)> arch(&ss, mode);

	return deserialize(arch, value);
}

template<typename T>
int deserialize( std::string &&src, T *value, int mode )
{
	std::stringstream ss(std::move(src));
	archive<decltype(ss)> arch(&ss, mode);

	return deserialize(arch, value);
}



// to string dynamic array
template<typename T>
int serialize_array(
	std::string &src,
	T const * const *value,
	int const *size,
	int mode
)
{
	std::stringstream ss;
	archive<decltype(ss)> arch(&ss, mode);

	int res = serialize(arch, value, size);
	src = std::move(ss.str());
	return res;
}

template<typename T>
std::string serialize_array(
	T const * const *value,
	int const *size,
	int mode
)
{
	std::stringstream ss;
	archive<decltype(ss)> arch(&ss, mode);

	serialize(arch, value, size);
	return ss.str();
}


template<typename T>
int deserialize_array(
	std::string const &src,
	T **value,
	int *size,
	int mode
)
{
	std::stringstream ss(src);
	archive<decltype(ss)> arch(&ss, mode);

	return deserialize(arch, value, size);
}

template<typename T>
int deserialize_array(
	std::string &&src,
	T **value,
	int *size,
	int mode
)
{
	std::stringstream ss(std::move(src));
	archive<decltype(ss)> arch(&ss, mode);

	return deserialize(arch, value, size);
}



// to string static array
template<typename T>
int serialize_static(
	std::string &src,
	T const *value,
	int size,
	int mode
)
{
	std::stringstream ss;
	archive<decltype(ss)> arch(&ss, mode);

	int res = serialize_static(arch, value, size);
	src = std::move(ss.str());
	return res;
}

template<typename T>
std::string serialize_static(
	T const *value,
	int size,
	int mode
)
{
	std::stringstream ss;
	archive<decltype(ss)> arch(&ss, mode);

	serialize_static(arch, value, size);
	return ss.str();
}


template<typename T>
int deserialize_static(
	std::string const &src,
	T *value,
	int size,
	int mode
)
{
	std::stringstream ss(src);
	archive<decltype(ss)> arch(&ss, mode);

	return deserialize_static(arch, value, size);
}

template<typename T>
int deserialize_static(
	std::string &&src,
	T *value,
	int size,
	int mode
)
{
	std::stringstream ss(std::move(src));
	archive<decltype(ss)> arch(&ss, mode);

	return deserialize_static(arch, value, size);
}





// dynamic arrays
template<class Ostream, typename Meta, typename T>
int serialize_array(
	archive<Ostream, Meta> &os,
	T const * const *value,
	int const *size,
	bool write
)
{
	int res = serialize(os, size, write);

	if(!*size)
		return res;

	for(auto *b = *value, *e = *value+*size; b != e; ++b)
		res += serialize(os, b, write);

	return res;
}

template<class Istream, typename Meta, typename T>
int deserialize_array(
	archive<Istream, Meta> &is,
	T **value,
	int *sizeptr
)
{
	int32_t size;
	int res = deserialize(is, &size);
	*sizeptr = size;

	if(!size)
	{
		*value = nullptr;
		return res;
	}

	*value = new T[size];
	for(auto *b = *value, *e = *value+size; b != e; ++b)
		res += deserialize(is, b);

	return res;
}





// static array
template<class Ostream, typename Meta, typename T>
int serialize_static(
	archive<Ostream, Meta> &os,
	T const * value,
	int size,
	bool write
)
{
	int res = 0;
	for(auto *b = value, *e = value+size; b != e; ++b)
		res += serialize(os, b, write);
	return res;
}

template<class Istream, typename Meta, typename T>
int deserialize_static(
	archive<Istream, Meta> &is,
	T *value,
	int size
)
{
	int res = 0;
	for(auto *b = value, *e = value+size; b != e; ++b)
		res += deserialize(is, b);
	return res;
}





// plain
template<class Ostream, typename Meta>
int serialize_plain(
	archive<Ostream, Meta> &os,
	void const *obj,
	int size,
	bool write
)
{
	if(!write)
		return size / sizeof(char);
	os.s->write( (char const *)obj, size / sizeof(char) );
	return os ? size : 0;
}

template<class Istream, typename Meta>
int deserialize_plain(
	archive<Istream, Meta> &is,
	void *obj,
	int size
)
{
	is.s->read( (char *)obj, size / sizeof(char) );
	return is ? size : 0;
}





// final
template<class Ostream, typename Meta, typename T>
int _serialize_final(
	archive<Ostream, Meta> &os,
	T const *value,
	std::true_type isfundamental,
	bool write
)
{
	if(!write)
		return sizeof *value;
	os.s->write( (char const *)value, sizeof *value );
	return (bool)os ? sizeof *value : 0;
}

template<class Istream, typename Meta, typename T>
int _deserialize_final(
	archive<Istream, Meta> &is,
	T *value,
	std::true_type isfundamental
)
{
	is.s->read( (char *)value, sizeof *value );
	return (bool)is ? sizeof *value : 0;
}



template<class Ostream, typename Meta, typename T>
int _serialize_final(
	archive<Ostream, Meta> &os,
	T const *value,
	std::false_type isfundamental,
	bool write
)
{
	return value->serialize(os, write);
}

template<class Ostream, typename Meta, typename T>
int _deserialize_final(
	archive<Ostream, Meta> &os,
	T *value,
	std::false_type isfundamental
)
{
	return value->deserialize(os);
}





// pointer
template<class Ostream, typename Meta, typename T>
int _serialize_dispatcher(
	archive<Ostream, Meta> &os,
	T const *obj,
	std::true_type,
	bool write
)
{
	if( !(os.mode & determine_pointers_mode) )
	{
		byte check = 0;

		if(*obj == nullptr)
			return serialize(os, &check, write);

		check = 1;
		return serialize(os, &check, write) + serialize(os, *obj, write);
	}

	if(!write && !os.lira)
		throw "Can't do not write with deterime pointers and no lira";

	if(!write)
	{
		id_t tmp;
		return serialize(os, &tmp, false);
	}

	if(*obj == nullptr)
		return serialize(os, &NULL_ID);

	auto it = os.objs.find(*obj);
	if(it != os.objs.end())
	{
		int res = serialize(os, &it->second.first);

		if(os.lira)
			++os.lira->objs[it->second.first].pc;

		if(os.freshness > it->second.second and os.lira)
		{
			it->second.second = os.freshness;
			int p = os.s->tellp();
			os.lira->_put(it->second.first, *obj, 2);
			os.s->seekp(p);
		}

		return res;
	}

	id_t id = os.newid();
	int res = serialize(os, &id);

	os.objs[*obj] = { id, os.freshness };
	os.idns[id]  = { *obj, *obj };

	if(!os.lira)
		return res + serialize(os, *obj);

	os.lira->shps[os.curid].insert(id);
	int p = os.s->tellp();
	int curid = os.curid;
	os.lira->_put(id, *obj, 2);
	os.curid = curid;
	++os.lira->objs[id].pc;
	os.s->seekp(p);
	return res;
}

template<class Ostream, typename Meta, typename T>
inline int _serialize_dispatcher(
	archive<Ostream, Meta> &os,
	T const *obj,
	std::false_type,
	bool write
)
{
	return _serialize_final(os, obj, typename std::is_fundamental<T>::type(), write);
}

template<class Istream, typename Meta, typename T>
int _deserialize_dispatcher(
	archive<Istream, Meta> &is,
	T *obj,
	std::true_type
)
{
	if( !(is.mode & determine_pointers_mode) )
	{
		byte check = 0;

		int res = deserialize(is, &check);
		if(!check)
		{
			*obj = nullptr;
			return res;
		}

		*obj = new typename std::remove_pointer<T>::type;
		res += deserialize(is, *obj);
		return res;
	}

	id_t id = NULL_ID;
	int res = deserialize(is, &id);

	if(id == NULL_ID)
	{
		*obj = nullptr;
		return res;
	}

	if(auto it = is.idns.find(id); it != is.idns.end())
	{
		*obj = std::any_cast<T>(it->second.second);
		return res;
	}

	*obj = new typename std::remove_pointer<T>::type;
	is.idns[id] = { *obj, *obj };
	is.objs[*obj] = { id, is.freshness };

	if(is.lira)
	{
		// TODO: сделать широкий поиск вместо глубокого
		int p = is.s->tellg();
		is.lira->get(id, *obj);
		is.s->seekg(p);
	}
	else
	{
		res += deserialize(is, *obj);
	}

	return res;
}

template<class Ostream, typename Meta, typename T>
inline int _deserialize_dispatcher(
	archive<Ostream, Meta> &os,
	T *obj,
	std::false_type
)
{
	return _deserialize_final(os, obj, typename std::is_fundamental<T>::type());
}





// shared pointers
template<class Ostream, typename Meta, typename T>
int serialize(
	archive<Ostream, Meta> &os,
	std::shared_ptr<T> const *obj,
	bool write
)
{
	/*
	 * Если разделяемые указатели не поддерживаются, просто
	 * записываем данные (добавляем также проверочный байт,
	 * который сообщит, если указатель нулевой)
	 */
	if( !(os.mode & determine_shared_mode) )
	{
		byte check = 0;

		if(obj->get() == nullptr)
			return serialize(os, &check, write);

		check = 1;
		return serialize(os, &check, write) + serialize(os, obj->get(), write);
	}

	if(!write && !os.lira)
		throw "Can't do not write when deterimne shared mode on and no lira";

	/*
	 * Если необходимо только посчитать число записанных
	 * байт, то (раз Лира есть) это всего лишь один
	 * идентификатор
	 */
	if(!write)
	{
		id_t tmp;
		return serialize(os, &tmp, false);
	}

	/*
	 * Если объект нулевой, то записываем нулевой id
	 */
	if(!obj->get())
		return serialize(os, &NULL_ID);

	/*
	 * Если объект уже был сериализован ранее, то
	 * проверям, может быть, его нужно обновить;
	 * обновление поддерживается, только если есть Лира
	 */
	auto it = os.objs.find(obj->get());
	if(it != os.objs.end())
	{
		int res = serialize(os, &it->second.first);

		if(os.lira)
			++os.lira->objs[it->second.first].pc;

		if(os.freshness > it->second.second and os.lira)
		{
			it->second.second = os.freshness;
			int p = os.s->tellp();
			os.lira->_put(it->second.first, obj->get(), 2);
			os.s->seekp(p);
		}

		return res;
	}

	/*
	 * Если объект новый, то присваиваем ему уникальный
	 * идентификатор и добавляем в имеющиеся
	 */
	id_t id = os.newid();
	int res = serialize(os, &id);

	os.objs[obj->get()] = { id, os.freshness };
	os.idns[id] = { obj->get(), *obj };

	/*
	 * Если Лиры нет, то мы просто записываем новый
	 * объект сплошняком
	 */
	if(!os.lira)
		return res + serialize(os, obj->get());

	/*
	 * Если же Лира есть, то добавляем новый объект
	 * через неё
	 */
	os.lira->shps[os.curid].insert(id);
	int p = os.s->tellp();
	int curid = os.curid;
	os.lira->_put(id, obj->get(), 2);
	os.curid = curid;
	++os.lira->objs[id].pc;
	os.s->seekp(p);
	return res;
}

/// Вспомогательная функция для десериализации std::shared_pointer
/*!
 * Вспомогательная функция для десериализации,
 * если включено разпознавание разделяемых
 * shared_pointer (т.е. установлен флаг
 * determine_shared_mode), то функция ведёт
 * себя особым образом
 */
template<class Istream, typename Meta, typename T>
int deserialize(
	archive<Istream, Meta> &is,
	std::shared_ptr<T> *obj
)
{
	/*
	 * Если разделяемые указатели не поддерживаются, то
	 * просто считываем объект, как часть существующего,
	 * не забыв проверить, не ноль ли он
	 */
	if( !(is.mode & determine_shared_mode) )
	{
		byte check = 0;

		int res = deserialize(is, &check);
		if(!check)
		{
			*obj = std::shared_ptr<T>(nullptr);
			return res;
		}

		*obj = std::shared_ptr<T>(new T);
		res += deserialize(is, obj->get());
		return res;
	}

	/*
	 * Если же разделяемые указатели поддерживаются, то
	 * они всегда характеризуются уникальным идентификатором;
	 * считываем его; проверяем, но ноль ли он
	 */
	id_t id = NULL_ID;
	int res = deserialize(is, &id);

	if(id == NULL_ID)
	{
		*obj = std::shared_ptr<T>(nullptr);
		return res;
	}

	/*
	 * Если объект присутствует в оперативной
	 * памяти, то просто возвращаем его
	 */
	if(auto it = is.idns.find(id); it != is.idns.end())
	{
		*obj = std::any_cast<std::shared_ptr<T>>(it->second.second);
		return res;
	}

	/*
	 * Если же объекта в оперативной памяти нет,
	 * то его нужно добавить, а затем считать
	 * (именно в таком порядке! иначе при цикли-
	 * ческих ссылках сериализация зациклится
	 * в бесконечность и будет переполнение стека)
	 */
	*obj = std::shared_ptr<T>(new T);
	is.idns[id] = { obj->get(), *obj };
	is.objs[obj->get()] = { id, is.freshness };

	/*
	 * Если Лира есть, то получаем объект через неё;
	 * иначе объект находится здесь же, просто
	 * считываем его
	 */
	if(is.lira)
	{
		// TODO: сделать широкий поиск вместо глубокого
		int p = is.s->tellg();
		is.lira->get(id, obj->get());
		is.s->seekg(p);
	}
	else
	{
		res += deserialize(is, obj->get());
	}

	return res;
}



// unique pointers
template<class Ostream, typename Meta, typename T>
int serialize(
	archive<Ostream, Meta> &os,
	std::unique_ptr<T> const *obj,
	bool write
)
{
	byte check = 0;
	if(obj->get() == nullptr)
		return serialize(os, &check, write);

	check = 1;
	return serialize(os, &check, write) + serialize(os, obj->get(), write);
}

template<class Istream, typename Meta, typename T>
int deserialize(
	archive<Istream, Meta> &is,
	std::unique_ptr<T> *obj
)
{
	byte check = 0;

	int res = deserialize(is, &check);
	if(!check)
	{
		*obj = std::unique_ptr<T>(nullptr);
		return res;
	}

	*obj = std::unique_ptr<T>(new T);
	res += deserialize(is, obj->get());
	return res;
}





/* STD-STRUCTS */

template<
	class Ostream,
	typename Meta,
	typename T,
	typename U
>
int serialize(
	archive<Ostream, Meta> &os,
	std::pair<T, U> const *p,
	bool write
)
{
	return serialize(os, &p->first, write) + serialize(os, &p->second, write);
}

template<
	class Istream,
	typename Meta,
	typename T,
	typename U
>
int deserialize(
	archive<Istream, Meta> &is,
	std::pair<T, U> *p
)
{
	return deserialize(is, &p->first) + deserialize(is, &p->second);
}





/* CONTAINERS */
template<
	class Ostream,
	typename Meta,
	class Container
>
int serialize_container(
	archive<Ostream, Meta> &os,
	Container const *cont,
	bool write
)
{
	int res = 0;
	int32_t size = cont->size();

	if( !(res = serialize(os, &size, write)) )
		return 0;

	for(auto b = cont->begin(), e = cont->end(); b != e; ++b)
		res += serialize(os, &*b, write);

	return res;
}



template<
	class Istream,
	typename Meta,
	class ResizableContainer
>
int deserialize_resizable_container(
	archive<Istream, Meta> &is,
	ResizableContainer *cont
)
{
	int res = 0;
	int32_t size;

	if( !(res = deserialize(is, &size)) )
		return 0;

	cont->resize(size);
	for(auto b = cont->begin(), e = cont->end(); b != e; ++b)
		res += deserialize(is, &*b);

	return res;
}



template<typename T>
struct remove_const_from_pair
{
	typedef typename std::remove_const<T>::type type;
};

template<typename T, typename U>
struct remove_const_from_pair<std::pair<T, U>>
{
	typedef std::pair<
		typename std::remove_const<T>::type,
		typename std::remove_const<U>::type
	> type;
};

template<
	class Istream,
	typename Meta,
	class Cont
>
int deserialize_inserted_container(
	archive<Istream, Meta> &is,
	Cont *cont
)
{
	typedef typename remove_const_from_pair<
		typename std::remove_reference<decltype(*cont->begin())>::type
	>::type obj_t;

	int res = 0;
	int32_t size;

	if( !(res = deserialize(is, &size)) )
		return 0;

	cont->clear();

	for(int32_t i = 0; i < size; ++i)
	{
		obj_t obj;
		res += deserialize(is, &obj);
		cont->insert(std::move(obj));
	}

	return res;
}





/******************** STANDART CONTAINERS *******************/

_NVX_SERIALIZABLE_RESIZABLE_CONTANER_DEFINE(std::basic_string);

_NVX_SERIALIZABLE_RESIZABLE_CONTANER_DEFINE(std::vector);
_NVX_SERIALIZABLE_RESIZABLE_CONTANER_DEFINE(std::list);

_NVX_SERIALIZABLE_INSERTED_CONTANER_DEFINE(std::set);
_NVX_SERIALIZABLE_INSERTED_CONTANER_DEFINE(std::map);
_NVX_SERIALIZABLE_INSERTED_CONTANER_DEFINE(std::multiset);
_NVX_SERIALIZABLE_INSERTED_CONTANER_DEFINE(std::multimap);

_NVX_SERIALIZABLE_INSERTED_CONTANER_DEFINE(std::unordered_set);
_NVX_SERIALIZABLE_INSERTED_CONTANER_DEFINE(std::unordered_map);
_NVX_SERIALIZABLE_INSERTED_CONTANER_DEFINE(std::unordered_multiset);
_NVX_SERIALIZABLE_INSERTED_CONTANER_DEFINE(std::unordered_multimap);


/*! @} */










/* LIRA */

struct _LiraPlace
{
	NVX_SERIALIZABLE_PLAIN();

	int p; // pos
	int s; // size

	bool operator<(_LiraPlace const &rhs) const
	{
		return s == rhs.s ? p < rhs.p : s < rhs.s;
	}
};

template<typename Meta>
struct _LiraObject
{
	typedef Meta meta_t;

	NVX_SERIALIZABLE(&pl, &cat, &pc, &meta);

	_LiraPlace pl;
	int cat; // category
	int pc;  // count of pointer that point to this

	meta_t meta = meta_t();
};

template<>
struct _LiraObject<void>
{
	NVX_SERIALIZABLE_PLAIN();

	_LiraPlace pl;
	int cat;
	int pc;
};



template<typename Meta = void>
class Lira
{
	typedef _LiraObject<Meta> object_t;
	typedef _LiraPlace place_t;

public:
	typedef Meta meta_t;

	enum Mode
	{
		none,
		recursive // default
	};



	Lira(
		std::iostream *ios,
		Mode mode = recursive,
		int idstart = 1024,
		int maxsize = int_max/2
	):
		mode(mode),
		maxid(idstart),
		ios(ios),
		fpls({ { 0, maxsize } }),
		arch(&ios)
	{
		arch.lira = this;
		return;
	}

	Lira(
		std::iostream *ios,
		std::iostream *head,
		Mode mode = recursive,
		int idstart = 1024,
		int maxsize = int_max/2
	):
		mode(mode),
		maxid(idstart),
		ios(ios),
		head(head),
		fpls({ { 0, maxsize } }),
		arch(&ios)
	{
		read_head(*head);
		head->clear();
		arch.lira = this;
		return;
	}

	Lira(
		char const *filename,
		Mode mode = recursive,
		int idstart = 1024,
		int maxsize = int_max/2
	):
		mode(mode),
		maxid(idstart),
		iosown(true),
		fpls({ { 0, maxsize } }),
		arch(nullptr)
	{
		ios = new std::fstream;
		open_io_file((std::fstream *)ios, filename);
		arch.s = ios;
		arch.lira = this;
		return;
	}

	Lira(
		char const *filename,
		char const *headfilename,
		Mode mode = recursive,
		int idstart = 1024,
		int maxsize = int_max/2
	):
		mode(mode),
		maxid(idstart),
		iosown(true),
		headown(true),
		fpls({ { 0, maxsize } }),
		arch(nullptr)
	{
		ios = new std::fstream;
		open_io_file((std::fstream *)ios, filename);

		head = new std::fstream;
		open_io_file((std::fstream *)head, headfilename);
		read_head(*head);
		head->clear();

		arch.s = ios;
		arch.lira = this;
		return;
	}

	~Lira()
	{
		if(iosown and ios)
			delete ios;

		if(head)
		{
			head->seekp(0);
			write_head(*head);
		}

		if(headown and head)
			delete head;

		return;
	}

	std::iostream *stream() const
	{
		return ios;
	}



	/*
	 * HEAD
	 */
	template<typename Istream>
	inline bool read_head(Istream &is)
	{
		bool res = (bool)(archive<Istream>(&is) >> &fpls >> &objs >> &cats >> &shps >> &stoid);
		if(!objs.empty())
			maxid = std::max( (prev(objs.end()))->first + 1, maxid ),
			shrid = objs.begin()->first - 1;
		return res;
	}

	template<typename Ostream>
	inline bool write_head(Ostream &os) const
	{
		return (bool)(archive<Ostream>(&os) << &fpls << &objs << &cats << &shps << &stoid);
	}

	inline bool read_head(char const *s)
	{
		std::ifstream fin(s);
		return read_head(fin);
	}

	inline bool write_head(char const *s) const
	{
		std::ofstream fout(s);
		return write_head(fout);
	}



	// head with additional information
	template<typename Istream, typename Add>
	inline bool read_head(Istream &is, Add *add)
	{
		bool res = (bool)(archive<Istream>(&is) >> &fpls >> &objs >> &cats >> &shps >> &stoid >> add);
		if(!objs.empty())
			maxid = std::max((--objs.end())->first + 1, maxid),
			shrid = objs.begin()->first - 1;
		return res;
	}

	template<typename Ostream, typename Add>
	inline bool write_head(Ostream &os, Add const *add) const
	{
		return (bool)(archive<Ostream>(&os) << &fpls << &objs << &cats << &shps << &stoid << add);
	}

	template<typename Add>
	inline bool read_head(char const *s, Add *add)
	{
		std::ifstream fin(s);
		return read_head(fin, add);
	}

	template<typename Add>
	inline bool write_head(char const *s, Add const *add) const
	{
		std::ofstream fout(s);
		return write_head(fout, add);
	}





	/*
	 * PUT, GET, DEL
	 */
	// put
	template<typename T>
	int put(T const *o, int cat = '\0')
	{
		int id = next_id();
		put(id, o, cat);
		return id;
	}

	template<typename T>
	void put(int id, T const *o, int cat = '\0')
	{
		if(mode == recursive)
			++arch.freshness;
		_put(id, o, cat);
		return;
	}

	template<typename T>
	int put(std::string const &id, T const *o, int cat = '\0')
	{
		auto it = stoid.find(id);
		if(it != stoid.end())
		{
			put(it->second, o, cat);
			return it->second;
		}
		int iid = next_id();
		stoid[id] = iid;
		put(iid, o, cat);
		return iid;
	}

	template<typename T>
	int put(std::shared_ptr<T> const *o, int cat = '\0')
	{
		int id = next_id();
		put(id, o, cat);
		return id;
	}

	template<typename T>
	void put(int id, std::shared_ptr<T> const *o, int cat = '\0')
	{
		if(mode == recursive)
			++arch.freshness;
		_put_first(id, o, cat);
		return;
	}



	// get
	template<typename T>
	bool get(int id, T *o) const
	{
		auto it = objs.find(id);
		if(it == objs.end())
			return false;
		ios->seekg(it->second.pl.p);
		deserialize(arch, o);
		return true;
	}

	template<typename T>
	bool get(int id, T *o, T const &def) const
	{
		if(get(id, o))
			return true;
		*o = def;
		return false;
	}

	template<typename T>
	inline T get(int id, bool *ok = nullptr) const
	{
		T t;
		ok ? *ok = get(id, &t) : get(id, &t);
		return t;
	}


	template<typename T>
	bool get(std::string const &id, T *o) const
	{
		auto it = stoid.find(id);
		if(it == stoid.end())
			return false;
		return get(it->second, o);
	}

	template<typename T>
	bool get(std::string const &id, T *o, T const &def) const
	{
		auto it = stoid.find(id);
		if(it == stoid.end())
		{
			*o = def;
			return false;
		}
		return get(it->second, o, def);
	}

	template<typename T>
	inline T get(std::string const &id, bool *ok = nullptr) const // TODO: get(3, bool *)
	{
		auto it = stoid.find(id);
		if(it == stoid.end())
		{
			if(ok) *ok = false;
			return T();
		}
		return get(it->second, ok);
	}



	// del
	bool del(int id)
	{
		auto objit = objs.find(id);
		if(objit == objs.end())
			return false;

		int cat = objit->second.cat;
		place_t o = objit->second.pl;
		objs.erase(objit);

		cats[cat].erase(id);

		if(auto it = arch.idns.find(id); it != arch.idns.end())
		{
			arch.objs.erase( arch.objs.find(it->second.first) );
			arch.idns.erase( it );
		}

		if(auto it = shps.find(id); it != shps.end())
		{
			for(int shid : it->second)
			{
				if(!--objs[shid].pc and shid < 0)
					del(shid);
			}
			shps.erase(it);
		}

		return _free(o);
	}

	bool del(std::string const &sid)
	{
		auto it = stoid.find(sid);
		if(it == stoid.end())
			return false;
		return del(it->second);
	}



	/*
	 * CATEGORY
	 */
	std::set<int> const &operator[](int cat) const
	{
		static std::set<int> const empty;

		auto it = cats.find(cat);
		if(it == cats.end())
			return empty;
		return it->second;
	}



private:
	template<typename Stream, typename M>
	friend class archive;

	inline int next_shared_id()
	{
		return shrid--;
	}

	inline int next_id()
	{
		return maxid++;
	}

	template<typename MetaType>
	friend struct _LiraObject;

	template<typename MetaType>
	friend void meta(Lira &u, int id, MetaType const &m)
	{
		u.objs[id].meta = m;
		return;
	}

	friend Meta meta(Lira &u, int id)
	{
		return u.objs[id].meta;
	}

	template<typename T>
	void _put(int id, T const *o, int cat = '\0')
	{
		maxid = std::max(maxid, id+1);

		// clear old
		auto it = objs.find(id);
		if(it != objs.end())
		{
			_free(it->second.pl);
		}

		// get free place
		int s = serialize(arch, o, false);
		place_t fp = _malloc(s);

		arch.curid = id;
		ios->seekp(fp.p);

		std::set<int> shpsidns;
		auto oldshps = it != objs.end() ?
			shps.find(id) : shps.end();
		if(oldshps != shps.end())
			shpsidns = std::move(oldshps->second),
			shps.erase(oldshps);

		serialize(arch, o);

		if(it == objs.end())
		{
			objs[id] = { fp, cat, 0 };
			cats[cat].insert(id);
			return;
		}

		it->second.pl = fp;
		if(cat != it->second.cat)
		{
			cats[it->second.cat].erase(id);
			cats[cat].insert(id);
		}

		for(int shid : shpsidns)
		{
			if(!--objs[shid].pc and shid < 0)
				del(shid);
		}

		return;
	}

	template<typename T>
	inline void _put_first(int id, T const *o, int cat = '\0')
	{
		return _put(id, o, cat);
	}

	template<typename T>
	void _put_first(int id, std::shared_ptr<T> const *o, int cat = '\0')
	{
		if(auto it = arch.objs.find(o->get()); it != arch.objs.end())
			it->second.second = -1;
		return _put(id, o, cat);
	}


	place_t _malloc(int sz)
	{
		auto f = fpls.lower_bound({0, sz});
		if(f == fpls.end())
			throw "memory out";
		place_t fp = *f;
		fpls.erase(f);

		// justify fpls places
		if(sz != fp.s)
			fpls.insert({ fp.p + sz, fp.s - sz});

		fp.s = sz;
		return fp;
	}

	bool _free(place_t o)
	{
		fpls.insert(o);
		typename decltype(fpls)::iterator l, r;
		l = r = fpls.end();

		for(auto b = fpls.begin(), e = fpls.end(); b != e; ++b) // TODO: optimize
		{
			if(b->p + b->s == o.p)
				l = b;
			else if(o.p + o.s == b->p)
				r = b;
		}

		if( l != fpls.end() )
		{
			place_t lo = *l;
			fpls.erase(l);
			fpls.erase(o);
			o.p = lo.p;
			o.s = lo.s + o.s;
			fpls.insert(o);
		}

		if( r != fpls.end() )
		{
			place_t ro = *r;
			fpls.erase(r);
			fpls.erase(o);
			o.s += ro.s;
			fpls.insert(o);
		}

		return true;
	}


	Mode mode = recursive;

	int maxid    = 0;
	int shrid    = -1;

	bool iosown  = false;
	bool headown = false;

	std::iostream    *ios  = nullptr;
	std::iostream    *head = nullptr;

	std::set<place_t>            fpls; // free spaces
	std::map<int, object_t>      objs; // objects in file
	std::map<int, std::set<int>> cats; // categoryes
	std::map<int, std::set<int>> shps; // obj ---(shared_pointers)---> objs

	std::map<std::string, int> stoid;

	mutable archive<std::iostream> arch;






	// friends
	template<class Ostream, typename M, typename T>
	friend int serialize(
		archive<Ostream, M> &os,
		std::shared_ptr<T> const *obj,
		bool write
	);

	template<class Ostream, typename M, typename T>
	friend int _serialize_dispatcher(
		archive<Ostream, M> &os,
		T const *obj,
		std::true_type,
		bool write
	);


};





}





#endif // NVX_SERIALIZATION_HPP
