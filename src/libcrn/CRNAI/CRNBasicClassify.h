/* Copyright 2008-2016 INSA Lyon, CoReNum, ENS-Lyon
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
 * file: CRNBasicClassify.h
 * \author Yann LEYDIER
 */

#ifndef CRNBASICCLASSIFY_HEADER
#define CRNBASICCLASSIFY_HEADER

#include <CRNObject.h>
#include <CRNData/CRNMap.h>
#include <CRNData/CRNVector.h>
#include <CRNException.h>
#include <CRNAI/CRNClassifResult.h>
#include <set>
#include <map>
#include <algorithm>
#include <limits>

namespace crn
{
	/****************************************************************************/
	/*! \brief Basic classification tools
	 * 
	 * Basic classifications tools
	 * 
	 * \author 	Yann LEYDIER
	 * \date		July 2008
	 * \version 0.2
	 * \ingroup classify
	 */
	class BasicClassify
	{
		public:
			/*! \brief Finds the nearest neighbor in a set of objects
			 * 
			 * Finds the nearest neighbor in a set of objects
			 *
			 * \throws	ExceptionProtocol	the sample is not metric
			 * \throws	ExceptionRuntime	the sample is not of the same class as the model
			 * 
			 * \param[in]	obj	the sample to classify
			 * \param[in]	begin	an input iterator to a pointer to the first prototype
			 * \param[in]	end	an input iterator after the last prototype
			 * \return	the index of-, the distance to- and the nearest neighbor
			 */
			template<
				typename ConstIterator,
				typename std::enable_if<IsMetric<typename std::iterator_traits<ConstIterator>::value_type>::value, int>::type = 0
				>
			static ClassifResult NearestNeighbor(const typename std::iterator_traits<ConstIterator>::value_type &obj, ConstIterator begin, ConstIterator end)
			{
				auto nearest = 0;
				auto mindist = std::numeric_limits<double>::max();
				auto classid = 0;
				ConstIterator prot;
				for (auto it = begin; it != end; ++it)
				{
					const auto d = Distance(obj, *it);
					if (d < mindist)
					{
						mindist = d;
						nearest = classid;
						prot = it;
					}
					classid += 1;
				}

				return ClassifResult(nearest, mindist, *prot);
			}

			/*! \brief Classify a sample using the k nearest neighbors
			 * 
			 * Classify a sample using the k nearest neighbors
			 * 
			 * \throws	std::bad_cast	the map does not contain objects of the same type as obj
			 * \throws	ExceptionRuntime	the sample is not of the same class as the model
			 * 
			 * \param[in]	obj	the sample to classify
			 * \param[in]	database a map with key=label value=SVector of samples
			 * \param[in]	k	the number of nearest neighbors to taken into account
			 * \return	the label of-, the distance to- and the nearest prototype whose class is most represented in the k nearest neighbors
			 */
			template<
				typename T,
				typename std::enable_if<IsMetric<T>::value, int>::type = 0
				>
			static ClassifResult kNearestNeighbors(const T &obj, const Map &database, int k)
			{
				std::set<ClassifResult> knn;
				std::set<ClassifResult>::iterator maxneighbor;
				auto classid = 0;
				for (Map::const_iterator dataclass = database.begin(); 
						dataclass != database.end(); ++dataclass)
				{
					String label = dataclass->first;
					SVector samples = std::dynamic_pointer_cast<Vector>(dataclass->second);
					if (!samples)
						throw ExceptionInvalidArgument("ClassifResult BasicClassify::"
								"kNearestNeighbors(const Object &obj, const Map &database, "
								"int k): invalid database.");
					for (Vector::const_iterator sample = samples->begin();
							sample != samples->end(); ++sample)
					{
						auto d = Distance(obj, dynamic_cast<const T&>(**sample)); // may throw
						if (knn.size() < (unsigned int)k)
						{ // knn list not already full
							knn.insert(ClassifResult(classid, label, d, *sample));
						}
						else
						{ // knn list full
							if (d < maxneighbor->distance)
							{ // add to list and remove the (k+1)th neighbor
								knn.insert(ClassifResult(classid, label, d, *sample));
								knn.erase(std::max_element(knn.begin(), knn.end()));
							}
						}
						maxneighbor = std::max_element(knn.begin(), knn.end());
					}
					classid += 1;
				}
				return chooseClass(knn);
			}

			/*! \brief Classify a sample using the k nearest neighbors
			 * 
			 * Classify a sample using the k nearest neighbors
			 * 
			 * \throws	std::bad_cast	the map does not contain objects of the same type as obj
			 * \throws	ExceptionRuntime	the sample is not of the same class as the model
			 * 
			 * \param[in]	obj	the sample to classify
			 * \param[in]	database a map with key=label value=SVector of samples
			 * \param[in]	epsilon	the maximal distance
			 * \return	the label of-, the distance to- and the nearest prototype whose class is most represented in the k nearest neighbors
			 */
			template<
				typename T,
				typename std::enable_if<IsMetric<T>::value, int>::type = 0
				>
			static ClassifResult EpsilonNeighbors(const T &obj, const Map &database, double epsilon)
			{
				std::set<ClassifResult> en;
				int classid = 0;
				for (Map::const_iterator dataclass = database.begin(); 
						dataclass != database.end(); ++dataclass)
				{
					String label = dataclass->first;
					SVector samples = std::dynamic_pointer_cast<Vector>(dataclass->second);
					if (!samples)
						throw ExceptionInvalidArgument("ClassifResult BasicClassify::"
									"EpsilonNeighbors(const Object &obj, const Map &database, "
									"double epsilon): invalid database.");
					for (Vector::const_iterator sample = samples->begin();
							sample != samples->end(); ++sample)
					{
						double d = Distance(obj, dynamic_cast<const T&>(**sample)); // may throw
						if (d < epsilon)
						{ // add to list
							en.insert(ClassifResult(classid, label, d, *sample));
						}
					}
					classid += 1;
				}
				return chooseClass(en);
			}

		private:
			/*! \cond */
			/*! \brief Internal.
			 * 
			 * Compares the second value of int pairs.
			 * 
			 * \param[in]	p1	first pair
			 * \param[in]	p2	second pair
			 * \return true if p1.second < p2.second, false else
			*/
			static bool pairintintvalcmp(const std::pair<int, int> &p1, const std::pair<int, int> &p2)
			{
				return p1.second < p2.second;
			}
			/*! \brief Internal.
			 * 
			 * Checks the value of the second member of an int pair.
			 */
			class pairintintvaleq: public std::unary_function<const std::pair<int, int> &, bool>
			{
				public:
					/*! Initialization of the functor.
					 * \param[in]	v	the reference value */
					pairintintvaleq(int v):val(v) {}
					/*! Functor operator
					 * \param[in]	p	the pair
					 * \return	true if p.second == reference value, false else
					 */
					bool operator()(const std::pair<int, int> &p)
					{
						return p.second == val;
					}
				private:
					int val; /*!< The reference value */
			};
			/*!
			 * \brief Internal.
			 * 
			 * Finds the best class out of nearest neighbors.
			 * 
			 * \param[in]	nn	a set of samples
			 * \return	The nearest sample of the most representative class
			 */
			static ClassifResult chooseClass(std::set<ClassifResult> &nn)
			{
				// count population for each class
				std::map<int, int> pop;
				for (std::set<ClassifResult>::iterator it = nn.begin(); 
						it != nn.end(); ++it)
				{
					pop[it->class_id] += 1;
				}
				// retrieve maximal population
				int maxpop = std::max_element(pop.begin(), pop.end(), 
						pairintintvalcmp)->second;
				// look for classes that have maximal population
				// (there can be several such classes)
				std::map<int, int>::iterator popit = pop.begin();
				pairintintvaleq sel(maxpop);
				std::set<ClassifResult> sameclass;
				while ((popit = std::find_if(popit, pop.end(), sel)) != pop.end())
				{
					// look for neighbors whose class has maximal population
					ClassifResult::SelectId csel(popit->first);
					std::set<ClassifResult>::iterator cit = nn.begin();
					while ((cit = std::find_if(cit, nn.end(), csel)) != nn.end())
					{
						sameclass.insert(*cit);
						++cit;
					}
					++popit;
				}	
				return *std::min_element(sameclass.begin(), sameclass.end());
			}
			/*! \endcond */
	};
}
#endif
