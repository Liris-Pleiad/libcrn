/* Copyright 2007-2014 Yann LEYDIER, CoReNum, INSA-Lyon
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
 * file: CRNRealCoeff.cpp
 * \author Yann LEYDIER
 */

#include <CRNData/CRNRealCoeff.h>
#include <CRNException.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

/*****************************************************************************/
/*!
 * Conversion to string
 *
 * \return	The string value
 */
String RealCoeff::ToString() const
{
	return String(GetValue()) + U' ' + String(coeff);
}

/*****************************************************************************/
/*!
 * Unsafe load
 *
 * \throws	ExceptionInvalidArgument	not a RealCoeff
 *
 * \param[in]	el	the element to load
 */
void RealCoeff::deserialize(xml::Element &el)
{
	if (el.GetValue() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("void RealCoeff::deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	SetValue(el.GetAttribute<double>("value"));
	coeff = el.GetAttribute<double>("coeff");
}

/*****************************************************************************/
/*!
 * Unsafe save
 *
 * \param[in]	parent	the parent element to which we will add the new element
 * \return The newly created element, nullptr if failed.
 */
xml::Element RealCoeff::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	el.SetAttribute("value", GetValue());
	el.SetAttribute("coeff", coeff);

	return el;
}

/*!
 * Unsafe method to add an object. 
 *
 * \param[in]	v	the term
 */
void RealCoeff::add(const Object &v)
{
	SetValue(GetValue() * coeff);
	const RealCoeff &r = static_cast<const RealCoeff&>(v);
	SetValue(GetValue() + r.GetValue() * r.GetCoeff());
	coeff = 1;
}

/*! 
 * Unsafe method to subtract an object. 
 *
 * \param[in]	v	the term
 */
void RealCoeff::sub(const Object &v)
{
	SetValue(GetValue() * coeff);
	const RealCoeff &r = static_cast<const RealCoeff&>(v);
	SetValue(GetValue() - r.GetValue() * r.GetCoeff());
	coeff = 1;
}

/*! 
 * Unsafe method to compute a sum of vectors. 
 *
 * \param[in]	plist	a list of terms and coefficients
 * \return	the newly created object or nullptr if failed
 */
UObject RealCoeff::sum(const std::vector<std::pair<const Object*, double> > &plist) const
{
	if (plist.size() == 0)
		return nullptr;
	Protocol id = plist[0].first->GetClassProtocols();
	for (size_t tmp = 1; tmp < plist.size(); tmp++)
		if (plist[tmp].first->GetClassProtocols() != id)
			return nullptr;
	double s = 0;
	for (size_t tmp = 0; tmp < plist.size(); tmp++)
	{
		const RealCoeff *tv = static_cast<const RealCoeff*>(plist[tmp].first);
		s += tv->GetValue() * tv->GetCoeff() * plist[tmp].second;
	}
	return std::make_unique<RealCoeff>(s, 1);
}

/*! 
 * Unsafe Method to compute a mean of vectors. 
 *
 * \param[in]	plist	a list of terms and coefficients
 * \return	the newly created object or nullptr if failed
 */
UObject RealCoeff::mean(const std::vector<std::pair<const Object*, double> > &plist) const
{
	if (plist.size() == 0)
		return nullptr;
	Protocol id = plist[0].first->GetClassProtocols();
	for (size_t tmp = 1; tmp < plist.size(); tmp++)
		if (plist[tmp].first->GetClassProtocols() != id)
			return nullptr;
	double s = 0;
	double c = 0;
	for (size_t tmp = 0; tmp < plist.size(); tmp++)
	{
		const RealCoeff *tv = static_cast<const RealCoeff*>(plist[tmp].first);
		s += tv->GetValue() * tv->GetCoeff() * plist[tmp].second;
		c += tv->GetCoeff() * plist[tmp].second;
	}
	s /= c;
	return std::make_unique<RealCoeff>(s, 1);
}

/*! 
 * Unsafe internal product
 *
 * \param[in]	obj	the term
 */
void RealCoeff::mult(const Object &obj)
{
	const RealCoeff &r = static_cast<const RealCoeff&>(obj);
	SetValue(GetValue() * r.GetValue());
	coeff *= r.coeff;
}

/*! 
 * Unsafe internal division
 *
 * \param[in]	obj	the term
 */
void RealCoeff::div(const Object &obj)
{
	const RealCoeff &r = static_cast<const RealCoeff&>(obj);
	SetValue(GetValue() * coeff / (r.GetValue() * r.coeff));
	coeff = 1;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(RealCoeff)
	CRN_DATA_FACTORY_REGISTER(U"RealCoeff", RealCoeff)
CRN_END_CLASS_CONSTRUCTOR(RealCoeff)


