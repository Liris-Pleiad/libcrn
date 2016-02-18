/* Copyright 2010-2016 CoReNum, INSA-Lyon
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
 * file: CRNDefaultAction.cpp
 * \author Yann LEYDIER
 */

#include <CRNUtils/CRNDefaultAction.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

void Action::deserialize(xml::Element &el)
{
}

xml::Element Action::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	return el;
}

/*! Singleton instance 
 * \return  the instance
 */
DefaultAction& DefaultAction::getInstance()
{
	static DefaultAction actions;
	return actions;
}

/*!
 * Gets a default action
 * \param[in]  name  the key name of the action
 * \return  the action functor or nullptr if none was found
 */
SAction DefaultAction::GetAction(const String &name)
{
	Map &act(getInstance().actions);
	auto it = act.Find(name);
	if (it != act.end())
		return std::static_pointer_cast<Action>(it->second);
	else
		return nullptr;
}

/*!
 * Sets a default action
 * \param[in]  name  the key name of the action
 * \param[in]  action  the functor
 */
void DefaultAction::SetAction(const String &name, SAction action)
{
	getInstance().actions.Set(name, action);
}

/*!
 * Loads a set of default actions
 *
 * \throws	ExceptionIO	cannot read file
 * \throws	ExceptionUninitialized	empty file
 * \throws	ExceptionNotFound	invalid file
 * \throws	ExceptionProtocol	load unimplemented
 *
 * \param[in]  filename  the path to the file containing the actions
 */
void DefaultAction::Load(const Path &filename)
{
	getInstance().actions.Load(filename);
}

/*!
 * Saves a set of default actions
 *
 * \throws	ExceptionIO	cannot write file
 * \throws	ExceptionProtocol	save unimplemented
 *
 * \param[in]  filename  the path to the file to create or overwrite
 */
void DefaultAction::Save(const Path &filename)
{
	getInstance().actions.Save(filename);
}

/*!
 * Gets the list of default actions
 * \return  a vector containing the name of the available default actions
 */
std::vector<String> DefaultAction::GetActionList()
{
	std::vector<String> lst;
	Map &act(getInstance().actions);
	for (auto it = act.begin(); it != act.end(); ++it)
	{
		lst.push_back(it->first);
	}
	return lst;
}

