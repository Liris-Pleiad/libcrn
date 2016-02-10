/* Copyright 2006-2014 Yann LEYDIER, CoReNum, INSA-Lyon
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
 * file: CRNReal.cpp
 * \author Yann LEYDIER
 */

#include <CRNData/CRNReal.h>
#include <math.h>
#include <CRNException.h>
#include <CRNMath/CRNProp3.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNMath/CRNMath.h>
#include <CRNi18n.h>

using namespace crn;

/*****************************************************************************/
/*!
 * Conversion to string
 *
 * \return	The string value
 */
String Real::ToString() const
{
	return String(val);
}

/*****************************************************************************/
/*!
 * Distance between two metric objects.
 *
 * \param[in]	obj	The object to compute the distance with
 * \return	The distance
 */
double Real::distance(const Object &obj) const
{
	return Abs(val - static_cast<const Real&>(obj).val);
}

/*****************************************************************************/
/*!
 * Unsafe load
 *
 * \throws	ExceptionInvalidArgument	not a Real
 *
 * \param[in]	el	the element to load
 */
void Real::deserialize(xml::Element &el)
{
	if (el.GetValue() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("void Real::deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	val = el.GetAttribute<double>("value");
}

/*****************************************************************************/
/*!
 * Unsafe save
 *
 * \param[in]	parent	the parent element to which we will add the new element
 * \return The newly created element
 */
xml::Element Real::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	el.SetAttribute("value", val);
	return el;
}

/*! 
 * Unsafe method to compute a sum of vectors. 
 *
 * \param[in]	plist	a list of terms and coefficients
 * \return	the newly created object or nullptr if failed
 */
UObject Real::sum(const std::vector<std::pair<const Object*, double> > &plist) const 
{ 
	double acc = 0;
	for (size_t tmp = 0; tmp < plist.size(); tmp++)
		acc += plist[tmp].second * ((const Real*)plist[tmp].first)->GetValue();
	return std::make_unique<Real>(acc); 
}

/*! 
 * Unsafe Method to compute a mean of vectors. 
 *
 * \param[in]	plist	a list of terms and coefficients
 * \return	the newly created object or nullptr if failed
 */
UObject Real::mean(const std::vector<std::pair<const Object*, double> > &plist) const
{
	UReal ans(static_cast<Real*>(sum(plist).release()));
	double coeffs = 0;
	for (size_t tmp = 0; tmp < plist.size(); tmp++)
		coeffs += plist[tmp].second;
	ans->Mult(1.0 / coeffs);
	return std::forward<UReal>(ans);
}

/*! 
 * Unsafe internal product
 *
 * \param[in]	obj	the term
 */
void Real::mult(const Object &obj)
{
	const Real &r = static_cast<const Real&>(obj);
	val *= r.val;
}

/*! 
 * Unsafe internal division
 *
 * \param[in]	obj	the term
 */
void Real::div(const Object &obj)
{
	const Real &r = static_cast<const Real&>(obj);
	val /= r.val;
}

/*!
 * UNSAFE Greater or Equal
 *
 * \param[in]	l	the object to compare
 * \return	true if success, false else
 */
Prop3 Real::ge(const Object &l) const
{
	const Real &r = static_cast<const Real&>(l);
	return val >= r.val;
}

/*!
 * UNSAFE Lower or Equal 
 *
 * \param[in]	l	the object to compare
 * \return	true if success, false else
 */
Prop3 Real::le(const Object &l) const
{
	const Real &r = static_cast<const Real&>(l);
	return val <= r.val;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Real)
	CRN_DATA_FACTORY_REGISTER(U"Real", Real)
CRN_END_CLASS_CONSTRUCTOR(Real)

