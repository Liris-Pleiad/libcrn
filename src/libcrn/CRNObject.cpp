/* Copyright 2006-2016 Yann LEYDIER, INSA-Lyon, CoReNum, ENS-Lyon
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
 * file: CRNObject.cpp
 * \author Yann LEYDIER
 */

#include <CRNObject.h>
#include <CRNException.h>
#include <CRNString.h>
#include <CRNMath/CRNProp3.h>
#include <math.h>
#include <CRNStringUTF8.h>
#include <CRNUtils/CRNXml.h>

using namespace crn;

const String& Object::GetClassName() const { static const String s; return s; }
/*!
 * Prints the object in a string
 *
 * \return	the string representing the object
 */
String Object::ToString() const
{
	return GetClassName(); 
}

/***************************************************************************/
/* Clonable informal protocol                                              */
/***************************************************************************/

/*****************************************************************************/
/*! 
 * Creates a new object, copied from this
 *
 * \return	A copied new object, or nullptr if failed
 */
UObject Object::Clone() const
{ 
	throw ExceptionProtocol(GetClassName().CStr() + StringUTF8("::Clone() not implemented"));
}

/***************************************************************************/
/* Serializable informal protocol                                          */
/***************************************************************************/

/*****************************************************************************/
/*! 
 * Initializes the object from an XML element. Safe. 
 *
 * \param[in]	el	the XML element to read
 */
void Object::Deserialize(xml::Element &el)
{
	deserialize(el);
	deserialize_internal_data(el);
}

/*****************************************************************************/
/*! 
 * Dumps the object to an XML element. Safe. 
 *
 * \param[in]	parent	the parent element to which we will add the new element
 * \return	The newly created element, nullptr if failed.
 */
xml::Element Object::Serialize(xml::Element &parent) const
{
	xml::Element el = serialize(parent);
	serialize_internal_data(el);
	return el;
}

/*****************************************************************************/
/*! 
 * Initializes the object from an XML element. Unsafe.
 *
 * \param[in]	el	the XML element to read
 */
void Object::deserialize(xml::Element &el) 
{ 
	throw ExceptionProtocol(GetClassName().CStr() + StringUTF8("::deserialize() not implemented"));
}

/*****************************************************************************/
/*! 
 * Dumps the object to an XML element. Unsafe. 
 *
 * \param[in]	parent	the parent element to which we will add the new element
 * \return	The newly created element, nullptr if failed.
 */
xml::Element Object::serialize(xml::Element &parent) const
{ 
	throw ExceptionProtocol(GetClassName().CStr() + StringUTF8("::serialize() not implemented"));
}

/*****************************************************************************/
/*! 
 * Internal. Initializes some internal data from an XML element. 
 *
 * \param[in]	el	the element that contains the serialized object
 */
void Object::deserialize_internal_data(xml::Element &el)
{
}

/*****************************************************************************/
/*! 
 * Internal. Dumps some internal data to an XML element.
 *
 * \param[in]	el	the element that contains the serialized object
 */
void Object::serialize_internal_data(xml::Element &el) const
{
}


/***************************************************************************/
/* Metric informal protocol                                                */
/***************************************************************************/

/*****************************************************************************/
/*!
 * Distance between two metric objects. Secure method.
 * It checks if the two objects are of the same type.
 *
 * \param[in]	obj	The object to compute the distance with
 * \return	The distance
 */
double Object::Distance(const Object &obj) const
{
	return distance(obj);
}

/*****************************************************************************/
/*!
 * Distance between two metric objects. Insecure method to be provided.
 * It checks if the two objects are of the same type.
 *
 * \param[in]	obj	The object to compute the distance with
 * \return	The distance, or HUGE_VAL (inf) if no object passed or not of the same class
 */
double Object::distance(const Object &obj) const 
{ 
	throw ExceptionProtocol(GetClassName().CStr() + StringUTF8("::distance() not implemented"));
}

/***************************************************************************/
/* Magma informal protocol                                                 */
/***************************************************************************/

/*****************************************************************************/
/*!
 * Safe method to add an object. 
 *
 * \param[in]	v	the term
 */
void Object::Add(const Object &v)
{
	add(v);
}

/*****************************************************************************/
/*!
 * Unsafe method to add an object. 
 *
 * \param[in]	v	the term
 */
void Object::add(const Object &v) 
{ 
	throw ExceptionProtocol(GetClassName().CStr() + StringUTF8("::add() not implemented"));
}

/*****************************************************************************/
/*! 
 * Safe method to test the equality of two vectors. 
 *
 * \param[in]	v	the term
 * \return true if success, false else
 */
bool Object::Equals(const Object &v) const
{
	return equals(v);
}

/*****************************************************************************/
/*! 
 * Unsafe method to test the equality of two vectors. 
 *
 * \param[in]	v	the term
 * \return true if success, false else
 */
bool Object::equals(const Object &v) const
{ 
	throw ExceptionProtocol(GetClassName().CStr() + StringUTF8("::equals() not implemented"));
}

/***************************************************************************/
/* Group informal protocol                                                 */
/***************************************************************************/

/*****************************************************************************/
/*! 
 * Safe method to subtract an object. 
 *
 * \param[in]	v	the term
 */
void Object::Sub(const Object &v)
{
	sub(v);
}

/*****************************************************************************/
/*! 
 * Unsafe method to subtract an object. 
 *
 * \param[in]	v	the term
 * \return true if success, false else
 */
void Object::sub(const Object &v) 
{ 
	throw ExceptionProtocol(GetClassName().CStr() + StringUTF8("::sub() not implemented"));
}

/***************************************************************************/
/* Vector over IR informal protocol                                        */
/***************************************************************************/

/*****************************************************************************/
/*! 
 * Multiply by a real 
 *
 * \param[in]	m	the factor
 */
void Object::Mult(double m)
{ 
	throw ExceptionProtocol(GetClassName().CStr() + StringUTF8("::Mult() not implemented"));
}

/*****************************************************************************/
/*! 
 * Safe method to compute a sum of vectors. 
 *
 * \throws	ExceptionDimension	empty list
 *
 * \param[in]	plist	a list of terms and coefficients
 * \return	the newly created object or nullptr if failed
 */
UObject Object::Sum(const std::vector<std::pair<const Object*, double> > &plist) const
{
	if (plist.size() == 0)
	{
		throw ExceptionDimension(GetClassName().CStr() + StringUTF8("::Sum(): empty list"));
	}
	return sum(plist);
}

/*****************************************************************************/
/*! 
 * Unsafe method to compute a sum of vectors. 
 *
 * \param[in]	plist	a list of terms and coefficients
 * \return	the newly created object or nullptr if failed
 */
UObject Object::sum(const std::vector<std::pair<const Object*, double> > &plist) const 
{ 
	throw ExceptionProtocol(GetClassName().CStr() + StringUTF8("::sum() not implemented"));
}

/*****************************************************************************/
/*! 
 * Safe Method to compute a mean of vectors. 
 *
 * \throws	ExceptionDimension	empty list
 *
 * \param[in]	plist	a list of terms and coefficients
 * \return	the newly created object or nullptr if failed
 */
UObject Object::Mean(const std::vector<std::pair<const Object*, double> > &plist) const
{
	if (plist.size() == 0)
	{
		throw ExceptionDimension(GetClassName().CStr() + StringUTF8("::Mean(): empty list"));
	}
	return mean(plist);
}

/*****************************************************************************/
/*! 
 * Unsafe Method to compute a mean of vectors. 
 *
 * \param[in]	plist	a list of terms and coefficients
 * \return	the newly created object or nullptr if failed
 */
UObject Object::mean(const std::vector<std::pair<const Object*, double> > &plist) const
{ 
	throw ExceptionProtocol(GetClassName().CStr() + StringUTF8("::mean() not implemented"));
}

/***************************************************************************/
/* Ring informal protocol                                                  */
/***************************************************************************/

/*****************************************************************************/
/*! 
 * Safe internal product
 *
 * \param[in]	obj	the term
 */
void Object::Mult(const Object &obj)
{
	mult(obj);
}

/*****************************************************************************/
/*! 
 * Unsafe internal product
 *
 * \param[in]	obj	the term
 */
void Object::mult(const Object &obj) 
{ 
	throw ExceptionProtocol(GetClassName().CStr() + StringUTF8("::mult() not implemented"));
}

/***************************************************************************/
/* Field informal protocol                                                 */
/***************************************************************************/

/*****************************************************************************/
/*! 
 * Unsafe internal division
 *
 * \param[in]	obj	the term
 */
void Object::Div(const Object &obj)
{
	div(obj);
}

/*****************************************************************************/
/*! 
 * Unsafe internal division
 *
 * \param[in]	obj	the term
 */
void Object::div(const Object &obj) 
{ 
	throw ExceptionProtocol(GetClassName().CStr() + StringUTF8("::div() not implemented"));
}

/***************************************************************************/
/* POSet informal protocol                                                 */
/***************************************************************************/

/*****************************************************************************/
/*!
 * Safe Greater or Equal
 *
 * \param[in]	l	the object to compare
 * \return	true if success, false else
 */
Prop3 Object::GE(const Object &l) const
{
	return ge(l);
}

/*****************************************************************************/
/*!
 * Safe Lower or equal
 *
 * \param[in]	l	the object to compare
 * \return	true if success, false else
 */
Prop3 Object::LE(const Object &l) const
{
	return le(l);
}

/*****************************************************************************/
/*!
 * Safe Greater Than
 *
 * \param[in]	l	the object to compare
 * \return	true if success, false else
 */
Prop3 Object::GT(const Object &l) const
{
	return !LE(l);
}

/*****************************************************************************/
/*!
 * Safe Lower Than
 *
 * \param[in]	l	the object to compare
 * \return	true if success, false else
 */
Prop3 Object::LT(const Object &l) const
{
	return !GE(l);
}

/*****************************************************************************/
/*!
 * UNSAFE Greater or Equal
 *
 * \param[in]	l	the object to compare
 * \return	true if success, false else
 */
Prop3 Object::ge(const Object &l) const
{ 
	throw ExceptionProtocol(GetClassName().CStr() + StringUTF8("::ge() not implemented"));
}

/*****************************************************************************/
/*!
 * UNSAFE Lower or Equal 
 *
 * \param[in]	l	the object to compare
 * \return	true if success, false else
 */
Prop3 Object::le(const Object &l) const
{ 
	throw ExceptionProtocol(GetClassName().CStr() + StringUTF8("::le() not implemented"));
}

