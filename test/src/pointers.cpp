#include <iostream>
#include <memory>
#include <sstream>

#include <nvx/iostream.hpp>
#include <nvx/random.hpp>
#include <nvx/type.hpp>

#include "assert.hpp"
#include "random_values.hpp"

#include "../../serialization.hpp"


using namespace nvx;
using namespace std;





struct Node
{
	Node(int val = 0, vector<Node *> childs = {}):
		val(val), childs(childs) {}

	~Node()
	{
		for (Node *child : childs)
			delete child;
	}

	int val;
	vector<Node *> childs;

	bool operator==(Node const &rhs) const
	{
		if (val != rhs.val || childs.size() != rhs.childs.size())
			return false;

		for (int i = 0; i < (int)childs.size(); ++i)
		{
			if ( !(*childs[i] == *rhs.childs[i]) )
				return false;
		}

		return true;
	}

	NVX_SERIALIZABLE(&val, &childs);
};

template<class Ostream>
inline Ostream &operator<<( Ostream &os, Node const &toprint )
{
	return os;
}





/************************ RANDOM NODE ***********************/
Node *random_node(int maxdepth)
{
	Node *node = new Node(random_value<int>());

	if (maxdepth <= 0)
		return node;

	for (int i = 0, e = rnd(1, 3); i != e; ++i)
		node->childs.push_back( random_node(rnd(maxdepth-1-i, maxdepth-1)) );

	return node;
}

namespace nvx
{
	template<>
	Node *random_value<Node *>()
	{
		return random_node(5);
	}
}





/********************** TEST FUNCTIONS **********************/
bool pointers()
{
	for (int _ = 0; _ < 100; ++_)
	{
		stringstream ss;

		Node *nodep = random_value<Node *>();
		unique_ptr<Node> node(nodep);
		archive(&ss) << &nodep;

		Node *noderp;
		archive(&ss) >> &noderp;
		unique_ptr<Node> noder(noderp);

		is_equal(*node, *noder);
	}

	return true;
}





// END
