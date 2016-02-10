/* Copyright 2014-2015 INSA-Lyon, Université Paris Descartes
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
 * file: CRNOutliers.h
 * \author Yann LEYDIER
 */

#ifndef CRNOutliers_HEADER
#define CRNOutliers_HEADER

#include <CRNException.h>
#include <CRNMath/CRNMath.h>
#include <vector>
#include <map>
#include <set>
#include <limits>

namespace crn
{
	class SquareMatrixDouble;

	/*! \brief Compute the Local Outlier Factor for each element from the distance matrix */
	std::vector<double> ComputeLOF(const SquareMatrixDouble &distmat, size_t k);
	/*! \brief Compute the Local Outlier Factor for each element from the distance matrix */
	std::vector<double> ComputeLOF(const std::vector<std::vector<double>> &distmat, size_t k);

	/*! \brief compute the local outlier probability for each element from the distance matrix */
	std::vector<double> ComputeLoOP(const SquareMatrixDouble &distmat, size_t k, double lambda);
	/*! \brief compute the local outlier probability for each element from the distance matrix */
	std::vector<double> ComputeLoOP(const std::vector<std::vector<double>> &distmat, size_t k, double lambda);

	/*! \brief Outlier E statistics of a set of angles (Mardia, Statistics of directional data (with discussion), 1975)
	 *
	 * The lower the value, the most outlying is the sample.
	 *
	 * \throws	ExceptionDomain	empty set of angles
	 * \param[in]	beg	iterator on the first angle
	 * \param[in]	en	iterator after the last angle
	 * \return	the E statistics of the set
	 */
	template<typename ITER> std::vector<double> AngularOutliersE(ITER beg, ITER en)
	{
		if (beg == en)
			throw ExceptionDomain("AngularOutliersE(): empty set of angles.");
		auto c = 0.0, s = 0.0;
		for (auto it = beg; it != en; ++it)
		{
			c += Cos(*it);
			s += Sin(*it);
		}
		auto e = std::vector<double>(std::distance(beg, en));
		const auto n = double(e.size());
		const auto cr = 1.0 - sqrt(Sqr(c / n) + Sqr(s / n));
		auto cnt = size_t(0);
		for (auto it = beg; it != en; ++it, ++cnt)
			e[cnt] = (1.0 - sqrt(Sqr((c - Cos(*it)) / (n - 1)) + Sqr((s - Sin(*it)) / (n - 1)))) / (cr);
		return e;
	}

	/*! \brief Outlier C statistics of a set of angles (Collett, Outliers in circular data. Appl. Statist., 29, 50-57., 1980)
	 *
	 * The higher the value, the most outlying is the sample.
	 *
	 * \throws	ExceptionDomain	empty set of angles
	 * \param[in]	beg	iterator on the first angle
	 * \param[in]	en	iterator after the last angle
	 * \return	the C statistics of the set
	 */
	template<typename ITER> std::vector<double> AngularOutliersC(ITER beg, ITER en)
	{
		if (beg == en)
			throw ExceptionDomain("AngularOutliersC(): empty set of angles.");
		auto c = 0.0, s = 0.0;
		for (auto it = beg; it != en; ++it)
		{
			c += Cos(*it);
			s += Sin(*it);
		}
		auto C = std::vector<double>(std::distance(beg, en));
		const auto n = double(C.size());
		const auto r = sqrt(Sqr(c / n) + Sqr(s / n));
		auto cnt = size_t(0);
		for (auto it = beg; it != en; ++it, ++cnt)
			C[cnt] = sqrt(Sqr((c - Cos(*it)) / (n - 1)) + Sqr((s - Sin(*it)) / (n - 1))) / r;
		return C;
	}

}

#endif
