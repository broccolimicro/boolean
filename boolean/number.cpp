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
number::number()
{

}

number::number(int width, int offset)
{
	for (int i = 0; i < width-1; i++)
		bits.push_back(cover(i+offset, 1));
	sign = cover(width-1+offset, 1);
}

number::number(const number &n)
{
	bits = n.bits;
	sign = n.sign;
}

number::~number()
{

}

number &number::simplify()
{
	for (int i = 0; i < (int)bits.size(); i++)
		bits[i].espresso();
	sign.espresso();
	return *this;
}

number &number::sext(int s)
{
	for (int i = 0; i < s; i++)
		bits.push_back(sign);
	return *this;
}

number &number::operator=(number n)
{
	bits = n.bits;
	sign = n.sign;
	return *this;
}

}

boolean::number operator&(boolean::number n0, boolean::number n1)
{
	boolean::number result;
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

boolean::number operator|(boolean::number n0, boolean::number n1)
{
	boolean::number result;
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

boolean::number operator~(boolean::number n)
{
	boolean::number result;
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(~n.bits[i]);
	result.sign = ~n.sign;
	return result;
}

boolean::number operator&(boolean::number n, boolean::cover c)
{
	for (int i = 0; i < (int)n.bits.size(); i++)
		n.bits[i] &= c;
	n.sign &= c;
	return n;
}

boolean::number operator|(boolean::number n, boolean::cover c)
{
	for (int i = 0; i < (int)n.bits.size(); i++)
		n.bits[i] |= c;
	n.sign |= c;
	return n;
}

boolean::number operator&(boolean::cover c, boolean::number n)
{
	for (int i = 0; i < (int)n.bits.size(); i++)
		n.bits[i] &= c;
	n.sign &= c;
	return n;
}

boolean::number operator|(boolean::cover c, boolean::number n)
{
	for (int i = 0; i < (int)n.bits.size(); i++)
		n.bits[i] |= c;
	n.sign |= c;
	return n;
}

boolean::number operator<<(boolean::number n, int s)
{
	boolean::number result;
	result.bits.reserve(s + (int)n.bits.size());
	for (int i = 0; i < s; i++)
		result.bits.push_back(boolean::cover(0));
	for (int i = 0; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i]);
	result.sign = n.sign;
	return result;
}

boolean::number operator>>(boolean::number n, int s)
{
	boolean::number result;
	result.bits.reserve(std::max((int)n.bits.size() - s, 0));
	for (int i = s; i < (int)n.bits.size(); i++)
		result.bits.push_back(n.bits[i]);
	result.sign = n.sign;
	return result;
}

boolean::number operator-(boolean::number n)
{
	boolean::number result;
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

boolean::number operator+(boolean::number n0, boolean::number n1)
{
	boolean::number result;
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

boolean::number operator-(boolean::number n0, boolean::number n1)
{
	boolean::number result;
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

boolean::number operator*(boolean::number n0, boolean::number n1)
{
	boolean::number result;
	for (int i = 0; i < (int)n1.bits.size(); i++)
		result = result + ((n0&n1.bits[i]) << i);
	return result;
}

boolean::number operator/(boolean::number n0, boolean::number n1)
{
	boolean::number result;
	result.bits.resize(n0.bits.size(), boolean::cover(0));
	result.sign = 0;

	boolean::cover n0_sign = n0.sign;
	boolean::cover n1_sign = n1.sign;

	// take the absolute value of each number
	n0 = (n0_sign&~n0) | (~n0_sign&n0);
	n1 = (n1_sign&~n1) | (~n1_sign&n1);

	int s = (int)n0.bits.size()-1;
	for (int i = s; i >= 0; i--)
	{
		boolean::number divisor = (n1 << i);
		boolean::cover condition = (divisor < n0);
		result.bits[i] = condition;
		n0 = (condition&(n0-divisor)) | (~condition&n0);
		n0.simplify();
	}

	boolean::cover sign = ((n0_sign&~n1_sign) | (~n0_sign&n1_sign));

	result = (sign&(-result)) | (~sign&result);
	return result;
}

boolean::cover operator<(boolean::number n0, boolean::number n1)
{
	boolean::cover n0n_n1p = n0.sign&~n1.sign;
	boolean::cover n0s_n1s = (n0.sign&n1.sign) | (~n0.sign&~n1.sign);
	boolean::cover disjunction(0);
	boolean::cover conjunction(1);

	// take the absolute value of each number
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
