/* Copyright 2014 CoReNum
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
 * file: CRNkMedoids.cpp
 * \author Yann LEYDIER
 */

#include <CRNAI/CRNkMedoids.h>
#include <CRNMath/CRNMath.h>
#include <algorithm>
#include <numeric> // for accumulate

using namespace crn;
using namespace kmedoids;

std::vector<size_t> init::Central::operator()(const std::vector<std::vector<double>> &distmat) const
{
	const auto nelem = distmat.size();

	// sum on each line
	auto lsum = std::vector<double>(nelem);
	std::transform(distmat.begin(), distmat.end(), lsum.begin(), 
			[](const std::vector<double> &l){ return std::accumulate(l.begin(), l.end(), 0.0); });

	// v
	auto v = std::vector<double>(nelem, 0.0);
	for (size_t j = 0; j < nelem; ++j)
		for (size_t i = 0; i < nelem; ++i)
			v[j] += distmat[i][j] / lsum[i];

	// initial medoids
	auto medoids = std::vector<size_t>(k);
	for (size_t tmp = 0; tmp < k; ++tmp)
	{
		auto minit = std::min_element(v.begin(), v.end());
		medoids[tmp] = minit - v.begin();
		*minit = std::numeric_limits<double>::max();
	}

	return medoids;
}

std::vector<size_t> init::PAM::operator()(const std::vector<std::vector<double>> &distmat) const
{
	const auto nelem = distmat.size();

	// sum on each line
	auto lsum = std::vector<double>(nelem);
	std::transform(distmat.begin(), distmat.end(), lsum.begin(), 
			[](const std::vector<double> &l){ return std::accumulate(l.begin(), l.end(), 0.0); });

	auto medoids = std::vector<size_t>(k);
	// choose the element with the lowest distance to others
	medoids[0] = std::min_element(lsum.begin(), lsum.end()) - lsum.begin();

	for (size_t m = 1; m < k; ++m)
	{
		// gain to add each element
		auto g = std::vector<double>(nelem, 0.0);
		for (size_t i = 0; i < nelem; ++i)
		{
			// check if already a medoid
			auto found = false;
			for (size_t tmp = 0; tmp < m; ++tmp)
				if (medoids[tmp] == i) { found = true; break; }
			if (found) continue;
			for (size_t j = 0; j < nelem; ++j)
			{
				// look for closest medoid
				auto ndist = std::numeric_limits<double>::max();
				for (size_t tmp = 0; tmp < m; ++tmp)
					if (distmat[j][medoids[tmp]] < ndist)
					{
						ndist = distmat[j][medoids[tmp]];
					}
				g[i] += crn::Max(ndist - distmat[j][i], 0.0);
			}
		}
		medoids[m] = std::max_element(g.begin(), g.end()) - g.begin();
	}
	return medoids;
}

void update::Local::operator()(std::vector<size_t> &medoids, const std::vector<std::multimap<double, size_t>> &clusters, const std::vector<std::vector<double>> &distmat) const
{
	for (size_t m = 0; m < medoids.size(); ++m)
	{
		auto ndist = std::numeric_limits<double>::max();
		auto nobj = size_t{0};
		for (const auto &o : clusters[m])
		{
			auto sdist = 0.0;
			for (const auto &n : clusters[m])
			{
				sdist += distmat[n.second][o.second];
			}
			if (sdist < ndist)
			{
				ndist = sdist;
				nobj = o.second;
			}
		}
		medoids[m] = nobj;
	}
}

void update::PAM::operator()(std::vector<size_t> &medoids, const std::vector<std::multimap<double, size_t>> &clusters, const std::vector<std::vector<double>> &distmat) const
{
	size_t mini = 0, minh = 0;
	auto mintih = std::numeric_limits<double>::max();
	for (size_t i = 0; i < medoids.size(); ++i)
	{
		for (size_t c = 0; c < clusters.size(); ++c)
			for (const auto &h : clusters[c])
			{
				// check if this is the medoid of the cluster
				if (h.second == medoids[c]) continue;
				auto tih = 0.0;
				for (size_t tc = 0; tc < clusters.size(); ++tc)
					for (const auto &j : clusters[tc])
					{
						if (distmat[medoids[i]][j.second] > j.first) // j is not in class i
							tih += crn::Min(distmat[j.second][h.second] - j.first, 0.0);
						else
						{ // j is in class i
							auto ndist = std::numeric_limits<double>::max();
							for (size_t tk = 0; tk < clusters.size(); ++tk)
							{
								if (tk == tc) continue;
								if (distmat[j.second][medoids[tk]] < ndist)
								{
									ndist = distmat[j.second][medoids[tk]];
								}
							}
							tih += crn::Min(distmat[j.second][h.second], ndist) - j.first;
						} // j is in class i
						if (tih > mintih)
							break; // slight optimization
					} // for each element
				if (tih < mintih)
				{
					mintih = tih;
					mini = i;
					minh = h.second;
				}
			} // for each non-medoid element (h)
	} // for each medoid (i)
	//std::cout << "tih = " << mintih << " for " << medoids[mini] << " -> " << minh << std::endl;
	if (mintih < 0)
		medoids[mini] = minh;
}

