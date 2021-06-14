#include <iostream>
#include <sstream>

#include <nvx/random.hpp>
#include <nvx/type.hpp>

#include "assert.hpp"

#include "../serialization.hpp"


using namespace nvx;
using namespace std;





/********************** GLOBAL OBJECTS **********************/
dre_t dre = create_dre();










/*************************** TESTS **************************/
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
		catch(char const *err)
		{
			fprintf(stderr, "%s\n", err);
			return false;
		}
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
		is_equal(vector, vectorr, "Error: vector != vectorr");
		is_equal(list, listr, "Error: list != listr");
		is_equal(set, setr, "Error: set != setr");
		is_equal(multiset, multisetr, "Error: multiset != multisetr");
		is_equal(unordered_set, unordered_setr, "Error: unordered_set != unordered_setr");
		is_equal(unordered_multiset, unordered_multisetr, "Error: unordered_multiset != unordered_multisetr");
		is_equal(map, mapr, "Error: map != mapr");
		is_equal(multimap, multimapr, "Error: multimap != multimapr");
		is_equal(unordered_map, unordered_mapr, "Error: unordered_map != unordered_mapr");
		is_equal(unordered_multimap, unordered_multimapr, "Error: unordered_multimap != unordered_multimapr");
	}
	catch(char const *err)
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
		catch(char const *err)
		{
			fprintf(stderr, "%s\n", err);
			return false;
		}
	}

	return true;
}



// main
int main( int argc, char *argv[] )
{
	auto tests = {
		make_pair(&primitive_types, "primitive_types"),
		make_pair(&std_containers,  "std_containers"),
		make_pair(&strings,         "strings"),
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
