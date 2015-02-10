/*
 * variable.h
 *
 *  Created on: Feb 5, 2015
 *      Author: nbingham
 */

#include <common/standard.h>

#ifndef boolean_variable_h
#define boolean_variable_h

namespace boolean
{

struct instance
{
	instance();
	instance(string name, vector<int> slice);
	~instance();

	string name;
	vector<int> slice;

	instance &operator=(instance i);
	string to_string();
};

struct variable
{
	variable();
	~variable();

	vector<instance> name;

	variable &operator=(variable i);

	string to_string();
};

struct variable_set
{
	variable_set();
	~variable_set();

	vector<variable> variables;

	int define(variable v);
	int find(variable v);
	int closest(variable v);
};

}

bool operator==(const boolean::instance &i0, const boolean::instance &i1);
bool operator!=(const boolean::instance &i0, const boolean::instance &i1);
bool operator<(const boolean::instance &i0, const boolean::instance &i1);
bool operator>(const boolean::instance &i0, const boolean::instance &i1);
bool operator<=(const boolean::instance &i0, const boolean::instance &i1);
bool operator>=(const boolean::instance &i0, const boolean::instance &i1);

bool operator==(const boolean::variable &v0, const boolean::variable &v1);
bool operator!=(const boolean::variable &v0, const boolean::variable &v1);
bool operator<(const boolean::variable &v0, const boolean::variable &v1);
bool operator>(const boolean::variable &v0, const boolean::variable &v1);
bool operator<=(const boolean::variable &v0, const boolean::variable &v1);
bool operator>=(const boolean::variable &v0, const boolean::variable &v1);

#endif
