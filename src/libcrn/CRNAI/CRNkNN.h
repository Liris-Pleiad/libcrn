/* Copyright 2014 INSA-Lyon
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
 * file: CRNkNN.h
 * \author Yann LEYDIER
 */

#ifndef CRNkNN_HEADER
#define CRNkNN_HEADER

#include <CRNException.h>
#include <CRNMath/CRNMath.h>
#include <vector>
#include <map>
#include <set>
#include <limits>
#include <algorithm>

namespace crn
{
	/*! \brief Iterative kNN
	 *
	 * An iterative kNN computation helper
	 *
	 * \version 0.2
	 * \date	Aug. 2014
	 * \author	Yann LEYDIER
	 * \ingroup cluster
	 */
	template<typename DataType, typename DistFunc> class IterativekNN
	{
		public:
			/*!
			 * \param[in]	neighborhood	the size of the neighborhood
			 * \param[in]	df	a distance function: double df(const DataType&, const DataType&)
			 * \param[in]	fast_min	minimum number of seeds for fast addition of an element
			 * \param[in]	fast_factor	sampling factor for fast addition of an element
			 * \param[in]	fast_max	maximum number of seeds for fast addition of an element
			 * \throws	ExceptionDomain	neighborhood <= 1
			 */
			IterativekNN(size_t neighborhood, const DistFunc &df, size_t fast_min = 50, size_t fast_factor = 10, size_t fast_max = 100): k(neighborhood), dist(df), min_fast(fast_min), sampling(fast_factor), max_fast(fast_max)
			{ if (k <= 1) throw ExceptionDomain("IterativekNN::IterativekNN(size_t neighborhood, const DistFunc &df): The neighborhood must be > 1."); }
			/*!
			 * \param[in]	neighborhood	the size of the neighborhood
			 * \param[in]	df	a distance function: double df(const DataType&, const DataType&)
			 * \param[in]	fast_min	minimum number of seeds for fast addition of an element
			 * \param[in]	fast_factor	sampling factor for fast addition of an element
			 * \param[in]	fast_max	maximum number of seeds for fast addition of an element
			 * \throws	ExceptionDomain	neighborhood <= 1
			 */
			IterativekNN(size_t neighborhood, DistFunc &&df, size_t fast_min = 50, size_t fast_factor = 10, size_t fast_max = 100): k(neighborhood), dist(std::move(df)), min_fast(fast_min), sampling(fast_factor), max_fast(fast_max)
			{ if (k <= 1) throw ExceptionDomain("IterativekNN::IterativekNN(size_t neighborhood, DistFunc &&df): The neighborhood must be > 1."); }

			/*! \brief Adds a element with full computation of nearest neighbors
			 * \param[in]	obj	the element to add
			 */
			void Add(const DataType &obj)
			{
				sample.emplace_back(obj);
				add();
			}

			/*! \brief Adds a element with full computation of nearest neighbors
			 * \param[in]	obj	the element to add
			 */
			void Add(DataType &&obj)
			{
				sample.emplace_back(std::move(obj));
				add();
			}

			/*! \brief Adds a element with partial computation of nearest neighbors
			 * \param[in]	obj	the element to add
			 */
			void FastAdd(const DataType &obj)
			{
				sample.emplace_back(obj);
				if (sample.size() < min_fast)
					add();
				else
					fastAdd();
			}

			/*! \brief Adds a element with partial computation of nearest neighbors
			 * \param[in]	obj	the element to add
			 */
			void FastAdd(DataType &&obj)
			{
				sample.emplace_back(std::move(obj));
				if (sample.size() < min_fast)
					add();
				else
					fastAdd();
			}

			/*! 
			 * \throws	ExceptionDimension	#samples < k
			 * \return	the Local Outlier Factor for all elements */
			std::vector<double> GetLOF() const
			{
				const auto ndata = sample.size();
				if (ndata < k)
					throw ExceptionDimension("IterativekNN::GetLOF(): #samples < k");

				std::vector<double> lrd(ndata, 0.0);
				for (size_t el = 0; el < ndata; ++el)
				{
					auto itn = sample[el].nn.begin();
					for (size_t kn = 0; kn < crn::Min(k, sample[el].nn.size()); ++kn, ++itn)
						lrd[el] += crn::Max(itn->first, sample[itn->second].nn.rbegin()->first);
					lrd[el] = double(k) / lrd[el];
				}
				std::vector<double> lof(ndata, 0.0);
				for (size_t el = 0; el < ndata; ++el)
				{
					for (const auto &nn : sample[el].nn)
					{
						lof[el] += lrd[nn.second];
					}
					lof[el] /= double(k) * lrd[el];
				}
				return lof;
			}

			/*!
			 * \throws	ExceptionDimension	#samples < k
			 * \param[in]	lambda	the precision of the density estimation (lambda=1 -> 68%, 2 -> 95%, 3 -> 99.7%)
			 * \return	the Local Outlier Probability for all elements
			 */
			std::vector<double> GetLoOP(double lambda) const
			{
				const auto ndata = sample.size();
				if (ndata < k)
					throw ExceptionDimension("IterativekNN::GetLoOP(): #samples < k");

				// pdist
				std::vector<double> pdist(ndata, 0.0);
				for (size_t tmp = 0; tmp < ndata; ++tmp)
				{
					for (const auto &neigh : sample[tmp].nn)
						pdist[tmp] += Sqr(neigh.first);
					pdist[tmp] = lambda * sqrt(pdist[tmp] / double(k));
				}

				// PLOF
				std::vector<double> plof(ndata, 0.0);
				for (size_t tmp = 0; tmp < ndata; ++tmp)
				{
					for (const auto &neigh : sample[tmp].nn)
						plof[tmp] += pdist[neigh.second];
					plof[tmp] = (double(k) * pdist[tmp] / plof[tmp]) + 1;
				}
				double nplof = sqrt(2.0) * lambda * double(ndata) / std::accumulate(plof.begin(), plof.end(), 0.0);

				// LoOP
				std::vector<double> loop(ndata, 0.0);
				for (size_t tmp = 0; tmp < ndata; ++tmp)
					loop[tmp] = Max(0.0, erf(plof[tmp] / nplof));

				return loop;
			}

			const DataType& GetElement(size_t el) const { return sample[el].data; }
			size_t GetNElements() const noexcept { return sample.size(); }

		private:
			/*! \internal */
			struct Element
			{
				Element(const DataType &d):data(d) {}
				Element(DataType &&d):data(std::move(d)) {}
				DataType data;
				std::multimap<double, size_t> nn;
			};

			inline void add()
			{
				sample.back().nn.insert(std::make_pair(0.0, sample.size() - 1));
				if (sample.size() == 1)
					return;
				std::set<size_t> need_update;
				// look for neighbours
				for (size_t el = 0; el < sample.size() - 1; ++el)
				{
					const double d = dist(sample.back().data, sample[el].data);
					// add to new point's kNN?
					double maxval = (sample.back().nn.size() >= k) ? sample.back().nn.rbegin()->first : std::numeric_limits<double>::max();
					if (d < maxval)
					{
						if (sample.back().nn.size() >= k)
							sample.back().nn.erase(maxval);
						sample.back().nn.insert(std::make_pair(d, el));
					}
					// add to old point's kNN?
					if (el == (sample.size() - 1))
						continue; // don't add twice
					maxval = (sample[el].nn.size() >= k) ? sample[el].nn.rbegin()->first : std::numeric_limits<double>::max();
					if (d < maxval)
					{
						if (sample[el].nn.size() >= k)
							sample[el].nn.erase(maxval);
						for (const auto &n : sample[el].nn)
						{
							need_update.insert(n.second);
						}
						sample[el].nn.insert(std::make_pair(d, sample.size() - 1));
					}

				}
				for (const auto &n : sample.back().nn)
					need_update.insert(n.second);
			}

			inline void fastAdd()
			{
				sample.back().nn.insert(std::make_pair(0.0, sample.size() - 1));
				if (sample.size() == 1)
					return;
				std::set<size_t> tovisit, visited;
				std::set<size_t> need_update;

				const size_t ninit = crn::Min((sample.size() - 2) / sampling + 1, max_fast);
				for (size_t tmp = 0; tmp < ninit; ++tmp)
					tovisit.insert((tmp * (sample.size() - 2)) / ninit);
				// look for neighbours
				while (!tovisit.empty())
				{
					const size_t el = *tovisit.begin();
					visited.insert(el);
					tovisit.erase(tovisit.begin());

					const double d = dist(sample.back().data, sample[el].data);
					// add to new point's kNN?
					double maxval = (sample.back().nn.size() >= k) ? sample.back().nn.rbegin()->first : std::numeric_limits<double>::max();
					if (d < maxval)
					{
						if (sample.back().nn.size() >= k)
							sample.back().nn.erase(maxval);
						sample.back().nn.insert(std::make_pair(d, el));
						for (const auto &n : sample[el].nn)
							if (visited.find(n.second) == visited.end())
								tovisit.insert(n.second);
					}
					// add to old point's kNN?
					if (el == (sample.size() - 1))
						continue; // don't add twice
					maxval = (sample[el].nn.size() >= k) ? sample[el].nn.rbegin()->first : std::numeric_limits<double>::max();
					if (d < maxval)
					{
						if (sample[el].nn.size() >= k)
							sample[el].nn.erase(maxval);
						for (const auto &n : sample[el].nn)
						{
							need_update.insert(n.second);
							if (visited.find(n.second) == visited.end())
								tovisit.insert(n.second);
						}
						sample[el].nn.insert(std::make_pair(d, sample.size() - 1));
					}
				}
				for (const auto &n : sample.back().nn)
					need_update.insert(n.second);
			}

			std::vector<Element> sample;
			DistFunc dist;
			const size_t k;
			const size_t sampling;
			const size_t max_fast;
			const size_t min_fast;
	};
}

#endif
