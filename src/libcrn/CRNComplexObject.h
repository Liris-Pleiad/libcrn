/* Copyright 2006-2014 INSA Lyon, CoReNum
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
 * file: CRNComplexObject.h
 * \author Yann LEYDIER
 */

#ifndef CRNCOMPLEXOBJECT_HEADER
#define CRNCOMPLEXOBJECT_HEADER

#include <CRNObject.h>
#include <map>
#include <CRNString.h>
#include <CRNIO/CRNPath.h>
#include <CRNData/CRNMapPtr.h>
#include <mutex>

namespace crn
{
	class ComplexObject;
	CRN_ALIAS_SMART_PTR(ComplexObject)
}
namespace crn
{
	/****************************************************************************/
	/*! \brief complex base abstract class
	 *
	 * A more complex base abstract class.
	 * 
	 * This class offers several informal protocols. If an informal protocal's
	 * method is called without having been implemented, an exception will be
	 * thrown. The overloadable methods are the following (in parenthesis, the
	 * flag to activate in CRNProtocols.h):
	 * 	- Savable (CRNPROTOCOL_SAVABLE)
	 * 		- const crn::Path completeFilename(const crn::Path &fn) const
	 * 		- void load(const crn::Path &fname)
	 * 		- void save(const crn::Path &fname)
	 * 
	 * \author 	Yann LEYDIER
	 * \date		August 2007
	 * \version 0.2
	 * \ingroup base
	 */
	class ComplexObject: public Object
	{
		/**************************************************************************/
		/* Complex object interface                                               */
		/**************************************************************************/
		public:
			/*! \brief Default constructor */
			ComplexObject(const String &s = U"");
			/*! \brief Destructor */
			virtual ~ComplexObject() override;

			/*! \brief No copy construction allowed */
			ComplexObject(const ComplexObject&) = delete;
			/*! \brief No assignment allowed */
			ComplexObject& operator=(const ComplexObject&) = delete;

			ComplexObject(ComplexObject&&) noexcept;
			ComplexObject& operator=(ComplexObject&&c); // XXX YANN ce serait bien de l'implémenter à la main pour le rendre noexcept

			/*! \brief Returns the name of the object */
			const String& GetName() const { return name; }
			/*! \brief Sets the name of the object */
			void SetName(const String &s) { name = s; }
			/*! \brief Sets the name of the object */
			void SetName(String &&s) noexcept { name = std::move(s); }

			/*! \brief Adds or replaces a user data */
			void SetUserData(const String &key, SObject value);
			/*! \brief Tests if a user data key exists */
			bool IsUserData(const String &key) const;
			/*! \brief Gets a user data by key */
			SObject GetUserData(const String &key);
			/*! \brief Gets a user data key by value */
			String GetUserDataKey(const SObject &value) const;
			/*! \brief Gets a user data by key */
			const SObject GetUserData(const String &key) const;
			/*! \brief Deletes a user data entry and frees the value */
			void DeleteUserData(const String &key);
			/*! \brief Deletes all user data entries */
			void ClearUserData();

			/*! \brief Converts object to string */
			virtual String ToString() const override;

		private:
			String name; /*!< The name of the object */
			std::unique_ptr<Map> user_data; /*!< A map of user-set objects */

		/**************************************************************************/
		/* Savable protocol                                                       */
		/**************************************************************************/
		public:
			/*! \brief Constructor from file name */
			ComplexObject(const String &s, const Path &fname);
			/*! \brief Loads the object from an XML file (Safe) */
			void Load(const Path &fname);
			/*! \brief Saves the object to an XML file (Safe) */
			void Save(const Path &fname);
			/*! \brief Saves the object to an already set XML file */
			void Save();
			/*! \brief Returns the file name of the object */
			const Path& GetFilename() const noexcept { return filename; }
		protected:
			/*! \brief Overwrites the filename */
			void setFilename(const Path &fname) { filename = fname; }
			/*! \brief Overwrites the filename */
			void setFilename(Path &&fname) noexcept { filename = std::move(fname); }
			std::unique_ptr<std::mutex> filelock; /*!< Protection to avoid multiple access to the file at the same time */
		private:
			/*! \brief Completes a relative file name with the default path */
			virtual Path completeFilename(const Path &fn) const;
			/*! \brief Loads the object from an XML file (Unsafe) */
			virtual void load(const Path &fname);
			/*! \brief Saves the object to an XML file (Unsafe) */
			virtual void save(const Path &fname);
			Path filename; /*!< The file name of the object */

		/**************************************************************************/
		/* Serializable protocol                                                  */
		/**************************************************************************/
		protected:
			/*! \brief Initializes some internal data from an XML element. */
			virtual void deserialize_internal_data(xml::Element &el) override;
			/*! \brief Dumps some internal data to an XML element. */
			virtual void serialize_internal_data(xml::Element &el) const override;

	};
}

#endif
