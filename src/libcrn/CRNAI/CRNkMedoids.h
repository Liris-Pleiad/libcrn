/* Copyright 2014-2016 INSA-Lyon, ENS-Lyon
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
 * file: CRNkMedoids.h
 * \author Yann LEYDIER
 */

#ifndef CRNkMedoids_HEADER
#define CRNkMedoids_HEADER

#include <CRNException.h>
#include <vector>
#include <map>
#include <tuple>
#include <limits>

namespace crn
{
	/*! \brief k medoids
	 * \ingroup cluster
	 */
	namespace kmedoids
	{
		namespace init
		{
			/*! \brief Finds the k most central elements
			 *
			 * Fast initialization with the most central elements.
			 *
			 * Park H-S, Jun C-H, A simple and fast algorithm for K-medoids clustering, ESA #36-2-2, 2009
			 */
			struct Central
			{
				Central(size_t n_classes):k(n_classes) {}
				std::vector<size_t> operator()(const std::vector<std::vector<double>> &distmat) const;
				private:
					size_t k;
			};

			/*! \brief PAM
			 *
			 * Adds iteratively medoids to maximize their spread.
			 */
			struct PAM
			{
				PAM(size_t n_classes):k(n_classes) {}
				std::vector<size_t> operator()(const std::vector<std::vector<double>> &distmat) const;
				private:
					size_t k;
			};
		}
		namespace update
		{
			/*! \brief Gets the element with the lower distance to other elements in the cluster
			 *
			 * Park H-S, Jun C-H, A simple and fast algorithm for K-medoids clustering, ESA #36-2-2, 2009
			 */
			struct Local
			{
				void operator()(std::vector<size_t> &medoids, const std::vector<std::multimap<double, size_t>> &clusters, const std::vector<std::vector<double>> &distmat) const;
			};

			/*! \brief PAM
			 *
			 * Updates one single medoid to optimize the spread.
			 */
			struct PAM
			{
				void operator()(std::vector<size_t> &medoids, const std::vector<std::multimap<double, size_t>> &clusters, const std::vector<std::vector<double>> &distmat) const;
			};
		}

		/*! \brief k medoids
		 * \throws	ExceptionDimension	the distance matrix is not square
		 * \param[in]	init	a functor to initialize the medoids: std::vector<size_t> init(const std::vector<std::vector<double>> &distmat)
		 * \param[in]	update	a functor to update the medoids: void update(std::vector<size_t> &medoids, const std::vector<std::vector<size_t>> &clusters, const std::vector<std::vector<double>> &distmat)
		 * \param[in]	distmat	the distance matrix
		 * \param[in]	maxiter	maximal number of iterations
		 * \return	a tuple containing (0)vector<class number>, (1)vector<multimap<distance, element_id>>, (2)vector<element_id>. (0) is the index of the cluster for each element. (1) is the list of elements, sorted by distance, for each cluster. (2) is the list of medoids.
		 */
		template<typename Init, typename Update> std::tuple<std::vector<size_t>, std::vector<std::multimap<double, size_t>>, std::vector<size_t>> Run(Init init, Update update, const std::vector<std::vector<double>> &distmat, size_t maxiter = std::numeric_limits<size_t>::max())
		{
			const auto nelem = distmat.size();
			// check
			for (const auto &row : distmat)
				if (row.size() != nelem)
					throw ExceptionDimension("kmedoids::Run(): The distance matrix is not square.");
			// init
			auto medoids = init(distmat);
			const auto k = medoids.size();
			/*
			std::cout << "init: ";
			for (auto m : medoids)
				std::cout << m << " ";
			std::cout << std::endl;
			*/
			// run
			auto clusters = std::vector<std::multimap<double, size_t>>(k);
			auto distsum = 0.0, precsum = std::numeric_limits<double>::max();
			auto iter = size_t{0};
			while (precsum != distsum)
			{
				precsum = distsum;
				distsum = 0.0;
				// classification
				std::vector<std::multimap<double, size_t>>(k).swap(clusters);
				for (auto o = size_t(0); o < nelem; ++o)
				{
					auto ndist = std::numeric_limits<double>::max();
					auto nclass = size_t{0};
					for (auto m = size_t(0); m < k; ++m)
						if (distmat[o][medoids[m]] < ndist)
						{
							ndist = distmat[o][medoids[m]];
							nclass = m;
						}
					clusters[nclass].insert(std::make_pair(ndist, o));
					distsum += ndist;
				}
				// update
				update(medoids, clusters, distmat);

				if (++iter >= maxiter)
					break;
			}
			auto classnum = std::vector<size_t>(nelem, 0);
			for (auto c = size_t(0); c < k; ++c)
				for (const auto &o : clusters[c])
					classnum[o.second] = c;
			return std::make_tuple(std::move(classnum), std::move(clusters), std::move(medoids));
		}
	}
}

#endif
