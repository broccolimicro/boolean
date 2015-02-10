/*
 * variable.cpp
 *
 *  Created on: Feb 5, 2015
 *      Author: nbingham
 */

#include "variable.h"
#include <common/text.h>

namespace boolean
{
instance::instance()
{

}

instance::instance(string name, vector<int> slice)
{
	this->name = name;
	this->slice = slice;
}

instance::~instance()
{

}

instance &instance::operator=(instance i)
{
	name = i.name;
	slice = i.slice;
	return *this;
}

string instance::to_string()
{
	string result = name;
	for (int i = 0; i < (int)slice.size(); i++)
		result += "[" + ::to_string(slice[i]) + "]";
	return result;
}

variable::variable()
{

}

variable::~variable()
{

}

variable &variable::operator=(variable v)
{
	name = v.name;
	return *this;
}

string variable::to_string()
{
	string result = "";
	for (int i = 0; i < (int)name.size(); i++)
	{
		if (i != 0)
			result += ".";

		result += name[i].to_string();
	}
	return result;
}

variable_set::variable_set()
{

}

variable_set::~variable_set()
{

}

int variable_set::define(variable v)
{
	for (int i = 0; i < (int)variables.size(); i++)
		if (variables[i] == v)
			return -1;

	variables.push_back(v);
	return (int)variables.size()-1;
}

int variable_set::find(variable v)
{
	for (int i = 0; i < (int)variables.size(); i++)
		if (variables[i] == v)
			return i;

	return -1;
}

int variable_set::closest(variable v)
{
	int index = 0;
	int min = edit_distance(v.to_string(), variables[0].to_string());

	for (int i = 1; i < (int)variables.size(); i++)
	{
		int dist = edit_distance(v.to_string(), variables[i].to_string());
		if (dist < min)
		{
			min = dist;
			index = i;
		}
	}

	return index;
}

}

bool operator==(const boolean::instance &i0, const boolean::instance &i1)
{
	return i0.name == i1.name && i0.slice == i1.slice;
}

bool operator!=(const boolean::instance &i0, const boolean::instance &i1)
{
	return i0.name != i1.name || i0.slice != i1.slice;
}

bool operator<(const boolean::instance &i0, const boolean::instance &i1)
{
	return (i0.name < i1.name) ||
		   (i0.name == i1.name && i0.slice < i1.slice);
}

bool operator>(const boolean::instance &i0, const boolean::instance &i1)
{
	return (i0.name > i1.name) ||
		   (i0.name == i1.name && i0.slice > i1.slice);
}

bool operator<=(const boolean::instance &i0, const boolean::instance &i1)
{
	return (i0.name < i1.name) ||
		   (i0.name == i1.name && i0.slice <= i1.slice);
}

bool operator>=(const boolean::instance &i0, const boolean::instance &i1)
{
	return (i0.name > i1.name) ||
		   (i0.name == i1.name && i0.slice >= i1.slice);
}

bool operator==(const boolean::variable &v0, const boolean::variable &v1)
{
	if (v0.name.size() != v1.name.size())
		return false;

	for (int i = 0; i < (int)v0.name.size(); i++)
		if (v0.name[i] != v1.name[i])
			return false;

	return true;
}

bool operator!=(const boolean::variable &v0, const boolean::variable &v1)
{
	if (v0.name.size() != v1.name.size())
		return true;

	for (int i = 0; i < (int)v0.name.size(); i++)
		if (v0.name[i] != v1.name[i])
			return true;

	return false;
}

bool operator<(const boolean::variable &v0, const boolean::variable &v1)
{
	int m = (int)min(v0.name.size(), v1.name.size());
	for (int i = 0; i < m; i++)
	{
		if (v0.name[i] < v1.name[i])
			return true;
		if (v0.name[i] > v1.name[i])
			return false;
	}

	if (v0.name.size() < v1.name.size())
		return true;
	else
		return false;
}

bool operator>(const boolean::variable &v0, const boolean::variable &v1)
{
	int m = (int)min(v0.name.size(), v1.name.size());
	for (int i = 0; i < m; i++)
	{
		if (v0.name[i] > v1.name[i])
			return true;
		if (v0.name[i] < v1.name[i])
			return false;
	}

	if (v0.name.size() > v1.name.size())
		return true;
	else
		return false;
}

bool operator<=(const boolean::variable &v0, const boolean::variable &v1)
{
	int m = (int)min(v0.name.size(), v1.name.size());
	for (int i = 0; i < m; i++)
	{
		if (v0.name[i] < v1.name[i])
			return true;
		if (v0.name[i] > v1.name[i])
			return false;
	}

	if (v0.name.size() <= v1.name.size())
		return true;
	else
		return false;
}

bool operator>=(const boolean::variable &v0, const boolean::variable &v1)
{
	int m = (int)min(v0.name.size(), v1.name.size());
	for (int i = 0; i < m; i++)
	{
		if (v0.name[i] > v1.name[i])
			return true;
		if (v0.name[i] < v1.name[i])
			return false;
	}

	if (v0.name.size() >= v1.name.size())
		return true;
	else
		return false;
}

