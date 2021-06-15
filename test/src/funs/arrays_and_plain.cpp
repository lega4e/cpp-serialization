#include <iostream>
#include <sstream>

#include <nvx/iostream.hpp>
#include <nvx/type.hpp>

#include <assert.hpp>
#include <random_value.hpp>

#include <serialization.hpp>


using namespace nvx;
using namespace std;





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
			assert_eq(dyn,   dyn+arrsize,     dynr,   dynr+arrsizer,    "dyn != dynr");
			assert_eq(stat,  stat+100,        statr,  statr+100,        "stat != statr");
			assert_eq(plain, plain+plainsize, plainr, plainr+plainsize, "plain != plainr");
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





// END
