/*
 * factor.cpp
 *
 *  Created on: May 30, 2015
 *      Author: nbingham
 */

#include "factor.h"
#include <common/standard.h>
#include <common/text.h>

struct arc
{
	vector<int> left;
	vector<int> right;
	float weight;
};

ostream &operator<<(ostream &os, arc a)
{
	os << a.weight << " " << a.left << " " << a.right;
	return os;
}

namespace boolean
{

factor::factor()
{
	op = OR;
}

factor::~factor()
{

}

factor &factor::xfactor(cover c, int t)
{
	cover nc = ~c;

	if (c.cubes.size() <= 1)
	{
		if (t == factor::AND)
		{
			terms = nc;
			op = 1-t;
		}
		else
		{
			terms = c;
			op = t;
		}
	}
	else if (nc.cubes.size() <= 1)
	{
		if (1-t == factor::AND)
		{
			terms = c;
			op = t;
		}
		else
		{
			terms = nc;
			op = 1-t;
		}
	}
	else
	{
		cover c_left, c_right, nc_left, nc_right;
		float c_weight, nc_weight;

		c_weight = partition(c, c_left, c_right);
		nc_weight = partition(nc, nc_left, nc_right);

		if (c_weight <= nc_weight)
		{
			subfactors.push_back(factor().xfactor(c_left, t));
			subfactors.push_back(factor().xfactor(c_right, t));
			op = t;
		}
		else if (nc_weight < c_weight)
		{
			subfactors.push_back(factor().xfactor(nc_left, 1-t));
			subfactors.push_back(factor().xfactor(nc_right, 1-t));
			op = 1-t;
		}
	}
	return *this;
}

factor &factor::hfactor(cover c)
{
	if (c.cubes.size() <= 1)
	{
		terms = c;
		op = factor::OR;
	}
	else
	{
		cover c_left, c_right;
		partition(c, c_left, c_right);
		subfactors.push_back(factor().hfactor(c_left));
		subfactors.push_back(factor().hfactor(c_right));
		op = factor::OR;

		if (subfactors[0].terms.cubes.size() > 0 && subfactors[1].terms.cubes.size() > 0)
		{
			cube common = supercube(subfactors[0].terms.cubes[0], subfactors[1].terms.cubes[0]);
			subfactors[0].terms.cofactor(common);
			subfactors[1].terms.cofactor(common);

			if (common != 1)
			{
				factor f;
				f.subfactors = subfactors;
				f.op = op;
				subfactors.clear();
				subfactors.push_back(f);
				terms.push_back(common);
				op = factor::AND;
			}
		}
	}
	return *this;
}

float factor::partition(cover c, cover &left, cover &right)
{
	vector<arc> lci_graph;

	for (int i = 0; i < (int)c.cubes.size(); i++)
		for (int j = i+1; j < (int)c.cubes.size(); j++)
		{
			arc add;
			add.left.push_back(i);
			add.right.push_back(j);
			add.weight = (float)similarity(c.cubes[i], c.cubes[j]);
			add.weight = add.weight*add.weight/(float)(c.cubes[i].width()*c.cubes[j].width());
			lci_graph.push_back(add);
		}

	while (lci_graph.size() > 1)
	{
		vector<int> count_index;
		int min_count = 10000;
		for (int i = 0; i < (int)lci_graph.size(); i++)
		{
			int count = abs((int)lci_graph[i].left.size() - (int)lci_graph[i].right.size());
			if (count < min_count)
			{
				count_index = vector<int>(1, i);
				min_count = count;
			}
			else if (count == min_count)
				count_index.push_back(i);
		}

		vector<int> weight_index;
		float max_weight = -1.0f;
		for (int i = 0; i < (int)count_index.size(); i++)
		{
			if (lci_graph[count_index[i]].weight > max_weight)
			{
				weight_index = vector<int>(1, count_index[i]);
				max_weight = lci_graph[count_index[i]].weight;
			}
			else if (lci_graph[count_index[i]].weight == max_weight)
				weight_index.push_back(count_index[i]);
		}

		int index = weight_index[rand()%weight_index.size()];

		arc rem = lci_graph[index];
		lci_graph.erase(lci_graph.begin() + index);
		vector<int> new_node(rem.left.size() + rem.right.size(), -1);
		merge(rem.left.begin(), rem.left.end(), rem.right.begin(), rem.right.end(), new_node.begin());

		for (int i = 0; i < (int)lci_graph.size(); i++)
		{
			if (lci_graph[i].left == rem.left || lci_graph[i].left == rem.right)
				lci_graph[i].left = new_node;
			if (lci_graph[i].right == rem.left || lci_graph[i].right == rem.right)
				lci_graph[i].right = new_node;
		}

		for (int i = 0; i < (int)lci_graph.size(); i++)
			for (int j = i+1; j < (int)lci_graph.size(); )
			{
				if ((lci_graph[i].left == lci_graph[j].left && lci_graph[i].right == lci_graph[j].right) ||
					(lci_graph[i].left == lci_graph[j].right && lci_graph[i].right == lci_graph[j].left))
				{
					lci_graph[i].weight = lci_graph[i].weight + lci_graph[j].weight;
					lci_graph.erase(lci_graph.begin() + j);
				}
				else
					j++;
			}
	}

	if (lci_graph.size() == 0)
		return 0.0f;

	left.cubes.clear();
	right.cubes.clear();

	for (int i = 0; i < (int)lci_graph[0].left.size(); i++)
		left.cubes.push_back(c.cubes[lci_graph[0].left[i]]);
	for (int i = 0; i < (int)lci_graph[0].right.size(); i++)
		right.cubes.push_back(c.cubes[lci_graph[0].right[i]]);
	return lci_graph[0].weight;
}

}
