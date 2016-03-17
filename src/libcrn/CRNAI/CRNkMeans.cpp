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
 * file: CRNkMeans.cpp
 * \author Yann LEYDIER
 */

#include <CRNAI/CRNkMeans.h>
#include <CRNException.h>
#include <CRNAI/CRNBasicClassify.h>
#include <CRNStatistics/CRNHistogram.h>
#include <CRNIO/CRNIO.h>
#include <CRNi18n.h>

using namespace crn;

#if 0
const String DEFNAME(U"unknown class");
static const Protocol CLASSREQ(crn::Protocol::VectorOverR|crn::Protocol::Metric|crn::Protocol::Clonable);

/*!
 * Constructor
 */
kMeans::kMeans():
	dataclass(DEFNAME)
{
}

/*!
 * Destructor
 */
kMeans::~kMeans()
{
}

/*!
 * Adds one sample if its class is the same as the dataclass.
 * If it is the first sample, then the dataclass is set to its class.
 * The sample is referenced.
 *
 * \throws	ExceptionInvalidArgument	The sample is not compatible with the already existing data.
 * \param[in]	sam	The sample to add
 */
void kMeans::AddSample(const SCObject &sam)
{
	if (dataclass == DEFNAME)
	{
		dataclass = sam->GetClassName();
		data.push_back(sam);
	}
	else
	{
		if (sam->GetClassName() == dataclass)
		{
			data.push_back(sam);
		}
		else
			throw ExceptionInvalidArgument(_("The sample is not compatible with the already existing data."));
	}
}

/*!
 * Dumps a summary to a string
 *
 * \return	A string describing the engine
 */
String kMeans::ToString() const
{ 
	String s = U"k-means of <" + dataclass;
	s += U"> with ";
	s += GetNbClasses();
	s += U" classes on ";
	s += GetNbSamples();
	s += U" samples";
	return s;
}

/*!
 * Adds a vector of sample if their class is the same as the dataclass.
 * If it is the first sample, then the dataclass is set to its class.
 * The samples are referenced.
 *
 * \param[in]	histo	the histogram
 * \return	the number of samples added
 */
size_t kMeans::AddSamples(const Histogram &histo)
{
	if (!histo.Size())
	{
		CRNWarning(String(U"int kMeans::AddSamples(const Histogram &histo): ") + 
				_("Void histogram."));
		return 0;
	}

	size_t cnt = 0;
	for (size_t tmp = 0; tmp < histo.Size(); tmp++)
	{
		SRealCoeff s = std::make_shared<RealCoeff>(double(tmp), histo.GetBin(tmp));
		try
		{
			AddSample(s);
			cnt += 1;
		}
		catch (...) { }
	}
	return cnt;
}

/*!
 * Adds one prototype if its class is the same as the dataclass.
 * If it is the first sample, then the dataclass is set to its class.
 * The prototype is copied.
 *
 * \throws	ExceptionInvalidArgument	The sample is not compatible with the already existing data.
 * \param[in]	sam	The prototype to add
 */
void kMeans::AddPrototype(const Object &sam)
{
	if (dataclass == DEFNAME)
	{
		dataclass = sam.GetClassName();
		proto.push_back(sam.Clone());
	}
	else
	{
		if (sam.GetClassName() == dataclass)
		{
			proto.push_back(sam.Clone());
		}
		else
			throw ExceptionInvalidArgument(_("The sample is not compatible with the already existing data."));
	}
}

/*!
 * Adds one random prototype out of the samples pool.
 * The prototype is copied.
 *
 * \throws	ExceptionInvalidArgument	The sample is not compatible with the already existing data.
 * \throws	ExceptionNotFound	No sample available
 */
void kMeans::AddRandomPrototype()
{
	size_t nb = GetNbSamples();
	if (!nb)
		throw ExceptionNotFound(_("No sample available."));
	float ran = ((float)rand() * (float)nb) / (float)RAND_MAX;
	size_t choice = (size_t)ran;
	if (choice == nb) 
	{
		choice = nb - 1;
	}
	AddPrototype(*data[choice]);
}

/*!
 * Runs the k-means
 *
 * \param[in]	maxcnt	maximal number of iterations
 * \return	the number of iterations
 */
int kMeans::Run(int maxcnt)
{
	int cnt = 0;
	size_t k = proto.size();
	classes.clear();
	for (size_t tmp = 0; tmp < k; tmp++)
	{
		classes.push_back(std::vector<SCObject>{});
	}
	std::vector<size_t> popclasses(k);
	std::fill(popclasses.begin(), popclasses.end(), 0);
	bool finished = false;
	while (!finished)
	{
		for (std::vector<SCObject> &c : classes)
			c.clear();
		// for each sample
		for (auto & elem : data)
		{
			// classify
			int c = Classify(*elem);
			if (c == -1)
			{
				CRNWarning(String(U"int kMeans::Run(): ") + 
						_("Error classifying a sample!"));
			}
			else
				classes[c].push_back(elem);
		}
		finished = true;
		// for each class
		for (size_t p = 0; p < k; p++)
		{
			// compute mean
			std::vector<std::pair<const Object*, double> > tomean;
			for (auto & elem : classes[p])
				tomean.push_back(std::make_pair(elem.get(), 1));
			try
			{
				SObject td = data.front()->Mean(tomean);
				// store prototype
				proto[p] = td;
			}
			catch (...)
			{ // the list was empty, do not update the prototype
			}
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

/*!
 * Returns the content of one class
 *
 * \param[in]	k	The id of the class
 * \return	the content of class k
 */
const std::vector<SCObject>& kMeans::GetClass(size_t k)
{
	if (k < classes.size())
		return classes[k];
	else
	{
		throw ExceptionNotFound(StringUTF8("kMeans::GetClass(size_t k): ") + 
				_("Wrong class number."));
	}
}

/*!
 * Finds the closest prototype.
 *
 * \param[in]	obj	the sample to classify
 * \param[out]	distance	the distance to the closest prototype
 * \return	the index of the closest prototype or -1 if failed
 */
int kMeans::Classify(const Object &obj, double *distance)
{
	if (obj.GetClassName() != dataclass)
		return -1;
	if (proto.empty())
		return -1;

	ClassifResult res = BasicClassify::NearestNeighbor(obj, proto.begin(), proto.end());
	if (distance)
		*distance = res.distance;
	return res.class_id;
}

#endif
