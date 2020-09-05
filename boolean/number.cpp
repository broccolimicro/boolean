/*
 * number.cpp
 *
 *  Created on: Feb 6, 2015
 *      Author: nbingham
 */

#include "number.h"
#include <common/standard.h>

namespace boolean
{
unsigned_int::unsigned_int()
{
}

unsigned_int::unsigned_int(int width, int offset)
{
	for (int i = 0; i < width; i++)
		bits.push_back(cover(i+offset, 1));
}

unsigned_int::unsigned_int(unsigned int value)
{
	while (value > 0) {
		bits.push_back(cover(value & 1));
		value >>= 1;
	}
}

unsigned_int::unsigned_int(const unsigned_int &n)
{
	bits = n.bits;
}

unsigned_int::unsigned_int(const signed_int &n)
{
	bits = n.bits;
	bits.push_back(n.sign);
}

unsigned_int::~unsigned_int()
{
}

unsigned_int &unsigned_int::simplify()
{
	for (int i = 0; i < (int)bits.size(); i++)
		bits[i].espresso();
	return *this;
}

unsigned_int &unsigned_int::ext(int count)
{
	for (int i = 0; i < count; i++)
		bits.push_back(0);
	return *this;
}

unsigned_int &unsigned_int::operator=(unsigned_int n)
{
	bits = n.bits;
	return *this;
}

unsigned_int &unsigned_int::operator=(signed_int n)
{
	bits = n.bits;
	bits.push_back(n.sign);
	return *this;
}

struct arc
{
	vector<pair<int, int> > left;
	vector<pair<int, int> > right;
	float weight;
};

float unsigned_int::partition(unsigned_int &left, unsigned_int &right)
{
	vector<arc> lci_graph;

	for (int i = 0; i < (int)bits.size(); i++) {
		if (not bits[i].is_null() and not bits[i].is_tautology()) {
			for (int j = i; j < (int)bits.size(); j++) {
				if (not bits[j].is_null() and not bits[j].is_tautology()) {
					for (int k = 0; k < (int)bits[i].cubes.size(); k++) {
						for (int l = (j==i?k+1:0); l < (int)bits[j].cubes.size(); l++) {
							arc add;
							add.left.push_back(pair<int, int>(i, k));
							add.right.push_back(pair<int, int>(j, l));
							add.weight = (float)similarity(bits[i].cubes[k], bits[j].cubes[l]);
							add.weight = add.weight*add.weight/(float)(bits[i].cubes[k].width()*bits[j].cubes[l].width());
							lci_graph.push_back(add);
						}
					}
				}
			}
		}
	}

	while (lci_graph.size() > 1)
	{
		printf("\r%d", lci_graph.size());
		fflush(stdout);
		vector<int> count_index;
		int min_count = numeric_limits<int>::max();
		for (int i = 0; i < (int)lci_graph.size(); i++)
		{
			int count = abs((int)lci_graph[i].left.size() - (int)lci_graph[i].right.size());
			if (count < min_count)
			{
				count_index.clear();
				count_index.push_back(i);
				min_count = count;
			}
			else if (count == min_count)
				count_index.push_back(i);
		}

		vector<int> weight_index;
		float max_weight = -numeric_limits<float>::infinity();
		for (int i = 0; i < (int)count_index.size(); i++)
		{
			if (lci_graph[count_index[i]].weight > max_weight)
			{
				weight_index.clear();
				weight_index.push_back(count_index[i]);
				max_weight = lci_graph[count_index[i]].weight;
			}
			else if (lci_graph[count_index[i]].weight == max_weight)
				weight_index.push_back(count_index[i]);
		}

		int index = 0;
		if (weight_index.size() > 0)
			index = weight_index[rand()%(int)weight_index.size()];
		else if (count_index.size() > 0)
			index = count_index[rand()%(int)count_index.size()];
		else if (lci_graph.size() > 1)
			index = rand()%(int)lci_graph.size();
		else
			break;

		arc rem = lci_graph[index];
		lci_graph.erase(lci_graph.begin() + index);
		vector<pair<int, int> > new_node(rem.left.size() + rem.right.size(), pair<int, int>(-1, -1));
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

	left.bits.clear();
	right.bits.clear();
	left.bits.resize(bits.size(), cover());
	right.bits.resize(bits.size(), cover());

	for (int i = 0; i < (int)lci_graph[0].left.size(); i++)
		left.bits[lci_graph[0].left[i].first].cubes.push_back(bits[lci_graph[0].left[i].first].cubes[lci_graph[0].left[i].second]);
	for (int i = 0; i < (int)lci_graph[0].right.size(); i++)
		right.bits[lci_graph[0].right[i].first].cubes.push_back(bits[lci_graph[0].right[i].first].cubes[lci_graph[0].right[i].second]);
	for (int i = 0; i < (int)bits.size(); i++) {
		if (bits[i].is_tautology()) {
			left.bits[i] = cover(1);
			right.bits[i] = cover(1);
		}
	}

	return lci_graph[0].weight;
}

cube unsigned_int::supercube() const
{
	cube result;
	bool init = false;
	if (bits.size() > 0)
	{
		for (int i = (int)bits.size()-1; i != -1; i--)
		{
			if (not bits[i].is_null()) {
				if (not init) {
					result = bits[i].supercube();
					init = true;
				} else {
					for (int j = (int)bits[i].cubes.size()-1; j != -1; j--) {
						if (bits[i].cubes[j].size() < result.size())
							result.trunk(bits[i].cubes[j].size());

						for (int k = 0; k < result.size(); k++)
							result.values[k] |= bits[i].cubes[j].values[k];
					}
				}
			}
		}
	}
	else
		result.values.push_back(0);

	return result;
}

void unsigned_int::cofactor(const cube &s1)
{
	for (int i = 0; i < (int)bits.size(); i++)
		bits[i].cofactor(s1);
}

bool unsigned_int::is_constant()
{
	for (int i = 0; i < (int)bits.size(); i++)
		if (not bits[i].is_null() and not bits[i].is_tautology())
			return false;
	return true;
}

int unsigned_int::max_width()
{
	int result = 0;
	for (int i = 0; i < (int)bits.size(); i++) {
		if (not bits[i].is_null() and not bits[i].is_tautology()) {
			for (int j = 0; j < (int)bits[i].cubes.size(); j++) {
				int w = bits[i].cubes[j].width();
				result = w > result ? w : result;
			}
		}
	}

	return result;
}

int unsigned_int::depth()
{
	int result = 0;
	for (int i = 0; i < (int)bits.size(); i++) {
		if (not bits[i].is_null() and not bits[i].is_tautology()) {
			result += (int)bits[i].cubes.size();
		}
	}
	return result;
}

signed_int::signed_int()
{

}

signed_int::signed_int(int width, int offset)
{
	for (int i = 0; i < width-1; i++)
		bits.push_back(cover(i+offset, 1));
	sign = cover(width-1+offset, 1);
}

signed_int::signed_int(int value)
{
	while (value != 0 and value != -1) {
		bits.push_back(cover(value & 1));
		value >>= 1;
	}
	sign = cover(value == -1);
}

signed_int::signed_int(const signed_int &n)
{
	bits = n.bits;
	sign = n.sign;
}

signed_int::signed_int(const unsigned_int &n)
{
	bits = n.bits;
	sign = cover(0);
}

signed_int::~signed_int()
{

}

signed_int &signed_int::simplify()
{
	for (int i = 0; i < (int)bits.size(); i++)
		bits[i].espresso();
	sign.espresso();
	return *this;
}

signed_int &signed_int::ext(int s)
{
	for (int i = 0; i < s; i++)
		bits.push_back(sign);
	return *this;
}

signed_int &signed_int::operator=(unsigned_int n)
{
	bits = n.bits;
	sign = cover(0);
	return *this;
}

signed_int &signed_int::operator=(signed_int n)
{
	bits = n.bits;
	sign = n.sign;
	return *this;
}

}

boolean::unsigned_int operator&(boolean::unsigned_int n0, boolean::unsigned_int n1)
{
	boolean::unsigned_int result;
	int m = (int)std::min(n0.bits.size(), n1.bits.size());
	for (int i = 0; i < m; i++)
		result.bits.push_back(n0.bits[i] & n1.bits[i]);
	return result;
}

boolean::unsigned_int operator|(boolean::unsigned_int n0, boolean::unsigned_int n1)
{
	boolean::unsigned_int result;
	int m = (int)std::min(n0.bits.size(), n1.bits.size());
	for (int i = 0; i < m; i++)
		result.bits.push_back(n0.bits[i] | n1.bits[i]);
	for (int i = m; i < (int)n0.bits.size(); i++)
		result.bits.push_back(n0.bits[i]);
	for (int i = m; i < (int)n1.bits.size(); i++)
		result.bits.push_back(n1.bits[i]);
	return result;
}

boolean::unsigned_int operator~(boolean::unsigned_int n)
{
	boolean::unsigned_int result;
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(~n.bits[i]);
	return result;
}

boolean::unsigned_int operator&(boolean::unsigned_int n, boolean::cover c)
{
	for (int i = 0; i < (int)n.bits.size(); i++)
		n.bits[i] &= c;
	return n;
}

boolean::unsigned_int operator|(boolean::unsigned_int n, boolean::cover c)
{
	for (int i = 0; i < (int)n.bits.size(); i++)
		n.bits[i] |= c;
	return n;
}

boolean::unsigned_int operator&(boolean::cover c, boolean::unsigned_int n)
{
	for (int i = 0; i < (int)n.bits.size(); i++)
		n.bits[i] &= c;
	return n;
}

boolean::unsigned_int operator|(boolean::cover c, boolean::unsigned_int n)
{
	for (int i = 0; i < (int)n.bits.size(); i++)
		n.bits[i] |= c;
	return n;
}

boolean::unsigned_int operator<<(boolean::unsigned_int n, int s)
{
	boolean::unsigned_int result;
	result.bits.reserve(s + (int)n.bits.size());
	for (int i = 0; i < s; i++)
		result.bits.push_back(boolean::cover(0));
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i]);
	return result;
}

boolean::unsigned_int operator>>(boolean::unsigned_int n, int s)
{
	boolean::unsigned_int result;
	result.bits.reserve(std::max((int)n.bits.size() - s, 0));
	for (int i = s; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i]);
	return result;
}

boolean::unsigned_int operator-(boolean::unsigned_int n)
{
	boolean::unsigned_int result;
	boolean::cover carry = 1, inf;
	for (int i = 0; i < (int)n.bits.size(); i++)
	{
		result.bits.push_back((~n.bits[i]&~carry) | (n.bits[i]&carry));
		carry = (~n.bits[i]&carry);
	}
	result.bits.push_back(carry);

	return result;
}

boolean::unsigned_int operator+(boolean::unsigned_int n0, boolean::unsigned_int n1)
{
	boolean::unsigned_int result;
	boolean::cover carry = 0, inf;
	int m = (int)std::min(n0.bits.size(), n1.bits.size());
	for (int i = 0; i < m; i++)
	{
		result.bits.push_back((n0.bits[i]&~n1.bits[i]&~carry) | (~n0.bits[i]&n1.bits[i]&~carry) | (~n0.bits[i]&~n1.bits[i]&carry));
		carry = (n0.bits[i]&n1.bits[i]) | (n0.bits[i]&carry) | (n1.bits[i]&carry);
	}
	for (int i = m; i < (int)n0.bits.size(); i++)
	{
		result.bits.push_back((n0.bits[i]&~carry) | (~n0.bits[i]&carry));
		carry = n0.bits[i]&carry;
	}
	for (int i = m; i < (int)n1.bits.size(); i++)
	{
		result.bits.push_back((n1.bits[i]&~carry) | (~n1.bits[i]&carry));
		carry = n1.bits[i]&carry;
	}
	result.bits.push_back(carry);

	return result;
}

boolean::unsigned_int operator-(boolean::unsigned_int n0, boolean::unsigned_int n1)
{
	boolean::unsigned_int result;
	boolean::cover carry = 1, inf;
	int m = (int)std::min(n0.bits.size(), n1.bits.size());
	for (int i = 0; i < m; i++)
	{
		result.bits.push_back((n0.bits[i]&n1.bits[i]&~carry) | (~n0.bits[i]&~n1.bits[i]&~carry) | (~n0.bits[i]&n1.bits[i]&carry));
		carry = (n0.bits[i]&~n1.bits[i]) | (n0.bits[i]&carry) | (~n1.bits[i]&carry);
	}
	for (int i = m; i < (int)n0.bits.size(); i++)
	{
		result.bits.push_back(~n0.bits[i]&~carry);
		carry = n0.bits[i] | carry;
	}
	for (int i = m; i < (int)n1.bits.size(); i++)
	{
		result.bits.push_back((~n1.bits[i]&~carry) | (n1.bits[i]&carry));
		carry = ~n1.bits[i]&carry;
	}
	result.bits.push_back(carry);

	return result;
}

boolean::unsigned_int operator*(boolean::unsigned_int n0, boolean::unsigned_int n1)
{
	boolean::unsigned_int result;
	for (int i = 0; i < (int)n1.bits.size(); i++)
		result = result + ((n0&n1.bits[i]) << i);
	return result;
}

boolean::unsigned_int operator/(boolean::unsigned_int n0, boolean::unsigned_int n1)
{
	boolean::unsigned_int result;
	result.bits.resize(n0.bits.size(), boolean::cover(0));

	int s = (int)n0.bits.size()-1;
	for (int i = s; i >= 0; i--)
	{
		boolean::unsigned_int divisor = (n1 << i);
		boolean::cover condition = (divisor < n0);
		result.bits[i] = condition;
		n0 = (condition&(n0-divisor)) | (~condition&n0);
		n0.simplify();
	}

	return result;
}

boolean::cover operator<(boolean::unsigned_int n0, boolean::unsigned_int n1)
{
	boolean::cover disjunction(0);
	boolean::cover conjunction(1);

	// if n1 has more bits than n0, all it takes is one of those bits
	// to be set for n0 to be less than n1
	for (int i = (int)n1.bits.size()-1; i >= (int)n0.bits.size(); i--)
		disjunction |= n1.bits[i];
	// if n0 has more bits than n1, then every bit must be 0
	for (int i = (int)n0.bits.size()-1; i >= (int)n1.bits.size(); i--)
		conjunction &= ~n0.bits[i];

	int m = (int)std::min((int)n0.bits.size()-1, (int)n1.bits.size()-1);
	for (int i = m; i >= 0; i--)
	{
		disjunction |= ~n0.bits[i]&n1.bits[i]&conjunction;
		conjunction &= (n0.bits[i]&n1.bits[i]) | (~n0.bits[i]&~n1.bits[i]);
	}

	return disjunction;
}

boolean::cover operator==(boolean::unsigned_int n0, boolean::unsigned_int n1)
{
	boolean::cover conjunction(1);

	// if n1 has more bits than n0, all it takes is one of those bits
	// to be set for n0 to be less than n1
	for (int i = (int)n1.bits.size()-1; i >= (int)n0.bits.size(); i--)
		conjunction &= ~n1.bits[i];
	for (int i = (int)n0.bits.size()-1; i >= (int)n1.bits.size(); i--)
		conjunction &= ~n0.bits[i];

	int m = (int)std::min((int)n0.bits.size()-1, (int)n1.bits.size()-1);
	for (int i = m; i >= 0; i--)
	{
		conjunction &= (n0.bits[i]&n1.bits[i]) | (~n0.bits[i]&~n1.bits[i]);
	}

	return conjunction;
}

boolean::signed_int operator&(boolean::signed_int n0, boolean::signed_int n1)
{
	boolean::signed_int result;
	int m = (int)std::min(n0.bits.size(), n1.bits.size());
	for (int i = 0; i < m; i++)
		result.bits.push_back(n0.bits[i] & n1.bits[i]);
	for (int i = m; i < (int)n0.bits.size(); i++)
		result.bits.push_back(n0.bits[i] & n1.sign);
	for (int i = m; i < (int)n1.bits.size(); i++)
		result.bits.push_back(n0.sign & n1.bits[i]);
	result.sign = n0.sign & n1.sign;
	return result;
}

boolean::signed_int operator|(boolean::signed_int n0, boolean::signed_int n1)
{
	boolean::signed_int result;
	int m = (int)std::min(n0.bits.size(), n1.bits.size());
	for (int i = 0; i < m; i++)
		result.bits.push_back(n0.bits[i] | n1.bits[i]);
	for (int i = m; i < (int)n0.bits.size(); i++)
		result.bits.push_back(n0.bits[i] | n1.sign);
	for (int i = m; i < (int)n1.bits.size(); i++)
		result.bits.push_back(n0.sign | n1.bits[i]);
	result.sign = n0.sign | n1.sign;
	return result;
}

boolean::signed_int operator~(boolean::signed_int n)
{
	boolean::signed_int result;
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(~n.bits[i]);
	result.sign = ~n.sign;
	return result;
}

boolean::signed_int operator&(boolean::signed_int n, boolean::cover c)
{
	for (int i = 0; i < (int)n.bits.size(); i++)
		n.bits[i] &= c;
	n.sign &= c;
	return n;
}

boolean::signed_int operator|(boolean::signed_int n, boolean::cover c)
{
	for (int i = 0; i < (int)n.bits.size(); i++)
		n.bits[i] |= c;
	n.sign |= c;
	return n;
}

boolean::signed_int operator&(boolean::cover c, boolean::signed_int n)
{
	for (int i = 0; i < (int)n.bits.size(); i++)
		n.bits[i] &= c;
	n.sign &= c;
	return n;
}

boolean::signed_int operator|(boolean::cover c, boolean::signed_int n)
{
	for (int i = 0; i < (int)n.bits.size(); i++)
		n.bits[i] |= c;
	n.sign |= c;
	return n;
}

boolean::signed_int operator<<(boolean::signed_int n, int s)
{
	boolean::signed_int result;
	result.bits.reserve(s + (int)n.bits.size());
	for (int i = 0; i < s; i++)
		result.bits.push_back(boolean::cover(0));
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i]);
	result.sign = n.sign;
	return result;
}

boolean::signed_int operator>>(boolean::signed_int n, int s)
{
	boolean::signed_int result;
	result.bits.reserve(std::max((int)n.bits.size() - s, 0));
	for (int i = s; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i]);
	result.sign = n.sign;
	return result;
}

boolean::signed_int operator-(boolean::signed_int n)
{
	boolean::signed_int result;
	boolean::cover carry = 1, inf;
	for (int i = 0; i < (int)n.bits.size(); i++)
	{
		result.bits.push_back((~n.bits[i]&~carry) | (n.bits[i]&carry));
		carry = (~n.bits[i]&carry);
	}

	result.sign = (~n.sign&~carry) | (n.sign&carry);
	inf = (~n.sign&carry);

	while (inf != carry)
	{
		carry = inf;
		result.bits.push_back(result.sign);
		result.sign = (~n.sign&~carry) | (n.sign&carry);
		inf = (~n.sign&carry);
	}

	return result;
}

boolean::signed_int operator+(boolean::signed_int n0, boolean::signed_int n1)
{
	boolean::signed_int result;
	boolean::cover carry = 0, inf;
	int m = (int)std::min(n0.bits.size(), n1.bits.size());
	for (int i = 0; i < m; i++)
	{
		result.bits.push_back((n0.bits[i]&~n1.bits[i]&~carry) | (~n0.bits[i]&n1.bits[i]&~carry) | (~n0.bits[i]&~n1.bits[i]&carry));
		carry = (n0.bits[i]&n1.bits[i]) | (n0.bits[i]&carry) | (n1.bits[i]&carry);
	}
	for (int i = m; i < (int)n0.bits.size(); i++)
	{
		result.bits.push_back((n0.bits[i]&~n1.sign&~carry) | (~n0.bits[i]&n1.sign&~carry) | (~n0.bits[i]&~n1.sign&carry));
		carry = (n0.bits[i]&n1.sign) | (n0.bits[i]&carry) | (n1.sign&carry);
	}
	for (int i = m; i < (int)n1.bits.size(); i++)
	{
		result.bits.push_back((n0.sign&~n1.bits[i]&~carry) | (~n0.sign&n1.bits[i]&~carry) | (~n0.sign&~n1.bits[i]&carry));
		carry = (n0.sign&n1.bits[i]) | (n0.sign&carry) | (n1.bits[i]&carry);
	}

	result.sign = (n0.sign&~n1.sign&~carry) | (~n0.sign&n1.sign&~carry) | (~n0.sign&~n1.sign&carry);
	inf = (n0.sign&n1.sign) | (n0.sign&carry) | (n1.sign&carry);

	while (inf != carry)
	{
		carry = inf;
		result.bits.push_back(result.sign);
		result.sign = (n0.sign&~n1.sign&~carry) | (~n0.sign&n1.sign&~carry) | (~n0.sign&~n1.sign&carry);
		inf = (n0.sign&n1.sign) | (n0.sign&carry) | (n1.sign&carry);
	}

	return result;
}

boolean::signed_int operator-(boolean::signed_int n0, boolean::signed_int n1)
{
	boolean::signed_int result;
	boolean::cover carry = 1, inf;
	int m = (int)std::min(n0.bits.size(), n1.bits.size());
	for (int i = 0; i < m; i++)
	{
		result.bits.push_back((n0.bits[i]&n1.bits[i]&~carry) | (~n0.bits[i]&~n1.bits[i]&~carry) | (~n0.bits[i]&n1.bits[i]&carry));
		carry = (n0.bits[i]&~n1.bits[i]) | (n0.bits[i]&carry) | (~n1.bits[i]&carry);
	}
	for (int i = m; i < (int)n0.bits.size(); i++)
	{
		result.bits.push_back((n0.bits[i]&n1.sign&~carry) | (~n0.bits[i]&~n1.sign&~carry) | (~n0.bits[i]&n1.sign&carry));
		carry = (n0.bits[i]&~n1.sign) | (n0.bits[i]&carry) | (~n1.sign&carry);
	}
	for (int i = m; i < (int)n1.bits.size(); i++)
	{
		result.bits.push_back((n0.sign&n1.bits[i]&~carry) | (~n0.sign&~n1.bits[i]&~carry) | (~n0.sign&n1.bits[i]&carry));
		carry = (n0.sign&~n1.bits[i]) | (n0.sign&carry) | (~n1.bits[i]&carry);
	}

	result.sign = (n0.sign&n1.sign&~carry) | (~n0.sign&~n1.sign&~carry) | (~n0.sign&n1.sign&carry);
	inf = (n0.sign&~n1.sign) | (n0.sign&carry) | (~n1.sign&carry);

	while (inf != carry)
	{
		carry = inf;
		result.bits.push_back(result.sign);
		result.sign = (n0.sign&n1.sign&~carry) | (~n0.sign&~n1.sign&~carry) | (~n0.sign&n1.sign&carry);
		inf = (n0.sign&~n1.sign) | (n0.sign&carry) | (~n1.sign&carry);
	}

	return result;
}

boolean::signed_int operator*(boolean::signed_int n0, boolean::signed_int n1)
{
	boolean::signed_int result;
	for (int i = 0; i < (int)n1.bits.size(); i++)
		result = result + ((n0&n1.bits[i]) << i);
	return result;
}

boolean::signed_int operator/(boolean::signed_int n0, boolean::signed_int n1)
{
	boolean::signed_int result;
	result.bits.resize(n0.bits.size(), boolean::cover(0));
	result.sign = 0;

	boolean::cover n0_sign = n0.sign;
	boolean::cover n1_sign = n1.sign;

	// take the absolute value of each signed_int
	n0 = (n0_sign&~n0) | (~n0_sign&n0);
	n1 = (n1_sign&~n1) | (~n1_sign&n1);

	int s = (int)n0.bits.size()-1;
	for (int i = s; i >= 0; i--)
	{
		boolean::signed_int divisor = (n1 << i);
		boolean::cover condition = (divisor < n0);
		result.bits[i] = condition;
		n0 = (condition&(n0-divisor)) | (~condition&n0);
		n0.simplify();
	}

	boolean::cover sign = ((n0_sign&~n1_sign) | (~n0_sign&n1_sign));

	result = (sign&(-result)) | (~sign&result);
	return result;
}

boolean::cover operator<(boolean::signed_int n0, boolean::signed_int n1)
{
	boolean::cover n0n_n1p = n0.sign&~n1.sign;
	boolean::cover n0s_n1s = (n0.sign&n1.sign) | (~n0.sign&~n1.sign);
	boolean::cover disjunction(0);
	boolean::cover conjunction(1);

	// take the absolute value of each signed_int
	n0 = (n0.sign&~n0) | (~n0.sign&n0);
	n1 = (n1.sign&~n1) | (~n1.sign&n1);

	// if n1 has more bits than n0, all it takes is one of those bits
	// to be set for n0 to be less than n1
	for (int i = (int)n1.bits.size()-1; i >= (int)n0.bits.size(); i--)
		disjunction |= n1.bits[i];
	// if n0 has more bits than n1, then every bit must be 0
	for (int i = (int)n0.bits.size()-1; i >= (int)n1.bits.size(); i--)
		conjunction &= ~n0.bits[i];

	int m = (int)std::min((int)n0.bits.size()-1, (int)n1.bits.size()-1);
	for (int i = m; i >= 0; i--)
	{
		disjunction |= ~n0.bits[i]&n1.bits[i]&conjunction;
		conjunction &= (n0.bits[i]&n1.bits[i]) | (~n0.bits[i]&~n1.bits[i]);
	}

	return n0n_n1p | (n0s_n1s&disjunction);
}
