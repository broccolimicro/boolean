/*
 * signed_int.cpp
 *
 *  Created on: Feb 6, 2015
 *      Author: nbingham
 */

#include <boolean/signed_int.h>

namespace boolean
{

signed_int::signed_int() { }
signed_int::signed_int(int width, int offset) : bitset(width, offset) { }

signed_int::signed_int(long value)
{
	while (value != 0 and value != -1) {
		bits.push_back(cover(value & 1));
		value >>= 1;
	}
	bits.push_back(cover(value & 1));
}

signed_int::signed_int(const bitset &n) : bitset(n) { }
signed_int::signed_int(const signed_int &n) : bitset(n) { }
signed_int::~signed_int() { }

cover signed_int::extend() const
{
	if (bits.empty()) {
		return boolean::cover();
	}
	return bits.back();
}

signed_int &signed_int::operator+=(const signed_int &n)
{
	*this = *this + n;
	return *this;
}

signed_int &signed_int::operator-=(const signed_int &n)
{
	*this = *this - n;
	return *this;
}

signed_int &signed_int::operator*=(const signed_int &n)
{
	*this = *this * n;
	return *this;
}

signed_int &signed_int::operator/=(const signed_int &n)
{
	*this = *this / n;
	return *this;
}

/*signed_int &signed_int::operator%=(const signed_int &n)
{
	*this = *this % n;
	return *this;
}*/

signed_int operator-(const signed_int &n)
{
	signed_int result;
	cover carry = 1;
	for (int i = 0; i < (int)n.bits.size(); i++)
	{
		result.bits.push_back((~n.bits[i]&~carry) | (n.bits[i]&carry));
		carry = ~n.bits[i]&carry;
	}

	cover inf = ~n.extend()&carry;
	while (inf != carry)
	{
		result.bits.push_back((~n.extend()&~carry) | (n.extend()&carry));
		carry = inf;
		inf = ~n.extend()&carry;
	}

	return result;
}

signed_int operator+(const signed_int &n0, const signed_int &n1)
{
	signed_int result;
	cover carry = 0, inf;
	int m = (int)std::min(n0.bits.size(), n1.bits.size());
	for (int i = 0; i < m; i++)
	{
		result.bits.push_back((n0.bits[i]&~n1.bits[i]&~carry) | (~n0.bits[i]&n1.bits[i]&~carry) | (~n0.bits[i]&~n1.bits[i]&carry));
		carry = (n0.bits[i]&n1.bits[i]) | (n0.bits[i]&carry) | (n1.bits[i]&carry);
	}
	for (int i = m; i < (int)n0.bits.size(); i++)
	{
		result.bits.push_back((n0.bits[i]&~n1.extend()&~carry) | (~n0.bits[i]&n1.extend()&~carry) | (~n0.bits[i]&~n1.extend()&carry));
		carry = (n0.bits[i]&n1.extend()) | (n0.bits[i]&carry) | (n1.extend()&carry);
	}
	for (int i = m; i < (int)n1.bits.size(); i++)
	{
		result.bits.push_back((n0.extend()&~n1.bits[i]&~carry) | (~n0.extend()&n1.bits[i]&~carry) | (~n0.extend()&~n1.bits[i]&carry));
		carry = (n0.extend()&n1.bits[i]) | (n0.extend()&carry) | (n1.bits[i]&carry);
	}

	inf = (n0.extend()&n1.extend()) | (n0.extend()&carry) | (n1.extend()&carry);
	while (inf != carry)
	{
		result.bits.push_back((n0.extend()&~n1.extend()&~carry) | (~n0.extend()&n1.extend()&~carry) | (~n0.extend()&~n1.extend()&carry));
		carry = inf;
		inf = (n0.extend()&n1.extend()) | (n0.extend()&carry) | (n1.extend()&carry);
	}

	return result;
}

signed_int operator-(const signed_int &n0, const signed_int &n1)
{
	signed_int result;
	cover carry = 1, inf;
	int m = (int)std::min(n0.bits.size(), n1.bits.size());
	for (int i = 0; i < m; i++)
	{
		result.bits.push_back((n0.bits[i]&n1.bits[i]&~carry) | (~n0.bits[i]&~n1.bits[i]&~carry) | (~n0.bits[i]&n1.bits[i]&carry));
		carry = (n0.bits[i]&~n1.bits[i]) | (n0.bits[i]&carry) | (~n1.bits[i]&carry);
	}
	for (int i = m; i < (int)n0.bits.size(); i++)
	{
		result.bits.push_back((n0.bits[i]&n1.extend()&~carry) | (~n0.bits[i]&~n1.extend()&~carry) | (~n0.bits[i]&n1.extend()&carry));
		carry = (n0.bits[i]&~n1.extend()) | (n0.bits[i]&carry) | (~n1.extend()&carry);
	}
	for (int i = m; i < (int)n1.bits.size(); i++)
	{
		result.bits.push_back((n0.extend()&n1.bits[i]&~carry) | (~n0.extend()&~n1.bits[i]&~carry) | (~n0.extend()&n1.bits[i]&carry));
		carry = (n0.extend()&~n1.bits[i]) | (n0.extend()&carry) | (~n1.bits[i]&carry);
	}

	inf = (n0.extend()&~n1.extend()) | (n0.extend()&carry) | (~n1.extend()&carry);
	while (inf != carry)
	{
		result.bits.push_back((n0.extend()&n1.extend()&~carry) | (~n0.extend()&~n1.extend()&~carry) | (~n0.extend()&n1.extend()&carry));
		carry = inf;
		inf = (n0.extend()&~n1.extend()) | (n0.extend()&carry) | (~n1.extend()&carry);
	}

	return result;
}

signed_int operator*(const signed_int &n0, const signed_int &n1)
{
	signed_int result;
	for (int i = 0; i < (int)n1.bits.size(); i++)
		result = result + ((n0&n1.bits[i]) << i);
	return result;
}

signed_int operator/(const signed_int &n0, const signed_int &n1)
{
	signed_int result;
	result.bits.resize(n0.bits.size(), cover(0));

	// take the absolute value of each signed_int
	signed_int m0 = (n0.extend()&~n0) | (~n0.extend()&n0);
	signed_int m1 = (n1.extend()&~n1) | (~n1.extend()&n1);

	int s = ((int)n0.bits.size())-1;
	for (int i = s; i >= 0; i--)
	{
		signed_int divisor = (m1 << i);
		cover condition = (divisor < m0);
		result.bits[i] = condition;
		m0 = (condition&(m0-divisor)) | (~condition&m0);
		m0.espresso();
	}

	cover sign = m0.extend() ^ m1.extend();

	result = (sign&(-result)) | (~sign&result);
	return result;
}

cover operator<(const signed_int &n0, const signed_int &n1)
{
	cover n0n_n1p = n0.extend()&~n1.extend();
	cover n0s_n1s = (n0.extend()&n1.extend()) | (~n0.extend()&~n1.extend());
	cover disjunction(0);
	cover conjunction(1);

	// take the absolute value of each signed_int
	signed_int m0 = (n0.extend()&~n0) | (~n0.extend()&n0);
	signed_int m1 = (n1.extend()&~n1) | (~n1.extend()&n1);

	// if n1 has more bits than n0, all it takes is one of those bits
	// to be set for n0 to be less than n1
	for (int i = (int)m1.bits.size()-1; i >= (int)m0.bits.size(); i--)
		disjunction |= m1.bits[i];
	// if n0 has more bits than n1, then every bit must be 0
	for (int i = (int)m0.bits.size()-1; i >= (int)m1.bits.size(); i--)
		conjunction &= ~m0.bits[i];

	int m = (int)std::min((int)m0.bits.size()-1, (int)m1.bits.size()-1);
	for (int i = m; i >= 0; i--)
	{
		disjunction |= ~m0.bits[i]&m1.bits[i]&conjunction;
		conjunction &= (m0.bits[i]&m1.bits[i]) | (~m0.bits[i]&~m1.bits[i]);
	}

	cover result = n0n_n1p | (n0s_n1s&disjunction);
	result.espresso();
	return result;
}

cover operator>(const signed_int &n0, const signed_int &n1)
{
	return n1 < n0;
}

cover operator>=(const signed_int &n0, const signed_int &n1)
{
	return ~(n0 < n1);
}

cover operator<=(const signed_int &n0, const signed_int &n1)
{
	return ~(n1 < n0);
}

}
