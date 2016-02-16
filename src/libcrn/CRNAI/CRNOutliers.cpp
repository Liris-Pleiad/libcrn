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
 * file: CRNOutliers.cpp
 * \author Yann LEYDIER
 */

#include <CRNAI/CRNOutliers.h>
#include <CRNMath/CRNSquareMatrixDouble.h>
#include <CRNStringUTF8.h>
#include <CRNi18n.h>
#include <numeric> // for accumulate

using namespace crn;

/*! \internal */
template<typename DISTMAT> std::vector<double> computeLOF(const DISTMAT &distmat, size_t ndata, size_t k)
{
	if (k <= 1)
		throw ExceptionDomain(crn::StringUTF8("std::vector<double> ComputeLOF(): ") + _("The neighborhood must be > 1."));
	if (ndata <= k)
		throw ExceptionLogic(crn::StringUTF8("std::vector<double> ComputeLOF(): ") + _("The neighborhood is greater than the number of elements."));

	// kNN and k-distance ( = kNN.rbegin()->first)
	std::vector<std::multimap<double, size_t>> knn(ndata);
	for (size_t tmp1 = 0; tmp1 < ndata; ++tmp1)
	{
		for (size_t tmp2 = 0; tmp2 < ndata; ++tmp2)
		{
			double maxval = (knn[tmp1].size() >= k) ? knn[tmp1].rbegin()->first : std::numeric_limits<double>::max();
			if (distmat[tmp1][tmp2] < maxval)
			{
				if (knn[tmp1].size() >= k)
					knn[tmp1].erase(maxval);
				knn[tmp1].insert(std::make_pair(distmat[tmp1][tmp2], tmp2));
			}
		}
	}

	// local reachability density
	std::vector<double> lrd(ndata, 0.0);
	for (size_t tmp1 = 0; tmp1 < ndata; ++tmp1)
	{
		for (const auto &nn : knn[tmp1])
			lrd[tmp1] += crn::Max(distmat[tmp1][nn.second], knn[nn.second].rbegin()->first); // reachdist(q,p) = max(d(q,p), kdist(p))
		lrd[tmp1] = double(k) / lrd[tmp1];
	}

	// local outlier factor
	std::vector<double> lof(ndata, 0.0);
	for (size_t tmp1 = 0; tmp1 < ndata; ++tmp1)
	{
		for (const auto &nn : knn[tmp1])
		{
			lof[tmp1] += lrd[nn.second];
		}
		lof[tmp1] /= double(k) * lrd[tmp1];
	}
	
	return lof;
}

/*! Compute the Local Outlier Factor for each element from the distance matrix
 * \param[in]	distamat	a square matrix
 * \param[in]	k	the size of the neighborhood
 *
 * \throws	ExceptionDomain	k<=1
 * \throws	ExceptionDimension	distmat is not square
 * \throws	ExceptionLogic	k > distmat.size()
 *
 * \ingroup cluster
 *
 * \return	the list of LOFs
 */
std::vector<double> crn::ComputeLOF(const crn::SquareMatrixDouble &distmat, size_t k)
{
	return computeLOF(distmat, distmat.GetRows(), k);
}

/*! Compute the Local Outlier Factor for each element from the distance matrix
 * \param[in]	distamat	a square matrix
 * \param[in]	k	the size of the neighborhood
 *
 * \throws	ExceptionDomain	k<=1
 * \throws	ExceptionDimension	distmat is not square
 * \throws	ExceptionLogic	k > distmat.size()
 *
 * \ingroup cluster
 *
 * \return	the list of LOFs
 */
std::vector<double> crn::ComputeLOF(const std::vector<std::vector<double>> &distmat, size_t k)
{
	const auto ndata = distmat.size();
	for (const auto &row : distmat)
		if (row.size() != ndata)
			throw ExceptionDimension(crn::StringUTF8("std::vector<double> ComputeLOF(): ") + _("The distance matrix is not square."));

	return computeLOF(distmat, ndata, k);
}

/*! \internal */
template<typename DISTMAT> std::vector<double> computeLoOP(const DISTMAT &distmat, size_t ndata, size_t k, double lambda)
{
	if (k <= 1)
		throw ExceptionDomain(crn::StringUTF8("std::vector<double> ComputeLoOP(): ") + _("The neighborhood must be > 1."));
	if (lambda <= 0)
		throw ExceptionDomain(crn::StringUTF8("std::vector<double> ComputeLoOP(): ") + _("lambda must be > 0."));
	if (ndata <= k)
		throw ExceptionLogic(crn::StringUTF8("std::vector<double> ComputeLoOP(): ") + _("The neighborhood is greater than the number of elements."));

	// kNN
	std::vector<std::multimap<double, size_t>> knn(ndata);
	for (size_t tmp1 = 0; tmp1 < ndata; ++tmp1)
	{
		for (size_t tmp2 = 0; tmp2 < ndata; ++tmp2)
		{
			double maxval = (knn[tmp1].size() >= k) ? knn[tmp1].rbegin()->first : std::numeric_limits<double>::max();
			if (distmat[tmp1][tmp2] < maxval)
			{
				if (knn[tmp1].size() >= k)
					knn[tmp1].erase(maxval);
				knn[tmp1].insert(std::make_pair(distmat[tmp1][tmp2], tmp2));
			}
		}
	}

	// pdist
	std::vector<double> pdist(ndata, 0.0);
	for (size_t tmp = 0; tmp < ndata; ++tmp)
	{
		for (const auto &neigh : knn[tmp])
			pdist[tmp] += Sqr(neigh.first);
		pdist[tmp] = lambda * sqrt(pdist[tmp] / double(k));
	}

	// PLOF
	std::vector<double> plof(ndata, 0.0);
	for (size_t tmp = 0; tmp < ndata; ++tmp)
	{
		for (const auto &neigh : knn[tmp])
			plof[tmp] += pdist[neigh.second];
		plof[tmp] = (double(k) * pdist[tmp] / plof[tmp]) + 1;
	}
	double nplof = sqrt(2.0) * lambda * double(ndata) / std::accumulate(plof.begin(), plof.end(), 0.0); // XXX Vérifier cette équation!!!
	
	// LoOP
	std::vector<double> loop(ndata, 0.0);
	for (size_t tmp = 0; tmp < ndata; ++tmp)
		loop[tmp] = Max(0.0, erf(plof[tmp] / nplof));
	
	return loop;
}

/*! Compute the Local Outlier Probability for each element from the distance matrix
 * \param[in]	distamat	a square matrix
 * \param[in]	k	the size of the neighborhood
 * \param[in]	lambda	the precision of the density estimation (lambda=1 -> 68%, 2 -> 95%, 3 -> 99.7%)
 *
 * \throws	ExceptionDomain	k<=1 or lambda<=0
 * \throws	ExceptionDimension	distmat is not square
 * \throws	ExceptionLogic	k > distmat.size()
 *
 * \ingroup cluster
 *
 * \return	the list of LoOPs
 */
std::vector<double> crn::ComputeLoOP(const crn::SquareMatrixDouble &distmat, size_t k, double lambda)
{
	return computeLoOP(distmat, distmat.GetRows(), k, lambda);
}

/*! Compute the Local Outlier Probability for each element from the distance matrix
 * \param[in]	distamat	a square matrix
 * \param[in]	k	the size of the neighborhood
 * \param[in]	lambda	the precision of the density estimation (lambda=1 -> 68%, 2 -> 95%, 3 -> 99.7%)
 *
 * \throws	ExceptionDomain	k<=1 or lambda<=0
 * \throws	ExceptionDimension	distmat is not square
 * \throws	ExceptionLogic	k > distmat.size()
 *
 * \ingroup cluster
 *
 * \return	the list of LoOPs
 */
std::vector<double> crn::ComputeLoOP(const std::vector<std::vector<double>> &distmat, size_t k, double lambda)
{
	const size_t ndata = distmat.size();
	for (const auto &row : distmat)
		if (row.size() != ndata)
			throw ExceptionDimension(crn::StringUTF8("std::vector<double> ComputeLoOP(const std::vector<std::vector<double>> &distmat, size_t k, double lambda): ") + _("The distance matrix is not square."));

	return computeLoOP(distmat, ndata, k, lambda);
}

