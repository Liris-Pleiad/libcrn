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

#include <CRNString.h>
#include <CRNStatistics/CRNHistogramPtr.h>
#include <CRNAI/CRNkMeansPtr.h>

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
	 * \version 0.2
	 * \ingroup cluster
	 */
	class kMeans
	{
		// TODO
#if 0
		public:
			/*! \brief Blank constructor */
			kMeans();
			/*! \brief Destructor */
			~kMeans();

			kMeans(const kMeans&) = delete;
			kMeans(kMeans&&) = default;
			kMeans& operator=(const kMeans&) = delete;
			kMeans& operator=(kMeans&&) = default;

			/*! \brief Dumps a short summary to a string */
			String ToString() const;

			/*! \brief Adds one prototype */
			void AddPrototype(const Object &sam);
			/*! \brief Adds one random prototype out of the samples pool */
			void AddRandomPrototype();
			/*! \brief Adds a std::vector of prototypes
			 *
			 * Adds a vector of prototypes if their class is the same as the dataclass.
			 * If it is the first sample, then the dataclass is set to its class.
			 * The prototypes are copied.
			 *
			 * \param[in]	beg	An iterator on the first object
			 * \param[in]	en	An iterator after the last object
			 * \return	the number of samples added
			 */
			template<typename ITER> size_t AddPrototypes(ITER beg, ITER en)
			{
				size_t nb = 0;
				for (ITER it = beg; it != en; ++it)
				{
					try
					{
						AddPrototype(**it);
						nb += 1;
					}
					catch (...) { }
				}
				return nb;
			}
			/*! \brief Returns the number of classes */
			size_t GetNbClasses() const noexcept { return proto.size(); }
			/*! \brief Returns the vector of prototypes */
			const std::vector<SObject>& GetPrototypes() noexcept { return proto; }
			/*! \brief Clears the prototypes */
			void ClearPrototypes() noexcept { proto.clear(); }

			/*! \brief Adds one sample */
			void AddSample(const SCObject &sam);
			/*! \brief Adds a vector of samples 
			 *
			 * Adds a vector of sample if their class is the same as the dataclass.
			 * If it is the first sample, then the dataclass is set to its class.
			 * The samples are referenced.
			 *
			 * \param[in]	beg	An iterator on the first object
			 * \param[in]	en	An iterator after the last object
			 * \return	the number of samples added
			 */
			template<typename ITER> size_t AddSamples(ITER beg, ITER en)
			{
				size_t nb = 0;
				for (ITER it = beg; it != en; ++it)
				{
					try
					{
						AddSample(*it);
						nb += 1;
					}
					catch (...) { }
				}
				return nb;
			}

			/*! \brief Adds samples from histogram */
			size_t AddSamples(const Histogram &histo);
			/*! \brief Returns the number of samples */
			size_t GetNbSamples() const noexcept { return data.size(); }
			/*! \brief Returns the vector of samples */
			const std::vector<SCObject>& GetSamples() noexcept { return data; }
			/*! \brief Clears the samples */
			void ClearSamples() noexcept { data.clear(); }

			/*! \brief Runs the k-means */
			int Run(int maxcnt = 100);
			/*! \brief Finds the closest prototype */
			int Classify(const Object &obj, double *distance = nullptr);

			/*! \brief Returns the content of one class */
			const std::vector<SCObject>& GetClass(size_t k);

		private:
			std::vector<SCObject> data; /*!< The samples */
			std::vector<SObject> proto; /*!< The prototypes */
			std::vector<std::vector<SCObject>> classes; /*!< The list of samples for each class */
			String dataclass; /*!< The class name of the samples and prototypes */
#endif
	};
}
#endif
