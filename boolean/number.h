/*
 * number.h
 *
 *  Created on: Feb 6, 2015
 *      Author: nbingham
 */

#pragma once

#include <boolean/cover.h>

namespace boolean
{
struct unsigned_int;
struct signed_int;

struct unsigned_int
{
	unsigned_int();
	unsigned_int(int width, int offset);
	unsigned_int(unsigned int value);
	unsigned_int(const cover &c);
	unsigned_int(const unsigned_int &n);
	unsigned_int(const signed_int &n);
	~unsigned_int();

	vector<cover> bits;

	unsigned_int &simplify();
	unsigned_int &ext(int count);

	unsigned_int &operator=(unsigned_int n);

	unsigned_int &operator|=(unsigned_int n);
	unsigned_int &operator&=(unsigned_int n);
	unsigned_int &operator^=(unsigned_int n);
	unsigned_int &operator+=(unsigned_int n);
	unsigned_int &operator-=(unsigned_int n);
	unsigned_int &operator*=(unsigned_int n);
	unsigned_int &operator/=(unsigned_int n);
	//unsigned_int &operator%=(unsigned_int n);
	unsigned_int &operator<<=(int s);
	unsigned_int &operator>>=(int s);

	unsigned_int &operator=(signed_int n);

	float partition(unsigned_int &left, unsigned_int &right);
	cube supercube() const;
	void cofactor(const cube &s1);
	bool is_constant();
	int max_width();
	int depth();
};

struct signed_int
{
	signed_int();
	signed_int(int width, int offset);
	signed_int(int value);
	signed_int(const unsigned_int &n);
	signed_int(const signed_int &n);
	~signed_int();

	vector<cover> bits;
	cover sign;

	signed_int &simplify();
	signed_int &ext(int s);

	signed_int &operator=(unsigned_int n);
	signed_int &operator=(signed_int n);
};

unsigned_int operator&(unsigned_int n0, unsigned_int n1);
unsigned_int operator|(unsigned_int n0, unsigned_int n1);
unsigned_int operator^(unsigned_int n0, unsigned_int n1);
unsigned_int operator~(unsigned_int n);

unsigned_int operator&(unsigned_int n, cover c);
unsigned_int operator|(unsigned_int n, cover c);

unsigned_int operator&(cover c, unsigned_int n);
unsigned_int operator|(cover c, unsigned_int n);

unsigned_int operator<<(unsigned_int n, int s);
unsigned_int operator>>(unsigned_int n, int s);

unsigned_int operator-(unsigned_int n);
unsigned_int operator+(unsigned_int n0, unsigned_int n1);
unsigned_int operator-(unsigned_int n0, unsigned_int n1);
unsigned_int operator*(unsigned_int n0, unsigned_int n1);
unsigned_int operator/(unsigned_int n0, unsigned_int n1);

cover operator==(unsigned_int n0, unsigned_int n1);
cover operator!=(unsigned_int n0, unsigned_int n1);
cover operator<(unsigned_int n0, unsigned_int n1);
cover operator>(unsigned_int n0, unsigned_int n1);
cover operator<=(unsigned_int n0, unsigned_int n1);
cover operator>=(unsigned_int n0, unsigned_int n1);

signed_int operator&(signed_int n0, signed_int n1);
signed_int operator|(signed_int n0, signed_int n1);
signed_int operator~(signed_int n);

signed_int operator&(signed_int n, cover c);
signed_int operator|(signed_int n, cover c);

signed_int operator&(cover c, signed_int n);
signed_int operator|(cover c, signed_int n);

signed_int operator<<(signed_int n, int s);
signed_int operator>>(signed_int n, int s);

signed_int operator-(signed_int n);
signed_int operator+(signed_int n0, signed_int n1);
signed_int operator-(signed_int n0, signed_int n1);
signed_int operator*(signed_int n0, signed_int n1);
signed_int operator/(signed_int n0, signed_int n1);

cover operator<(signed_int n0, signed_int n1);

}

