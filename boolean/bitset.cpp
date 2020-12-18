/*
 * bitset.cpp
 *
 *  Created on: Dec 18, 2020
 *      Author: nbingham
 */

#include <boolean/bitset.h>
#include <common/standard.h>

namespace boolean
{

bitset::bitset()
{
}

bitset::bitset(int width, int offset)
{
	for (int i = 0; i < width; i++)
		bits.push_back(cover(i+offset, 1));
}

bitset::bitset(const cover &c)
{
	bits.push_back(c);
}

bitset::bitset(const bitset &n)
{
	bits = n.bits;
}

bitset::~bitset()
{
}

bitset &bitset::espresso()
{
	for (int i = 0; i < (int)bits.size(); i++)
		bits[i].espresso();
	return *this;
}

bitset &bitset::resize(size_t size)
{
	bits.resize(size, extend());
	return *this;
}

bitset &bitset::append(const bitset &b)
{
	bits.insert(bits.end(), b.bits.begin(), b.bits.end());
	return *this;
}

bitset bitset::subset(size_t start, size_t length)
{
	bitset result;
	if (length == (size_t)-1) {
		length = bits.size() - start;
	}

	result.bits.insert(result.bits.end(), bits.begin() + start, bits.begin() + start + length);
	return result;
}

cover bitset::extend() const
{
	return 0;
}

cube bitset::supercube() const
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

bool bitset::is_constant() const
{
	for (int i = 0; i < (int)bits.size(); i++)
		if (not bits[i].is_null() and not bits[i].is_tautology())
			return false;
	return true;
}

int bitset::max_width() const
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

int bitset::depth() const
{
	int result = 0;
	for (int i = 0; i < (int)bits.size(); i++) {
		if (not bits[i].is_null() and not bits[i].is_tautology()) {
			result += (int)bits[i].cubes.size();
		}
	}
	return result;
}

void bitset::cofactor(const cube &s1)
{
	for (int i = 0; i < (int)bits.size(); i++)
		bits[i].cofactor(s1);
}

float bitset::partition(bitset &left, bitset &right) const
{
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

	struct node
	{
		node(index idx) {	indices.push_back(idx); }
		~node() { }

		vector<index> indices;
		vector<list<arc>::iterator> arcs;
	};

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
			// We cannot remove zero weighted edges here because we need to be able
			// to split on one. This algorithm ultimately must return an *edge*.
			add.weight = add.weight*add.weight/(float)(bits[l.bit].cubes[l.cube].width()*bits[r.bit].cubes[r.cube].width());
			arcs.push_back(add);
			i->arcs.push_back(std::prev(arcs.end()));
			j->arcs.push_back(std::prev(arcs.end()));
		}
	}

	// We want to consecutively merge the heaviest arcs
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
	left.bits.resize(bits.size(), boolean::cover());
	right.bits.resize(bits.size(), boolean::cover());

	arc a = arcs.back();
	for (vector<index>::iterator i = a.left->indices.begin(); i != a.left->indices.end(); i++) {
		left.bits[i->bit].cubes.push_back(bits[i->bit].cubes[i->cube]);
	}
	for (vector<index>::iterator i = a.right->indices.begin(); i != a.right->indices.end(); i++) {
		right.bits[i->bit].cubes.push_back(bits[i->bit].cubes[i->cube]);
	}
	for (int i = 0; i < (int)bits.size(); i++) {
		if (bits[i].is_tautology()) {
			left.bits[i] = boolean::cover(1);
			right.bits[i] = boolean::cover(1);
		}
	}

	return a.weight;
}

bitset bitset::decompose_hfactor(map<boolean::cube, int> &factors, int width, int offset, vector<int> hide) const
{
	if (max_width() >= width and depth() > 1) {
		boolean::cube common = supercube();
		common.hide(hide);
		if (common.width() < width)
		{
			bitset c_left, c_right;
			bitset result, left_result, right_result;
			partition(c_left, c_right);

			left_result = c_left.decompose_hfactor(factors, width, offset, hide);
			right_result = c_right.decompose_hfactor(factors, width, offset, hide);
			return left_result | right_result;
		}
		else
		{
			int index = -1;
			map<boolean::cube, int>::iterator loc = factors.find(common);
			if (loc == factors.end()) {
				index = offset + factors.size();
				factors.insert(pair<boolean::cube, int>(common, index));
			} else {
				index = loc->second;
			}

			return boolean::cube(index, 1) & boolean::cofactor(*this, common).decompose_hfactor(factors, width, offset, hide);
		}
	}

	return *this;
}

bitset bitset::decompose_xfactor(map<boolean::cube, int> &factors, int width, int offset, vector<int> hide) const
{
	if (max_width() >= width and depth() > 1) {
		bitset nc = ~*this;
		boolean::cube common = supercube();
		boolean::cube ncommon = nc.supercube();
		common.hide(hide);
		ncommon.hide(hide);
		int cw = common.width(), ncw = ncommon.width();

		if (cw < width and ncw < width) {
			bitset c_left, c_right, nc_left, nc_right;
			bitset result, left_result, right_result;
			float c_weight, nc_weight;
			
			c_weight = partition(nc_left, c_right);
			nc_weight = nc.partition(nc_left, nc_right);

			if (c_weight <= nc_weight)
			{
				left_result = c_left.decompose_xfactor(factors, width, offset, hide);
				right_result = c_right.decompose_xfactor(factors, width, offset, hide);
				result = left_result | right_result;
			}
			else if (nc_weight < c_weight)
			{
				left_result = nc_left.decompose_xfactor(factors, width, offset, hide);
				right_result = nc_right.decompose_xfactor(factors, width, offset, hide);
				result = ~(left_result | right_result);
			}
			return result;
		} else if (cw >= ncw) {
			map<boolean::cube, int>::iterator loc = factors.find(common);
			int index = -1;
			if (loc == factors.end()) {
				index = offset + factors.size();
				factors.insert(pair<boolean::cube, int>(common, index));
			} else {
				index = loc->second;
			}
		
			return boolean::cube(index, 1) & boolean::cofactor(*this, common).decompose_xfactor(factors, width, offset, hide);
		} else if (ncw > cw) {
			map<boolean::cube, int>::iterator loc = factors.find(ncommon);
			int index = -1;
			if (loc == factors.end()) {
				index = offset + factors.size();
				factors.insert(pair<boolean::cube, int>(ncommon, index));
			} else {
				index = loc->second;
			}
			
			return ~(boolean::cube(index, 1) & boolean::cofactor(nc, ncommon).decompose_xfactor(factors, width, offset, hide));
		}
	}

	return *this;
}


bitset &bitset::operator=(const bitset &n)
{
	bits = n.bits;
	return *this;
}

bitset &bitset::operator|=(const bitset &n)
{
	*this = *this | n;
	return *this;
}

bitset &bitset::operator&=(const bitset &n)
{
	*this = (*this & n);
	return *this;
}

bitset &bitset::operator^=(const bitset &n)
{
	*this = *this ^ n;
	return *this;
}

bitset &bitset::operator<<=(int s)
{
	*this = *this << s;
	return *this;
}

bitset &bitset::operator>>=(int s)
{
	*this = *this >> s;
	return *this;
}

bitset cofactor(const bitset &s0, const cube &s1)
{
	bitset result;
	result.bits.reserve(s0.bits.size());
	for (int i = 0; i < (int)s0.bits.size(); i++) {
		result.bits.push_back(cofactor(s0.bits[i], s1));
	}
	return result;
}

bitset operator&(const bitset &n0, const bitset &n1)
{
	bitset result;
	int m = (int)std::min(n0.bits.size(), n1.bits.size());
	for (int i = 0; i < m; i++) {
		result.bits.push_back(n0.bits[i] & n1.bits[i]);
	}
	if (not n1.extend().is_null()) {
		for (int i = m; i < (int)n0.bits.size(); i++) {
			result.bits.push_back(n0.bits[i] & n1.extend());
		}
	}
	if (not n0.extend().is_null()) {
		for (int i = m; i < (int)n1.bits.size(); i++) {
			result.bits.push_back(n0.extend() & n1.bits[i]);
		}
	}
	return result;
}

bitset operator|(const bitset &n0, const bitset &n1)
{
	bitset result;
	int m = (int)std::min(n0.bits.size(), n1.bits.size());
	for (int i = 0; i < m; i++) {
		result.bits.push_back(n0.bits[i] | n1.bits[i]);
	}
	if (not n1.extend().is_tautology()) {
		for (int i = m; i < (int)n0.bits.size(); i++) {
			result.bits.push_back(n0.bits[i] | n1.extend());
		}
	}
	if (not n0.extend().is_tautology()) {
		for (int i = m; i < (int)n1.bits.size(); i++) {
			result.bits.push_back(n0.extend() | n1.bits[i]);
		}
	}
	return result;
}

bitset operator^(const bitset &n0, const bitset &n1)
{
	bitset result;
	int m = (int)std::min(n0.bits.size(), n1.bits.size());
	for (int i = 0; i < m; i++) {
		result.bits.push_back(n0.bits[i] ^ n1.bits[i]);
	}
	for (int i = m; i < (int)n0.bits.size(); i++) {
		result.bits.push_back(n0.bits[i] ^ n1.extend());
	}
	for (int i = m; i < (int)n1.bits.size(); i++) {
		result.bits.push_back(n0.extend() ^ n1.bits[i]);
	}
	return result;
}

bitset operator~(const bitset &n)
{
	bitset result;
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(~n.bits[i]);
	return result;
}

bitset operator&(const bitset &n, const cover &c)
{
	bitset result;
	result.bits.reserve(n.bits.size());
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i] & c);
	return result;
}

bitset operator|(const bitset &n, const cover &c)
{
	bitset result;
	result.bits.reserve(n.bits.size());
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i] | c);
	return result;
}

bitset operator^(const bitset &n, const cover &c)
{
	bitset result;
	result.bits.reserve(n.bits.size());
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i] ^ c);
	return result;
}

bitset operator&(const bitset &n, const cube &c)
{
	bitset result;
	result.bits.reserve(n.bits.size());
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i] & c);
	return result;
}

bitset operator|(const bitset &n, const cube &c)
{
	bitset result;
	result.bits.reserve(n.bits.size());
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i] | c);
	return result;
}

bitset operator^(const bitset &n, const cube &c)
{
	bitset result;
	result.bits.reserve(n.bits.size());
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i] ^ c);
	return result;
}

bitset operator&(const cover &c, const bitset &n)
{
	bitset result;
	result.bits.reserve(n.bits.size());
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i] & c);
	return result;
}

bitset operator|(const cover &c, const bitset &n)
{
	bitset result;
	result.bits.reserve(n.bits.size());
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i] | c);
	return result;
}

bitset operator^(const cover &c, const bitset &n)
{
	bitset result;
	result.bits.reserve(n.bits.size());
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i] ^ c);
	return result;
}

bitset operator&(const cube &c, const bitset &n)
{
	bitset result;
	result.bits.reserve(n.bits.size());
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i] & c);
	return result;
}

bitset operator|(const cube &c, const bitset &n)
{
	bitset result;
	result.bits.reserve(n.bits.size());
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i] | c);
	return result;
}

bitset operator^(const cube &c, const bitset &n)
{
	bitset result;
	result.bits.reserve(n.bits.size());
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i] ^ c);
	return result;
}

bitset operator<<(const bitset &n, int s)
{
	bitset result;
	result.bits.reserve(s + (int)n.bits.size());
	for (int i = 0; i < s; i++)
		result.bits.push_back(cover(0));
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i]);
	return result;
}

bitset operator>>(const bitset &n, int s)
{
	bitset result;
	result.bits.reserve(std::max((int)n.bits.size() - s, 0));
	for (int i = s; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i]);
	return result;
}

cover operator==(const bitset &n0, const bitset &n1)
{
	cover conjunction(1);

	// if n1 has more bits than n0, all it takes is one of those bits
	// to be set for n0 to be less than n1
	conjunction &= (n0.extend()&n1.extend()) | (~n0.extend()&~n1.extend());
	for (int i = (int)n1.bits.size()-1; i >= (int)n0.bits.size(); i--)
		conjunction &= (n0.extend()&n1.bits[i]) | (~n0.extend()&~n1.bits[i]);
	for (int i = (int)n0.bits.size()-1; i >= (int)n1.bits.size(); i--)
		conjunction &= (n0.bits[i]&n1.extend()) | (~n0.bits[i]&~n1.extend());

	if (not conjunction.is_null()) {
		int m = (int)std::min((int)n0.bits.size()-1, (int)n1.bits.size()-1);
		for (int i = m; i >= 0; i--)
		{
			conjunction &= (n0.bits[i]&n1.bits[i]) | (~n0.bits[i]&~n1.bits[i]);
		}
	}

	return conjunction;
}

cover operator!=(const bitset &n0, const bitset &n1)
{
	return ~(n0 == n1);
}

}
