/* Copyright 2008-2016 INSA Lyon, CoReNum, INSA-Lyon
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
 * file: CRNkMeansClassifier.cpp
 * \author Yann LEYDIER
 */

#include <CRNi18n.h>
#include <CRNAI/CRNkMeansClassifier.h>
#include <CRNAI/CRNkMeans.h>
#include <CRNAI/CRNClassifResult.h>
#include <CRNData/CRNVector.h>
#include <CRNStatistics/CRNHistogram.h>
#include <CRNException.h>

using namespace crn;

/*!
 * Default constructor
 */
kMeansClassifier::kMeansClassifier()
{
}

/*!
 * Destructor.
 */
kMeansClassifier::~kMeansClassifier()
{
}

/*!
 * Sets the prototypes of the k-means
 *
 * \throws	ExceptionInvalidArgument	the model is not a Vector
 * \throws	ExceptionDomain	some samples were not added
 * \param[in]	newmodel	a SVector of prototypes
 */
void kMeansClassifier::SetModel(const SCVector &newmodel)
{
	classifier.ClearPrototypes();
	if (newmodel->GetClassName() == U"Vector")
	{
		SCVector model(std::static_pointer_cast<const Vector>(newmodel));
		if (classifier.AddPrototypes(model->begin(), model->end()) != model->Size())
			throw ExceptionDomain(_("Some samples were not added."));
	}
	else
	{
		throw ExceptionInvalidArgument(_("The model is not a Vector."));
	}
}

/*!
 * Updates the k-means with new samples
 *
 * \throws	ExceptionInvalidArgument	the model is not a Vector
 * \throws	ExceptionDomain	some samples were not added
 * \param[in]	data	a SVector of samples
 */
void kMeansClassifier::UpdateModel(const SCVector &data)
{
	classifier.ClearSamples();
	SCVector neighbors(std::static_pointer_cast<const Vector>(data));
	if (classifier.AddSamples(neighbors->begin(), neighbors->end()) != neighbors->Size())
		throw ExceptionDomain(_("Some samples were not added."));

	classifier.Run();
}

/*!
 * Classifies a sample.
 *
 * \param[in]	sample	the sample to classify
 * \return	a ClassifResult containing the index of the class, the distance to the prototype and a reference to the prototype.
 */
ClassifResult kMeansClassifier::Classify(const Object &sample)
{
	double dist;
	int id = classifier.Classify(sample, &dist);
	ClassifResult res(id, dist, classifier.GetPrototypes()[id]);
	return res;
}

