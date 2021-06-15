#include <iostream>
#include <sstream>

#include <nvx/iostream.hpp>
#include <nvx/type.hpp>

#include <assert.hpp>
#include <random_value.hpp>

#include <serialization.hpp>


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
	bool assert_eq(Plain const &lhs, Plain const &rhs, char const *err, bool silent)
	{
		return
			assert_eq(lhs.a, rhs.a, err, silent) &&
			assert_eq(lhs.b, rhs.b, err, silent) &&
			assert_eq(lhs.c, rhs.c, err, silent);
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
	bool assert_eq(NoPlain const &lhs, NoPlain const &rhs, char const *err, bool silent)
	{
		return
			assert_eq(lhs.stat,   lhs.stat+16,         rhs.stat,   rhs.stat+16,         err, silent) &&
			assert_eq(lhs.dyn,    lhs.dyn+lhs.n,       rhs.dyn,    rhs.dyn+rhs.n,       err, silent) &&
			assert_eq(lhs.dynvec, lhs.dynvec+lhs.vecn, rhs.dynvec, rhs.dynvec+rhs.vecn, err, silent) &&
			assert_eq(lhs.a,   rhs.a,   err, silent) &&
			assert_eq(lhs.a,   rhs.a,   err, silent) &&
			assert_eq(lhs.b,   rhs.b,   err, silent) &&
			assert_eq(lhs.c,   rhs.c,   err, silent) &&
			assert_eq(lhs.s,   rhs.s,   err, silent) &&
			assert_eq(lhs.vec, rhs.vec, err, silent) &&
			assert_eq(lhs.map, rhs.map, err, silent);
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
			assert_eq(plain,   plainr,   "plain != plainr");
			assert_eq(noplain, noplainr, "noplain != noplainr");
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
