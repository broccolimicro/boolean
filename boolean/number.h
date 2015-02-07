/*
 * number.h
 *
 *  Created on: Feb 6, 2015
 *      Author: nbingham
 */

#include "cover.h"

#ifndef boolean_number_h
#define boolean_number_h

namespace boolean
{
struct number
{
	number();
	number(int width, int offset = 0);
	number(const number &n);
	~number();

	vector<cover> bits;
	cover sign;

	number &simplify();
	number &sext(int s);

	number &operator=(number n);
};

}

boolean::number operator&(boolean::number n0, boolean::number n1);
boolean::number operator|(boolean::number n0, boolean::number n1);
boolean::number operator~(boolean::number n);

boolean::number operator&(boolean::number n, boolean::cover c);
boolean::number operator|(boolean::number n, boolean::cover c);

boolean::number operator&(boolean::cover c, boolean::number n);
boolean::number operator|(boolean::cover c, boolean::number n);

boolean::number operator<<(boolean::number n, int s);
boolean::number operator>>(boolean::number n, int s);

boolean::number operator-(boolean::number n);
boolean::number operator+(boolean::number n0, boolean::number n1);
boolean::number operator-(boolean::number n0, boolean::number n1);
boolean::number operator*(boolean::number n0, boolean::number n1);
boolean::number operator/(boolean::number n0, boolean::number n1);

boolean::cover operator<(boolean::number n0, boolean::number n1);

#endif
