/* Copyright 2007-2015 Yann LEYDIER, INSA-Lyon
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
 * file: CRNFeatureExtractorAngleZoning.cpp
 * \author Yann LEYDIER
 */

#include <CRNFeature/CRNFeatureExtractorAngleZoning.h>
#include <CRNException.h>
#include <CRNStatistics/CRNHistogram.h>
#include <CRNImage/CRNImageGradient.h>
#include <CRNBlock.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

/*****************************************************************************/
/*!
 * Returns a CRNPROTOCOL_FEATURE object computed from a block
 *
 * \param[in]	b	the block to process
 * \return a SHistogram containing the profiles
 */
SObject FeatureExtractorAngleZoning::Extract(Block &b)
{
	// create feature vector
	SVector feat = std::make_shared<Vector>(Protocol::Feature);
	for (int tmp = 0; tmp < xdiv * ydiv; ++tmp)
		feat->PushBack(std::make_shared<Histogram>(zonesize));
	// extract angles
	SImageGradient ig = b.GetGradient();
	FOREACHPIXEL(x, y, *ig)
	{
		if (ig->IsSignificant(x, y))
		{
			size_t index = Cap(x / xdiv, size_t(0), size_t(xdiv - 1)) + Cap(y / ydiv, size_t(0), size_t(ydiv - 1)) * xdiv;
			int angle = Cap((ig->At(x, y).theta.value * zonesize) / 256, 0, zonesize - 1);
			std::static_pointer_cast<Histogram>((*feat)[index])->IncBin(angle);
		}
	}
	// normalize
	for (SObject oh : feat)
	{
		SHistogram h(std::static_pointer_cast<Histogram>(oh));
		int tot = h->CumulateBins();
		if (tot != 0)
		{
			for (size_t tmp = 0; tmp < h->Size(); ++tmp)
			{
				h->SetBin(tmp, (h->GetBin(tmp) * maxval) / tot);
			}
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
SObject FeatureExtractorAngleZoning::ExtractWithMask(Block &b, ImageIntGray &mask)
{
	// create feature vector
	SVector feat = std::make_shared<Vector>(Protocol::Feature);
	for (int tmp = 0; tmp < xdiv * ydiv; ++tmp)
		feat->PushBack(std::make_shared<Histogram>(zonesize));
	// extract angles
	SImageGradient ig = b.GetGradient();
	int ccid = b.GetName().ToInt();
	int ox = b.GetRelativeBBox().GetLeft(); // offset from parent block
	int oy = b.GetRelativeBBox().GetTop();
	FOREACHPIXEL(x, y, *ig)
	{
		if (ig->IsSignificant(x, y) && (mask.At(x + ox, y + oy) == ccid))
		{
			size_t index = Cap(x / xdiv, size_t(0), size_t(xdiv - 1)) + Cap(y / ydiv, size_t(0), size_t(ydiv - 1)) * xdiv;
			int angle = Cap((ig->At(x, y).theta.value * zonesize) / 256, 0, zonesize - 1);
			std::static_pointer_cast<Histogram>((*feat)[index])->IncBin(angle);
		}
	}
	// normalize
	for (SObject oh : feat)
	{
		SHistogram h(std::static_pointer_cast<Histogram>(oh));
		int tot = h->CumulateBins();
		if (tot != 0)
		{
			for (size_t tmp = 0; tmp < h->Size(); ++tmp)
			{
				h->SetBin(tmp, (h->GetBin(tmp) * maxval) / tot);
			}
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
void FeatureExtractorAngleZoning::deserialize(xml::Element &el)
{
	if (el.GetName() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool FeatureExtractorAngleZoning::deserialize("
					"xml::Element &el): ") + _("Wrong XML element."));
	}
	int x = el.GetAttribute<int>("xdiv", false); // may throw
	int y = el.GetAttribute<int>("ydiv", false); // may throw
	int m = el.GetAttribute<int>("maxval", false); // may throw
	int z = el.GetAttribute<int>("zonesize", false); // may throw
	xdiv = x;
	ydiv = y;
	maxval = m;
	zonesize = z;
}

/*****************************************************************************/
/*! 
 * Dumps the object to an XML element. Unsafe. 
 */
xml::Element FeatureExtractorAngleZoning::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	el.SetAttribute("xdiv", xdiv);
	el.SetAttribute("ydiv", ydiv);
	el.SetAttribute("maxval", maxval);
	el.SetAttribute("zonesize", zonesize);
	return el;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(FeatureExtractorAngleZoning)
	CRN_DATA_FACTORY_REGISTER(U"FeatureExtractorAngleZoning", FeatureExtractorAngleZoning)
CRN_END_CLASS_CONSTRUCTOR(FeatureExtractorAngleZoning)

