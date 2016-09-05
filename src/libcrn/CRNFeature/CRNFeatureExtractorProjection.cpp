/* Copyright 2007-2016 Yann LEYDIER, INSA-Lyon, ENS-Lyon
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
 * file: CRNFeatureExtractorProjection.cpp
 * \author Yann LEYDIER
 */

#include <CRNFeature/CRNFeatureExtractorProjection.h>
#include <CRNImage/CRNImageBW.h>
#include <CRNException.h>
#include <CRNStatistics/CRNHistogram.h>
#include <CRNBlock.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNi18n.h>

using namespace crn;

/*! Constructor
 * 
 * \throws	ExceptionInvalidArgument	invalid direction
 *
 * \param[in]	orientations	the orientations to extract (Orientation::HORIZONTAL, Orientation::VERTICAL)
 * \param[in]	un_size	the unitary size of a projection (set to 0 for no resizing (Caution: this may lead to incomparable features!))
 * \param[in]	max_val	the maximal value in the projection (set to 0 for no scaling)
 */
FeatureExtractorProjection::FeatureExtractorProjection(Orientation orientations, int un_size, int max_val):
	dirs(orientations),
	size(un_size),
	maxval(max_val)
{
	if (dirs == Orientation::INVALID)
		throw ExceptionInvalidArgument(StringUTF8("FeatureExtractorProjection: ") + _("no orientation given."));
	if (size < 0)
		throw ExceptionDomain(crn::StringUTF8("FeatureExtractorProjection: ") + _("negative size."));
	if (max_val < 0)
		throw ExceptionDomain(crn::StringUTF8("FeatureExtractorProjection: ") + _("negative maximal value."));
}

/*****************************************************************************/
/*!
 * Returns a CRNPROTOCOL_FEATURE object computed from a block
 *
 * \param[in]	b	the block to process
 * \return a SHistogram containing the profiles
 */
SObject FeatureExtractorProjection::Extract(Block &b)
{
	SHistogram h = nullptr;
	if (!!(dirs & Orientation::HORIZONTAL))
	{
		// extract
		auto hp = HorizontalProjection(*b.GetBW());
		// scale
		if (maxval)
		{
			for (size_t tmp = 0; tmp < hp.Size(); ++tmp)
			{
				hp[tmp] *= maxval;
				hp[tmp] /= b.GetAbsoluteBBox().GetWidth();
			}
		}
		// resize
		if (size)
			hp.Resize(size);
		// append
		if (!h)
			h = CloneAs<Histogram>(hp);
		else
			h->Append(hp);
	}
	if (!!(dirs & Orientation::VERTICAL))
	{
		// extract
		auto hp = VerticalProjection(*b.GetBW());
		// scale
		if (maxval)
		{
			for (size_t tmp = 0; tmp < hp.Size(); ++tmp)
			{
				hp[tmp] *= maxval;
				hp[tmp] /= b.GetAbsoluteBBox().GetHeight();
			}
		}
		// resize
		if (size)
			hp.Resize(size);
		// append
		if (!h)
			h = CloneAs<Histogram>(hp);
		else
			h->Append(hp);
	}
	return h;
}

/*****************************************************************************/
/*! 
 * Initializes the object from an XML element. Unsafe. 
 *
 * \throws	ExceptionInvalidArgument	not a FeatureExtractorProjection
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	wrong attribute
 */
void FeatureExtractorProjection::deserialize(xml::Element &el)
{
	if (el.GetName() != GetClassName())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool FeatureExtractorProjection::deserialize("
					"xml::Element &el): ") + _("Wrong XML element."));
	}
	int d = el.GetAttribute<int>("direction", false); // may throw
	int s = el.GetAttribute<int>("size", false); // may throw
	int m = el.GetAttribute<int>("maxval", false); // may throw
	dirs = Orientation(d);
	size = s;
	maxval = m;
}

/*****************************************************************************/
/*! 
 * Dumps the object to an XML element. Unsafe. 
 */
xml::Element FeatureExtractorProjection::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName()));
	el.SetAttribute("directions", int(dirs));
	el.SetAttribute("size", size);
	el.SetAttribute("maxval", maxval);
	return el;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(FeatureExtractorProjection)
	CRN_DATA_FACTORY_REGISTER(U"FeatureExtractorProjection", FeatureExtractorProjection)
CRN_END_CLASS_CONSTRUCTOR(FeatureExtractorProjection)

