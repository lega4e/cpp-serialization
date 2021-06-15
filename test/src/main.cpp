#include <iostream>
#include <sstream>

#include <nvx/random.hpp>
#include <nvx/type.hpp>

#include "funs/all.hpp"


using namespace nvx;
using namespace std;





// global objects
dre_t dre = create_dre();





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
		make_pair(&pointers,                    "pointers"),
		make_pair(&shared_pointers_simple,      "shared_pointers_simple"),
		make_pair(&circle_shared_pointers,      "circle_shared_pointers"),
	};

	int success = 0;
	int fail = 0;
	for (auto test : tests)
	{
		printf("========================================\n");
		printf("Testing %s...\n", test.second);
		fflush(stdout);

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
		fflush(stdout);
	}

	printf("Success: %i\n", success);
	printf("Fail:    %i\n", fail);
	printf("Total:   %i\n", success + fail);



	return 0;
}





// end
