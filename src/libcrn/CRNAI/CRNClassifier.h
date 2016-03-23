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
 * file: CRNClassifier.h
 * \author Yann LEYDIER
 */

#ifndef CRNCLASSIFIER_HEADER
#define CRNCLASSIFIER_HEADER

#include <CRNComplexObject.h>
#include <CRNData/CRNVectorPtr.h>

/*! \defgroup ai	Artificial Intelligence and Machine learning */

namespace crn
{
	class ClassifResult;

	/*! \defgroup classify Classification 
	 * \ingroup	ai */

	/****************************************************************************/
	/*! \brief Base abstract classifier class
	 * 
	 * Base abstract classifier class
	 * 
	 * \author 	Yann LEYDIER
	 * \date		July 2008
	 * \version 0.3
	 * \ingroup classify
	 */
	class Classifier
	{
		public:
			/*! \brief Sets the model of the classifier */
			virtual void SetModel(const SCVector &newmodel) = 0;
			/*! \brief Updates the model of the classifier using contextual data */
			virtual void UpdateModel(const SCVector &data) = 0;
			/*! \brief Classifies a sample */
			virtual ClassifResult Classify(const Object &sample) = 0;
	};

	CRN_ALIAS_SMART_PTR(Classifier)
}
#endif
