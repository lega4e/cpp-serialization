#include <iostream>
#include <sstream>

#include <nvx/random.hpp>
#include <nvx/type.hpp>

#include "assert.hpp"

#include "../serialization.hpp"
#include "random_values.hpp"


using namespace nvx;
using namespace std;





/*************************** TESTS **************************/
dre_t dre = create_dre();

bool primitive_types()
{
	for (int i = 0; i < 100; ++i)
	{
		stringstream ss;

		bool    boolval,    boolvalr;
		short   shortval,   shortvalr;
		int     intval,     intvalr;
		long    longval,    longvalr;
		llong   llongval,   llongvalr;
		char    charval,    charvalr;
		uint    uintval,    uintvalr;
		ulong   ulongval,   ulongvalr;
		ullong  ullongval,  ullongvalr;
		float   floatval,   floatvalr;
		double  doubleval,  doublevalr;
		ldouble ldoubleval, ldoublevalr;

		boolval    = uidis_t<int>( 0, 1 )(dre);
		shortval   = uidis_t<short>(  short_min,  short_max  )(dre);
		intval     = uidis_t<int>(    int_min,    int_max    )(dre);
		longval    = uidis_t<long>(   long_min,   long_max   )(dre);
		llongval   = uidis_t<llong>(  llong_min,  llong_max  )(dre);
		charval    = uidis_t<char>(   char_min,   char_max   )(dre);
		uintval    = uidis_t<uint>(   uint_min,   uint_max   )(dre);
		ulongval   = uidis_t<ulong>(  ulong_min,  ulong_max  )(dre);
		ullongval  = uidis_t<ullong>( ullong_min, ullong_max )(dre);

		floatval   = urdis_t<float>()(dre);
		doubleval  = urdis_t<double>()(dre);
		ldoubleval = urdis_t<ldouble>()(dre);

		archive<stringstream>(&ss) <<
			&boolval   <<
			&shortval  <<
			&intval    <<
			&longval   <<
			&llongval  <<
			&charval   <<
			&uintval   <<
			&ulongval  <<
			&ullongval <<
			&floatval  <<
			&doubleval <<
			&ldoubleval;

		archive<stringstream>(&ss) >>
			&boolvalr   >>
			&shortvalr  >>
			&intvalr    >>
			&longvalr   >>
			&llongvalr  >>
			&charvalr   >>
			&uintvalr   >>
			&ulongvalr  >>
			&ullongvalr >>
			&floatvalr  >>
			&doublevalr >>
			&ldoublevalr;

		try
		{
			is_equal(boolval,    boolvalr,    (char const *)"Equal fail: boolval != boolvalr");
			is_equal(shortval,   shortvalr,   (char const *)"Equal fail: shortval != shortvalr");
			is_equal(intval,     intvalr,     (char const *)"Equal fail: intval != intvalr");
			is_equal(longval,    longvalr,    (char const *)"Equal fail: longval != longvalr");
			is_equal(llongval,   llongvalr,   (char const *)"Equal fail: llongval != llongvalr");
			is_equal(charval,    charvalr,    (char const *)"Equal fail: charval != charvalr");
			is_equal(uintval,    uintvalr,    (char const *)"Equal fail: uintval != uintvalr");
			is_equal(ulongval,   ulongvalr,   (char const *)"Equal fail: ulongval != ulongvalr");
			is_equal(ullongval,  ullongvalr,  (char const *)"Equal fail: ullongval != ullongvalr");
			is_equal(floatval,   floatvalr,   (char const *)"Equal fail: floatval != floatvalr");
			is_equal(doubleval,  doublevalr,  (char const *)"Equal fail: doubleval != doublevalr");
			is_equal(ldoubleval, ldoublevalr, (char const *)"Equal fail: ldoubleval != ldoublevalr");
		}
		catch (char const *err)
		{
			fprintf(stderr, "%s\n", err);
			return false;
		}
	}

	return true;
}

bool pointers_to_primitive_types()
{
	bool ret = true;

	for (int i = 0; i < 100; ++i)
	{
		stringstream ss;

		bool    *boolval,    *boolvalr;
		short   *shortval,   *shortvalr;
		int     *intval,     *intvalr;
		long    *longval,    *longvalr;
		llong   *llongval,   *llongvalr;
		char    *charval,    *charvalr;
		uint    *uintval,    *uintvalr;
		ulong   *ulongval,   *ulongvalr;
		ullong  *ullongval,  *ullongvalr;
		float   *floatval,   *floatvalr;
		double  *doubleval,  *doublevalr;
		ldouble *ldoubleval, *ldoublevalr;

		boolval    = new bool(uidis_t<int>( 0, 1 )(dre));
		shortval   = new short(uidis_t<short>(   short_min,  short_max  )(dre));
		intval     = new int(uidis_t<int>(       int_min,    int_max    )(dre));
		longval    = new long(uidis_t<long>(     long_min,   long_max   )(dre));
		llongval   = new llong(uidis_t<llong>(   llong_min,  llong_max  )(dre));
		charval    = new char(uidis_t<char>(     char_min,   char_max   )(dre));
		uintval    = new uint(uidis_t<uint>(     uint_min,   uint_max   )(dre));
		ulongval   = new ulong(uidis_t<ulong>(   ulong_min,  ulong_max  )(dre));
		ullongval  = new ullong(uidis_t<ullong>( ullong_min, ullong_max )(dre));

		floatval   = new float(urdis_t<float>()(dre));
		doubleval  = new double(urdis_t<double>()(dre));
		ldoubleval = new ldouble(urdis_t<ldouble>()(dre));

		archive<stringstream>(&ss) <<
			&boolval   <<
			&shortval  <<
			&intval    <<
			&longval   <<
			&llongval  <<
			&charval   <<
			&uintval   <<
			&ulongval  <<
			&ullongval <<
			&floatval  <<
			&doubleval <<
			&ldoubleval;

		archive<stringstream>(&ss) >>
			&boolvalr   >>
			&shortvalr  >>
			&intvalr    >>
			&longvalr   >>
			&llongvalr  >>
			&charvalr   >>
			&uintvalr   >>
			&ulongvalr  >>
			&ullongvalr >>
			&floatvalr  >>
			&doublevalr >>
			&ldoublevalr;

		try
		{
			is_equal(*boolval,    *boolvalr,    "Equal fail: boolval != boolvalr");
			is_equal(*shortval,   *shortvalr,   "Equal fail: shortval != shortvalr");
			is_equal(*intval,     *intvalr,     "Equal fail: intval != intvalr");
			is_equal(*longval,    *longvalr,    "Equal fail: longval != longvalr");
			is_equal(*llongval,   *llongvalr,   "Equal fail: llongval != llongvalr");
			is_equal(*charval,    *charvalr,    "Equal fail: charval != charvalr");
			is_equal(*uintval,    *uintvalr,    "Equal fail: uintval != uintvalr");
			is_equal(*ulongval,   *ulongvalr,   "Equal fail: ulongval != ulongvalr");
			is_equal(*ullongval,  *ullongvalr,  "Equal fail: ullongval != ullongvalr");
			is_equal(*floatval,   *floatvalr,   "Equal fail: floatval != floatvalr");
			is_equal(*doubleval,  *doublevalr,  "Equal fail: doubleval != doublevalr");
			is_equal(*ldoubleval, *ldoublevalr, "Equal fail: ldoubleval != ldoublevalr");
		}
		catch (char const *err)
		{
			fprintf(stderr, "%s\n", err);
			ret = false;
		}

		delete boolval;
		delete shortval;
		delete intval;
		delete longval;
		delete llongval;
		delete charval;
		delete uintval;
		delete ulongval;
		delete ullongval;
		delete floatval;
		delete doubleval;
		delete ldoubleval;

		delete boolvalr;
		delete shortvalr;
		delete intvalr;
		delete longvalr;
		delete llongvalr;
		delete charvalr;
		delete uintvalr;
		delete ulongvalr;
		delete ullongvalr;
		delete floatvalr;
		delete doublevalr;
		delete ldoublevalr;

		if (!ret)
			return false;
	}

	return true;
}

bool arrays_and_plain()
{
	bool ret = true;

	int    stat[100], statr[100];
	int    *dyn,      *dynr;
	double *plain,    *plainr;

	for (int _ = 0; _ < 100; ++_)
	{
		stringstream ss;

		int arrsize = disI(5, 100)(dre), arrsizer;
		int plainsize = disI(5, 100)(dre);

		dyn    = new int[arrsize];
		plain  = new double[plainsize];
		plainr = new double[plainsize];

		for (int i = 0; i < arrsize; ++i)
			dyn[i] = disI(int_min, int_max)(dre);

		for (int i = 0; i < plainsize; ++i)
			plain[i] = disD()(dre);

		for (int i = 0; i < 100; ++i)
			stat[i] = disI(int_min, int_max)(dre);

		archive arch(&ss);
		serialize_array(arch,  &dyn,  &arrsize);
		serialize_static(arch, stat,  100);
		serialize_plain(arch,  plain, sizeof(double) * plainsize);

		deserialize_array(arch,  &dynr,  &arrsizer);
		deserialize_static(arch, statr,  100);
		deserialize_plain(arch,  plainr, sizeof(double) * plainsize);

		try
		{
			is_equal(dyn,   dyn+arrsize,     dynr,   dynr+arrsizer,    "dyn != dynr");
			is_equal(stat,  stat+100,        statr,  statr+100,        "stat != statr");
			is_equal(plain, plain+plainsize, plainr, plainr+plainsize, "plain != plainr");
		}
		catch (char const *err)
		{
			fprintf(stderr, "%s\n", err);
			ret = false;
		}

		delete[] dyn;
		delete[] dynr;
		delete[] plain;
		delete[] plainr;

		if (!ret)
			return false;
	}


	return true;
}



template<class Container>
void push_random_ivalues(Container &cont, disI dis, int count)
{
	for (int i = 0; i < count; ++i)
		cont.push_back(dis(dre));
	return;
}

template<class Container>
void insert_random_ivalues(Container &cont, disI dis, int count)
{
	for (int i = 0; i < count; ++i)
		cont.insert(dis(dre));
	return;
}

template<class Container>
void push_random_pair_ivalues(Container &cont, disI dis, int count)
{
	for (int i = 0; i < count; ++i)
		cont.push( pair<int, int>{dis(dre), dis(dre)} );
	return;
}

template<class Container>
void insert_random_pair_ivalues(Container &cont, disI dis, int count)
{
	for (int i = 0; i < count; ++i)
		cont.insert( pair<int, int>{dis(dre), dis(dre)} );
	return;
}

bool std_containers()
{
	vector<int>                  vector,             vectorr;
	list<int>                    list,               listr;
	set<int>                     set,                setr;
	multiset<int>                multiset,           multisetr;
	unordered_set<int>           unordered_set,      unordered_setr;
	unordered_multiset<int>      unordered_multiset, unordered_multisetr;
	map<int, int>                map,                mapr;
	multimap<int, int>           multimap,           multimapr;
	unordered_map<int, int>      unordered_map,      unordered_mapr;
	unordered_multimap<int, int> unordered_multimap, unordered_multimapr;

	disI dis(int_min, int_max);
	disI sizedis(1, 100);

	push_random_ivalues(        vector,             dis, sizedis(dre) );
	push_random_ivalues(        list,               dis, sizedis(dre) );
	insert_random_ivalues(      set,                dis, sizedis(dre) );
	insert_random_ivalues(      multiset,           dis, sizedis(dre) );
	insert_random_ivalues(      unordered_set,      dis, sizedis(dre) );
	insert_random_ivalues(      unordered_multiset, dis, sizedis(dre) );
	insert_random_pair_ivalues( map,                dis, sizedis(dre) );
	insert_random_pair_ivalues( multimap,           dis, sizedis(dre) );
	insert_random_pair_ivalues( unordered_map,      dis, sizedis(dre) );
	insert_random_pair_ivalues( unordered_multimap, dis, sizedis(dre) );

	stringstream ss;

	archive(&ss) <<
		&vector <<
		&list <<
		&set <<
		&multiset <<
		&unordered_set <<
		&unordered_multiset <<
		&map <<
		&multimap <<
		&unordered_map <<
		&unordered_multimap;

	archive(&ss) >>
		&vectorr >>
		&listr >>
		&setr >>
		&multisetr >>
		&unordered_setr >>
		&unordered_multisetr >>
		&mapr >>
		&multimapr >>
		&unordered_mapr >>
		&unordered_multimapr;

	try
	{
		is_equal(vector,             vectorr,             "Error: vector != vectorr");
		is_equal(list,               listr,               "Error: list != listr");
		is_equal(set,                setr,                "Error: set != setr");
		is_equal(multiset,           multisetr,           "Error: multiset != multisetr");
		is_equal(unordered_set,      unordered_setr,      "Error: unordered_set != unordered_setr");
		is_equal(unordered_multiset, unordered_multisetr, "Error: unordered_multiset != unordered_multisetr");
		is_equal(map,                mapr,                "Error: map != mapr");
		is_equal(multimap,           multimapr,           "Error: multimap != multimapr");
		is_equal(unordered_map,      unordered_mapr,      "Error: unordered_map != unordered_mapr");
		is_equal(unordered_multimap, unordered_multimapr, "Error: unordered_multimap != unordered_multimapr");
	}
	catch (char const *err)
	{
		fprintf(stderr, "%s\n", err);
		return false;
	}

	return true;
}



bool strings()
{
	string s,  sr;
	wstring ws, wsr;

	for (int i = 0; i < 100; ++i)
	{
		stringstream ss;

		s  = "";
		ws = L"";

		for (int i = 0, e = disI(10, 100)(dre); i != e; ++i)
			s.push_back(uidis_t<char>(char_min, char_max)(dre));

		for (int i = 0, e = disI(10, 100)(dre); i != e; ++i)
			ws.push_back(uidis_t<wchar_t>(
				numeric_limits<wchar_t>::min(),
				numeric_limits<wchar_t>::max()
			)(dre));

		archive(&ss) << &s  << &ws;
		archive(&ss) >> &sr >> &wsr;

		try
		{
			is_equal(s, sr, "s != sr");
			is_equal(ws, wsr, "ws != wsr");
		}
		catch (char const *err)
		{
			fprintf(stderr, "%s\n", err);
			return false;
		}
	}

	return true;
}



// structs & classes
struct Plain
{
	int a, b, c;

	static Plain random()
	{
		disI dis(int_min, int_max);
		return Plain { dis(dre), dis(dre), dis(dre) };
	}

	NVX_SERIALIZABLE_PLAIN();
};

template<>
bool is_equal(Plain const &lhs, Plain const &rhs, char const *err, bool silent)
{
	return
		is_equal(lhs.a, rhs.a, err, silent) &&
		is_equal(lhs.b, rhs.b, err, silent) &&
		is_equal(lhs.c, rhs.c, err, silent);
}

template<>
Plain random_value<Plain>()
{
	return {
		random_value<int>(),
		random_value<int>(),
		random_value<int>()
	};
}

template<class Ostream>
inline Ostream &operator<<( Ostream &os, Plain const &toprint )
{
	return os;
}

struct NoPlain
{
	char                            stat[16];
	int                             a, b, c;
	int                             *dyn;
	int                             n;
	string                          s;
	vector<int>                     vec;
	vector<vector<int>>             *dynvec;
	int                             vecn;
	unordered_multimap<string, int> map;

	NVX_SERIALIZABLE(
		NVX_SERIALIZABLE_STATIC_ARRAY(stat, 16),
		&a, &b, &c, &s, &vec, &map,
		NVX_SERIALIZABLE_DYNAMIC_ARRAY(&dyn, &n),
		NVX_SERIALIZABLE_DYNAMIC_ARRAY(&dynvec, &vecn)
	);
};

template<>
bool is_equal(NoPlain const &lhs, NoPlain const &rhs, char const *err, bool silent)
{
	return
		is_equal(lhs.stat,   lhs.stat+16,         rhs.stat,   rhs.stat+16,         err, silent) &&
		is_equal(lhs.dyn,    lhs.dyn+lhs.n,       rhs.dyn,    rhs.dyn+rhs.n,       err, silent) &&
		is_equal(lhs.dynvec, lhs.dynvec+lhs.vecn, rhs.dynvec, rhs.dynvec+rhs.vecn, err, silent) &&
		is_equal(lhs.a,   rhs.a,   err, silent) &&
		is_equal(lhs.a,   rhs.a,   err, silent) &&
		is_equal(lhs.b,   rhs.b,   err, silent) &&
		is_equal(lhs.c,   rhs.c,   err, silent) &&
		is_equal(lhs.s,   rhs.s,   err, silent) &&
		is_equal(lhs.vec, rhs.vec, err, silent) &&
		is_equal(lhs.map, rhs.map, err, silent);
}


template<>
NoPlain random_value<NoPlain>()
{
	NoPlain s;

	randomize(s.stat, s.stat+16);
	randomize(s.a);
	randomize(s.b);
	randomize(s.c);
	s.dyn = random_array<int>(s.n);
	randomize(s.dyn, s.dyn + s.n);
	randomize(s.s);
	randomize(s.vec);
	s.dynvec = random_array<vector<vector<int>>>(s.vecn);
	randomize(s.dynvec, s.dynvec+s.vecn);
	randomize(s.map);

	return s;
}

template<class Ostream>
inline Ostream &operator<<( Ostream &os, NoPlain const &toprint )
{
	return os;
}

bool user_structs()
{
	bool ret = true;

	for (int _ = 0; _ < 1; ++_)
	{
		stringstream ss;

		Plain   plain   = random_value<Plain>(),   plainr;
		NoPlain noplain = random_value<NoPlain>(), noplainr;

		archive(&ss) << &plain << &noplain;
		archive(&ss) >> &plainr >> &noplainr;

		try
		{
			is_equal(plain,   plainr,   "plain != plainr");
			is_equal(noplain, noplainr, "noplain != noplainr");
		}
		catch(char const *err)
		{
			fprintf(stderr, "%s\n", err);
			ret = false;
		}

		delete[] noplain.dyn;
		delete[] noplain.dynvec;

		delete[] noplainr.dyn;
		delete[] noplainr.dynvec;

		if (!ret)
			break;
	}

	return true;
}



// main
int main( int argc, char *argv[] )
{
	auto tests = {
		make_pair(&primitive_types,             "primitive_types"),
		make_pair(&pointers_to_primitive_types, "pointers_to_primitive_types"),
		make_pair(&arrays_and_plain,            "arrays_and_plain"),
		make_pair(&std_containers,              "std_containers"),
		make_pair(&strings,                     "strings"),
		make_pair(&user_structs,                "user_structs"),
	};

	int success = 0;
	int fail = 0;
	for (auto test : tests)
	{
		printf("========================================\n");
		printf("Testing %s...\n", test.second);
		if ((*test.first)())
		{
			++success;
			printf("Success\n");
		}
		else
		{
			++fail;
			printf("Fail\n");
		}

		printf("\n");
	}

	printf("Success: %i\n", success);
	printf("Fail:    %i\n", fail);
	printf("Total:   %i\n", success + fail);



	return 0;
}





// end
