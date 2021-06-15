#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>

#include <nvx/iostream.hpp>
#include <nvx/random.hpp>
#include <nvx/type.hpp>

#include <assert.hpp>
#include <random_value.hpp>

#include <serialization.hpp>

#pragma GCC diagnostic ignored "-Wparentheses"


using namespace nvx;
using namespace std;





/************************** STRUCT **************************/
struct  SimpleNode;
typedef SimpleNode          snode_t;
typedef shared_ptr<snode_t> snode_p;

struct SimpleNode
{
	int val;

	snode_p left;
	snode_p right;

	bool operator==(snode_t const &rhs) const
	{
		return
			val == rhs.val &&
			( left  == nullptr && rhs.left  == nullptr ||
			  left  != nullptr && rhs.left  != nullptr &&
			  *left  == *rhs.left ) &&
			( right == nullptr && rhs.right == nullptr ||
			  right != nullptr && rhs.right != nullptr &&
			  *right == *rhs.right );
	}

	NVX_SERIALIZABLE(&val, &left, &right);
};

template<class Ostream>
inline Ostream &operator<<( Ostream &os, snode_t const &toprint )
{
	return os;
}



struct  CircleNode;
typedef CircleNode                cnode_t;
typedef shared_ptr<cnode_t>       cnode_p;
typedef shared_ptr<cnode_t const> cnode_pc;

struct CircleNode
{
	int val;

	vector<cnode_p> chs;

	bool operator==(cnode_t const &rhs) const
	{
		_cmpnodes.clear();
		return _equal(*this, rhs);
	}

	static cnode_p random(int depth)
	{
		_allnodes.clear();
		return _random(depth);
	}

	void destroy()
	{
		auto nodes = move(chs);
		chs.clear();

		while(!nodes.empty())
		{
			nodes.back()->destroy();
			nodes.pop_back();
		}

		return;
	}

	NVX_SERIALIZABLE(&val, &chs);

private:
	static bool _equal(cnode_t const &lhs, cnode_t const &rhs)
	{
		if (!_cmpnodes.insert(&lhs).second)
		{
			return true;
		}

		if (lhs.val != rhs.val || lhs.chs.size() != rhs.chs.size())
			return false;

		for (int i = 0; i < (int)lhs.chs.size(); ++i)
		{
			if (!( _equal(*lhs.chs[i], *rhs.chs[i]) ))
				return false;
		}

		return true;
	}

	static cnode_p _random(int depth)
	{
		cnode_p node(new cnode_t);
		node->val = random_value<int>();
		_allnodes.push_back(node);

		if (depth <= 0)
			return node;

		for (int i = 0, e = rnd(1, 4); i != e; ++i)
			node->chs.push_back(cnode_t::_random(depth - 1 - i));

		vector<cnode_p> additional;
		for (int i = 0, e = rnd(1, (int)_allnodes.size()); i != e; ++i)
			additional.push_back(_allnodes[rnd(0, (int)_allnodes.size()-1)]);

		for (cnode_p add : additional)
			node->chs.push_back(add);

		return node;
	}

	static vector<cnode_p> _allnodes;
	static set<cnode_t const *>   _cmpnodes;
};

vector<cnode_p>      cnode_t::_allnodes;
set<cnode_t const *> cnode_t::_cmpnodes;


void push_node(cnode_p node, set<cnode_p> &nset)
{
	if (!nset.insert(node).second)
		return;

	for (cnode_p ch : node->chs)
		push_node(ch, nset);

	return;
}

bool same_structs(cnode_p lhs, cnode_p rhs)
{
	set<cnode_p> lhsnodes;
	set<cnode_p> rhsnodes;

	push_node(lhs, lhsnodes);
	push_node(rhs, rhsnodes);

	return lhsnodes.size() == rhsnodes.size();
}

template<class Ostream>
inline Ostream &operator<<( Ostream &os, cnode_t const &toprint )
{
	return os;
}





/********************** TEST FUNCTIONS **********************/
bool shared_pointers_simple()
{
	for (int _ = 0; _ < 100; ++_)
	{
		stringstream ss;

		/*
		 *  root
		 *  |   \
		 *  |    \
		 *  1_   2____
		 *  | \   \  |
		 *  |  \  |  |
		 *  3  4  5  6
		 *      \ |
		 *       \|
		 *        7
		 */
		snode_p root(new snode_t {
			random_value<int>(), // root
			snode_p(new snode_t { random_value<int>(), // 1
				snode_p(new snode_t { random_value<int>() }), // 3
				snode_p(new snode_t { random_value<int>(),    // 4
					snode_p(new snode_t { random_value<int>() }) // 7
				})
			}),
			snode_p(new snode_t { random_value<int>(), // 2
				snode_p(new snode_t { random_value<int>() }), // 5
				snode_p(new snode_t { random_value<int>() }) // 6
			})
		});

		//       2     5    7            1      4      7
		root->right->left->left = root->left->right->left;

		snode_p rootr;
		archive(&ss) << &root;
		archive(&ss) >> &rootr;

		try
		{
			assert_eq(*root, *rootr);
			assert_eq(
				rootr->right->left->left.get(),
				rootr->left->right->left.get(),
				"Failed poitners"
			);

			rootr->right->left->left->val = random_value<int>();
			assert_eq(
				*root->right->left->left,
				*root->left->right->left,
				"Failed when change one of shared"
			);
		}
		catch(char const *err)
		{
			fprintf(stderr, "%s\n", err);
			return false;
		}
	}

	return true;
}

bool circle_shared_pointers()
{
	bool ret = true;
	vector<cnode_p> nodes;

	for (int _ = 0; _ < 1; ++_)
	{
		stringstream ss;

		cnode_p root = cnode_t::random(4);
		cnode_p rootr;

		archive(&ss) << &root;
		archive(&ss) >> &rootr;

		nodes.push_back(root);
		nodes.push_back(rootr);

		try
		{
			assert_eq(*root, *rootr);
			if (!same_structs(root, rootr))
				throw "Not same structs";
		}
		catch(char const *err)
		{
			fprintf(stderr, "%s\n", err);
			ret = false;
		}

		root->destroy();
		rootr->destroy();

		if (!ret)
			return false;
	}

	return true;
}





// END
