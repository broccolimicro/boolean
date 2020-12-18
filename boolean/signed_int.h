/*
 * signed_int.h
 *
 *  Created on: Feb 6, 2015
 *      Author: nbingham
 */

#pragma once

#include <boolean/bitset.h>

namespace boolean
{

struct signed_int : bitset
{
	signed_int();
	signed_int(int width, int offset);
	signed_int(long value);
	signed_int(const bitset &n);
	signed_int(const signed_int &n);
	~signed_int();

	cover extend() const;

	signed_int &operator+=(const signed_int &n);
	signed_int &operator-=(const signed_int &n);
	signed_int &operator*=(const signed_int &n);
	signed_int &operator/=(const signed_int &n);
	//signed_int &operator%=(const signed_int &n);
};

signed_int operator-(const signed_int &n);
signed_int operator+(const signed_int &n0, const signed_int &n1);
signed_int operator-(const signed_int &n0, const signed_int &n1);
signed_int operator*(const signed_int &n0, const signed_int &n1);
signed_int operator/(const signed_int &n0, const signed_int &n1);

cover operator<(const signed_int &n0, const signed_int &n1);
cover operator>(const signed_int &n0, const signed_int &n1);
cover operator<=(const signed_int &n0, const signed_int &n1);
cover operator>=(const signed_int &n0, const signed_int &n1);

}

