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

string instance::to_string() const
{
	string result = name;
	for (int i = 0; i < (int)slice.size(); i++)
		result += "[" + ::to_string(slice[i]) + "]";
	return result;
}

variable::variable()
{
	region = 0;
}

variable::~variable()
{

}

string variable::to_string() const
{
	string result = "";
	for (int i = 0; i < (int)name.size(); i++)
	{
		if (i != 0)
			result += ".";

		result += name[i].to_string();
	}

	if (region != 0)
		result += "'" + ::to_string(region);

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
	bool exists = false;
	for (int i = 0; i < (int)variables.size(); i++)
		if (variables[i].name == v.name)
		{
			exists = true;
			if (variables[i].region == v.region)
				return i;
		}

	if (exists)
		return define(v);

	return -1;
}

int variable_set::closest(variable v) const
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

cube variable_set::remote(cube c) const
{
	cube result;
	map<vector<instance>, int> values;
	int i = 0;
	for (; i < c.size()*16 && i < (int)variables.size(); i++)
		if (c.get(i) != 2)
		{
			map<vector<instance>, int>::iterator loc = values.find(variables[i].name);
			if (loc != values.end())
				loc->second &= (c.get(i)+1);
			else
				values.insert(pair<vector<instance>, int>(variables[i].name, c.get(i)+1));
		}
	for (; i < c.size()*16; i++)
		result.set(i, c.get(i));

	for (int i = 0; i < (int)variables.size(); i++)
	{
		map<vector<instance>, int>::iterator loc = values.find(variables[i].name);
		if (loc != values.end())
			result.set(i, loc->second-1);
	}

	return result;
}

cover variable_set::remote(cover c) const
{
	cover result;
	for (int i = 0; i < (int)c.cubes.size(); i++)
		result.cubes.push_back(remote(c.cubes[i]));
	return result;
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

bool operator==(const boolean::instance &i0, const boolean::instance &i1)
{
	return i0.name == i1.name && i0.slice == i1.slice;
}

bool operator!=(const boolean::instance &i0, const boolean::instance &i1)
{
	return i0.name != i1.name || i0.slice != i1.slice;
}

bool operator<(const boolean::variable &v0, const boolean::variable &v1)
{
	return (v0.name < v1.name) ||
		   (v0.name == v1.name && v0.region < v1.region);
}

bool operator>(const boolean::variable &v0, const boolean::variable &v1)
{
	return (v0.name > v1.name) ||
		   (v0.name == v1.name && v0.region > v1.region);
}

bool operator<=(const boolean::variable &v0, const boolean::variable &v1)
{
	return (v0.name < v1.name) ||
		   (v0.name == v1.name && v0.region <= v1.region);
}

bool operator>=(const boolean::variable &v0, const boolean::variable &v1)
{
	return (v0.name > v1.name) ||
		   (v0.name == v1.name && v0.region >= v1.region);
}

bool operator==(const boolean::variable &v0, const boolean::variable &v1)
{
	return (v0.name == v1.name && v0.region == v1.region);
}

bool operator!=(const boolean::variable &v0, const boolean::variable &v1)
{
	return (v0.name != v1.name || v0.region != v1.region);
}

}
