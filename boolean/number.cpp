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

struct node;

struct index
{
	index(int bit, int cube)
	{
		this->bit = bit;
		this->cube = cube;
	}
	~index() {}

	int bit;
	int cube;
};

struct arc
{
	list<node>::iterator left;
	list<node>::iterator right;
	float weight;
};

bool operator<(arc a1, arc a2) {
	return a1.weight < a2.weight;
}

struct node
{
	node(index idx) {	indices.push_back(idx); }
	~node() { }

	vector<index> indices;
	vector<list<arc>::iterator> arcs;
};

float unsigned_int::partition(unsigned_int &left, unsigned_int &right)
{
	list<node> nodes;
	list<arc> arcs;

	// Build a weighted undirected graph in which the weights are the similarity
	// between a given pair of cubes. The similarity is the number of literals
	// shared by the two cubes. Arcs connect groups of cubes, but we start with
	// each arc connecting only single cubes.
	for (int i = 0; i < (int)bits.size(); i++) {
		if (not bits[i].is_null() and not bits[i].is_tautology()) {
			for (int j = 0; j < (int)bits[i].cubes.size(); j++) {
				nodes.push_back(node(index(i, j)));
			}
		}
	}

	for (list<node>::iterator i = nodes.begin(); i != nodes.end(); i++) {
		for (list<node>::iterator j = std::next(i); j != nodes.end(); j++) {
			arc add;
			add.left = i;
			add.right = j;
			index l = i->indices[0];
			index r = j->indices[0];
			add.weight = (float)similarity(bits[l.bit].cubes[l.cube], bits[r.bit].cubes[r.cube]);
			if (add.weight > 0) {
				add.weight = add.weight*add.weight/(float)(bits[l.bit].cubes[l.cube].width()*bits[r.bit].cubes[r.cube].width());
				arcs.push_back(add);
				i->arcs.push_back(std::prev(arcs.end()));
				j->arcs.push_back(std::prev(arcs.end()));
			}
		}
	}

	// We want to consecutively merge the heaviest arcs
	printf("Partitioning %lu arcs\n", arcs.size());
	while (arcs.begin() != arcs.end() and std::next(arcs.begin()) != arcs.end())
	{
		//printf("%lu %lu\n", arcs.size(), nodes.size());

		// grab the heaviest arc
		list<arc>::iterator m = arcs.begin();
		unsigned int mhi, mlo;
		if (m->left->indices.size() > m->right->indices.size()) {
			mhi = m->left->indices.size();
			mlo = m->right->indices.size();
		} else {
			mhi = m->right->indices.size();
			mlo = m->left->indices.size();
		}
		for (list<arc>::iterator a = std::next(m); a != arcs.end(); a++) {
			unsigned ahi, alo;
			if (a->left->indices.size() > a->right->indices.size()) {
				ahi = a->left->indices.size();
				alo = a->right->indices.size();
			} else {
				ahi = a->right->indices.size();
				alo = a->left->indices.size();
			}
			if (ahi < mhi or (ahi == mhi and (alo < mlo or (alo == mlo and a->weight > m->weight)))) {
				m = a;
				mhi = ahi;
				mlo = alo;
			}
		}
		
		list<node>::iterator l = m->left;
		list<node>::iterator r = m->right;
		// merge the right node into the left node, updating any arcs along the way
		l->indices.insert(l->indices.end(), r->indices.begin(), r->indices.end());
		for (vector<list<arc>::iterator>::iterator a = r->arcs.begin(); a != r->arcs.end(); a++) {
			if ((*a)->left == r) {
				(*a)->left = l;
			}
			if ((*a)->right == r) {
				(*a)->right = l;
			}
		}
		l->arcs.insert(l->arcs.begin(), r->arcs.begin(), r->arcs.end());
		
		nodes.erase(r);
		
		// remove looping and duplicate arcs
		vector<list<arc>::iterator> to_erase;
		for (vector<list<arc>::iterator>::iterator a = l->arcs.begin(); a != l->arcs.end();) {
			if ((*a)->left == (*a)->right) {
				if (find(to_erase.begin(), to_erase.end(), *a) == to_erase.end()) {
					to_erase.push_back(*a);
				}
				a = l->arcs.erase(a);
			} else {
				for (vector<list<arc>::iterator>::iterator b = std::next(a); b != l->arcs.end();) {
					if (((*a)->left == (*b)->left and (*a)->right == (*b)->right)
					  or ((*a)->left == (*b)->right and (*a)->right == (*b)->left)) {
						(*a)->weight += (*b)->weight;
						if (find(to_erase.begin(), to_erase.end(), *b) == to_erase.end()) {
							to_erase.push_back(*b);
						}
						if ((*b)->left == l) {
							(*b)->right->arcs.erase(find((*b)->right->arcs.begin(), (*b)->right->arcs.end(), *b));
						} else {
							(*b)->left->arcs.erase(find((*b)->left->arcs.begin(), (*b)->left->arcs.end(), *b));
						}
						b = l->arcs.erase(b);
					} else {
						b++;
					}
				}
				a++;
			}
		}

		for (vector<list<arc>::iterator>::iterator a = to_erase.begin(); a != to_erase.end(); a++) {
			arcs.erase(*a);
		}
	}

	if (arcs.size() == 0)
		return 0.0f;

	left.bits.clear();
	right.bits.clear();
	left.bits.resize(bits.size(), cover());
	right.bits.resize(bits.size(), cover());

	arc a = arcs.back();
	for (vector<index>::iterator i = a.left->indices.begin(); i != a.left->indices.end(); i++) {
		left.bits[i->bit].cubes.push_back(bits[i->bit].cubes[i->cube]);
	}
	for (vector<index>::iterator i = a.right->indices.begin(); i != a.right->indices.end(); i++) {
		right.bits[i->bit].cubes.push_back(bits[i->bit].cubes[i->cube]);
	}
	for (int i = 0; i < (int)bits.size(); i++) {
		if (bits[i].is_tautology()) {
			left.bits[i] = cover(1);
			right.bits[i] = cover(1);
		}
	}

	return a.weight;
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
