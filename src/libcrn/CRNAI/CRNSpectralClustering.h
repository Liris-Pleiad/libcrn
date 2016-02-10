/* Copyright 2012-2014 INSA Lyon, CoReNum
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
 * file: CRNSpectralClustering.h
 * \author Yann LEYDIER
 */

#ifndef CRNSpectralClustering_HEADER
#define CRNSpectralClustering_HEADER

#include <CRN.h>
#include <CRNMath/CRNMatrixDoublePtr.h>
#include <vector>
#include <map>
#include <limits>

namespace crn
{
	class SpectralClustering;
	CRN_ALIAS_SMART_PTR(SpectralClustering)
}
namespace crn
{
	class SquareMatrixDouble;
	/****************************************************************************/
	/*! \brief Spectral clustering
	 *
	 * Spectral clustering using Ng, Jordan & Weiss formula
	 *
	 * \author 	Yann LEYDIER
	 * \date		September 2012
	 * \version 0.1
	 * \ingroup cluster
	 */
	class SpectralClustering
	{
		public:
			SpectralClustering(const SpectralClustering&) = delete;
			SpectralClustering(SpectralClustering&&) = default;
			SpectralClustering& operator=(const SpectralClustering&) = delete;
			SpectralClustering& operator=(SpectralClustering&&) = default;

			/*! \brief Clustering with local automatic scale */
			static SpectralClustering CreateLocalScaleFromNN(const SquareMatrixDouble &distance_matrix, size_t sigma_neighborhood = 7, double epsilon = std::numeric_limits<double>::max());
			/*! \brief Clustering with global automatic scale */
			static SpectralClustering CreateGlobalScaleFromDimension(const SquareMatrixDouble &distance_matrix, size_t dimension, double epsilon = std::numeric_limits<double>::max());
			/*! \brief Clustering with global automatic scale */
			static SpectralClustering CreateGlobalScaleFromNN(const SquareMatrixDouble &distance_matrix, size_t sigma_neighborhood = 1, double epsilon = std::numeric_limits<double>::max());
			/*! \brief Clustering with global fixed scale */
			static SpectralClustering CreateFixedScale(const SquareMatrixDouble &distance_matrix, double sigma, double epsilon = std::numeric_limits<double>::max());
			
			/*! \brief Gets the data projected on each coordinates (higher eigenvalues are associated the to most significant coordinates) */
			const std::multimap<double, MatrixDouble>& GetEigenpairs() const noexcept { return eigenpairs; }
			/*! \brief Returns the eigenvalues (sorted from highest to lowest) */
			std::vector<double> GetEigenvalues() const;
			/*! \brief Estimates the number of clusters */
			size_t EstimateClusterNumber(double limit = 1.0) const;
			/*! \brief Projects the data on the first coordinates */
			std::vector<std::vector<double>> ProjectData(size_t ncoordinates, bool normalize) const;

		private:
			/*! \brief Computes the projection */
			SpectralClustering(const SquareMatrixDouble &w);

			std::multimap<double, MatrixDouble> eigenpairs; /*!< the projected data */
	};
}

#endif
