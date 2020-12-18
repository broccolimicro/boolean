/*
 * unsigned_int.cpp
 *
 *  Created on: Feb 6, 2015
 *      Author: nbingham
 */

#include <boolean/unsigned_int.h>
#include <common/standard.h>

namespace boolean
{

unsigned_int::unsigned_int() { }
unsigned_int::unsigned_int(int width, int offset) : bitset(width, offset) { }

unsigned_int::unsigned_int(unsigned long value)
{
	while (value > 0) {
		bits.push_back(cover(value & 1));
		value >>= 1;
	}
}

unsigned_int::unsigned_int(const cover &c) : bitset(c) { }
unsigned_int::unsigned_int(const bitset &n) : bitset(n) { }
unsigned_int::unsigned_int(const unsigned_int &n) : bitset(n) { }
unsigned_int::~unsigned_int() { }

unsigned_int &unsigned_int::operator+=(const unsigned_int &n)
{
	*this = *this + n;
	return *this;
}

unsigned_int &unsigned_int::operator-=(const unsigned_int &n)
{
	*this = *this - n;
	return *this;
}

unsigned_int &unsigned_int::operator*=(const unsigned_int &n)
{
	*this = *this * n;
	return *this;
}

unsigned_int &unsigned_int::operator/=(const unsigned_int &n)
{
	*this = *this / n;
	return *this;
}

/*unsigned_int &unsigned_int::operator%=(const unsigned_int &n)
{
	*this = *this % n;
	return *this;
}*/

unsigned_int operator+(const unsigned_int &n0, const unsigned_int &n1)
{
	unsigned_int result;
	cover carry = 0, inf;
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

unsigned_int operator-(const unsigned_int &n0, const unsigned_int &n1)
{
	unsigned_int result;
	cover carry = 1, inf;
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

unsigned_int operator*(const unsigned_int &n0, const unsigned_int &n1)
{
	unsigned_int result;
	for (int i = 0; i < (int)n1.bits.size(); i++)
		result = result + ((n0&n1.bits[i]) << i);
	return result;
}

unsigned_int operator/(const unsigned_int &n0, const unsigned_int &n1)
{
	unsigned_int result;
	result.bits.resize(n0.bits.size(), cover(0));
	unsigned_int iter = n0;

	int s = (int)iter.bits.size()-1;
	for (int i = s; i >= 0; i--)
	{
		unsigned_int divisor = (n1 << i);
		cover condition = (divisor < iter);
		result.bits[i] = condition;
		iter = (condition&(iter-divisor)) | (~condition&iter);
		iter.espresso();
	}

	return result;
}

cover operator<(const unsigned_int &n0, const unsigned_int &n1)
{
	cover disjunction(0);
	cover conjunction(1);

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

cover operator>(const unsigned_int &n0, const unsigned_int &n1)
{
	return n1 < n0;
}

cover operator>=(const unsigned_int &n0, const unsigned_int &n1)
{
	return ~(n0 < n1);
}

cover operator<=(const unsigned_int &n0, const unsigned_int &n1)
{
	return ~(n1 < n0);
}

}
