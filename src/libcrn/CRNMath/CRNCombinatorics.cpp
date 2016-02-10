/* Copyright 2008-2014 INSA Lyon
 * 
 * This file is part of libcrn.
 * 
 * libcrn is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libcrn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with libcrn.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * file: CRNCombinatorics.cpp
 * \author Jean DUONG
 */

#include <CRNMath/CRNCombinatorics.h>

using namespace crn;

/*! Counts combinations of p objects from n: \f$C_n^p\f$
 *
 * \param[in]	n	total number of objects
 * \param[in]	p	number of objects to choose
 *
 * \return	number of combinations of p objects from n
 */
int Combinatorics::CountCombinations(int n, int p) noexcept
{	
	if ((p == 0) || (p == n))
	{
		return 1;
	}
	
	if (p == 1)
	{
		return n;
	}
	
	return (CountCombinations(n - 1, p - 1) + CountCombinations(n - 1, p));
}

/*! Counts arrangements of p objects from n: \f$A_n^p\f$
 *
 * \param[in]	n	total number of objects
 * \param[in]	p	number of objects to choose
 *
 * \return	number arrangements of p objects from n
 */
int Combinatorics::CountArrangements(int n, int p) noexcept
{
	if (p > n)
	{
		return 0;
	}

	int Arr = 1;
	
	for (int k = n - p + 1; k <= n; k++)
	{
		Arr *= k;
	}
	
	return Arr;
}

