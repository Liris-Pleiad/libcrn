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
 * file: CRNFeatureExtractorProfile.cpp
 * \author Yann LEYDIER
 */

#include <CRNFeature/CRNFeatureExtractorProfile.h>
#include <CRNImage/CRNImageBW.h>
#include <CRNException.h>
#include <CRNStatistics/CRNHistogram.h>
#include <CRNBlock.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

/*! Constructor
 * 
 * \throws	ExceptionDomain	negative size or maximal value
 * \throws	ExceptionInvalidArgument	invalid direction
 *
 * \param[in]	directions	the directions to extract (Directions::LEFT, Directions::RIGHT, Directions::TOP, Directions::BOTTOM).
 * \param[in]	un_size	the unitary size of a profile (set to 0 for no resizing (Caution: this may lead to incomparable features!))
 * \param[in]	max_val	the maximal value in the profiles (set to 0 for no scaling)
 */
FeatureExtractorProfile::FeatureExtractorProfile(Direction directions, int un_size, int max_val):
	dirs(directions),
	size(un_size),
	maxval(max_val)
{
	if (dirs == Direction::INVALID)
		throw ExceptionInvalidArgument(StringUTF8("FeatureExtractorProfile: ") + _("no direction given."));
	if (size < 0)
		throw ExceptionDomain(crn::StringUTF8("FeatureExtractorProfile: ") + _("negative size."));
	if (max_val < 0)
		throw ExceptionDomain(crn::StringUTF8("FeatureExtractorProfile: ") + _("negative maximal value."));
}

/*****************************************************************************/
/*!
 * Returns a CRNPROTOCOL_FEATURE object computed from a block
 *
 * \param[in]	b	the block to process
 * \return a SHistogram containing the profiles
 */
SObject FeatureExtractorProfile::Extract(Block &b)
{
	SHistogram h = nullptr;
	if (!!(dirs & Direction::LEFT))
	{
		// extract
		auto hp = LeftProfile(*b.GetBW());
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
			h = hp.CloneAs<Histogram>();
		else
			h->Append(hp);
	}
	if (!!(dirs & Direction::RIGHT))
	{
		// extract
		auto hp = RightProfile(*b.GetBW());
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
			h = hp.CloneAs<Histogram>();
		else
			h->Append(hp);
	}
	if (!!(dirs & Direction::TOP))
	{
		// extract
		auto hp = TopProfile(*b.GetBW());
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
			h = hp.CloneAs<Histogram>();
		else
			h->Append(hp);
	}
	if (!!(dirs & Direction::BOTTOM))
	{
		// extract
		auto hp = BottomProfile(*b.GetBW());
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
			h = hp.CloneAs<Histogram>();
		else
			h->Append(hp);
	}
	return h;
}

/*****************************************************************************/
/*! 
 * Initializes the object from an XML element. Unsafe. 
 * \throws	ExceptionInvalidArgument	not a FeatureExtractorProjection
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	wrong attribute
 */
void FeatureExtractorProfile::deserialize(xml::Element &el)
{
	if (el.GetName() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool FeatureExtractorProfile::deserialize("
					"xml::Element &el): ") + _("Wrong XML element."));
	}
	int d = el.GetAttribute<int>("directions", false); // may throw
	int s = el.GetAttribute<int>("size", false); // may throw
	int m = el.GetAttribute<int>("maxval", false); // may throw
	dirs = Direction(d);
	size = s;
	maxval = m;
}

/*****************************************************************************/
/*! 
 * Dumps the object to an XML element. Unsafe. 
 */
xml::Element FeatureExtractorProfile::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	el.SetAttribute("directions", int(dirs));
	el.SetAttribute("size", size);
	el.SetAttribute("maxval", maxval);
	return el;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(FeatureExtractorProfile)
	CRN_DATA_FACTORY_REGISTER(U"FeatureExtractorProfile", FeatureExtractorProfile)
CRN_END_CLASS_CONSTRUCTOR(FeatureExtractorProfile)

