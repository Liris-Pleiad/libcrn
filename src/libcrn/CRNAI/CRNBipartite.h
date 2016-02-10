/* Copyright 2015 INSA-Lyon, Université Paris Descartes
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
 * file: CRNBipartite.h
 * \author Yann LEYDIER
 */

#ifndef CRNBipartite_HEADER
#define CRNBipartite_HEADER

#include <vector>
#include <tuple>

namespace crn
{
	class SquareMatrixDouble;

	/*! Kuhn-Munkres' algorithm */
	std::tuple<double, std::vector<std::pair<size_t, size_t>>> Hungarian(const std::vector<std::vector<double>> &distmat);
	/*! Kuhn-Munkres' algorithm */
	std::tuple<double, std::vector<std::pair<size_t, size_t>>> Hungarian(const SquareMatrixDouble &distmat);
}

#endif
