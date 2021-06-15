#include <iostream>
#include <sstream>

#include <nvx/iostream.hpp>
#include <nvx/type.hpp>

#include "assert.hpp"
#include "random_values.hpp"

#include "../../serialization.hpp"


using namespace nvx;
using namespace std;





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


	vector             = random_value<decltype(vector)>();
	list               = random_value<decltype(list)>();
	set                = random_value<decltype(set)>();
	multiset           = random_value<decltype(multiset)>();
	unordered_set      = random_value<decltype(unordered_set)>();
	unordered_multiset = random_value<decltype(unordered_multiset)>();
	map                = random_value<decltype(map)>();
	multimap           = random_value<decltype(multimap)>();
	unordered_map      = random_value<decltype(unordered_map)>();
	unordered_multimap = random_value<decltype(unordered_multimap)>();

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





// END
