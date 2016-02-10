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
 * file: CRNMultivariateRandomTools.h
 * \author Jean DUONG, Yann LEYDIER
 */

#ifndef CRNMULTIVARIATERANDOMTOOLS_HEADER
#define CRNMULTIVARIATERANDOMTOOLS_HEADER

#include <CRN.h>
#include <CRNMath/CRNMatrixDoublePtr.h>

namespace crn
{
	class MultivariateGaussianMixture;
	class MultivariateGaussianPDF;
	class SquareMatrixDouble;
	/****************************************************************************/
	/*! \brief Multivariate random toolbox class
	 *
	 * Tools for random samples
	 * 
	 * \author 	Jean DUONG
	 * \date	August 2008
	 * \version	0.1
	 * \ingroup math
	 */
	class MultivariateRandomTools
	{
		public:
			/*! \brief Creates a data sample following a Gaussian probability law */
			static MatrixDouble NewGaussianSample(const MatrixDouble& Mu, const SquareMatrixDouble& Sigma, size_t n = 1, size_t m = 100, bool reseed = true);
			/*! \brief Creates a data sample following a Gaussian probability law */
			static MatrixDouble NewGaussianSample(const MultivariateGaussianPDF& P, size_t n = 1, size_t m = 100, bool reseed = true);
			/*! \brief Creates a data sample following a Gaussian probability law */
			static MatrixDouble NewGaussianMixtureSample(const MultivariateGaussianMixture& Mx, size_t n = 1, size_t m = 100, bool reseed = true);
			
		private:
			/*! \brief Internal */
			static MatrixDouble makeMLTransform(const MatrixDouble& M, const SquareMatrixDouble& L, const MatrixDouble& X);

	};
}
#endif
