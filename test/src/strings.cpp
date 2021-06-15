#include <iostream>
#include <sstream>

#include <nvx/iostream.hpp>
#include <nvx/type.hpp>

#include "assert.hpp"
#include "random_values.hpp"

#include "../../serialization.hpp"


using namespace nvx;
using namespace std;





bool strings()
{
	string s,  sr;
	wstring ws, wsr;

	for (int i = 0; i < 100; ++i)
	{
		stringstream ss;

		s  = random_value<string>();
		ws = random_value<wstring>();

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





// END
