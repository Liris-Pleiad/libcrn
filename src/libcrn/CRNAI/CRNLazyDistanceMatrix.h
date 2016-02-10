/* Copyright 2015 Université Paris Descartes
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
 * file: CRNLazyDistanceMatrix.h
 * \author Yann LEYDIER
 */

#ifndef CRNLazyDistanceMatrix_HEADER
#define CRNLazyDistanceMatrix_HEADER

#include <CRNMath/CRNSquareMatrixDouble.h>
#include <vector>

namespace crn
{
	/*! \brief	Lazy computation of a distance matrix
	 *
	 * Computes the elements of the distance matrix only when needed.
	 * \author	Yann LEYDIER
	 * \version	0.1
	 * \date	June 2015
	 * \ingroup	cluster
	 */
	template<typename DATA, typename DISTANCE> class LazyDistanceMatrix
	{
		public:
			/*!
			 * \param[in]	d	the population
			 * \param[in]	dist	the distance function
			 */
			LazyDistanceMatrix(const std::vector<DATA> &d, DISTANCE &&dist):
				data(d), 
				distance(std::forward<DISTANCE>(dist)),
				distmat(d.size()),
			 	cache(d.size(), std::vector<bool>(d.size(), false))
			{ }
			LazyDistanceMatrix(const LazyDistanceMatrix&) = default;
			LazyDistanceMatrix(LazyDistanceMatrix&&) = default;
			LazyDistanceMatrix& operator=(const LazyDistanceMatrix&) = delete;
			LazyDistanceMatrix& operator=(LazyDistanceMatrix&&) = delete;

			/*! \brief Returns the number of rows.
			 * \return		The number of rows.
			 */
			inline size_t GetRows() const noexcept { return data.size(); }

			/*! \brief Returns the number of columns.
			 * \return		The number of columns.
			 */
			inline size_t GetCols() const noexcept { return data.size(); }

			/*! \brief	Distance between two elements
			 * \warning	no bound check is performed on indices
			 * \param[in]	i	first element index in population
			 * \param[in]	j	second element index in population
			 * \return	the distance
			 */
			inline double At(size_t i, size_t j)
			{
				if (!cache[i][j])
				{
					distmat[i][j] = distmat[j][i] = distance(data[i], data[j]);
					cache[i][j] = cache[j][i] = true;
				}
				return distmat[i][j];
			}

			/*! \brief	Access to the whole distance matrix
			 * \return	the fully computed distance matrix
			 */
			const SquareMatrixDouble& GetDistanceMatrix()
			{
				for (size_t i = 0; i < data.size(); ++i)
					for (size_t j = i + 1; j < data.size(); ++j)
						if (!cache[i][j])
							At(i, j);
				return distmat;
			}

		private:
			SquareMatrixDouble distmat;
			const std::vector<DATA> &data;
			DISTANCE distance;
			std::vector<std::vector<bool>> cache;
	};

	/*! \brief	Helper to avoid typing long type names
	 * \param[in]	d	the population
	 * \param[in]	dist	the distance function
	 * \return	a distance matrix with lazy computation of distances
	 * \ingroup	cluster
	 */
	template<typename DATA, typename DISTANCE> inline LazyDistanceMatrix<DATA, DISTANCE> MakeLazyDistanceMatrix(const std::vector<DATA> &d, DISTANCE &&dist) { return LazyDistanceMatrix<DATA, DISTANCE>{d, std::forward<DISTANCE>(dist)}; }
}

#endif

