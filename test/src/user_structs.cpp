#include <iostream>
#include <sstream>

#include <nvx/iostream.hpp>
#include <nvx/type.hpp>

#include "assert.hpp"
#include "random_values.hpp"

#include "../../serialization.hpp"


using namespace nvx;
using namespace std;





/*************************** PLAIN **************************/
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

namespace nvx
{
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
}


template<class Ostream>
inline Ostream &operator<<( Ostream &os, Plain const &toprint )
{
	return os;
}





/************************* NO PLAIN *************************/
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


namespace nvx
{
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
	}

template<class Ostream>
inline Ostream &operator<<( Ostream &os, NoPlain const &toprint )
{
	return os;
}





/************************* FUNCTION *************************/
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




// END
