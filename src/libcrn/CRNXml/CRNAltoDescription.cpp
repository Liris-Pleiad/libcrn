/* Copyright 2011-2016 CoReNum
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
 * file: CRNAltoDescription.cpp
 * \author Yann LEYDIER
 */

#include <CRNXml/CRNAlto.h>
#include <CRNException.h>
#include <CRNi18n.h>

using namespace crn;
using namespace xml;

/*! Constructor from file
 * \param[in]	el	the xml element to read
 */
Alto::Description::Description(const Element &el):
	Element(el)
{
}

/*! Constructor from file name (measurement unit is always Pixel)
 * \throws	crn::ExceptionInvalidArgument	null filename
 * \param[in]	imagename	the image wrapped in the alto
 */
Alto::Description::Description(const Element &el, const Path &imagename):
	Element(el)
{
	if (imagename.IsEmpty())
		throw ExceptionInvalidArgument(_("Null image filename."));
	Element sel = PushBackElement("MesurementUnit");
	sel.PushBackText("pixel");
	sel = PushBackElement("sourceImageInformation");
	sel = sel.PushBackElement("fileName");
	Path uri(imagename);
	uri.ToURI();
	sel.PushBackText(uri);
}

/*!
 * \throws	ExceptionDomain	invalid measurement unit
 * \return	the measurement unit of all metrics in the Alto
 */
Option<Alto::Description::MeasurementUnit> Alto::Description::GetMeasurementUnit()
{
	Option<MeasurementUnit> munit;
	StringUTF8 mu;
	Element e = GetFirstChildElement("MeasurementUnit");
	if (e)
	{
		mu = e.GetFirstChildText();
		mu.ToLower();
		if (mu == "pixel")
			munit = MeasurementUnit::Pixel;
		else if (mu == "mm10")
			munit = MeasurementUnit::MM;
		else if (mu == "inch1200")
			munit = MeasurementUnit::Inch;
		else
			throw ExceptionDomain(_("Invalid measurement unit."));
	}
	return munit;
}

/*!
 * \return	the filename of the image described by the Alto
 */
Option<StringUTF8> Alto::Description::GetFilename()
{
	Option<StringUTF8> fn;
	Element e = GetFirstChildElement("sourceImageInformation");
	if (e)
	{
		e = e.GetFirstChildElement("fileName");
		if (e)
			fn = e.GetFirstChildText();
	}
	return fn;
}


