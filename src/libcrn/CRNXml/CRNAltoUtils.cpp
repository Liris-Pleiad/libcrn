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
 * file: CRNAltoUtils.cpp
 * \author Yann LEYDIER
 */

#include <CRNXml/CRNAltoUtils.h>
#include <algorithm>

using namespace crn;
using namespace xml;

/*! Gets the list of style references
 * \param[in]	el	the element
 * \return	the style references
 * \ingroup xml
 * \author Yann LEYDIER
 */
std::vector<Id> crn::xml::GetStyleRefs(const Element &el)
{
	std::vector<Id> styles;
	StringUTF8 str = el.GetAttribute<StringUTF8>("STYLEREFS");
	if (str.IsNotEmpty())
		styles = str.Split(" ");
	return styles;
}
	
/*! Adds a style reference to an element
 * \param[in]	el	the element
 * \param[in]	styles	the element's list of styles
 * \param[in]	id	the style to add
 * \ingroup xml
 * \author Yann LEYDIER
 */
void crn::xml::AddStyleRef(Element &el, const Id &id)
{
	std::vector<Id> styles(GetStyleRefs(el));
	if (std::find(styles.begin(), styles.end(), id) != styles.end())
		return;
	StringUTF8 av;
	for (const Id &sid : styles)
		av += sid + " ";
	av += id;
	el.SetAttribute("STYLEREFS", av);
}

/*! Removes a style reference to an element
 * \param[in]	el	the element
 * \param[in]	styles	the element's list of styles
 * \param[in]	id	the style to add
 * \ingroup xml
 * \author Yann LEYDIER
 */
void crn::xml::RemoveStyleRef(Element &el, const Id &id)
{
	std::vector<Id> tmpvec;
	std::vector<Id> styles(GetStyleRefs(el));
	std::remove_copy(styles.begin(), styles.end(), std::back_inserter(tmpvec), id);
	styles.swap(tmpvec);
	StringUTF8 av;
	for (const Id &sid : styles)
		av += sid + " ";
	el.SetAttribute("STYLEREFS", av);
}

