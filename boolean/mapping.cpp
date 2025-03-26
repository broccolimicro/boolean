#include <boolean/mapping.h>

#include <stdio.h>

using namespace std;

namespace boolean {

mapping::mapping(bool ident) {
	isIdentity = ident;
}

mapping::mapping(int num) {
	identity(num);
}

mapping::mapping(vector<int> nets) {
	this->nets = nets;
	isIdentity = false;
}

mapping::~mapping() {
}

int mapping::unmap(int net) const {
	if (isIdentity) {
		return net;
	}

	for (int i = 0; i < (int)nets.size(); i++) {
		if (nets[i] == net) {
			return i;
		}
	}
	return -1;
}

int mapping::map(int net) const {
	if (isIdentity) {
		return net;
	}

	if (net >= 0 and net < (int)nets.size()) {
		return nets[net];
	}
	return -1;
}

void mapping::identity(int num) {
	nets.clear();
	isIdentity = false;

	nets.reserve(num);
	for (int i = 0; i < num; i++) {
		nets.push_back(i);
	}
}

void mapping::apply(const mapping &m) {
	// this: current -> main
	// m: other -> current
	// want: other -> main

	vector<int> updated;
	updated.reserve(m.nets.size());
	for (int i = 0; i < (int)m.nets.size(); i++) {
		updated.push_back(nets[m.nets[i]]);
	}
	nets = updated;
	isIdentity = isIdentity and m.isIdentity;
}

void mapping::set(int from, int to) {
	isIdentity = false;
	if (from >= (int)nets.size()) {
		nets.resize(from+1, -1);
	}
	if (from >= 0) {
		nets[from] = to;
	}
}

void mapping::set(vector<int> from, int to) {
	isIdentity = false;
	int m = (int)nets.size()-1;
	for (int i = 0; i < (int)from.size(); i++) {
		if (from[i] > m) {
			m = from[i];
		}
	}
	nets.resize(m+1,-1);
	for (int i = 0; i < (int)from.size(); i++) {
		if (from[i] >= 0) {
			nets[from[i]] = to;
		}
	}
}

bool mapping::has(int from) const {
	return isIdentity or (from >= 0 and from < (int)nets.size() and nets[from] >= 0);
}

int mapping::size() const {
	return (int)nets.size();
}

mapping mapping::reverse() const {
	mapping result(isIdentity);
	int hi = 0;
	for (int i = 0; i < (int)nets.size(); i++) {
		if (nets[i] > hi) {
			hi = nets[i];
		}
	}
	result.nets.resize(hi+1, -1);
	for (int i = 0; i < (int)nets.size(); i++) {
		if (nets[i] >= 0) {
			result.nets[nets[i]] = i;
		}
	}
	return result;
}

void mapping::reverse_inplace() {
	nets = reverse().nets;
}

void mapping::print() const {
	printf("map{%s", isIdentity ? "identity" : "");
	for (int i = 0; i < (int)nets.size(); i++) {
		if (i != 0) {
			printf(", ");
		}
		printf("%d -> %d", i, nets[i]);
	}
	printf("}\n");
}

}
