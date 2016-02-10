/* Copyright 2008-2014 INSA Lyon, CoReNum, INSA-Lyon
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
 * file: CRNkMeansClassifier.h
 * \author Yann LEYDIER
 */

#ifndef CRNKMEANSCLASSIFIER_HEADER
#define CRNKMEANSCLASSIFIER_HEADER

#include <CRNAI/CRNClassifier.h>
#include <CRNAI/CRNkMeans.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief A classifier based on k-means
	 *
	 * A classifier based on k-means
	 *
	 * \author 	Yann LEYDIER
	 * \date		July 2008
	 * \version 0.2
	 * \ingroup classify
	 */
	class kMeansClassifier : public Classifier
	{
		public:
			/*! \brief Constructor */
			kMeansClassifier();
			/*! \brief Destructor */
			virtual ~kMeansClassifier() override;

			kMeansClassifier(const kMeansClassifier &) = delete;
			kMeansClassifier& operator=(const kMeansClassifier &) = delete;
			kMeansClassifier(kMeansClassifier &&) = default;
			kMeansClassifier& operator=(kMeansClassifier &&) = default;

			/*! \brief Sets the model of the classifier */
			virtual void SetModel(const SCVector &newmodel) override;
			/*! \brief Updates the model of the classifier using contextual data */
			virtual void UpdateModel(const SCVector &data) override;
			/*! \brief Classifies a sample */
			virtual ClassifResult Classify(const Object &sample) override;

		private:
			kMeans classifier; /*!< The actual k-means engine */
	};

	CRN_ALIAS_SMART_PTR(kMeansClassifier)
}
#endif
