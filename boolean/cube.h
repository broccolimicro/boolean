/*
 * cube.h
 *
 *  Created on: May 11, 2013
 *      Author: nbingham
 */

#include <vector>
#include <ostream>

using std::vector;
using std::pair;
using std::ostream;

#ifndef cube_h
#define cube_h

namespace boolean
{
struct cover;

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

	cube xoutnulls();
	cube mask();
	cube inverse();

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
void merge_distances(const cube &s0, const cube &s1, int *vn, int *xv, int *vx);
bool mergible(const cube &s0, const cube &s1);

cube supercube_of_complement(const cube &s);

cube transition(const cube &s1, const cube &s2);

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

unsigned int count_ones(unsigned int x);
unsigned int count_zeros(unsigned int x);
}

#endif
