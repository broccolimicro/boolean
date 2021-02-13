#include <boolean/cube.h>
#include "ut.hpp"

boost::ut::suite cube = [] {
using namespace boost::ut;

"create"_test = [] {
	boolean::cube c;
};

};

int main() {}
