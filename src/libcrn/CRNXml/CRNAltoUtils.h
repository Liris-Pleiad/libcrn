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
 * file: CRNAltoUtils.h
 * \author Yann LEYDIER
 */

#ifndef CRNAltoUtils_HEADER
#define CRNAltoUtils_HEADER

#include <CRNXml/CRNXml.h>

namespace crn
{
	namespace xml
	{
		using Id = crn::StringUTF8;

		/*! \brief Gets the list of style references */
		std::vector<Id> GetStyleRefs(const Element &el);
		/*! \brief Adds a style reference to an element */
		void AddStyleRef(Element &el, const Id &id);
		/*! \brief Removes a style reference to an element */
		void RemoveStyleRef(Element &el, const Id &id);
	}
}

#endif

