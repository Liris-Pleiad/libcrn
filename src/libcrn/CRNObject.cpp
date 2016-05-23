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
#include <CRNProtocols.h>
#include <CRNData/CRNInt.h>
#include <CRNData/CRNReal.h>
#include <CRNMath/CRNProp3.h>
#include <CRNi18n.h>

using namespace crn;
using namespace crn::literals;

/*! Clones an object if possible
 * \throws	ExceptionProtocol	not a clonable object
 */
UObject crn::Clone(const Object &obj)
{
	return Cloner::Clone(obj);
}

/*! Clones an object if possible
 * \throws	ExceptionUninitialized	null object
 * \throws	ExceptionProtocol	not a clonable object
 */
UObject crn::Clone(const UCObject &obj)
{
	if (!obj)
		throw ExceptionUninitialized("Clone(): "_s + _("null pointer."));
	return Clone(*obj);
}

/*! Clones an object if possible
 * \throws	ExceptionUninitialized	null object
 * \throws	ExceptionProtocol	not a clonable object
 */
UObject crn::Clone(const SCObject &obj)
{
	if (!obj)
		throw ExceptionUninitialized("Clone(): "_s + _("null pointer."));
	return Clone(*obj);
}

UInt crn::Clone(int i)
{
	return std::make_unique<Int>(i);
}

UReal crn::Clone(double d)
{
	return std::make_unique<Real>(d);
}

UProp3 crn::Clone(bool b)
{
	return std::make_unique<Prop3>(b);
}

/*! Reads an object from XML if possible
 * \throws	ExceptionProtocol	not a serializable object
 */
void crn::Deserialize(Object &obj, xml::Element &el)
{
	Serializer::Deserialize(obj, el);
}

/*! Reads an object from XML if possible
 * \throws	ExceptionUninitialized	null object
 * \throws	ExceptionProtocol	not a serializable object
 */
void crn::Deserialize(const UObject &obj, xml::Element &el)
{
	if (!obj)
		throw ExceptionUninitialized("Deserialize(): "_s + _("null pointer."));
	Deserialize(*obj, el);
}

/*! Reads an object from XML if possible
 * \throws	ExceptionUninitialized	null object
 * \throws	ExceptionProtocol	not a serializable object
 */
void crn::Deserialize(const SObject &obj, xml::Element &el)
{
	if (!obj)
		throw ExceptionUninitialized("Deserialize(): "_s + _("null pointer."));
	Deserialize(*obj, el);
}

/*! Writes an object to XML if possible
 * \throws	ExceptionProtocol	not a serializable object
 */
xml::Element crn::Serialize(const Object &obj, xml::Element &parent)
{
	return Serializer::Serialize(obj, parent);
}

/*! Writes an object to XML if possible
 * \throws	ExceptionProtocol	not a serializable object
 */
xml::Element crn::Serialize(const UCObject &obj, xml::Element &parent)
{
	if (!obj)
		throw ExceptionUninitialized("Serialize(): "_s + _("null pointer."));
	return Serialize(*obj, parent);
}

/*! Writes an object to XML if possible
 * \throws	ExceptionProtocol	not a serializable object
 */
xml::Element crn::Serialize(const SCObject &obj, xml::Element &parent)
{
	if (!obj)
		throw ExceptionUninitialized("Serialize(): "_s + _("null pointer."));
	return Serialize(*obj, parent);
}

/*! Distance between two objects
 * \throws	ExceptionProtocol	not a metric object
 */
double crn::Distance(const Object &o1, const Object &o2)
{
	return Ruler::ComputeDistance(o1, o2);
}

/*! Distance between two objects 
 * \throws	ExceptionUninitialized	null object
 * \throws	ExceptionProtocol	not a metric object
 */
double crn::Distance(const UCObject &o1, const UCObject &o2)
{
	if (!o1 || !o2)
		throw ExceptionUninitialized("Distance(): "_s + _("null pointer."));
	return Distance(*o1, *o2);
}

/*! Distance between two objects 
 * \throws	ExceptionUninitialized	null object
 * \throws	ExceptionProtocol	not a metric object
 */
double crn::Distance(const SCObject &o1, const SCObject &o2)
{
	if (!o1 || !o2)
		throw ExceptionUninitialized("Distance(): "_s + _("null pointer."));
	return Distance(*o1, *o2);
}

