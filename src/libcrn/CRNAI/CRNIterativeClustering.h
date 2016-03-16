/* Copyright 2012-2014 INSA-Lyon, CoReNum
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
 * file: CRNIterativeClustering.h
 * \author Yann LEYDIER
 */

#ifndef CRNIterativeClustering_HEADER
#define CRNIterativeClustering_HEADER

#include <CRNString.h>
#include <vector>
#include <set>

namespace crn
{
	/*! \brief A utility class to create clusters iteratively
	 *
	 * A utility class to create clusters iteratively
	 *
	 * \warning	The template type must define operator<
	 *
	 * \author Yann LEYDIER
	 * \date	Sept 2012
	 * \version 0.1
	 * \ingroup cluster
	 */
	template<
		typename T,
		typename std::enable_if<
			// operator=
			std::is_copy_assignable<T>::value &&
			// operator<
			protocol::HasLT<T>::value
			, int>::type = 0
		> 
	struct IterativeClustering
	{
		public:
			IterativeClustering() = default;
			IterativeClustering(const IterativeClustering&) = default;
			IterativeClustering(IterativeClustering&&) = default;
			IterativeClustering& operator=(const IterativeClustering&) = default;
			IterativeClustering& operator=(IterativeClustering&&) = default;

			/*! \brief Gets the current clusters
			 * \return	a vector of clusters
			 */
			inline const std::vector<std::set<T>>& GetClusters() const { return clusters; }

			enum class Operation { None, Create, Add, Merge };

			/*! \brief Associates two elements and merges clusters if needed 
			 * \param[in]	v1	first element
			 * \param[in]	v2	second element
			 * \return	the operator that was done (None: pair already associated, Create: new cluster created, Add: one element added to a cluster, Merge: two clusters were merged)
			 */
			Operation Associate(const T &v1, const T &v2)
			{
				auto ret = Operation::None;
				auto found = size_t{0};
				auto leftover = v1;
				for (auto cluster = size_t(0); cluster < clusters.size(); ++cluster)
				{ // for each cluster
					if (clusters[cluster].find(v1) != clusters[cluster].end())
					{ // found v1
						if (clusters[cluster].insert(v2).second)
						{
							ret = Operation::Add;
							found = cluster;
							leftover = v2;
							break;
						}
						else // not inserted: was already in
							return Operation::None;
					}
					if (clusters[cluster].find(v2) != clusters[cluster].end())
					{ // found v2
						if (clusters[cluster].insert(v1).second)
						{
							ret = Operation::Add;
							found = cluster;
							leftover = v1;
							break;
						}
						else // not inserted: was already in
							return Operation::None;
					}
				}
				if (ret == Operation::None)
				{ // create new cluster
					clusters.push_back(std::set<T>());
					clusters.back().insert(v1);
					clusters.back().insert(v2);
					return Operation::Create;
				}
				// check if we need to merge
				for (auto cluster = found + 1; cluster < clusters.size(); ++cluster)
				{
					if (clusters[cluster].find(leftover) != clusters[cluster].end())
					{ // merge
						clusters[found].insert(clusters[cluster].begin(), clusters[cluster].end());
						clusters.erase(clusters.begin() + cluster);
						return Operation::Merge;
					}
				}
				return ret;
			}

			/*! \brief Prints the clusters to a string
			 * \warning	the elements must be convertible to crn::String
			 * \return	a string representing the clusters
			 */
			crn::String ToString() const
			{
				auto s = crn::String{};
				for (const auto &c : clusters)
				{
					s += U"{ ";
					for (const auto& v : c)
					{
						s += v;
						s += U" ";
					}
					s += U"} ";
				}
				return s;
			}

		private:
			std::vector<std::set<T>> clusters; /*!< the clusters */
	};

}

#endif


