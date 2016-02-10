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
 * file: CRNUnivariateRandomTools.h
 * \author Jean DUONG, Yann LEYDIER
 */

#ifndef CRNUNIVARIATERANDOMTOOLS_HEADER
#define CRNUNIVARIATERANDOMTOOLS_HEADER

#include <vector>

#include <CRN.h>
#include <CRNMath/CRNMatrixDoublePtr.h>

namespace crn
{
	class UnivariateGaussianMixture;
	/****************************************************************************/
	/*! \brief Univariate random toolbox class
	 *
	 * Tools for random samples
	 * 
	 * \author 	Jean DUONG
	 * \date	August 2008
	 * \version	0.2
	 * \ingroup	math
	 */
	namespace UnivariateRandomTools
	{
		/*! \brief Creates a data sample following an uniform probability law */
		std::vector<double> NewUniformSample(size_t n = 1, bool reseed = true);
		/*! \brief Creates a data sample following a Gaussian probability law */
		std::vector<double> NewGaussianSample(double mu = 0.0, double sigma = 1.0, size_t n = 1, size_t m = 100, bool reseed = true);
		/*! \brief Creates a data sample following a Gaussian probability law */
		std::vector<double> NewGaussianMixtureSample(const UnivariateGaussianMixture& Mx, size_t n = 1, size_t m = 100, bool reseed = true);
	};
}
#endif
