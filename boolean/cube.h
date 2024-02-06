/*
 * cube.h
 *
 *  Created on: May 11, 2013
 *      Author: nbingham
 */

#pragma once

#include <vector>
#include <iostream>

using std::vector;
using std::ostream;
using std::pair;

namespace boolean
{
struct cover;

/*

This structure represents a four-valued logic. When it is used to
represent state information the values represent the following:

- (11)	wire is stable at either GND or VDD, but we don't know which
1 (10)	wire is stable at VDD
0 (01)	wire is stable at GND
X (00)	voltage on wire is unstable

When it is used to do boolean logic, the values represent the following:

- (11)  cube covering both 0 and 1
1 (10)  cube covering 1
0 (01)  cube covering 0
X (00)  empty cube

 */
struct cube
{
	cube();
	cube(const cube &m);
	cube(int val);
	cube(int uid, int val);
	~cube();

	vector<unsigned int> values;

	// Array Operators
	int size() const;
	void extendX(int num);
	void extendN(int num);
	void trunk(int size);

	// Single Variable Operators
	int get(int uid) const;
	void set(int uid, int val);

	void sv_union(int uid, int val);
	void sv_intersect(int uid, int val);
	void sv_invert(int uid);
	void sv_or(int uid, int val);
	void sv_and(int uid, int val);
	void sv_not(int uid);

	bool is_subset_of(const cube &s) const;
	bool is_subset_of(const cover &s) const;
	bool is_strict_subset_of(const cube &s) const;
	bool is_tautology() const;
	bool is_null() const;
	int memory_width() const;
	int width() const;

	cube xoutnulls() const;
	cube mask() const;
	cube mask(int v) const;
	cube mask(cube c) const;
	cube flipped_mask(cube c) const;
	cube combine_mask(cube c) const;
	cube inverse() const;
	cube flip() const;
	//cube deconflict(cube c) const;
	cube remote(vector<vector<int> > groups) const;

	cube get_cover(int n) const;

	cover expand(vector<int> uids) const;

	vector<int> vars() const;
	void vars(vector<int> *result) const;
	cube refactor(vector<pair<int, int> > uids) const;

	void intersect(const cube &s1);
	void intersect(const cube &s1, const cube &s2);
	void intersect(const cube &s1, const cube &s2, const cube &s3);
	void intersect(const cover &s1);

	void supercube(const cube &s1);
	void supercube(const cube &s1, const cube &s2);
	void supercube(const cube &s1, const cube &s2, const cube &s3);
	void supercube(const cover &s1);

	void hide(int uid);
	void hide(vector<int> uids);
	void cofactor(int uid, int val);
	void cofactor(const cube &s1);

	cube &operator=(cube s);
	cube &operator=(int val);

	cube &operator&=(cube s);
	cube &operator&=(int val);

	cube &operator|=(cube s);
	cube &operator|=(int val);

	cube &operator>>=(cube s);

	//void hash(hasher &hash) const;
};

ostream &operator<<(ostream &os, cube m);

cover operator~(cube s1);

cube operator&(cube s1, cube s2);
cube operator&(cube s1, int s2);
cube operator&(int s1, cube s2);

cube intersect(const cube &s1, const cube &s2);
cube intersect(const cube &s1, const cube &s2, const cube &s3);

bool are_mutex(const cube &s1, const cube &s2);
bool are_mutex(const cube &s1, const cube &s2, const cube &s3);
bool are_mutex(const cube &s1, const cube &s2, const cube &s3, const cube &s4);
bool are_mutex(const cube &s1, const cover &s2);

cover operator|(cube s1, cube s2);
cover operator|(cube s1, int s2);
cover operator|(int s1, cube s2);

cube supercube(const cube &s1, const cube &s2);
cube supercube(const cube &s1, const cube &s2, const cube &s3);
cube supercube(const cube &s1, const cube &s2, const cube &s3, const cube &s4);

cube basic_consensus(cube s1, cube s2);
cube consensus(cube s1, cube s2);

cube prime(cube s1, cube s2);

cover basic_sharp(cube s1, cube s2);
cover sharp(cube s1, cube s2);

cover basic_disjoint_sharp(cube s1, cube s2);
cover disjoint_sharp(cube s1, cube s2);

cover crosslink(cube s1, cube s2);

cube cofactor(const cube &s1, int uid, int val);
cube cofactor(const cube &s1, const cube &s2);

int distance(const cube &s0, const cube &s1);
int similarity(const cube &s0, const cube &s1);
bool similarity_g0(const cube &s0, const cube &s1);
void merge_distances(const cube &s0, const cube &s1, int *vn, int *xv, int *vx);
bool mergible(const cube &s0, const cube &s1);

cube supercube_of_complement(const cube &s);

cube local_assign(const cube &encoding, const cube &assignment, bool stable);
cube remote_assign(const cube &encoding, const cube &assignment, bool stable);
bool vacuous_assign(const cube &encoding, const cube &assignment, bool stable);
int passes_guard(const cube &local, const cube &global, const cube &guard);
bool violates_mutex(const cube &global, const cube &mutex);
cube interfere(const cube &left, const cube &right);
cube difference(const cube &left, const cube &right);

bool operator==(cube s1, cube s2);
bool operator==(cube s1, int s2);
bool operator==(int s1, cube s2);

bool operator!=(cube s1, cube s2);
bool operator!=(cube s1, int s2);
bool operator!=(int s1, cube s2);

bool operator<(cube s1, cube s2);
bool operator>(cube s1, cube s2);
bool operator<=(cube s1, cube s2);
bool operator>=(cube s1, cube s2);
}

