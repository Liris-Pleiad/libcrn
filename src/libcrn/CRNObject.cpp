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

using namespace crn;

/*! Clones an object if possible
 * \throws	ExceptionProtocol	not a clonable object
 */
UObject crn::Clone(const Object &obj)
{
	return Cloner::Clone(obj);
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

/*! Writes an object to XML if possible
 * \throws	ExceptionProtocol	not a serializable object
 */
xml::Element crn::Serialize(const Object &obj, xml::Element &parent)
{
	return Serializer::Serialize(obj, parent);
}

/*! Distance between two objects */
double crn::Distance(const Object &o1, const Object &o2)
{
	return Ruler::Distance(o1, o2);
}

