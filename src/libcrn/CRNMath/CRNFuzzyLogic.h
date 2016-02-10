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
 * file: CRNFuzzyLogic.h
 * \author Jean DUONG
 */

#ifndef CRNFUZZYLOGIC_HEADER
#define CRNFUZZYLOGIC_HEADER

#include <CRN.h>

namespace crn
{
	class RealToRealFunction;

	/*! \brief Fuzzy logic toolbox
	 *
	 * Tools for fuzzy logic
	 * 
	 * \author 	Jean DUONG
	 * \date	January 2009
	 * \version	0.2
	 * \ingroup	math
	 */
	namespace FuzzyLogic
	{
		/*! \brief Computes the compatibility function between a regular function and a approximated function */
		RealToRealFunction MakeCompatibility(const RealToRealFunction &f, const RealToRealFunction &g, size_t nbBins);
		/*! \brief Defuzzification over an interval using center of gravity method */
		double DefuzzificationCOG(double min, double max, const RealToRealFunction& f);
	};
}

#endif

