/* Copyright 2012-2015 CoReNum, INSA-Lyon
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
 * file: CRNFeatureExtractorAngleRadialZoning.cpp
 * \author Yann LEYDIER
 */

#include <CRNFeature/CRNFeatureExtractorAngleRadialZoning.h>
#include <CRNException.h>
#include <CRNStatistics/CRNHistogram.h>
#include <CRNImage/CRNImageGray.h>
#include <CRNImage/CRNImageGradient.h>
#include <CRNBlock.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

static inline size_t chooseangle(const crn::Angle<ByteAngle> &a, const std::vector<crn::Angle<ByteAngle> > &angles)
{
	std::vector<uint8_t> distances(angles.size());
	for (size_t tmp = 0; tmp < angles.size(); ++tmp)
		distances[tmp] = crn::AngularDistance(a, angles[tmp]);
	return std::min_element(distances.begin(), distances.end()) - distances.begin();
}

/*****************************************************************************/
/*!
 * Returns a CRNPROTOCOL_FEATURE object computed from a block
 *
 * \param[in]	b	the block to process
 * \return a SHistogram containing the profiles
 */
SObject FeatureExtractorAngleRadialZoning::Extract(Block &b)
{
	SImageGray ig(b.GetGray());
	size_t xbar, ybar;
	if (autocenter)
	{
		size_t m00 = 0;
		xbar = 0;
		ybar = 0;
		FOREACHPIXEL(x, y, *ig)
		{
			int val = 255 - ig->At(x, y);
			m00 += val;
			xbar += x * val;
			ybar += y * val;
		}
		if (m00)
		{
			xbar /= m00;
			ybar /= m00;
		}
		else
		{
			xbar = ig->GetWidth() / 2;
			ybar = ig->GetHeight() / 2;
		}
	}
	else
	{
		xbar = ig->GetWidth() / 2;
		ybar = ig->GetHeight() / 2;
	}

	// create feature vector
	auto feat = std::make_shared<Vector>();
	for (size_t tmp = 0; tmp < nbzones; ++tmp)
		feat->PushBack(std::make_shared<Histogram>(zonesize));
	// extract angles
	SImageGradient igr = b.GetGradient();
	unsigned int cnt = 0;
	FOREACHPIXEL(x, y, *ig)
	{
		if (igr->IsSignificant(x, y))
		{
			size_t octant = chooseangle(crn::Angle<ByteAngle>::Atan(double(y) - double(ybar), double(x) - double(xbar)), zonetab);
			SHistogram h(std::static_pointer_cast<Histogram>((*feat)[octant]));
			h->IncBin(chooseangle(igr->At(x, y).theta, feattab));
			cnt += 1;
		}
	}
	// normalize
	if (cnt)
	{
		for (SObject oh : feat)
		{
			SHistogram h(std::static_pointer_cast<Histogram>(oh));
			for (size_t tmp = 0; tmp < zonesize; ++tmp)
				h->SetBin(tmp, (h->GetBin(tmp) * maxval) / cnt);
		}
	}
	return feat;
}

/*****************************************************************************/
/*!
 * Returns a CRNPROTOCOL_FEATURE object computed from a block
 *
 * \param[in]	b	the block to process
 * \param[in]	mask	the mask of the connected component to process
 * \return a SHistogram containing the profiles
 */
SObject FeatureExtractorAngleRadialZoning::ExtractWithMask(Block &b, ImageIntGray &mask)
{
	SImageGray ig(b.GetGray());
	size_t xbar, ybar;
	if (autocenter)
	{
		size_t m00 = 0;
		xbar = 0;
		ybar = 0;
		FOREACHPIXEL(x, y, *ig)
		{
			int val = 255 - ig->At(x, y);
			m00 += val;
			xbar += x * val;
			ybar += y * val;
		}
		if (m00)
		{
			xbar /= m00;
			ybar /= m00;
		}
		else
		{
			xbar = ig->GetWidth() / 2;
			ybar = ig->GetHeight() / 2;
		}
	}
	else
	{
		xbar = ig->GetWidth() / 2;
		ybar = ig->GetHeight() / 2;
	}

	// create feature vector
	auto feat = std::make_shared<Vector>();
	for (size_t tmp = 0; tmp < nbzones; ++tmp)
		feat->PushBack(std::make_shared<Histogram>(zonesize));
	// extract angles
	SImageGradient igr = b.GetGradient();
	int ccid = b.GetName().ToInt();
	int ox = b.GetRelativeBBox().GetLeft(); // offset from parent block
	int oy = b.GetRelativeBBox().GetTop();
	unsigned int cnt = 0;
	FOREACHPIXEL(x, y, *ig)
	{
		if (igr->IsSignificant(x, y) && (mask.At(x + ox, y + oy) == ccid))
		{
			size_t octant = chooseangle(crn::Angle<ByteAngle>::Atan(double(y) - double(ybar), double(x) - double(xbar)), zonetab);
			SHistogram h(std::static_pointer_cast<Histogram>((*feat)[octant]));
			h->IncBin(chooseangle(igr->At(x, y).theta, feattab));
			cnt += 1;
		}
	}
	// normalize
	if (cnt)
	{
		for (SObject oh : feat)
		{
			SHistogram h(std::static_pointer_cast<Histogram>(oh));
			for (size_t tmp = 0; tmp < zonesize; ++tmp)
				h->SetBin(tmp, (h->GetBin(tmp) * maxval) / cnt);
		}
	}
	return feat;
}

/*****************************************************************************/
/*! 
 * Initializes the object from an XML element. Unsafe. 
 * \throws	ExceptionInvalidArgument	not a FeatureExtractorProjection
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	wrong attribute
 */
void FeatureExtractorAngleRadialZoning::deserialize(xml::Element &el)
{
	if (el.GetName() != GetClassName())
	{
		throw ExceptionInvalidArgument(StringUTF8("void FeatureExtractorAngleRadialZoning::deserialize("
					"xml::Element &el): ") + _("Wrong XML element."));
	}
	int n = el.GetAttribute<int>("nbzones", false); // may throw
	int m = el.GetAttribute<int>("maxval", false); // may throw
	int z = el.GetAttribute<int>("zonesize", false); // may throw
	bool a = el.GetAttribute<bool>("autocenter", false); // may throw
	nbzones = n;
	maxval = m;
	zonesize = z;
	autocenter = a;
	initTables();
}

/*****************************************************************************/
/*! 
 * Dumps the object to an XML element. Unsafe. 
 */
xml::Element FeatureExtractorAngleRadialZoning::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName()));
	el.SetAttribute("nbzones", nbzones);
	el.SetAttribute("maxval", maxval);
	el.SetAttribute("zonesize", zonesize);
	el.SetAttribute("autocenter", autocenter ? 1 : 0);
	return el;
}

/*! creates tables to speed up the process */
void FeatureExtractorAngleRadialZoning::initTables()
{
	std::vector<Angle<ByteAngle> >(nbzones).swap(zonetab);
	for (size_t tmp = 0; tmp < nbzones; ++tmp)
		zonetab[tmp] = uint8_t((tmp * 255) / nbzones);
	std::vector<Angle<ByteAngle> >(zonesize).swap(feattab);
	for (size_t tmp = 0; tmp < zonesize; ++tmp)
		feattab[tmp] = uint8_t((tmp * 255) / zonesize);
}

CRN_BEGIN_CLASS_CONSTRUCTOR(FeatureExtractorAngleRadialZoning)
	CRN_DATA_FACTORY_REGISTER(U"FeatureExtractorAngleRadialZoning", FeatureExtractorAngleRadialZoning)
CRN_END_CLASS_CONSTRUCTOR(FeatureExtractorAngleRadialZoning)

