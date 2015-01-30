/*
 * boolean.h
 *
 *  Created on: May 11, 2013
 *      Author: nbingham
 */

#include "common.h"
#include "cover.h"

#ifndef boolean_h
#define boolean_h

struct tokenizer;
struct variable_space;

/**
 * This structure stores the boolean form of a binary boolean boolean (a sum of cubes).
 */
struct boolean
{
	boolean();
	boolean(const boolean &c);
	boolean(int val);
	boolean(int var, int val);
	boolean(cube m);
	boolean(cover m);
	boolean(string exp, variable_space &vars, tokenizer *tokens = NULL);
	~boolean();

	pair<cover, bool> terms[2];

	bool require(int r);
	bool require();

	void espresso();

	bool is_subset_of(const cube &s);
	bool is_subset_of(const cover &s);
	bool is_subset_of(boolean &s);


	vector<int> vars();
	void vars(vector<int> *result);
	boolean refactor(vector<pair<int, int> > uids);

	void hide(int uid);
	void hide(vector<int> uids);
	void cofactor(int uid, int val);
	void cofactor(const cube &s1);

	boolean &operator=(const boolean &c);
	boolean &operator=(cover c);
	boolean &operator=(cube c);
	boolean &operator=(int c);

	boolean &operator&=(const boolean &c);
	boolean &operator&=(cover c);
	boolean &operator&=(cube c);
	boolean &operator&=(int c);

	boolean &operator|=(const boolean &c);
	boolean &operator|=(cover c);
	boolean &operator|=(cube c);
	boolean &operator|=(int c);

	cover &operator[](int i);
};

ostream &operator<<(ostream &os, boolean c);
string to_string(boolean &c, const vector<string> &v, bool safe = false);

boolean transition(boolean &c1, const cube &c2);

bool mergible(boolean &b1, boolean &b2);

bool are_mutex(boolean &c1, boolean &c2);
bool are_mutex(boolean &c1, boolean &c2, boolean &c3);
bool are_mutex(boolean &c1, boolean &c2, boolean &c3, boolean &c4);

boolean operator&(boolean &c1, boolean &c2);
boolean operator|(boolean &c1, boolean &c2);
boolean operator~(const boolean &c1);

bool operator==(boolean &c1, boolean &c2);
bool operator==(boolean &c1, const cover &c2);
bool operator==(const cover &c1, boolean &c2);
bool operator==(boolean &c1, const cube &c2);
bool operator==(const cube &c1, boolean &c2);
bool operator==(boolean &c1, int c2);
bool operator==(int c1, boolean &c2);

bool operator!=(boolean &c1, boolean &c2);
bool operator!=(boolean &c1, const cover &c2);
bool operator!=(const cover &c1, boolean &c2);
bool operator!=(boolean &c1, const cube &c2);
bool operator!=(const cube &c1, boolean &c2);
bool operator!=(boolean &c1, int c2);
bool operator!=(int c1, boolean &c2);

#endif
