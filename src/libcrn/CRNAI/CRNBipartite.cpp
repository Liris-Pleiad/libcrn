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
 * file: CRNBipartite.cpp
 * \author Yann LEYDIER
 */

#include <CRNAI/CRNBipartite.h>
#include <CRNMath/CRNSquareMatrixDouble.h>
#include <CRNException.h>
#include <CRNStringUTF8.h>
#include <limits>
#include <algorithm>
#include <CRNi18n.h>

using namespace crn;
using namespace crn::literals;

struct kuhn_munkres
{
	kuhn_munkres(const std::vector<std::vector<double>> &dm):
		c(dm),
		n(dm.size()),
		row_covered(dm.size(), false),
		col_covered(dm.size(), false),
		path(dm.size() * 2, std::vector<size_t>(dm.size() * 2, 0)),
		marked(dm.size(), std::vector<uint8_t>(dm.size(), 0))
	{ }
	kuhn_munkres(const crn::SquareMatrixDouble &dm):
		c(dm),
		n(dm.GetRows()),
		row_covered(dm.GetRows(), false),
		col_covered(dm.GetRows(), false),
		path(dm.GetRows() * 2, std::vector<size_t>(dm.GetRows() * 2, 0)),
		marked(dm.GetRows(), std::vector<uint8_t>(dm.GetRows(), 0))
	{ }
	crn::SquareMatrixDouble c;
	const size_t n;
	std::vector<bool> row_covered, col_covered;
	size_t z0_r = 0, z0_c = 0;
	std::vector<std::vector<size_t>> path;
	std::vector<std::vector<uint8_t>> marked;
};

static inline void clear_covers(kuhn_munkres &km)
{
	//Clear all covered matrix cells
	std::fill(km.row_covered.begin(), km.row_covered.end(), false);
	std::fill(km.col_covered.begin(), km.col_covered.end(), false);
}

static inline std::pair<int, int> find_a_zero(const kuhn_munkres &km)
{
	// Find the first uncovered element with value 0
	for (int i = 0; i < int(km.n); ++i)
		for (int j = 0; j < int(km.n); ++j)
		{
			if ((km.c[i][j] == 0) && !km.row_covered[i] && !km.col_covered[j])
				return std::make_pair(i, j);
		}
	return std::make_pair(-1, -1);
}

static inline int find_star_in_row(const kuhn_munkres &km, size_t row)
{
	// Find the first starred element in the specified row. Returns the column index, or -1 if no starred element was found.
	for (int j = 0; j < int(km.n); ++j)
		if (km.marked[row][j] == 1)
			return j;
	return -1;
}

static inline int find_star_in_col(const kuhn_munkres &km, size_t col)
{
	// Find the first starred element in the specified row. Returns the row index, or -1 if no starred element was found.
	for (int i = 0; i < int(km.n); ++i)
		if (km.marked[i][col] == 1)
			return i;
	return -1;
}

static inline int find_prime_in_row(const kuhn_munkres &km, size_t row)
{
	// Find the first prime element in the specified row. Returns the column index, or -1 if no starred element was found.
	for (int j = 0; j < int(km.n); ++j)
		if (km.marked[row][j] == 2)
			return j;
	return -1;
}

static inline void convert_path(kuhn_munkres &km, size_t count)
{
	for (size_t i = 0; i <= count; ++i)
		if (km.marked[km.path[i][0]][km.path[i][1]] == 1)
			km.marked[km.path[i][0]][km.path[i][1]] = 0;
		else
			km.marked[km.path[i][0]][km.path[i][1]] = 1;
}

void inline erase_primes(kuhn_munkres &km)
{
	// Erase all prime markings
	for (int i = 0; i < int(km.n); ++i)
		for (int j = 0; j < int(km.n); ++j)
			if (km.marked[i][j] == 2)
				km.marked[i][j] = 0;
}

static inline double find_smallest(const kuhn_munkres &km)
{
	// Find the smallest uncovered value in the matrix.
	auto minval = std::numeric_limits<double>::max();
	for (size_t i = 0; i < km.n; ++i)
		for (size_t j = 0; j < km.n; ++j)
			if (!km.row_covered[i] && !km.col_covered[j])
				if (minval > km.c[i][j])
					minval = km.c[i][j];
	return minval;
}

static inline int step1(kuhn_munkres &km)
{
	// For each row of the matrix, find the smallest element and subtract it from every element in its row. Go to Step 2.
	for (size_t i = 0; i < km.n; ++i)
	{
		// Find the minimum value for this row and subtract that minimum from every element in the row.
		auto minval = *std::min_element(km.c[i], km.c[i] + km.c.GetCols());
		for (auto v = size_t(0); v < km.c.GetCols(); ++v)
			km.c[i][v] -= minval;
	}
	return 2;
}

static inline int step2(kuhn_munkres &km)
{
	// Find a zero (Z) in the resulting matrix. If there is no starred zero in its row or column, star Z. Repeat for each element in the matrix. Go to Step 3.
	for (size_t i = 0; i < km.n; ++i)
		for (size_t j = 0; j < km.n; ++j)
			if ((km.c[i][j] == 0) && !km.row_covered[i] && !km.col_covered[j])
			{
				km.marked[i][j] = 1;
				km.row_covered[i] = true;
				km.col_covered[j] = true;
			}

	clear_covers(km);
	return 3;
}

static inline int step3(kuhn_munkres &km)
{
	// Cover each column containing a starred zero. If K columns are covered, the starred zeros describe a complete set of unique assignments. In this case, Go to DONE, otherwise, Go to Step 4.
	auto count = size_t(0);
	for (size_t i = 0; i < km.n; ++i)
		for (size_t j = 0; j < km.n; ++j)
			if (km.marked[i][j] == 1)
			{
				km.col_covered[j] = true;
				count += 1;
			}
	if (count >= km.n)
		return 0;
	else
		return 4;
}

static inline int step4(kuhn_munkres &km)
{
	// Find a noncovered zero and prime it. If there is no starred zero in the row containing this primed zero, Go to Step 5. Otherwise, cover this row and uncover the column containing the starred zero. Continue in this manner until there are no uncovered zeros left. Save the smallest uncovered value and Go to Step 6.
	while (true)
	{
		auto rc = find_a_zero(km);
		if (rc.first < 0)
			return 6;
		km.marked[rc.first][rc.second] = 2;
		auto star_col = find_star_in_row(km, rc.first);
		if (star_col >= 0)
		{
			rc.second = star_col;
			km.row_covered[rc.first] = true;
			km.col_covered[rc.second] = false;
		}
		else
		{
			km.z0_r = rc.first;
			km.z0_c = rc.second;
			return 5;
		}
	}
}

static inline int step5(kuhn_munkres &km)
{
	// Construct a series of alternating primed and starred zeros as follows. Let Z0 represent the uncovered primed zero found in Step 4.  Let Z1 denote the starred zero in the column of Z0 (if any).  Let Z2 denote the primed zero in the row of Z1 (there will always be one). Continue until the series terminates at a primed zero that has no starred zero in its column. Unstar each starred zero of the series, star each primed zero of the series, erase all primes and uncover every line in the matrix. Return to Step 3
	auto count = size_t(0);
	km.path[count][0] = km.z0_r;
	km.path[count][1] = km.z0_c;
	while (true)
	{
		auto row = find_star_in_col(km, km.path[count][1]);
		if (row >= 0)
		{
			count += 1;
			km.path[count][0] = row;
			km.path[count][1] = km.path[count-1][1];
		}
		else
			break;

		auto col = find_prime_in_row(km, km.path[count][0]);
		count += 1;
		km.path[count][0] = km.path[count-1][0];
		km.path[count][1] = col;
	}
	convert_path(km, count);
	clear_covers(km);
	erase_primes(km);
	return 3;
}

static inline int step6(kuhn_munkres &km)
{
	// Add the value found in Step 4 to every element of each covered row, and subtract it from every element of each uncovered column.  Return to Step 4 without altering any stars, primes, or covered lines.
	auto minval = find_smallest(km);
	for (size_t i = 0; i < km.n; ++i)
		for (size_t j = 0; j < km.n; ++j)
		{
			if (km.row_covered[i])
				km.c[i][j] += minval;
			if (!km.col_covered[j])
				km.c[i][j] -= minval;
		}
	return 4;
}

template<typename T> std::tuple<double, std::vector<std::pair<size_t, size_t>>> hung(const T &distmat)
{
	auto km = kuhn_munkres(distmat);
	auto step = 1;
	while (step)
	{
		switch (step)
		{
			case 1:
				step = step1(km);
				break;
			case 2:
				step = step2(km);
				break;
			case 3:
				step = step3(km);
				break;
			case 4:
				step = step4(km);
				break;
			case 5:
				step = step5(km);
				break;
			case 6:
				step = step6(km);
				break;
		}
	}
	auto pairs = std::vector<std::pair<size_t, size_t>>{};
	auto cost = 0.0;
	// Look for the starred columns
	for (size_t i = 0; i < km.n; ++i)
		for (size_t j = 0; j < km.n; ++j)
			if (km.marked[i][j] == 1)
			{
				pairs.emplace_back(i, j);
				cost += distmat[i][j];
			}

	return std::make_tuple(cost, std::move(pairs));
}

/*! Kuhn-Munkres' algorithm
 *
 * Adapted from Brian M. Clapper's python implementation "munkres-1.0.7" (BSD license)
 * \ingroup	ai
 * \throws	ExceptionInvalidArgument	invalid distance matrix
 * \param[in]	distmat	a distance matrix
 * \return	the total cost and the list of pair indices
 */
std::tuple<double, std::vector<std::pair<size_t, size_t>>> crn::Hungarian(const std::vector<std::vector<double>> &distmat)
{
	if (distmat.empty())
		throw ExceptionInvalidArgument(u8"Hugarian(): "_s + _("empty distance matrix."));
	for (const auto &row : distmat)
		if (row.size() != distmat.size())
			throw ExceptionInvalidArgument(u8"Hugarian(): "_s + _("the distance matrix is not square."));
	return hung(distmat);
}

/*! Kuhn-Munkres' algorithm
 *
 * Adapted from Brian M. Clapper's python implementation "munkres-1.0.7" (BSD license)
 * \ingroup	ai
 * \throws	ExceptionInvalidArgument	invalid distance matrix
 * \param[in]	distmat	a distance matrix
 * \return	the total cost and the list of pair indices
 */
std::tuple<double, std::vector<std::pair<size_t, size_t>>> crn::Hungarian(const SquareMatrixDouble &distmat)
{
	return hung(distmat);
}


