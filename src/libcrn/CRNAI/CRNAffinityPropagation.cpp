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
 * file: CRNAffinityPropagation.cpp
 * \author Yann LEYDIER
 */

#include <CRNAI/CRNAffinityPropagation.h>
#include <CRNMath/CRNSquareMatrixDouble.h>
#include <CRNException.h>
#include <CRNi18n.h>

using namespace crn;

static std::pair<std::vector<size_t>, std::vector<size_t>> affinityPropagation(const crn::SquareMatrixDouble &s, double damping, size_t stable_iters_stop, size_t max_iter)
{
	if ((damping < 0.0) || (damping >= 1.0))
		throw crn::ExceptionDomain(_("The damping must be in [0, 1[."));
	if (stable_iters_stop <= 1)
		throw crn::ExceptionDomain(_("The number of stable iterations to stop must be >1."));
	if (max_iter <= 1)
		throw crn::ExceptionDomain(_("The maximal number of iterations must be >1."));

	const auto N = s.GetRows();

	// main loop
	auto r = crn::SquareMatrixDouble{N};
	auto a = crn::SquareMatrixDouble{N};
	auto identical = size_t(0);
	auto clusters = std::vector<size_t>(N, 0);
	for (auto cnt = size_t(0); cnt < max_iter; ++cnt)
	{
		// update responsibility
		for (auto i = size_t(0); i < N; ++i)
			for (auto k = size_t(0); k < N; ++k)
			{
				auto m = -std::numeric_limits<double>::max();
				for (auto kp = size_t(0); kp < N; ++kp)
					if (kp != k)
					{
						const auto v = a[i][kp] - s[i][kp];
						if (v > m)
							m = v;
					}
				r[i][k] = damping * r[i][k] + (1.0 - damping) * (-s[i][k] - m);
			}
		// update availability
		for (auto i = size_t(0); i < N; ++i)
			for (auto k = size_t(0); k < N; ++k)
			{
				if (i == k)
				{
					auto v = 0.0;
					for (auto ip = size_t(0); ip < N; ++ip)
						if (ip != k)
							v += crn::Max(0.0, r[ip][k]);
					a[i][k] = damping * a[i][k] + (1.0 - damping) * v;
				}
				else
				{
					auto v = r[k][k];
					for (auto ip = size_t(0); ip < N; ++ip)
						if ((ip != i) && (ip != k))
							v += crn::Max(0.0, r[ip][k]);
					a[i][k] = damping * a[i][k] + (1.0 - damping) * crn::Min(0.0, v);
				}
			}

		// compute clusters
		auto newclusters = std::vector<size_t>(N, 0);
		for (auto i = size_t(0); i < N; ++i)
		{
			auto c = size_t(0);
			auto maxval = -std::numeric_limits<double>::max();
			for (auto k = size_t(0); k < N; ++k)
			{
				const auto val = r[i][k] + a[i][k];
				if (val > maxval)
				{
					maxval = val;
					c = k;
				}
			}
			newclusters[i] = c;
		}

		// check if there were changes
		if (clusters == newclusters)
			identical += 1;
		else
		{
			clusters.swap(newclusters);
			identical = 0;
		}
		if (identical >= stable_iters_stop)
			break;
	} // main loop
	
	auto protos = std::vector<size_t>{};
	for (auto i = size_t(0); i < N; ++i)
		if (clusters[i] == i)
			protos.push_back(i);
	return std::make_pair(std::move(protos), std::move(clusters));
}

/*! Computes clusters and their prototypes 
 * \param[in]	distance_matrix	the distance matrix of the elements to cluster
 * \param[in]	nclusters	the strategy to limit the number of clusters
 * \param[in]	damping	the damping rate. 0 = no damping, default = 0.5. The higher the value the slower the algorithm will converge. Low values may result in oscillations.
 * \param[in]	stable_iters_stop	number of consecutive identical clusterings to stop the algorithm (default = 10).
 * \param[in]	max_iter	maximum number of iterations (default = 100).
 * \return	a pair containing in first the indexes of the cluster prototypes and in second the cluster number for each element.
 */
std::pair<std::vector<size_t>, std::vector<size_t>> crn::AffinityPropagation(const crn::SquareMatrixDouble &distance_matrix, AProClusters nclusters, double damping, size_t stable_iters_stop, size_t max_iter)
{
	const auto N = distance_matrix.GetRows();

	// create similarity matrix
	auto s = distance_matrix;
	auto diag = 0.0;
	if (nclusters == AProClusters::MEDIUM)
	{ // pick median value
		auto vect = s.Std();
		std::sort(vect.begin(), vect.end());
		diag = vect[(vect.size() + N) / 2]; // N first values are 0.0, do not count them
	}
	else //if (nclusters == AProClusters::LOW)
	{
		diag = s.GetMax();
	}
	for (auto tmp = size_t(0); tmp < N; ++tmp)
		s[tmp][tmp] = diag;
	s *= -1;

	return affinityPropagation(s, damping, stable_iters_stop, max_iter);
}

/*! Computes clusters and their prototypes 
 * \param[in]	distance_matrix	the distance matrix of the elements to cluster
 * \param[in]	preference	the "preference" value that drives the number of cluster (e.g: mean, median or max distance). The higher the value, the smaller the number of clusters.
 * \param[in]	damping	the damping rate. 0 = no damping, default = 0.5. The higher the value the slower the algorithm will converge. Low values may result in oscillations.
 * \param[in]	stable_iters_stop	number of consecutive identical clusterings to stop the algorithm (default = 10).
 * \param[in]	max_iter	maximum number of iterations (default = 100).
 * \return	a pair containing in first the indexes of the cluster prototypes and in second the cluster number for each element.
 */
std::pair<std::vector<size_t>, std::vector<size_t>> crn::AffinityPropagation(const crn::SquareMatrixDouble &distance_matrix, double preference, double damping, size_t stable_iters_stop, size_t max_iter)
{
	// create similarity matrix
	auto s = distance_matrix;
	for (auto tmp = size_t(0); tmp < s.GetRows(); ++tmp)
		s[tmp][tmp] = preference;
	s *= -1;
	return affinityPropagation(s, damping, stable_iters_stop, max_iter);
}

/*! Computes clusters and their prototypes 
 * \param[in]	distance_matrix	the distance matrix of the elements to cluster
 * \param[in]	preference	the "preference" values that tells which elements are more likely to be cluster prototypes and that drives the number of cluster. The higher the values, the smaller the number of clusters.
 * \param[in]	damping	the damping rate. 0 = no damping, default = 0.5. The higher the value the slower the algorithm will converge. Low values may result in oscillations.
 * \param[in]	stable_iters_stop	number of consecutive identical clusterings to stop the algorithm (default = 10).
 * \param[in]	max_iter	maximum number of iterations (default = 100).
 * \return	a pair containing in first the indexes of the cluster prototypes and in second the cluster number for each element.
 */
std::pair<std::vector<size_t>, std::vector<size_t>> crn::AffinityPropagation(const crn::SquareMatrixDouble &distance_matrix, const std::vector<double> &preference, double damping, size_t stable_iters_stop, size_t max_iter)
{
	if (distance_matrix.GetRows() != preference.size())
		throw crn::ExceptionDimension{_("The preference is not the same dimension as the distance matrix.")};
	// create similarity matrix
	auto s = distance_matrix;
	for (auto tmp = size_t(0); tmp < s.GetRows(); ++tmp)
		s[tmp][tmp] = preference[tmp];
	s *= -1;
	return affinityPropagation(s, damping, stable_iters_stop, max_iter);
}

