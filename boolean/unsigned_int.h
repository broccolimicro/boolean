/*
 * unsigned_int.h
 *
 *  Created on: Feb 6, 2015
 *      Author: nbingham
 */

#pragma once

#include <boolean/bitset.h>

namespace boolean
{

struct unsigned_int : bitset
{
	unsigned_int();
	unsigned_int(int width, int offset);
	unsigned_int(unsigned long value);
	unsigned_int(const cover &c);
	unsigned_int(const bitset &n);
	unsigned_int(const unsigned_int &n);
	~unsigned_int();

	unsigned_int &operator+=(const unsigned_int &n);
	unsigned_int &operator-=(const unsigned_int &n);
	unsigned_int &operator*=(const unsigned_int &n);
	unsigned_int &operator/=(const unsigned_int &n);
	//unsigned_int &operator%=(const unsigned_int &n);
};

unsigned_int operator+(const unsigned_int &n0, const unsigned_int &n1);
unsigned_int operator-(const unsigned_int &n0, const unsigned_int &n1);
unsigned_int operator*(const unsigned_int &n0, const unsigned_int &n1);
unsigned_int operator/(const unsigned_int &n0, const unsigned_int &n1);

cover operator<(const unsigned_int &n0, const unsigned_int &n1);
cover operator>(const unsigned_int &n0, const unsigned_int &n1);
cover operator<=(const unsigned_int &n0, const unsigned_int &n1);
cover operator>=(const unsigned_int &n0, const unsigned_int &n1);

}

