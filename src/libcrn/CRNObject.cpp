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

using namespace crn;

UObject crn::Clone(const Object &obj)
{
	return Cloner::Clone(obj);
}

void crn::Deserialize(Object &obj, xml::Element &el)
{
	Serializer::Deserialize(obj, el);
}

xml::Element crn::Serialize(const Object &obj, xml::Element &parent)
{
	return Serializer::Serialize(obj, parent);
}

