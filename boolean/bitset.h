/*
 * bitset.h
 *
 *  Created on: Dec 18, 2020
 *      Author: nbingham
 */

#pragma once

#include <boolean/cover.h>

namespace boolean
{

struct bitset
{
	bitset();
	bitset(int width, int offset);
	bitset(const cover &c);
	bitset(const bitset &n);
	~bitset();

	vector<cover> bits;

	bitset &espresso();
	bitset &resize(size_t size);
	bitset &append(const bitset &b);
	bitset subset(size_t start, size_t length=-1);
	virtual cover extend() const;
	cube supercube() const;
	bool is_constant() const;
	int max_width() const;
	int depth() const;
	
	void cofactor(const cube &s1);
	float partition(bitset &left, bitset &right) const;
	bitset decompose_hfactor(map<boolean::cube, int> &factors, int width = 2, int offset = 0, vector<int> hide = vector<int>()) const;
	bitset decompose_xfactor(map<boolean::cube, int> &factors, int width = 2, int offset = 0, vector<int> hide = vector<int>()) const;

	bitset &operator=(const bitset &n);

	bitset &operator|=(const bitset &n);
	bitset &operator&=(const bitset &n);
	bitset &operator^=(const bitset &n);
	bitset &operator<<=(int s);
	bitset &operator>>=(int s);
};

bitset cofactor(const bitset &s0, const cube &s1);

bitset operator&(const bitset &n0, const bitset &n1);
bitset operator|(const bitset &n0, const bitset &n1);
bitset operator^(const bitset &n0, const bitset &n1);
bitset operator~(const bitset &n);

bitset operator&(const bitset &n, const cover &c);
bitset operator|(const bitset &n, const cover &c);
bitset operator^(const bitset &n, const cover &c);
bitset operator&(const bitset &n, const cube &c);
bitset operator|(const bitset &n, const cube &c);
bitset operator^(const bitset &n, const cube &c);

bitset operator&(const cover &c, const bitset &n);
bitset operator|(const cover &c, const bitset &n);
bitset operator^(const cover &c, const bitset &n);
bitset operator&(const cube &c, const bitset &n);
bitset operator|(const cube &c, const bitset &n);
bitset operator^(const cube &c, const bitset &n);

bitset operator<<(const bitset &n, int s);
bitset operator>>(const bitset &n, int s);

cover operator==(const bitset &n0, const bitset &n1);
cover operator!=(const bitset &n0, const bitset &n1);

}
