/* Copyright 2007-2016 Yann LEYDIER, CoReNum, INSA-Lyon, ENS-Lyon
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
 * file: CRNkMeans.h
 * \author Yann LEYDIER
 */

#ifndef CRNKMEANS_HEADER
#define CRNKMEANS_HEADER

#include <CRNAI/CRNBasicClassify.h>
#include <vector>
#include <random>

/*! \defgroup cluster Clustering 
	 * \ingroup	ai */

namespace crn
{
	/****************************************************************************/
	/*! \brief k-means clustering algorithm
	 *
	 * k-means clustering utility.
	 * Caution: the samples are not freed when the object is destroyed.
	 *
	 * \author 	Yann LEYDIER
	 * \date		March 2007
	 * \version 0.3
	 * \ingroup cluster
	 */
	template<
		typename T,
		typename std::enable_if<IsMetric<typename std::result_of<decltype(Dereference<T>)&(const T&)>::type>::value, int>::type = 0,
		typename std::enable_if<IsVectorOverR<typename std::result_of<decltype(Dereference<T>)&(const T&)>::type>::value, int>::type = 0
		> class kMeans
	{
		public:
			using value_type = typename std::result_of<decltype(Dereference<T>)&(const T&)>::type;
			kMeans(const kMeans&) = delete;
			kMeans(kMeans&&) = default;
			kMeans& operator=(const kMeans&) = delete;
			kMeans& operator=(kMeans&&) = default;

			/*! \brief Adds one prototype */
			void AddPrototype(const T &sam)
			{
				proto.push_back(Dereference(sam));
			}
			/*! \brief Adds one random prototype out of the samples pool
			 * \throws	ExceptionNotFound	No sample available
			 */
			void AddRandomPrototype()
			{
				const auto nb = GetNbSamples();
				if (!nb)
					throw ExceptionNotFound("No sample available.");
				auto generator = std::default_random_engine{};
				auto distribution = std::uniform_int_distribution<size_t>{0, nb - 1};
				AddPrototype(data[distribution(generator)]);
			}
			/*! \brief Returns the number of classes */
			size_t GetNbClasses() const noexcept { return proto.size(); }
			/*! \brief Returns the vector of prototypes */
			const std::vector<value_type>& GetPrototypes() const noexcept { return proto; }
			/*! \brief Clears the prototypes */
			void ClearPrototypes() noexcept { proto.clear(); }

			/*! \brief Adds one sample
			 * \param[in]	sam	The sample to add
			 */
			void AddSample(const T &sam)
			{
				data.push_back(sam);
			}
			/*! \brief Adds one sample
			 * \param[in]	sam	The sample to add
			 */
			void AddSample(T &&sam)
			{
				data.push_back(std::move(sam));
			}
			/*! \brief Returns the number of samples */
			size_t GetNbSamples() const noexcept { return data.size(); }
			/*! \brief Returns the vector of samples */
			const std::vector<const T>& GetSamples() const noexcept { return data; }
			/*! \brief Clears the samples */
			void ClearSamples() noexcept { data.clear(); }

			/*! \brief Runs the k-means
			 * \param[in]	maxcnt	maximal number of iterations
			 * \return	the number of iterations
			 */
			size_t Run(size_t maxcnt = 100)
			{
				auto cnt = size_t(0);
				const auto k = proto.size();
				classes.clear();
				classes.resize(k);
				auto popclasses = std::vector<size_t>(k, 0);
				auto finished = false;
				while (!finished)
				{
					for (auto &c : classes)
						c.clear();
					// for each sample
					for (auto elnum = size_t(0); elnum < data.size(); ++elnum)
					{
						// classify
						classes[Classify(*data[elnum])].push_back(elnum);
					}
					finished = true;
					// for each class
					for (auto p = size_t(0); p < k; p++)
					{
						// compute mean
						auto sum = SumType<value_type>(Zero(*data.front()));
						for (const auto &num : classes[p])
							sum += *data[num];
						proto[p] = value_type(sum * (1.0 / double(classes[p].size())));
						// check if finished
						if (popclasses[p] != classes[p].size())
						{
							popclasses[p] = classes[p].size();
							finished = false;
						}
					}
					cnt += 1;
					if (cnt > maxcnt)
						break;
				}
				return cnt;
			}

			/*! \brief Finds the closest prototype
			 * \param[in]	obj	the sample to classify
			 * \param[out]	distance	the distance to the closest prototype
			 * \return	the index of the closest prototype
			 */
			size_t Classify(const value_type &obj, double *distance = nullptr)
			{
				if (proto.empty())
					throw ExceptionDimension();
				auto res = BasicClassify::NearestNeighbor(obj, proto.begin(), proto.end());
				if (distance)
					*distance = res.distance;
				return res.class_id;
			}

			/*! \brief Returns the content of one class
			 * \throws	ExceptionDimension	wrong class number
			 * \param[in]	k	The id of the class
			 * \return	the content of class k
			 */
			const std::vector<size_t>& GetClass(size_t k) const
			{
				if (k >= classes.size())
					throw ExceptionDimension("kMeans::GetClass(): Wrong class number.");
				return classes[k];
			}

		private:
			std::vector<const T> data; /*!< The samples */
			std::vector<value_type> proto; /*!< The prototypes */
			std::vector<std::vector<size_t>> classes; /*!< The list of samples for each class */
	};
}
#endif
