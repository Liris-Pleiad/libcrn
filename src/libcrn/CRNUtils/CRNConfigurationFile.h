/* Copyright 2010-2015 CoReNum, INSA-Lyon, Université Paris Descartes
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
 * file: CRNConfigurationFile.h
 * \author Yann LEYDIER
 */

#ifndef CRNConfigurationFile_HEADER
#define CRNConfigurationFile_HEADER

#include <CRNData/CRNData.h>
#include <CRNData/CRNMap.h>
#include <CRNIO/CRNPath.h>

namespace crn
{
	class String;

	/*! \brief Configuration file management utility class
	 *
	 * Configuration file management utility class
	 * \author Yann LEYDIER
	 * \version 0.1
	 * \date Dec 2010
	 * \ingroup utils
	 */
	class ConfigurationFile
	{
		public:
			/*! \brief Constructor */
			ConfigurationFile(const String &application_name, const StringUTF8 &file_name = "");
			ConfigurationFile(const ConfigurationFile &) = delete;
			ConfigurationFile(ConfigurationFile&&) = default;
			ConfigurationFile& operator=(const ConfigurationFile &) = delete;
			ConfigurationFile& operator=(ConfigurationFile&&) = default;
			/*! \brief Loads the file */
			Path Load();
			/*! \brief Saves the file to the user's personal space */
			Path Save();

			/*! \brief Gets a value */
			SObject& operator[](const String &key);
			/*! \brief Gets a value */
			SObject GetData(const String &key);
			/*! \brief Gets a value */
			SCObject GetData(const String &key) const;
			/*! \brief Gets a path */
			Path GetPath(const String &key) const;
			/*! \brief Gets a string */
			String GetString(const String &key) const;
			/*! \brief Gets a UTF8 string */
			StringUTF8 GetStringUTF8(const String &key) const;
			/*! \brief Gets a Prop3 */
			Prop3 GetProp3(const String &key) const;
			/*! \brief Gets an int */
			int GetInt(const String &key) const;
			/*! \brief Gets a double */
			double GetDouble(const String &key) const;

			/*! \brief Sets a key/value pair 
			 * \throws	ExceptionProtocol	value is not serializable
			 */
			template<typename T> void SetData(const String &key, T value)
			{
				data.Set(key, Data::ToCRN(value));
			}

			/*! \brief Returns the path to the user configuration directory */
			Path GetUserDirectory() const;
		private:
			Map data; /*!< data storage */
			Path appname; /*!< name of the application */
			Path filename; /*!< name of the configuration file */

		CRN_DECLARE_CLASS_CONSTRUCTOR(ConfigurationFile)
	};

	CRN_ALIAS_SMART_PTR(ConfigurationFile)
}

#endif


