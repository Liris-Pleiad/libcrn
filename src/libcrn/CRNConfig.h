/* Copyright 2009-2014 INSA Lyon, CoReNum
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
 * file: CRNConfig.h
 * \author Yann LEYDIER
 */

#ifndef CRNConfig_HEADER
#define CRNConfig_HEADER
#include <CRNUtils/CRNConfigurationFile.h>

namespace crn
{
	/*! \defgroup utils Commodities */

	/*! \brief Global configuration utility class
	 *
	 * Global configuration utility class
	 * \author Yann LEYDIER
	 * \version 0.1
	 * \date May 2009
	 * \ingroup utils
	 */
	class Config
	{
		public:
			/*! \brief Destructor */
			~Config();

			/*! \brief Saves to user's local config */
			static bool Save();

			/*! \brief Sets the top directory name */
			static void SetTopDataPath(const Path &dir);
			/*! \brief Gets the top directory name */
			static Path GetTopDataPath();
			/*! \brief Gets the translation files path */
			static crn::Path GetLocalePath();
			/*! \brief Sets the translation files path */
			static void SetLocalePath(const crn::Path &dir);
			/*! \brief Gets the data directory name */
			static crn::Path GetStaticDataPath();
			/*! \brief Sets the data directory name */
			static void SetStaticDataPath(const crn::Path &dir);

		private:
			/*! \brief Singleton */
			static Config& getInstance();
			/*! \brief Configuration file key */
			static String topDirKey();
			/*! \brief Configuration file key */
			static String localeDirKey();
			/*! \brief Configuration file key */
			static String staticDataDirKey();
			/*! \brief Configuration file key */
			static String verboseKey();
			/*! \brief Constructor */
			Config();
			ConfigurationFile conf; /*!< Configuration file manager */

		CRN_DECLARE_CLASS_CONSTRUCTOR(Config)
	};
}

#endif


