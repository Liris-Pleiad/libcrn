/* Copyright 2011-2014 CoReNum
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
 * file: CRNCartesian2DFunction.h
 * \author Yann LEYDIER
 */

#ifndef CRNCartesian2DFunction_HEADER
#define CRNCartesian2DFunction_HEADER

#include <CRNObject.h>

namespace crn
{
	/*! \brief Base class for 2D functions
	 *
	 * Base class for 2D functions
	 *
	 * \version 1.0
	 * \author	Yann LEYDIER
	 * \date	Oct 2011
	 * \ingroup	math
	 */
	class Cartesian2DFunction: public Object
	{
		public:
			virtual ~Cartesian2DFunction() override {}
			/*! \brief Gets ordinate at x */
			virtual double operator[](double x) const = 0;
			/*! \brief Gets ordinate at x */
			virtual int operator[](int x) const { return int(operator[](double(x))); }
	};

	CRN_ALIAS_SMART_PTR(Cartesian2DFunction)
}

#endif

