#include <iostream>
#include <sstream>

#include <nvx/random.hpp>
#include <nvx/type.hpp>

#include "../serialization.hpp"


using namespace nvx;
using namespace std;





/********************** GLOBAL OBJECTS **********************/
dre_t dre = create_dre();





/********************* ASSERT FUNCTIONS *********************/
template<typename T, typename U>
void is_equal(T const &lhs, U const &rhs, char const *err)
{
	if (lhs != rhs)
	{
		cerr << "lhs: " << lhs << ", " << rhs << endl;
		throw err;
	}
	return;
}





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





// main
int main( int argc, char *argv[] )
{
	auto tests = {
		make_pair(&primitive_types, "primitive_types"),
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
