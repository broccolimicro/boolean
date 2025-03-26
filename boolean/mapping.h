#pragma once

#include <vector>

namespace boolean {

struct mapping {
	mapping(bool ident=false);
	mapping(int num);
	mapping(std::vector<int> nets);
	~mapping();

	// list of nets from old subckt to include in new subckt
	std::vector<int> nets;
	bool isIdentity;

	int unmap(int net) const;	
	int map(int net) const;
	void identity(int num);
	void apply(const mapping &m);

	void set(int from, int to);
	void set(std::vector<int> from, int to);
	bool has(int from) const;

	int size() const;

	mapping reverse() const;
	void reverse_inplace();

	void print() const;
};

}
