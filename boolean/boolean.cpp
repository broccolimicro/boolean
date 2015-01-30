/*
 * boolean.cpp
 *
 *  Created on: May 11, 2013
 *      Author: nbingham
 */

#include "common.h"
#include "boolean.h"
#include "cube.h"
#include "tokenizer.h"
#include "variable_space.h"
#include "message.h"

boolean::boolean()
{
	terms[0].second = false;
	terms[1].second = false;
}

boolean::boolean(const boolean &c)
{
	terms[0] = c.terms[0];
	terms[1] = c.terms[1];
}

boolean::boolean(int val)
{
	terms[val].first.push_back(cube());
	terms[0].second = true;
	terms[1].second = true;
}

boolean::boolean(int var, int val)
{
	terms[1].first.push_back(cube(var, val));
	terms[0].first.push_back(cube(var, 1-val));
	terms[1].second = true;
	terms[0].second = true;
}

boolean::boolean(cube m)
{
	terms[1].first.push_back(m);
	terms[1].second = true;
	terms[0].second = false;
}

boolean::boolean(cover m)
{
	terms[1].first = m;
	terms[1].second = true;
	terms[0].second = false;
}

boolean::boolean(string exp, variable_space &vars, tokenizer *tokens)
{
	terms[1].first = cover(exp, vars, tokens);
	terms[1].second = true;
	terms[0].second = false;
}

boolean::~boolean()
{
}

bool boolean::require(int r)
{
	if (!terms[r].second && terms[1-r].second)
	{
		terms[r].first = ~terms[1-r].first;
		terms[r].second = true;
	}
	else if (!terms[r].second && !terms[1-r].second)
	{
		error("", "Operator on uninitialized boolean", "", __FILE__, __LINE__);
		return false;
	}

	return true;
}

bool boolean::require()
{
	if (!terms[0].second && !terms[1].second)
	{
		error("", "Operator on uninitialized boolean", "", __FILE__, __LINE__);
		return false;
	}
	else if (terms[0].second)
	{
		terms[1].first = ~terms[0].first;
		terms[1].second = true;
	}
	else if (terms[1].second)
	{
		terms[0].first = ~terms[1].first;
		terms[0].second = true;
	}

	return true;
}

void boolean::espresso()
{
	require();

	::espresso(terms[1].first, cover(), terms[0].first);
	::espresso(terms[0].first, cover(), terms[1].first);
}

bool boolean::is_subset_of(const cube &s)
{
	require(1);

	return terms[1].first.is_subset_of(s);
}

bool boolean::is_subset_of(const cover &s)
{
	require(1);

	return terms[1].first.is_subset_of(s);
}

bool boolean::is_subset_of(boolean &s)
{
	require(1);
	s.require(1);

	return terms[1].first.is_subset_of(s.terms[1].first);
}

vector<int> boolean::vars()
{
	vector<int> result;
	if (terms[1].second)
		terms[1].first.vars(&result);
	else if (terms[0].second)
		terms[0].first.vars(&result);
	else
		error("", "Operator on uninitialized boolean", "", __FILE__, __LINE__);
	return result;
}

void boolean::vars(vector<int> *result)
{
	if (terms[1].second)
		terms[1].first.vars(result);
	else if (terms[0].second)
		terms[0].first.vars(result);
	else
		error("", "Operator on uninitialized boolean", "", __FILE__, __LINE__);
}

boolean boolean::refactor(vector<pair<int, int> > uids)
{
	boolean result;
	if (terms[0].second)
	{
		result.terms[0].first = terms[0].first.refactor(uids);
		result.terms[0].second = true;
	}

	if (terms[1].second)
	{
		result.terms[1].first = terms[1].first.refactor(uids);
		result.terms[1].second = true;
	}
	return result;
}

void boolean::hide(int uid)
{
	require(1);

	terms[1].first.hide(uid);
	terms[0].second = false;
}

void boolean::hide(vector<int> uids)
{
	require(1);

	terms[1].first.hide(uids);
	terms[0].second = false;
}

void boolean::cofactor(int uid, int val)
{
	require(1);

	if (terms[0].second)
		terms[0].first |= ::cofactor(terms[1].first, uid, 1-val);

	terms[1].first.cofactor(uid, val);
}

void boolean::cofactor(const cube &s1)
{
	require(1);

	if (terms[0].second)
		terms[0].first |= ::cofactor(terms[1].first, s1);

	terms[1].first.cofactor(s1);
}

boolean &boolean::operator=(const boolean &c)
{
	terms[1] = c.terms[1];
	terms[0] = c.terms[0];
	return *this;
}

boolean &boolean::operator=(cover c)
{
	terms[1].first = c;
	terms[1].second = true;
	terms[0].second = false;
	return *this;
}

boolean &boolean::operator=(cube c)
{
	terms[1].first = vector<cube>(1, c);
	terms[1].second = true;
	terms[0].second = false;
	return *this;
}

boolean &boolean::operator=(int c)
{
	terms[0].first.clear();
	terms[1].first.clear();
	terms[c].first.push_back(cube());
	terms[0].second = true;
	terms[1].second = true;
	return *this;
}

boolean &boolean::operator&=(const boolean &c)
{
	if (c.terms[1].second && c.terms[0].second)
	{
		if (terms[1].second)
			terms[1].first &= c.terms[1].first;

		if (terms[0].second)
			terms[0].first |= c.terms[0].first;
	}
	else if (c.terms[1].second)
	{
		require(1);
		terms[1].first &= c.terms[1].first;
	}
	else if (c.terms[0].second)
	{
		require(0);
		terms[0].first |= c.terms[0].first;
	}

	return *this;
}

boolean &boolean::operator&=(cover c)
{
	require(1);
	terms[1].first &= c;
	terms[0].second = false;
	return *this;
}

boolean &boolean::operator&=(cube c)
{
	require(1);
	terms[1].first &= c;
	terms[0].second = false;
	return *this;
}

boolean &boolean::operator&=(int c)
{
	if (c == 0)
	{
		terms[1].first.clear();
		terms[0].first = vector<cube>(1, cube());
		terms[1].second = true;
		terms[0].second = true;
	}
	return *this;
}

boolean &boolean::operator|=(const boolean &c)
{
	if (c.terms[1].second && c.terms[0].second)
	{
		if (terms[1].second)
			terms[1].first |= c.terms[1].first;

		if (terms[0].second)
			terms[0].first &= c.terms[0].first;
	}
	else if (c.terms[1].second)
	{
		require(1);
		terms[1].first |= c.terms[1].first;
	}
	else if (c.terms[0].second)
	{
		require(0);
		terms[0].first &= c.terms[0].first;
	}

	return *this;
}

boolean &boolean::operator|=(cover c)
{
	require(1);
	terms[1].first |= c;
	terms[0].second = false;
	return *this;
}

boolean &boolean::operator|=(cube c)
{
	require(1);
	terms[1].first.push_back(c);
	terms[0].second = false;
	return *this;
}
boolean &boolean::operator|=(int c)
{
	if (c == 1)
	{
		terms[1].first = vector<cube>(1, cube());
		terms[0].first.clear();
		terms[1].second = true;
		terms[0].second = true;
	}
	return *this;
}

cover &boolean::operator[](int i)
{
	require(1);
	return terms[i].first;
}

ostream &operator<<(ostream &os, boolean c)
{
	os << c.terms[1];
	return os;
}

string to_string(boolean &c, const vector<string> &v, bool safe)
{
	c.require(1);
	return to_string(c.terms[1].first, v, safe);
}

boolean transition(boolean &c1, const cube &c2)
{
	c1.require(1);
	return boolean(transition(c1.terms[1].first, c2));
}

bool mergible(boolean &b1, boolean &b2)
{
	b1.require(1);
	b2.require(1);
	return mergible(b1.terms[1].first, b2.terms[1].first);
}

bool are_mutex(boolean &c1, boolean &c2)
{
	c1.require(1);
	c2.require(1);
	return are_mutex(c1.terms[1].first, c2.terms[1].first);
}

bool are_mutex(boolean &c1, boolean &c2, boolean &c3)
{
	c1.require(1);
	c2.require(1);
	c3.require(1);
	return are_mutex(c1.terms[1].first, c2.terms[1].first, c3.terms[1].first);
}

bool are_mutex(boolean &c1, boolean &c2, boolean &c3, boolean &c4)
{
	c1.require(1);
	c2.require(1);
	c3.require(1);
	c4.require(1);
	return are_mutex(c1.terms[1].first, c2.terms[1].first, c3.terms[1].first, c4.terms[1].first);
}

boolean operator&(boolean &c1, boolean &c2)
{
	boolean result;
	if (c2.terms[1].second && c2.terms[0].second)
	{
		if (c1.terms[1].second)
		{
			result.terms[1].first = c1.terms[1].first & c2.terms[1].first;
			result.terms[1].second = true;
		}

		if (c1.terms[0].second)
		{
			result.terms[0].first = c1.terms[0].first | c2.terms[0].first;
			result.terms[0].second = true;
		}
	}
	else if (c2.terms[1].second)
	{
		c1.require(1);
		result.terms[1].first = c1.terms[1].first & c2.terms[1].first;
		result.terms[1].second = true;
	}
	else if (c2.terms[0].second)
	{
		c1.require(0);
		result.terms[0].first = c1.terms[0].first | c2.terms[0].first;
		result.terms[0].second = true;
	}
	return result;
}

boolean operator|(boolean &c1, boolean &c2)
{
	boolean result;

	if (c2.terms[1].second && c2.terms[0].second)
	{
		if (c1.terms[1].second)
		{
			result.terms[1].first = c1.terms[1].first | c2.terms[1].first;
			result.terms[1].second = true;
		}

		if (c1.terms[0].second)
		{
			result.terms[0].first = c1.terms[0].first & c2.terms[0].first;
			result.terms[0].second = true;
		}
	}
	else if (c2.terms[1].second)
	{
		c1.require(1);
		result.terms[1].first = c1.terms[1].first | c2.terms[1].first;
		result.terms[1].second = true;
	}
	else if (c2.terms[0].second)
	{
		c1.require(0);
		result.terms[0].first = c1.terms[0].first & c2.terms[0].first;
		result.terms[0].second = true;
	}

	return result;
}

boolean operator~(const boolean &c1)
{
	boolean result;
	result.terms[1] = c1.terms[0];
	result.terms[0] = c1.terms[1];
	return result;
}

bool operator==(boolean &c1, boolean &c2)
{
	c1.require();
	c2.require();
	return (are_mutex(c1.terms[1].first, c2.terms[0].first) && are_mutex(c1.terms[0].first, c2.terms[1].first));
}

bool operator==(boolean &c1, const cover &c2)
{
	c1.require();
	return (are_mutex(c1.terms[0].first, c2) && are_mutex(c1.terms[1].first, ~c2));
}

bool operator==(const cover &c1, boolean &c2)
{
	c2.require();
	return (are_mutex(c1, c2.terms[0].first) && are_mutex(~c1, c2.terms[1].first));
}

bool operator==(boolean &c1, const cube &c2)
{
	c1.require();
	return (are_mutex(c1.terms[0].first, c2) && are_mutex(c1.terms[1].first, ~c2));
}

bool operator==(const cube &c1, boolean &c2)
{
	c2.require();
	return (are_mutex(c1, c2.terms[0].first) && are_mutex(~c1, c2.terms[1].first));
}

bool operator==(boolean &c1, int c2)
{
	c1.require(1-c2);
	return c1.terms[1-c2].first.is_null();
}

bool operator==(int c1, boolean &c2)
{
	c2.require(1-c1);
	return c2.terms[1-c1].first.is_null();
}

bool operator!=(boolean &c1, boolean &c2)
{
	c1.require();
	c2.require();
	return (!are_mutex(c1.terms[1].first, c2.terms[0].first) || !are_mutex(c1.terms[0].first, c2.terms[1].first));
}

bool operator!=(boolean &c1, const cover &c2)
{
	c1.require();
	return (!are_mutex(c1.terms[1].first, ~c2) || !are_mutex(c1.terms[0].first, c2));
}

bool operator!=(const cover &c1, boolean &c2)
{
	c2.require();
	return (!are_mutex(c1, c2.terms[0].first) || !are_mutex(~c1, c2.terms[1].first));
}

bool operator!=(boolean &c1, const cube &c2)
{
	c1.require();
	return (!are_mutex(c1.terms[1].first, ~c2) || !are_mutex(c1.terms[0].first, c2));
}

bool operator!=(const cube &c1, boolean &c2)
{
	c2.require();
	return (!are_mutex(c1, c2.terms[0].first) || !are_mutex(~c1, c2.terms[1].first));
}

bool operator!=(boolean &c1, int c2)
{
	c1.require(1-c2);
	return !c1.terms[1-c2].first.is_null();
}

bool operator!=(int c1, boolean &c2)
{
	c2.require(1-c1);
	return !c2.terms[1-c1].first.is_null();
}
