/*
 * factor.h
 *
 *  Created on: May 29, 2015
 *      Author: nbingham
 */

#include "cover.h"

#ifndef boolean_factor_h
#define boolean_factor_h

namespace boolean
{

struct factor
{
	const static int OR = 0;
	const static int AND = 1;

	factor();
	~factor();

	cover terms;
	vector<factor> subfactors;
	int op;

	factor &xfactor(cover c, int op);
	factor &hfactor(cover c);

	static float partition(cover c, cover &left, cover &right);
};

}

#endif
