/* Copyright 2013-2016 INSA-Lyon, ENS-Lyon
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
 * file: CRN2Means.h
 * \author Yann LEYDIER
 */

#ifndef CRN2MEANS_HEADER
#define CRN2MEANS_HEADER

#include <CRNType.h>
#include <CRNMath/CRNMath.h>
#include <type_traits>

namespace crn
{
	/****************************************************************************/
	/*! \brief 2-means clustering algorithm for scalar data
	 *
	 * 2-means clustering utility for scalar data.
	 *
	 * \throws	ExceptionDomain	the range is empty
	 * \param[in]	beg	iterator on the first element
	 * \param[in]	en	iterator after the last element
	 * \param[in]	stop_crit	the maximal difference between two iterations to stop the algorithm
	 *
	 * \author 	Yann LEYDIER
	 * \date		Jul 2013
	 * \version 0.1
	 * \ingroup cluster
	 */
	template<
		typename ITER, 
		typename std::enable_if<
			// can copy
			std::is_copy_assignable<typename std::iterator_traits<ITER>::value_type>::value &&
			// can assign 0
			std::is_assignable<typename std::add_lvalue_reference<typename std::decay<typename std::iterator_traits<ITER>::value_type>::type>::type, int>::value && 
			// operator<
			protocol::HasLT<typename std::iterator_traits<ITER>::value_type>::value && 
			// operators +, -, *(double)
			IsVectorOverR<typename std::iterator_traits<ITER>::value_type>::value ,
			int>::type = 0
		>
		std::pair<typename std::iterator_traits<ITER>::value_type, typename std::iterator_traits<ITER>::value_type> 
		TwoMeans(ITER beg, ITER en, double stop_crit = 0.00001)
	{
		using datatype = typename std::iterator_traits<ITER>::value_type;
		using dectype = DecimalType<datatype>;
		if (beg == en)
			throw ExceptionDomain("Empty range");
		// set prototypes as min & max
		datatype p1 = *beg;
		datatype p2 = *beg;
		for (ITER it = beg; it != en; ++it)
		{
			if (*it < p1) p1 = *it;
			if (!(*it < p2)) p2 = *it;
		}
		// run 2 means
		bool cont = true;
		while (cont)
		{
			dectype s1 = 0, s2 = 0;
			size_t n1 = 0, n2 = 0;
			for (ITER it = beg; it != en; ++it)
			{
				if (Abs(*it - p1) < Abs(*it - p2))
				{
					s1 += *it;
					n1 += 1;
				}
				else
				{
					s2 += *it;
					n2 += 1;
				}
			}
			if (n1)
				s1 = datatype(s1 * (1.0 / double(n1)));
			if (n2)
				s2 = datatype(s2 * (1.0 / double(n2)));
			cont = double(Abs(dectype(p1) - s1) + Abs(dectype(p2) - s2)) > stop_crit;
			p1 = datatype(s1);
			p2 = datatype(s2);
		}
		return std::make_pair(p1, p2);
	}

	/****************************************************************************/
	/*! \brief 2-means clustering algorithm for scalar data
	 *
	 * 2-means clustering utility for scalar data.
	 *
	 * \throws	ExceptionDomain	the range is empty
	 * \param[in]	beg	iterator on the first element
	 * \param[in]	en	iterator after the last element
	 * \param[in]	p1	first prototype
	 * \param[in]	p2	first prototype
	 * \param[in]	stop_crit	the maximal difference between two iterations to stop the algorithm
	 *
	 * \author 	Yann LEYDIER
	 * \date		Jul 2013
	 * \version 0.1
	 * \ingroup cluster
	 */
	template<typename ITER> 
		std::pair<typename std::iterator_traits<ITER>::value_type, typename std::iterator_traits<ITER>::value_type> 
		TwoMeans(ITER beg, ITER en, typename std::iterator_traits<ITER>::value_type p1, typename std::iterator_traits<ITER>::value_type p2, double stop_crit = 0.00001)
			noexcept(std::is_nothrow_constructible<typename std::iterator_traits<ITER>::value_type>::value 
					&& std::is_nothrow_copy_assignable<typename std::iterator_traits<ITER>::value_type>::value)
	{
		using datatype = typename std::iterator_traits<ITER>::value_type;
		using dectype = DecimalType<datatype>;
		if (beg == en)
			throw ExceptionDomain("Empty range");
		// run 2 means
		bool cont = true;
		while (cont)
		{
			dectype s1 = 0, s2 = 0;
			size_t n1 = 0, n2 = 0;
			for (ITER it = beg; it != en; ++it)
			{
				if (Abs(*it - p1) < Abs(*it - p2))
				{
					s1 += *it;
					n1 += 1;
				}
				else
				{
					s2 += *it;
					n2 += 1;
				}
			}
			if (n1)
				s1 = datatype(s1 / double(n1));
			if (n2)
				s2 = datatype(s2 / double(n2));
			cont = double(Abs(dectype(p1) - s1) + Abs(dectype(p2) - s2)) > stop_crit;
			p1 = datatype(s1);
			p2 = datatype(s2);
		}
		return std::make_pair(p1, p2);
	}
}

#endif

