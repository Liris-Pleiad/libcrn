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
 * file: CRNAffinityPropagation.h
 * \author Yann LEYDIER
 */

#ifndef CRNAffinityPropagation_HEADER
#define CRNAffinityPropagation_HEADER

#include <vector>
#include <map>

namespace crn
{
	class SquareMatrixDouble;

	/*!@{
	 * \ingroup	cluster
	 */
	/*! \brief Strategies to limit the number of classes in affinity propagation */
	enum class AProClusters { MEDIUM, LOW };

	/*! \brief Computes clusters and their prototypes */
	std::pair<std::vector<size_t>, std::vector<size_t>> AffinityPropagation(const SquareMatrixDouble &distance_matrix, AProClusters nclusters, double damping = 0.5, size_t stable_iters_stop = 10, size_t max_iter = 100);
	/*! \brief Computes clusters and their prototypes */
	std::pair<std::vector<size_t>, std::vector<size_t>> AffinityPropagation(const SquareMatrixDouble &distance_matrix, double preference, double damping = 0.5, size_t stable_iters_stop = 10, size_t max_iter = 100);
	/*! \brief Computes clusters and their prototypes */
	std::pair<std::vector<size_t>, std::vector<size_t>> AffinityPropagation(const SquareMatrixDouble &distance_matrix, const std::vector<double> &preference, double damping = 0.5, size_t stable_iters_stop = 10, size_t max_iter = 100);
	/*!@}*/
}

#endif
