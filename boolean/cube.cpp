/*
 * cube.cpp
 *
 *  Created on: Jul 4, 2014
 *      Author: nbingham
 */

#include "cube.h"
#include "cover.h"
#include <common/math.h>

using std::min;
using std::max;

namespace boolean
{
cube::cube()
{
}

cube::cube(const cube &m)
{
	values = m.values;
}

cube::cube(int val)
{
	if (val == 0)
		values.push_back(0x00000000);
}

cube::cube(int uid, int val)
{
	extendX(uid/16 + 1);

	int w		= uid/16;
	unsigned int i		= 2*(uid%16);
	unsigned int v		= (val+1) << i;
	unsigned int m		= 3 << i;

	values[w] = (values[w] & ~m) | (v & m);
}

cube::~cube()
{
}

int cube::size() const
{
	return (int)values.size();
}

void cube::extendX(int num)
{
	values.insert(values.end(), num, 0xFFFFFFFF);
}

void cube::extendN(int num)
{
	values.insert(values.end(), num, 0x00000000);
}

void cube::trunk(int size)
{
	values.erase(values.begin() + size, values.end());
}

int cube::get(int uid) const
{
	int w = uid/16;
	if (w >= size())
		return 2;
	else
		return ((values[w] >> (2*(uid%16))) & 3) - 1;
}

void cube::set(int uid, int val)
{
	int w	= uid/16;
	if (w >= size())
		extendX(w+1 - size());

	unsigned int i	= 2*(uid%16);
	unsigned int v	= (val+1) << i;
	unsigned int m	= 3   << i;
	values[w] = (values[w] & ~m) | (v & m);
}

void cube::sv_union(int uid, int val)
{
	int w	= uid/16;
	if (w >= size())
		extendX(w+1 - size());

	values[w] |= ((val+1) << (2*(uid%16)));
}

void cube::sv_intersect(int uid, int val)
{
	int w	= uid/16;
	if (w >= size())
		extendX(w+1 - size());

	unsigned int i = 2*(uid%16);
	values[w] &= (((val+1) << i) | ~(3 << i));
}

void cube::sv_invert(int uid)
{
	int w	= uid/16;
	if (w >= size())
		extendX(w+1 - size());

	values[w] ^= (3 << (2*(uid%16)));
}

void cube::sv_or(int uid, int val)
{
	int w	= uid/16;
	if (w >= size())
		extendX(w+1 - size());

	unsigned int i = 2*(uid%16);
	unsigned int v = ((val+1) << i) | (0x55555555 & ~(3 << i));
	values[w] = (((values[w]&(v << 1)) | (values[w]&v) | (v&(values[w] << 1))) & 0xAAAAAAAA) | (values[w] & v & 0x55555555);
}

void cube::sv_and(int uid, int val)
{
	int w	= uid/16;
	if (w >= size())
		extendX(w+1 - size());

	unsigned int i = 2*(uid%16);
	unsigned int v = ((val+1) << i) | (0xAAAAAAAA & ~(3 << i));
	values[w] = (values[w] & v & 0xAAAAAAAA) | (((values[w]&(v >> 1)) | (values[w]&v) | (v&(values[w] >> 1))) & 0x55555555);
}

void cube::sv_not(int uid)
{
	int w	= uid/16;
	if (w >= size())
		extendX(w+1 - size());

	unsigned int i = 2*(uid%16);
	unsigned int m0 = (1 << i);
	unsigned int m1 = (2 << i);
	values[w] = (values[w] & ~(m0 | m1)) | (((values[w] & m0) << 1) & 0xFFFFFFFE) | (((values[w] & m1) >> 1) & 0x7FFFFFFF);
}

bool cube::is_subset_of(const cube &s) const
{
	int m0 = min(size(), s.size());
	int i = 0;
	for (; i < m0; i++)
		if ((values[i] & s.values[i]) != values[i])
			return false;
	for (; i < s.size(); i++)
		if (s.values[i] != 0xFFFFFFFF)
			return false;
	return true;
}

bool cube::is_subset_of(const cover &s) const
{
	return boolean::cofactor(s, *this).is_tautology();
}

bool cube::is_strict_subset_of(const cube &s) const
{
	int m0 = min(size(), s.size());
	int i = 0;
	bool eq = true;
	for (; i < m0; i++)
	{
		eq = (eq && (values[i] == s.values[i]));
		if ((values[i] & s.values[i]) != values[i])
			return false;
	}
	for (; i < size(); i++)
		eq = (eq && (values[i] == 0xFFFFFFFF));

	return !eq;
}

bool cube::is_tautology() const
{
	for (int i = 0; i < size(); i++)
		if (values[i] != 0xFFFFFFFF)
			return false;

	return true;
}

bool cube::is_null() const
{
	for (int i = 0; i < size(); i++)
		if (((values[i]>>1) | values[i] | 0xAAAAAAAA) != 0xFFFFFFFF)
			return true;

	return false;
}

int cube::memory_width() const
{
	// This looks ugly, but its just a loop unrolled binary search
	for (int i = size()-1; i != -1; i--)
		if (values[i] != 0xFFFFFFFF)
		{
			if ((values[i] & 0xFFFF0000) != 0xFFFF0000)
			{
				if ((values[i] & 0xFF000000) != 0xFF000000)
				{
					if ((values[i] & 0xF0000000) != 0xF0000000)
					{
						if ((values[i] & 0xC0000000) != 0xC0000000)
							return i*16 + 16;
						else
							return i*16 + 15;
					}
					else
					{
						if ((values[i] & 0x0C000000) != 0x0C000000)
							return i*16 + 14;
						else
							return i*16 + 13;
					}
				}
				else
				{
					if ((values[i] & 0x00F00000) != 0x00F00000)
					{
						if ((values[i] & 0x00C00000) != 0x00C00000)
							return i*16 + 12;
						else
							return i*16 + 11;
					}
					else
					{
						if ((values[i] & 0x000C0000) != 0x000C0000)
							return i*16 + 10;
						else
							return i*16 + 9;
					}
				}
			}
			else
			{
				if ((values[i] & 0x0000FF00) != 0x0000FF00)
				{
					if ((values[i] & 0x0000F000) != 0x0000F000)
					{
						if ((values[i] & 0x0000C000) != 0x0000C000)
							return i*16 + 8;
						else
							return i*16 + 7;
					}
					else
					{
						if ((values[i] & 0x00000C00) != 0x00000C00)
							return i*16 + 6;
						else
							return i*16 + 5;
					}
				}
				else
				{
					if ((values[i] & 0x000000F0) != 0x000000F0)
					{
						if ((values[i] & 0x000000C0) != 0x000000C0)
							return i*16 + 4;
						else
							return i*16 + 3;
					}
					else
					{
						if ((values[i] & 0x0000000C) != 0x0000000C)
							return i*16 + 2;
						else
							return i*16 + 1;
					}
				}
			}
		}

	return 0;
}

int cube::width() const
{
	int result = 0;
	for (int i = 0; i < size(); i++)
		if (values[i] != 0xFFFFFFFF)
		{
			unsigned int x = values[i] & (values[i] >> 1) & 0x55555555;
			x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
			x = (x + (x >> 4)) & 0x0F0F0F0F;
			x += (x >> 8);
			x += (x >> 16);
			result += (16 - (x & 0x0000003F));
		}

	return result;
}

cube cube::xoutnulls()
{
	cube result(*this);
	for (int i = 0; i < result.size(); i++)
	{
		unsigned int a = ~result.values[i] & (~result.values[i] >> 1) & 0x55555555;
		result.values[i] |= (a | (a << 1));
	}
	return result;
}

cube cube::mask()
{
	cube result(*this);
	for (int i = 0; i < result.size(); i++)
		result.values[i] = (((((result.values[i]>>1)&0x55555555)^(result.values[i]&0x55555555)) | ((result.values[i]&0xAAAAAAAA)^((result.values[i]<<1)&0xAAAAAAAA))));
	return result;
}

cube cube::inverse()
{
	cube result(*this);
	for (int i = 0; i < result.size(); i++)
		result.values[i] = ((((result.values[i] << 1) & 0xAAAAAAAA) | ((result.values[i] >> 1) & 0x55555555)));
	return result;
}

cube cube::get_cover(int n) const
{
	if (size() == 0)
		return cube();

	unsigned int v1 = 1, v2 = 0;
	unsigned int mask = 1;

	int i = 0;
	for (; i < 5 && i < n; i++)
	{
		v2 = 0;

		if ((mask & values[0]) != 0)
			v2 |= v1;

		mask <<= 1;

		if ((mask & values[0]) != 0)
			v2 |= (v1 << (mask >> (i+1)));

		mask <<= 1;

		v1 = v2;
	}

	int s = 1 << max((n - 5), 0);

	cube c1;
	c1.extendN(s);
	c1.values[0] = v1;

	if (n <= 5)
		return c1;

	cube c2;
	c2.extendN(s);
	int mask_idx = 0;
	int total = 0;

	for (; i < n; i++)
	{
		int x = (mask >> i)/32;
		total += x;

		if (size() <= mask_idx || (mask & values[mask_idx]) != 0)
			for (int k = total; k != -1; k--)
				c2.values[k] |= c1.values[k];

		mask <<= 1;

		if (size() <= mask_idx || (mask & values[mask_idx]) != 0)
			for (int k = total; k != -1 && k >= x; k--)
				c2.values[k] |= c1.values[k-x];

		mask <<= 1;

		if (mask == 0)
		{
			mask_idx++;
			mask = 1;
		}

		c1 = c2;

		for (int k = 0; k <= total; k++)
			c2.values[k] = 0;
	}

	return c1;
}

cover cube::expand(vector<int> uids) const
{
	cover r1(*this);
	for (int i = 0; i < (int)uids.size(); i++)
	{
		for (int j = r1.size()-1; j >= 0 && j < r1.size(); j--)
		{
			if (r1[j].get(uids[i]) == 2)
			{
				r1.push_back(r1[j] & cube(uids[i], 0));
				r1[j] &= cube(uids[i], 1);
			}
		}
	}
	return r1;
}

vector<int> cube::vars() const
{
	vector<int> result;
	for (int i = 0; i < size()*16; i++)
		if (get(i) != 2)
			result.push_back(i);

	return result;
}

void cube::vars(vector<int> *result) const
{
	for (int i = 0; i < size()*16; i++)
		if (get(i) != 2)
			result->push_back(i);
}

cube cube::refactor(vector<pair<int, int> > uids) const
{
	cube result;
	for (int i = 0; i < (int)uids.size(); i++)
		result.set(uids[i].second, get(uids[i].first));
	return result;
}

void cube::intersect(const cube &s1)
{
	if (size() < s1.size())
		extendX(s1.size() - size());

	for (int i = 0; i < s1.size(); i++)
		values[i] &= s1.values[i];
}

void cube::intersect(const cube &s1, const cube &s2)
{
	int m12 = min(s1.size(), s2.size());
	int max_size = max(s1.size(), s2.size());

	if (size() < max_size)
		extendX(max_size - size());

	int i = 0;
	for (; i < m12; i++)
		values[i] &= s1.values[i] & s2.values[i];
	for (; i < s1.size(); i++)
		values[i] &= s1.values[i];
	for (; i < s2.size(); i++)
		values[i] &= s2.values[i];
}

void cube::intersect(const cube &s1, const cube &s2, const cube &s3)
{
	int m12 = min(s1.size(), s2.size());
	int m23 = min(s2.size(), s3.size());
	int m13 = min(s1.size(), s3.size());
	int m123 = min(m12, s3.size());
	int max_size = max(s1.size(), max(s2.size(), s3.size()));

	if (size() < max_size)
		extendX(max_size - size());

	int i = 0;
	for (; i < m123; i++)
		values[i] &= s1.values[i] & s2.values[i] & s3.values[i];
	for (; i < m12; i++)
		values[i] &= s1.values[i] & s2.values[i];
	for (; i < m23; i++)
		values[i] &= s2.values[i] & s3.values[i];
	for (; i < m13; i++)
		values[i] &= s1.values[i] & s3.values[i];
	for (; i < s1.size(); i++)
		values[i] &= s1.values[i];
	for (; i < s2.size(); i++)
		values[i] &= s2.values[i];
	for (; i < s3.size(); i++)
		values[i] &= s3.values[i];
}

void cube::intersect(const cover &s1)
{
	for (int i = 0; i < s1.size(); i++)
	{
		if (s1[i].size() > size())
			extendX(s1[i].size() - size());

		for (int j = 0; j < s1[i].size(); j++)
			values[j] &= s1[i].values[j];
	}
}

void cube::supercube(const cube &s1)
{
	if (size() > s1.size())
		trunk(s1.size());

	for (int i = 0; i < size(); i++)
		values[i] |= s1.values[i];
}

void cube::supercube(const cube &s1, const cube &s2)
{
	int min_size = min(s1.size(), s2.size());
	if (size() > min_size)
		trunk(min_size);

	for (int i = 0; i < size(); i++)
		values[i] |= s1.values[i] | s2.values[i];
}

void cube::supercube(const cube &s1, const cube &s2, const cube &s3)
{
	int min_size = min(s1.size(), min(s2.size(), s3.size()));
	if (size() > min_size)
		trunk(min_size);

	for (int i = 0; i < size(); i++)
		values[i] |= s1.values[i] | s2.values[i] | s3.values[i];
}

void cube::supercube(const cover &s1)
{
	for (int i = 0; i < s1.size(); i++)
	{
		if (s1[i].size() < size())
			trunk(s1[i].size());

		for (int j = 0; j < size(); j++)
			values[j] |= s1[i].values[j];
	}
}

void cube::hide(int uid)
{
	set(uid, 2);
}

void cube::hide(vector<int> uids)
{
	for (int i = 0; i < (int)uids.size(); i++)
		set(uids[i], 2);
}

void cube::cofactor(int uid, int val)
{
	int cmp = get(uid);
	if (cmp == 1-val)
		set(uid, -1);
	else
		set(uid, 2);
}

void cube::cofactor(const cube &s1)
{
	if (size() < s1.size())
		extendX(s1.size() - size());

	for (int i = 0; i < s1.size(); i++)
	{
		unsigned int a = (s1.values[i] ^ (s1.values[i] >> 1)) & 0x55555555;
		a = a | (a << 1);
		unsigned int b = s1.values[i] & values[i];
		b = (b | (b >> 1)) & 0x55555555;
		b = b | (b << 1);
		values[i] = (values[i] | a) & b;
	}
}

cube &cube::operator=(cube s)
{
	values = s.values;
	return *this;
}

cube &cube::operator=(int val)
{
	values.clear();
	if (val == 0)
		values.push_back(0x00000000);
	return *this;
}

cube &cube::operator&=(cube s)
{
	if (size() < s.size())
		extendX(s.size() - size());

	for (int i = 0; i < s.size(); i++)
		values[i] &= s.values[i];
	return *this;
}

cube &cube::operator&=(int val)
{
	if (val == 0)
		for (int i = 0; i < size(); i++)
			values[i] = 0x00000000;

	return *this;
}

cube &cube::operator|=(cube s)
{
	if (size() < s.size())
		extendX(s.size() - size());

	int i = 0;
	for (; i < s.size(); i++)
		values[i] |= s.values[i];
	for (; i < size(); i++)
		values[i] = 0xFFFFFFFF;
	return *this;
}

cube &cube::operator|=(int val)
{
	if (val == 1)
		for (int i = 0; i < size(); i++)
			values[i] = 0xFFFFFFFF;

	return *this;
}

cube &cube::operator>>=(cube s)
{
	if (size() < s.size())
		extendX(s.size() - size());

	for (int i = 0; i < s.size(); i++)
	{
		unsigned int v = s.values[i] & (s.values[i] >> 1) & 0x55555555;
		v = v | (v<<1);
		values[i] = ((values[i] & v) | (s.values[i] & ~v));
	}
	return *this;
}

ostream &operator<<(ostream &os, cube m)
{
	char c[4] = {'N', '0', '1', 'X'};
	os.put('[');
	for (int i = 0; i < m.size()*16; i++)
		os.put(c[m.get(i)+1]);
	os.put(']');
	return os;
}

cover operator~(cube s1)
{
	cover result;
	for (int i = 0; i < (int)s1.values.size(); i++)
		for (int j = 0; j < 16; j++)
		{
			unsigned int val = s1.values[i] & 3;
			if (val == 1 || val == 2)
				result.push_back(cube(i*16 + j, 2-val));
			s1.values[i] >>= 2;
		}

	return result;
}

cube operator&(cube s1, cube s2)
{
	if (s1.size() < s2.size())
		s1.extendX(s2.size() - s1.size());

	for (int i = 0; i < s2.size(); i++)
		s1.values[i] &= s2.values[i];
	return s1;
}

cube operator&(cube s1, int s2)
{
	if (s2 == 0)
		return cube(0);
	else
		return s1;
}

cube operator&(int s1, cube s2)
{
	if (s1 == 0)
		return cube(0);
	else
		return s2;
}

cube intersect(const cube &s1, const cube &s2)
{
	cube result;
	result.values.reserve(max(s1.size(), s2.size()));

	int m12 = min(s1.size(), s2.size());

	int i = 0;
	for (; i < m12; i++)
		result.values.push_back(s1.values[i] & s2.values[i]);
	for (; i < s1.size(); i++)
		result.values.push_back(s1.values[i]);
	for (; i < s2.size(); i++)
		result.values.push_back(s2.values[i]);

	return result;
}

cube intersect(const cube &s1, const cube &s2, const cube &s3)
{
	cube result;
	result.values.reserve(max(max(s1.size(), s2.size()), s3.size()));

	int m12 = min(s1.size(), s2.size());
	int m23 = min(s2.size(), s3.size());
	int m13 = min(s1.size(), s3.size());
	int m123 = min(m12, s3.size());

	int i = 0;
	for (; i < m123; i++)
		result.values.push_back(s1.values[i] & s2.values[i] & s3.values[i]);
	for (; i < m12; i++)
		result.values.push_back(s1.values[i] & s2.values[i]);
	for (; i < m23; i++)
		result.values.push_back(s2.values[i] & s3.values[i]);
	for (; i < m13; i++)
		result.values.push_back(s1.values[i] & s3.values[i]);
	for (; i < s1.size(); i++)
		result.values.push_back(s1.values[i]);
	for (; i < s2.size(); i++)
		result.values.push_back(s2.values[i]);
	for (; i < s3.size(); i++)
		result.values.push_back(s3.values[i]);

	return result;
}

bool are_mutex(const cube &s1, const cube &s2)
{
	int m12 = min(s1.size(), s2.size());
	for (int i = 0; i < m12; i++)
	{
		unsigned int v = s1.values[i] & s2.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}
	return false;
}

bool are_mutex(const cube &s1, const cube &s2, const cube &s3)
{
	int m12 = min(s1.size(), s2.size());
	int m23 = min(s2.size(), s3.size());
	int m13 = min(s1.size(), s3.size());
	int m123 = min(m12, s3.size());

	int i = 0;
	for (; i < m123; i++)
	{
		unsigned int v = s1.values[i] & s2.values[i] & s3.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}
	for (; i < m12; i++)
	{
		unsigned int v = s1.values[i] & s2.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}
	for (; i < m23; i++)
	{
		unsigned int v = s2.values[i] & s3.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}
	for (; i < m13; i++)
	{
		unsigned int v = s1.values[i] & s3.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}

	return false;
}

bool are_mutex(const cube &s1, const cube &s2, const cube &s3, const cube &s4)
{
	int m12 = min(s1.size(), s2.size());
	int m13 = min(s1.size(), s2.size());
	int m14 = min(s1.size(), s2.size());
	int m23 = min(s1.size(), s2.size());
	int m24 = min(s1.size(), s2.size());
	int m34 = min(s3.size(), s4.size());
	int m123 = min(m12, s3.size());
	int m124 = min(m12, s4.size());
	int m134 = min(s1.size(), m34);
	int m234 = min(s2.size(), m34);
	int m1234 = min(m12, m34);

	int i = 0;
	for (; i < m1234; i++)
	{
		unsigned int v = s1.values[i] & s2.values[i] & s3.values[i] & s4.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}
	for (; i < m123; i++)
	{
		unsigned int v = s1.values[i] & s2.values[i] & s3.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}
	for (; i < m124; i++)
	{
		unsigned int v = s1.values[i] & s2.values[i] & s4.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}
	for (; i < m134; i++)
	{
		unsigned int v = s1.values[i] & s3.values[i] & s4.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}
	for (; i < m234; i++)
	{
		unsigned int v = s2.values[i] & s3.values[i] & s4.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}
	for (; i < m12; i++)
	{
		unsigned int v = s1.values[i] & s2.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}
	for (; i < m13; i++)
	{
		unsigned int v = s1.values[i] & s3.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}
	for (; i < m14; i++)
	{
		unsigned int v = s1.values[i] & s4.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}
	for (; i < m23; i++)
	{
		unsigned int v = s2.values[i] & s3.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}
	for (; i < m24; i++)
	{
		unsigned int v = s2.values[i] & s4.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}
	for (; i < m34; i++)
	{
		unsigned int v = s3.values[i] & s4.values[i];
		if ((((v>>1) | v | 0xAAAAAAAA) != 0xFFFFFFFF))
			return true;
	}

	return false;
}

bool are_mutex(const cube &s1, const cover &s2)
{
	for (int i = 0; i < s2.size(); i++)
		if (!are_mutex(s1, s2[i]))
			return false;

	return true;
}

cover operator|(cube s1, cube s2)
{
	cover result;
	result.reserve(2);
	result.push_back(s1);
	result.push_back(s2);
	return result;
}

cover operator|(cube s1, int s2)
{
	if (s2 == 0)
		return cover(s1);
	else
		return cover(1);
}

cover operator|(int s1, cube s2)
{
	if (s1 == 0)
		return cover(s2);
	else
		return cover(1);
}

cube supercube(const cube &s1, const cube &s2)
{
	cube result;
	result.extendN(min(s1.size(), s2.size()));

	for (int i = 0; i < result.size(); i++)
	{
		if (((s1.values[i]>>1) | s1.values[i] | 0xAAAAAAAA) != 0xFFFFFFFF)
			return s2;
		else if (((s2.values[i]>>1) | s2.values[i] | 0xAAAAAAAA) != 0xFFFFFFFF)
			return s1;
		else
			result.values[i] = s1.values[i] | s2.values[i];
	}

	return result;
}

cube supercube(const cube &s1, const cube &s2, const cube &s3)
{
	if (s1.is_null())
		return supercube(s2, s3);

	if (s2.is_null())
		return supercube(s1, s3);

	if (s3.is_null())
		return supercube(s1, s2);

	cube result;
	result.extendN(min(min(s1.size(), s2.size()), s3.size()));

	for (int i = 0; i < result.size(); i++)
		result.values[i] = s1.values[i] | s2.values[i] | s3.values[i];

	return result;
}

cube supercube(const cube &s1, const cube &s2, const cube &s3, const cube &s4)
{
	if (s1.is_null())
		return supercube(s2, s3, s4);

	if (s2.is_null())
		return supercube(s1, s3, s4);

	if (s3.is_null())
		return supercube(s1, s2, s4);

	if (s4.is_null())
		return supercube(s1, s2, s3);

	cube result;
	result.extendN(min(min(s1.size(), s2.size()), min(s3.size(), s4.size())));

	for (int i = 0; i < result.size(); i++)
		result.values[i] = s1.values[i] | s2.values[i] | s3.values[i] | s4.values[i];

	return result;
}

cube basic_consensus(cube s1, cube s2)
{
	if (s1.size() < s2.size())
		s1.extendX(s2.size() - s1.size());

	for (int i = 0; i < s2.size(); i++)
	{
		// a will have a 1 where s1i & s2i != 0
		// apply before set operator of s1i & s2i
		s1.values[i] &= s2.values[i];
		unsigned int a = ~(s1.values[i] | (s1.values[i] >> 1)) & 0x55555555;
		// apply active set operator of s1i | s2i
		s1.values[i] |= (a | (a << 1));
	}

	return s1;
}

cube consensus(cube s1, cube s2)
{
	if (s1.size() < s2.size())
		s1.extendX(s2.size() - s1.size());

	int count = 0;
	for (int i = 0; i < s2.size(); i++)
	{
		// a will have a 1 where s1i & s2i != 0
		// apply before set operator of s1i & s2i
		s1.values[i] &= s2.values[i];
		unsigned int a = ~(s1.values[i] | (s1.values[i] >> 1)) & 0x55555555;
		// count the number of bits set to 1 (derived from Hacker's Delight)
		unsigned int b = (a & 0x33333333) + ((a >> 2) & 0x33333333);
		b = (b & 0x0F0F0F0F) + ((b >> 4) & 0x0F0F0F0F);
		b += (b >> 8);
		b += (b >> 16);
		count += b & 0x0000003F;
		// apply active set operator of s1i | s2i
		s1.values[i] |= (a | (a << 1));
	}

	if (count > 1)
		return cube(0);
	else
		return s1;
}

cube prime(cube s1, cube s2)
{
	if (s1.size() < s2.size())
		s1.extendX(s2.size() - s1.size());

	for (int i = 0; i < s2.size(); i++)
	{
		// b will have a 1 where s1i & s2i != 0
		unsigned int a = ~(s1.values[i] & s2.values[i]);
		unsigned int b = a & (a >> 1) & 0x55555555;
		// apply active set operator of s1i & s2i
		// apply before set operator of s1i
		s1.values[i] |= (b | (b << 1));
	}

	return s1;
}

cover basic_sharp(cube s1, cube s2)
{
	cover result;

	if (s1.size() < s2.size())
		s1.extendX(s2.size() - s1.size());

	for (int i = 0; i < s2.size(); i++)
	{
		// b will have a 1 where s1i is not a subset of s2i
		unsigned int a = (s1.values[i] & s2.values[i]) ^ s1.values[i];
		unsigned int b = (a | (a >> 1)) & 0x55555555;

		// We need to find each of those ones
		for (int j = 0; b != 0; j+=2)
		{
			// If we found one
			if (b & 1)
			{
				// apply the before rule s1i
				result.push_back(s1);
				// apply the active rule s1i & ~s2i
				result.back().values[i] &= (~(3 << j) | (s1.values[i] & ~s2.values[i]));
			}
			b >>= 2;
		}
	}

	return result;
}

cover sharp(cube s1, cube s2)
{
	cover result;

	if (s1.size() < s2.size())
		s1.extendX(s2.size() - s1.size());

	for (int i = 0; i < s2.size(); i++)
	{
		// b will have a 1 where s1i is not a subset of s2i
		unsigned int a = (s1.values[i] & s2.values[i]) ^ s1.values[i];
		unsigned int b = (a | (a >> 1)) & 0x55555555;

		// We need to find each of those 1's
		for (int j = 0; b != 0; j+=2)
		{
			// If we found one
			if (b & 1)
			{
				// apply the before rule s1i
				result.push_back(s1);
				// apply the active rule s1i & ~s2i
				result.back().values[i] &= (~(3 << j) | (s1.values[i] & ~s2.values[i]));
			}
			b >>= 2;
		}
	}

	return result;
}

cover basic_disjoint_sharp(cube s1, cube s2)
{
	cover result;

	if (s1.size() < s2.size())
		s1.extendX(s2.size() - s1.size());

	for (int i = 0; i < s2.size(); i++)
	{
		// b will have a 1 where s1i is not a subset of s2i
		unsigned int a = (s1.values[i] & s2.values[i]) ^ s1.values[i];
		unsigned int b = (a | (a >> 1)) & 0x55555555;

		// We need to find each of those 1's
		for (int j = 0; b != 0; j+=2)
		{
			// If we found one
			if (b & 1)
			{
				// apply the before rule s1i
				result.push_back(s1);
				// apply the active rule s1i & ~s2i
				// apply the after rule s1i & s2i
				result.back().values[i] &= (~(3 << j) | (s1.values[i] & ~s2.values[i])) & ((0xFFFFFFFF << j) | s2.values[i]);
				for (int k = i+1; k < s2.size(); k++)
					result.back().values[k] &= s2.values[k];
			}
			b >>= 2;
		}
	}

	return result;
}

cover disjoint_sharp(cube s1, cube s2)
{
	cover result;

	if (s1.size() < s2.size())
		s1.extendX(s2.size() - s1.size());

	for (int i = 0; i < s2.size(); i++)
	{
		// b will have a 1 where s1i is not a subset of s2i
		unsigned int a = (s1.values[i] & s2.values[i]) ^ s1.values[i];
		unsigned int b = (a | (a >> 1)) & 0x55555555;

		// We need to find each of those 1's
		for (int j = 0; b != 0; j+=2)
		{
			// If we found one
			if (b & 1)
			{
				// apply the before rule s1i
				result.push_back(s1);
				// apply the active rule s1i & ~s2i
				// apply the after rule s1i & s2i
				result.back().values[i] &= (~(3 << j) | (s1.values[i] & ~s2.values[i])) & ((0xFFFFFFFF << j) | s2.values[i]);
				for (int k = i+1; k < s2.size(); k++)
					result.back().values[k] &= s2.values[k];
			}
			b >>= 2;
		}
	}

	return result;
}

cover crosslink(cube s1, cube s2)
{
	cover result;

	if (s1.size() < s2.size())
		s1.extendX(s2.size() - s1.size());

	for (int i = 0; i < s2.size(); i++)
	{
		// b will have a 1 where s1i & s2i = null
		unsigned int a = ~(s1.values[i] & s2.values[i]);
		unsigned int b = a & (a >> 1) & 0x55555555;

		// We need to find each of those 1's
		for (int j = 0; b != 0; j+=2)
		{
			// If we found one
			if (b & 1)
			{
				// apply the before rule s1i
				result.push_back(s1);
				// apply the active rule s1i | s2i
				// apply the after rule s2i
				result.back().values[i] = (s1.values[i] & (0xFFFFFFFC << j)) | ((3 << j) & (s1.values[i] | s2.values[i])) | (~(0xFFFFFFFF << j) & s2.values[i]);
				for (int k = i+1; k < s2.size(); k++)
					result.back().values[k] = s2.values[k];
			}
			b >>= 2;
		}
	}

	return result;
}

cube cofactor(cube s1, int uid, int val)
{
	int cmp = s1.get(uid);
	if (cmp == 1-val)
		s1.set(uid, -1);
	else
		s1.set(uid, 2);

	return s1;
}

cube cofactor(cube s1, cube s2)
{
	if (s1.size() < s2.size())
		s1.extendX(s2.size() - s1.size());

	for (int i = 0; i < s2.size(); i++)
	{
		unsigned int a = (s2.values[i] ^ (s2.values[i] >> 1)) & 0x55555555;
		a = a | (a << 1);
		unsigned int b = s2.values[i] & s1.values[i];
		b = (b | (b >> 1)) & 0x55555555;
		b = b | (b << 1);
		s1.values[i] = (s1.values[i] | a) & b;
	}

	return s1;
}

int distance(const cube &s0, const cube &s1)
{
	int size = min(s0.size(), s1.size());
	int count = 0;
	for (int i = 0; i < size; i++)
	{
		// XOR to see what bits are different
		unsigned int a = s0.values[i] & s1.values[i];
		// OR together any differences in the bit pairs (a single value)
		a = (~(a | (a >> 1))) & 0x55555555;

		// count the number of bits set to 1 (derived from Hacker's Delight)
		a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
		a = (a & 0x0F0F0F0F) + ((a >> 4) & 0x0F0F0F0F);
		a += (a >> 8);
		a += (a >> 16);
		count += a & 0x0000003F;
	}

	return count;
}

int similarity(const cube &s0, const cube &s1)
{
	int size = min(s0.size(), s1.size());
	int count = 0;
	for (int i = 0; i < size; i++)
	{
		unsigned int a = (s0.values[i] ^ (s0.values[i] >> 1)) & (s1.values[i] ^ (s1.values[i] >> 1)) & 0x55555555;
		a = (a | (a << 1)) & s0.values[i] & s1.values[i];

		// count the number of bits set to 1 (derived from Hacker's Delight)
		a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
		a = (a & 0x0F0F0F0F) + ((a >> 4) & 0x0F0F0F0F);
		a += (a >> 8);
		a += (a >> 16);
		count += a & 0x0000003F;
	}

	return count;
}

bool similarity_g0(const cube &s0, const cube &s1)
{
	int size = min(s0.size(), s1.size());
	for (int i = 0; i < size; i++)
	{
		unsigned int a = (s0.values[i] ^ (s0.values[i] >> 1)) & (s1.values[i] ^ (s1.values[i] >> 1)) & 0x55555555;
		a = (a | (a << 1)) & s0.values[i] & s1.values[i];

		// count the number of bits set to 1 (derived from Hacker's Delight)
		a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
		a = (a & 0x0F0F0F0F) + ((a >> 4) & 0x0F0F0F0F);
		a += (a >> 8);
		a += (a >> 16);
		if ((a & 0x0000003F) > 0)
			return true;
	}
	return false;
}

/*
 *
 */
void merge_distances(const cube &s0, const cube &s1, int *vn, int *xv, int *vx)
{
	int m0 = min(s0.size(), s1.size());
	int vn_temp = 0;
	int xv_temp = 0;
	int vx_temp = 0;
	for (int i = 0; i < m0; i++)
	{
		// XOR to see what bits are different
		unsigned int a = s0.values[i] ^ s1.values[i];
		unsigned int b = s0.values[i] & 0x55555555 & (s0.values[i] >> 1);
		unsigned int c = s1.values[i] & 0x55555555 & (s1.values[i] >> 1);
		unsigned int d =  b & ~c;
		unsigned int e = ~b &  c;
		// OR together any differences in the bit pairs (a single value)
		a = (a & (a >> 1)) & 0x55555555;

		// count the number of bits set to 1 (derived from Hacker's Delight)
		a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
		a = (a & 0x0F0F0F0F) + ((a >> 4) & 0x0F0F0F0F);
		a = a + (a >> 8);
		a = a + (a >> 16);
		vn_temp += a & 0x0000003F;

		d = (d & 0x33333333) + ((d >> 2) & 0x33333333);
		d = (d & 0x0F0F0F0F) + ((d >> 4) & 0x0F0F0F0F);
		d = d + (d >> 8);
		d = d + (d >> 16);
		xv_temp += d & 0x0000003F;

		e = (e & 0x33333333) + ((e >> 2) & 0x33333333);
		e = (e & 0x0F0F0F0F) + ((e >> 4) & 0x0F0F0F0F);
		e = e + (e >> 8);
		e = e + (e >> 16);
		vx_temp += e & 0x0000003F;
	}
	for (int i = m0; i < s0.size(); i++)
	{
		// XOR to see what bits are different
		unsigned int a = ~s0.values[i];
		unsigned int b = s0.values[i] & 0x55555555 & (s0.values[i] >> 1);
		unsigned int c = 0x55555555;
		unsigned int d =  b & ~c;
		unsigned int e = ~b &  c;
		// OR together any differences in the bit pairs (a single value)
		a = (a & (a >> 1)) & 0x55555555;

		// count the number of bits set to 1 (derived from Hacker's Delight)
		a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
		a = (a & 0x0F0F0F0F) + ((a >> 4) & 0x0F0F0F0F);
		a = a + (a >> 8);
		a = a + (a >> 16);
		vn_temp += a & 0x0000003F;

		d = (d & 0x33333333) + ((d >> 2) & 0x33333333);
		d = (d & 0x0F0F0F0F) + ((d >> 4) & 0x0F0F0F0F);
		d = d + (d >> 8);
		d = d + (d >> 16);
		xv_temp += d & 0x0000003F;

		e = (e & 0x33333333) + ((e >> 2) & 0x33333333);
		e = (e & 0x0F0F0F0F) + ((e >> 4) & 0x0F0F0F0F);
		e = e + (e >> 8);
		e = e + (e >> 16);
		vx_temp += e & 0x0000003F;
	}
	for (int i = m0; i < s1.size(); i++)
	{
		// XOR to see what bits are different
		unsigned int a = ~s1.values[i];
		unsigned int b = 0x55555555;
		unsigned int c = s1.values[i] & 0x55555555 & (s1.values[i] >> 1);
		unsigned int d =  b & ~c;
		unsigned int e = ~b &  c;
		// OR together any differences in the bit pairs (a single value)
		a = (a & (a >> 1)) & 0x55555555;

		// count the number of bits set to 1 (derived from Hacker's Delight)
		a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
		a = (a & 0x0F0F0F0F) + ((a >> 4) & 0x0F0F0F0F);
		a = a + (a >> 8);
		a = a + (a >> 16);
		vn_temp += a & 0x0000003F;

		d = (d & 0x33333333) + ((d >> 2) & 0x33333333);
		d = (d & 0x0F0F0F0F) + ((d >> 4) & 0x0F0F0F0F);
		d = d + (d >> 8);
		d = d + (d >> 16);
		xv_temp += d & 0x0000003F;

		e = (e & 0x33333333) + ((e >> 2) & 0x33333333);
		e = (e & 0x0F0F0F0F) + ((e >> 4) & 0x0F0F0F0F);
		e = e + (e >> 8);
		e = e + (e >> 16);
		vx_temp += e & 0x0000003F;
	}

	if (vn != NULL)
		*vn = vn_temp;
	if (xv != NULL)
		*xv = xv_temp;
	if (vx != NULL)
		*vx = vx_temp;
}

bool mergible(const cube &s0, const cube &s1)
{
	/* check for the following redundancy rules
	 * a&b | a&~b = a
	 * a | a&b = a
	 */

	int vn = 0, xv = 0, vx = 0;
	merge_distances(s0, s1, &vn, &xv, &vx);

	return (vn + (xv > 0) + (vx > 0) <= 1);
}

cube supercube_of_complement(const cube &s)
{
	cube result;
	result.values.reserve(s.size());
	int count = 0;
	for (int i = 0; i < s.size(); i++)
	{
		// OR together any differences in the bit pairs (a single value)
		unsigned int a = (s.values[i] ^ (s.values[i] >> 1)) & 0x55555555;

		// count the number of bits set to 1 (derived from Hacker's Delight)
		a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
		a = (a & 0x0F0F0F0F) + ((a >> 4) & 0x0F0F0F0F);
		a += (a >> 8);
		a += (a >> 16);
		count += a & 0x0000003F;

		if (count > 1)
			return cube();

		result.values.push_back(((s.values[i] & 0xAAAAAAAA) >> 1) | ((s.values[i] & 0x55555555) << 1));
	}

	if (count == 1)
		return result;
	else
		return cube(0);
}

cube local_transition(const cube &s1, const cube &s2)
{
	cube result;
	result.values.reserve(max(s1.size(), s2.size()));

	int m0 = min(s1.size(), s2.size());
	int i = 0;
	for (; i < m0; i++)
	{
		unsigned int v = s2.values[i] & (s2.values[i] >> 1) & 0x55555555;
		v = v | (v<<1);
		result.values.push_back((s1.values[i] & v) | (s2.values[i] & ~v));
	}
	for (; i < s1.size(); i++)
		result.values.push_back(s1.values[i]);
	for (; i < s2.size(); i++)
		result.values.push_back(s2.values[i]);

	return result;
}

cube remote_transition(const cube &s1, const cube &s2)
{
	cube result;
	result.values.reserve(max(s1.size(), s2.size()));

	int m0 = min(s1.size(), s2.size());
	int i = 0;
	for (; i < m0; i++)
	{
		unsigned int v = s2.values[i] & (s2.values[i] >> 1) & 0x55555555;
		v = v | (v<<1);
		result.values.push_back(s1.values[i] | (s2.values[i] & ~v));
	}
	for (; i < s1.size(); i++)
		result.values.push_back(s1.values[i]);
	for (; i < s2.size(); i++)
		result.values.push_back(s2.values[i]);

	return result;
}


bool operator==(cube s1, cube s2)
{
	int i = 0;
	int size = min(s1.size(), s2.size());
	for (; i < size; i++)
		if (s1.values[i] != s2.values[i])
			return false;
	for (; i < s1.size(); i++)
		if (s1.values[i] != 0xFFFFFFFF)
			return false;
	for (; i < s2.size(); i++)
		if (0xFFFFFFFF != s2.values[i])
			return false;

	return true;
}

bool operator==(cube s1, int s2)
{
	if (s2 == 0)
	{
		for (int i = 0; i < s1.size(); i++)
			if (((s1.values[i]>>1) | s1.values[i] | 0xAAAAAAAA) != 0xFFFFFFFF)
				return true;

		return false;
	}
	else
	{
		for (int i = 0; i < s1.size(); i++)
			if (s1.values[i] != 0xFFFFFFFF)
				return false;

		return true;
	}
}

bool operator==(int s1, cube s2)
{
	if (s1 == 0)
	{
		for (int i = 0; i < s2.size(); i++)
			if (((s2.values[i]>>1) | s2.values[i] | 0xAAAAAAAA) != 0xFFFFFFFF)
				return true;

		return false;
	}
	else
	{
		for (int i = 0; i < s2.size(); i++)
			if (s2.values[i] != 0xFFFFFFFF)
				return false;

		return true;
	}
}

bool operator!=(cube s1, cube s2)
{
	int i = 0;
	int size = min(s1.size(), s2.size());
	for (; i < size; i++)
		if (s1.values[i] != s2.values[i])
			return true;
	for (; i < s1.size(); i++)
		if (s1.values[i] != 0xFFFFFFFF)
			return true;
	for (; i < s2.size(); i++)
		if (0xFFFFFFFF != s2.values[i])
			return true;

	return false;
}

bool operator!=(cube s1, int s2)
{
	if (s2 == 0)
	{
		for (int i = 0; i < s1.size(); i++)
			if (((s1.values[i]>>1) | s1.values[i] | 0xAAAAAAAA) != 0xFFFFFFFF)
				return false;

		return true;
	}
	else
	{
		for (int i = 0; i < s1.size(); i++)
			if (s1.values[i] != 0xFFFFFFFF)
				return true;

		return false;
	}
}

bool operator!=(int s1, cube s2)
{
	if (s1 == 0)
	{
		for (int i = 0; i < s2.size(); i++)
			if (((s2.values[i]>>1) | s2.values[i] | 0xAAAAAAAA) != 0xFFFFFFFF)
				return false;

		return true;
	}
	else
	{
		for (int i = 0; i < s2.size(); i++)
			if (s2.values[i] != 0xFFFFFFFF)
				return true;

		return false;
	}
}

bool operator<(cube s1, cube s2)
{
	int m0 = min(s1.size(), s2.size());
	int i, count0 = 0, count1 = 0;
	for (i = 0; i < m0; i++)
	{
		count0 += count_ones(s1.values[i]);
		count1 += count_ones(s2.values[i]);
	}
	for (; i < s1.size(); i++)
	{
		count0 += count_ones(s1.values[i]);
		count1 += 32;
	}
	for (; i < s2.size(); i++)
	{
		count0 += 32;
		count1 += count_ones(s2.values[i]);
	}

	if (count0 > count1)
		return true;
	else if (count1 > count0)
		return false;

	i--;

	for (; i >= s1.size(); i--)
		if (0xFFFFFFFF != s2.values[i])
			return false;
	for (; i >= s2.size(); i--)
		if (s1.values[i] != 0xFFFFFFFF)
			return true;
	for (; i >= 0; i--)
		if (s1.values[i] != s2.values[i])
			return (s1.values[i] < s2.values[i]);

	return false;
}

bool operator>(cube s1, cube s2)
{
	int m0 = min(s1.size(), s2.size());
	int i, count0 = 0, count1 = 0;
	for (i = 0; i < m0; i++)
	{
		count0 += count_ones(s1.values[i]);
		count1 += count_ones(s2.values[i]);
	}
	for (; i < s1.size(); i++)
	{
		count0 += count_ones(s1.values[i]);
		count1 += 32;
	}
	for (; i < s2.size(); i++)
	{
		count0 += 32;
		count1 += count_ones(s2.values[i]);
	}

	if (count0 > count1)
		return false;
	else if (count1 > count0)
		return true;

	for (; i >= s1.size(); i--)
		if (0xFFFFFFFF != s2.values[i])
			return true;
	for (; i >= s2.size(); i--)
		if (s1.values[i] != 0xFFFFFFFF)
			return false;
	for (; i >= 0; i--)
		if (s1.values[i] != s2.values[i])
			return (s1.values[i] > s2.values[i]);

	return false;
}

bool operator<=(cube s1, cube s2)
{
	int m0 = min(s1.size(), s2.size());
	int i, count0 = 0, count1 = 0;
	for (i = 0; i < m0; i++)
	{
		count0 += count_ones(s1.values[i]);
		count1 += count_ones(s2.values[i]);
	}
	for (; i < s1.size(); i++)
	{
		count0 += count_ones(s1.values[i]);
		count1 += 32;
	}
	for (; i < s2.size(); i++)
	{
		count0 += 32;
		count1 += count_ones(s2.values[i]);
	}

	if (count0 > count1)
		return true;
	else if (count1 > count0)
		return false;

	for (; i >= s1.size(); i--)
		if (0xFFFFFFFF != s2.values[i])
			return false;
	for (; i >= s2.size(); i--)
		if (s1.values[i] != 0xFFFFFFFF)
			return true;
	for (; i >= 0; i--)
		if (s1.values[i] != s2.values[i])
			return (s1.values[i] < s2.values[i]);

	return true;
}

bool operator>=(cube s1, cube s2)
{
	int m0 = min(s1.size(), s2.size());
	int i, count0 = 0, count1 = 0;
	for (i = 0; i < m0; i++)
	{
		count0 += count_ones(s1.values[i]);
		count1 += count_ones(s2.values[i]);
	}
	for (; i < s1.size(); i++)
	{
		count0 += count_ones(s1.values[i]);
		count1 += 32;
	}
	for (; i < s2.size(); i++)
	{
		count0 += 32;
		count1 += count_ones(s2.values[i]);
	}

	if (count0 > count1)
		return false;
	else if (count1 > count0)
		return true;

	for (; i >= s1.size(); i--)
		if (0xFFFFFFFF != s2.values[i])
			return true;
	for (; i >= s2.size(); i--)
		if (s1.values[i] != 0xFFFFFFFF)
			return false;
	for (; i >= 0; i--)
		if (s1.values[i] != s2.values[i])
			return (s1.values[i] > s2.values[i]);

	return true;
}

/*bool are_mutex(cube s1, maxterm s2)
{
	int size = min(s1.size(), s2.size());
	for (int i = 0; i < size; i++)
		if (s1.values[i] & s2.values[i] > 0)
			return false;
	for (int i = 0; i < s2.size(); i++)
		if (s2.values[i] > 0)
			return false;
	return true;
}

bool are_mutex(maxterm s1, cube s2)
{
	int size = min(s1.size(), s2.size());
	for (int i = 0; i < size; i++)
		if (s1.values[i] & s2.values[i] > 0)
			return false;
	for (int i = 0; i < s1.size(); i++)
		if (s1.values[i] > 0)
			return false;
	return true;
}

bool are_mutex(maxterm s1, cover s2)
{
	for (int i = 0; i < s2.size(); i++)
	{
		int size = min(s1.size(), s2[i].size());
		for (int j = 0; j < size; j++)
			if (s1.values[j] & s2[i].values[j] > 0)
				return false;
		for (int j = 0; j < s1.size(); j++)
			if (s1.values[j] > 0)
				return false;
	}
	return true;
}

bool are_mutex(cover s1, maxterm s2)
{
	for (int i = 0; i < s1.size(); i++)
	{
		int size = min(s1[i].size(), s2.size());
		for (int j = 0; j < size; j++)
			if (s1[i].values[j] & s2.values[j] > 0)
				return false;
		for (int j = 0; j < s2.size(); j++)
			if (s2.values[j] > 0)
				return false;
	}
	return true;
}
*/

}
