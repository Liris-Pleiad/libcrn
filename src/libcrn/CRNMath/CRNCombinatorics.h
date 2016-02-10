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
 * file: CRNCombinatorics.h
 * \author Jean DUONG
 */

#ifndef CRNCOMBINATORICS_HEADER
#define CRNCOMBINATORICS_HEADER

#include <CRN.h>
#include <vector>

namespace crn
{
	/****************************************************************************/
	/*! \brief Combinatorics toolbox
	 *
	 * Tools for combinatorics
	 * 
	 * \author 	Jean DUONG
	 * \date	March 2008
	 * \version	0.1
	 * \ingroup	math
	 */
	namespace Combinatorics
	{
		/*! \brief \f$C_n^p\f$ */
		int CountCombinations(int n, int p) noexcept;
		/*! \brief \f$A_n^p\f$ */
		int CountArrangements(int n, int p) noexcept;
		
		/*! Get minimal elements of a collection given an order relation */
		template <class ITER, class CMP> std::vector<typename std::iterator_traits<ITER>::value_type> Minimals(ITER i_begin, ITER i_end, CMP comp = std::less<typename std::iterator_traits<ITER>::value_type>())
		{			
			std::vector<typename std::iterator_traits<ITER>::value_type> minimals;
			
			for (ITER i = i_begin; i != i_end; ++i)
			{
				ITER j = i_begin;
				bool is_minimal = true;
				
				while ((j != i_end) && is_minimal)
				{
					if (j != i)
					{
						if(comp(*j, *i))
						{
							is_minimal = false;
						}
						else
						{
							++j;
						}
					}
					else
					{
						++j;
					}
				}
				
				if (is_minimal)
				{
					minimals.push_back(*i);
				}
			}
			
			return minimals;
		}
	}
}

#endif
