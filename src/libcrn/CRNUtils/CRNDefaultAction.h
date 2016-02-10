/* Copyright 2010-2014 CoReNum, INSA-Lyon
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
 * file: CRNDefaultAction.h
 * \author Yann LEYDIER
 */

#ifndef CRNDefaultAction_HEADER
#define CRNDefaultAction_HEADER

#include <CRNData/CRNMap.h>

namespace crn
{
	/*! \defgroup action Actions 
	 * \ingroup	utils */
	/*@{*/
	/*! \brief Base functor class
	 * 
	 * A base class to write functors. 
	 *
	 * A default serialization method is offered. Parameters to the functor can be stored in the UserData and will automatically be (de)serialized.
	 *
	 * All derived class must initialize with CRN_DATA_FACTORY_REGISTER(classname)!
	 */
	class Action: public ComplexObject
	{
		public:
			/*! \brief	Destructor */
			virtual ~Action() override { }
			/*! \brief	The class is serializable */
			virtual Protocol GetClassProtocols() const noexcept override { return Protocol::Serializable; }

		private:
			virtual void deserialize(xml::Element &el) override;
			virtual xml::Element serialize(xml::Element &parent) const override;
	};
	/*@}*/

	CRN_ALIAS_SMART_PTR(Action)
}

namespace crn
{
	/*! \brief A class containing functors
	 *
	 * A class containing functors stored with a name. The whole set can be saved to a file or loaded from a file.
	 * \ingroup action
	 */
	class DefaultAction
	{
		public:
			/*! \brief Gets a default action */
			static SAction GetAction(const String &name);
			/*! \brief Sets a default action */
			static void SetAction(const String &name, SAction action);
			/*! \brief Loads a set of default actions */
			static void Load(const Path &filename);
			/*! \brief Saves the set of default actions */
			static void Save(const Path &filename);

			/*! \brief Gets the list of default actions */
			static std::vector<String> GetActionList();

		private:
			/*! \brief Default constructor */
			DefaultAction():actions(Protocol::Serializable) {}
			Map actions; /*!< the set of actions */
			/*! \brief Singleton instance */
			static DefaultAction& getInstance();
	};
}

#endif
