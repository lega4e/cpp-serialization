/* DEFINITIONS */
/*
 * autor:   lis
 * created: Oct  5 14:14:45 2020
 */





// main
template<class Ostream, typename T>
int serialize(archive<Ostream> &os, T const *value)
{
	return _serialize_dispatcher(
		os, value,
		typename std::is_pointer<T>::type()
	);
}

template<class Istream, typename T>
int deserialize(archive<Istream> &is, T *value)
{
	return _deserialize_dispatcher(
		is, value,
		typename std::is_pointer<T>::type()
	);
}





// to string
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
int serialize(
	std::string &src,
	T const * const *value,
	int *size,
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
std::string serialize(
	T const * const *value,
	int *size,
	int mode
)
{
	std::stringstream ss;
	archive<decltype(ss)> arch(&ss, mode);

	serialize(arch, value, size);
	return ss.str();
}


template<typename T>
int deserialize(
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
int deserialize(
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
template<class Ostream, typename T>
int serialize(archive<Ostream> &os, T const * const *value, int const *size)
{
	int res = serialize(os, size);

	if(!*size)
		return res;

	for(auto *b = *value, *e = *value+*size; b != e; ++b)
		res += serialize(os, b);

	return res;
}

template<class Istream, typename T>
int deserialize(archive<Istream> &is, T **value, int *sizeptr)
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
template<class Ostream, typename T>
int serialize_static(archive<Ostream> &os, T const * value, int size)
{
	int res = 0;
	for(auto *b = value, *e = value+size; b != e; ++b)
		res += serialize(os, b);
	return res;
}

template<class Istream, typename T>
int deserialize_static(archive<Istream> &is, T *value, int size)
{
	int res = 0;
	for(auto *b = value, *e = value+size; b != e; ++b)
		res += deserialize(is, b);
	return res;
}





// plain
template<class Ostream>
int serialize_plain(archive<Ostream> &os, void const *obj, int size)
{
	os.s->write( (char const *)obj, size / sizeof(char) );
	return os ? size : 0;
}

template<class Istream>
int deserialize_plain(archive<Istream> &is, void *obj, int size)
{
	is.s->read( (char *)obj, size / sizeof(char) );
	return is ? size : 0;
}





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
)
{
	int32_t size = str->size();
	return
		serialize(os, &size) +
		serialize_plain( os, (void *)str->data(), sizeof(*str->data())*size );
}

template<
	class Istream, 
	typename CharType,
	class CharTraitsType,
	class Alloc
>
int deserialize(
	archive<Istream> &is,
	std::basic_string<CharType, CharTraitsType, Alloc> *str
)
{
	int32_t size;
	int res = deserialize( is, &size );
	str->resize(size);
	res += deserialize_plain( is, str->data(), sizeof(*str->data()) * size );
	return res;
}





// final
template<class Ostream, typename T>
int _serialize_final(archive<Ostream> &os, T const *value, std::true_type isfundamental)
{
	os.s->write( (char const *)value, sizeof *value );
	return (bool)os.s ? sizeof *value : 0;
}

template<class Istream, typename T>
int _deserialize_final(archive<Istream> &is, T *value, std::true_type isfundamental)
{
	is.s->read( (char *)value, sizeof *value );
	return (bool)is.s ? sizeof *value : 0; 
}



template<class Ostream, typename T>
int _serialize_final(archive<Ostream> &os, T const *value, std::false_type isfundamental)
{
	return value->serialize(os);
}

template<class Ostream, typename T>
int _deserialize_final(archive<Ostream> &os, T *value, std::false_type isfundamental)
{
	return value->deserialize(os);
}





// pointer
template<class Ostream, typename T>
int _serialize_dispatcher( archive<Ostream> &os, T const *obj, std::true_type )
{
	if( !(os.mode & determine_pointers_mode) )
	{
		int8_t check = 0;

		if(*obj == nullptr)
			return serialize(os, &check);

		check = 1;
		return serialize(os, &check) + serialize(os, *obj);
	}

	int64_t ptr = (int64_t)*obj;
	int res = serialize(os, &ptr);

	if(!ptr)
		return res;

	auto it = os.map.find(ptr);
	if(it != os.map.end())
		return res;

	os.map.insert({ptr, *obj});
	return res + serialize(os, *obj);
}

template<class Ostream, typename T>
inline int _serialize_dispatcher( archive<Ostream> &os, T const *obj, std::false_type )
{
	return _serialize_final(os, obj, typename std::is_fundamental<T>::type());
}

template<class Istream, typename T>
int _deserialize_dispatcher( archive<Istream> &is, T *obj, std::true_type )
{
	if( !(is.mode & determine_pointers_mode) )
	{
		int8_t check = 0;

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

	int64_t ptr;
	int res = deserialize(is, &ptr);

	if(!ptr)
	{
		*obj = nullptr;
		return res;
	}

	auto it = is.map.find(ptr);

	if(it == is.map.end())
	{
		*obj = new typename std::remove_pointer<T>::type;
		res += deserialize(is, *obj);
		is.map.insert({ ptr, *obj });
	}
	else
	{
		*obj = std::any_cast<T>(it->second);
	}

	return res;
}

template<class Ostream, typename T>
inline int _deserialize_dispatcher( archive<Ostream> &os, T *obj, std::false_type )
{
	return _deserialize_final(os, obj, typename std::is_fundamental<T>::type());
}





// shared pointers
template<class Ostream, typename T>
int serialize( archive<Ostream> &os, std::shared_ptr<T> const *obj )
{
	if( !(os.mode & determine_shared_mode) )
	{
		int8_t check = 0;

		if(obj->get() == nullptr)
			return serialize(os, &check);

		check = 1;
		return serialize(os, &check) + serialize(os, obj->get());
	}

	int64_t ptr = (int64_t)obj->get();
	int res = serialize(os, &ptr);

	if(!ptr)
		return res;

	auto it = os.map.find(ptr);
	if(it != os.map.end())
		return res;

	os.map.insert({ptr, 0});
	return res + serialize(os, obj->get());
}

template<class Istream, typename T>
int deserialize( archive<Istream> &is, std::shared_ptr<T> *obj )
{
	if( !(is.mode & determine_shared_mode) )
	{
		int8_t check = 0;

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

	int64_t ptr;
	int res = deserialize(is, &ptr);
	
	if(!ptr)
	{
		*obj = std::shared_ptr<T>(nullptr);
		return res;
	}

	auto it = is.map.find(ptr);

	if(it == is.map.end())
	{
		*obj = std::shared_ptr<T>(new T);
		res += deserialize(is, obj->get());
		is.map.insert({ ptr, *obj });
	}
	else
	{
		*obj = std::any_cast<std::shared_ptr<T>>(it->second);
	}

	return res;
}





/* STD-STRUCTS */
template<class Ostream, typename T, typename U>
int serialize( archive<Ostream> &os, std::pair<T, U> const *p )
{
	return serialize(os, &p->first) + serialize(os, &p->second);
}

template<class Istream, typename T, typename U>
int deserialize( archive<Istream> &is, std::pair<T, U> *p )
{
	return deserialize(is, &p->first) + deserialize(is, &p->second);
}





/* CONTAINERS */
// common
/*
 * class Container:
 *     size();  -> int32_t
 *     begin();
 *     end();
 */
template<class Ostream, class Container>
int serialize_container(archive<Ostream> &os, Container const *cont)
{
	int res = 0;
	int32_t size = cont->size();

	if( !(res = serialize(os, &size)) )
		return 0;

	for(auto b = cont->begin(), e = cont->end(); b != e; ++b)
		res += serialize(os, &*b);

	return res;
}



/*
 * class ResizableContainer:
 *     resize(int32_t);
 *     begin();
 *     end();
 */
template<class Istream, class ResizableContainer>
int deserialize_resizable_container(archive<Istream> &is, ResizableContainer *cont)
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



template<typename T, class Istream, class Cont>
int deserialize_inserted_container(
	archive<Istream> &is,
	Cont *cont
)
{
	int res = 0;
	int32_t size;

	if( !(res = deserialize(is, &size)) )
		return 0;

	T obj;
	for(int32_t i = 0; i < size; ++i)
	{
		res += deserialize(is, &obj);
		cont->insert(obj);
	}

	return res;
}





// vector
template<class Ostream, typename T, typename Alloc>
int serialize(archive<Ostream> &os, std::vector<T, Alloc> const *cont)
{
	return serialize_container(os, cont);
}

template<class Istream, typename T, typename Alloc>
int deserialize(archive<Istream> &is, std::vector<T, Alloc> *cont)
{
	return deserialize_resizable_container(is, cont);
}





// list
template<class Ostream, typename T, typename Alloc>
int serialize(archive<Ostream> &os, std::list<T, Alloc> const *list)
{
	return serialize_container(os, list);
}

template<class Istream, typename T, typename Alloc>
int deserialize(archive<Istream> &is, std::list<T, Alloc> *list)
{
	return deserialize_resizable_container(is, list);
}





// set; TODO: balanced?
template<
	class Ostream,
	typename Key,
	typename Compare,
	typename Alloc
>
int serialize(
	archive<Ostream> &os,
	std::set<Key, Compare, Alloc> const *set
)
{
	return serialize_container(os, set);
}

template<
	class Istream,
	typename Key,
	typename Compare,
	typename Alloc
>
int deserialize(
	archive<Istream> &is,
	std::set<Key, Compare, Alloc> *set
)
{
	return deserialize_inserted_container<Key>(is, set);
}





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
)
{
	return serialize_container(os, set);
}

template<
	class Istream,
	typename Key,
	typename Compare,
	typename Alloc
>
int deserialize(
	archive<Istream> &is,
	std::multiset<Key, Compare, Alloc> *set
)
{
	return deserialize_inserted_container<Key>(is, set);
}
	



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
)
{
	return serialize_container(os, set);
}

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
)
{
	return deserialize_inserted_container<Value>(is, set);
}





// unordered multiset
template<
	class Ostream,
	typename Value,
	typename Hash,
	typename Pred,
	typename Alloc
>
int serialize(
	archive<Ostream> &os,
	std::unordered_multiset<Value, Hash, Pred, Alloc> const *set
)
{
	return serialize_container(os, set);
}

template<
	class Istream,
	typename Value,
	typename Hash,
	typename Pred,
	typename Alloc
>
int deserialize(
	archive<Istream> &is,
	std::unordered_multiset<Value, Hash, Pred, Alloc> *set
)
{
	return deserialize_inserted_container<Value>(is, set);
}





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
)
{
	return serialize_container(os, map);
}

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
)
{
	return deserialize_inserted_container<std::pair<Key, T>>(is, map);
}



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
)
{
	return serialize_container(os, map);
}

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
)
{
	return deserialize_inserted_container<std::pair<Key, T>>(is, map);
}



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
)
{
	return serialize_container(os, map);
}

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
)
{
	return deserialize_inserted_container<std::pair<Key, T>>(is, map);
}



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
)
{
	return serialize_container(os, map);
}

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
)
{
	return deserialize_inserted_container<std::pair<Key, T>>(is, map);
}





/* END IMPLEMENTATION */
