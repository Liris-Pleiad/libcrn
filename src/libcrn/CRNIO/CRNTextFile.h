/* Copyright 2006-2014 Yann LEYDIER, CoReNum, INSA-Lyon
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
 * file: CRNTextFile.h
 * \author Yann LEYDIER
 */

#ifndef CRNTEXTFILE_HEADER
#define CRNTEXTFILE_HEADER

#include <CRNComplexObject.h>
#include <CRNIO/CRNPath.h>
#include <vector>
#include <set>
#include <map>

namespace crn
{
	/****************************************************************************/
	/*! \brief A text file
	 *
	 * A UTF-8 text file.
	 *
	 * \author 	Yann LEYDIER
	 * \date		24 October 2006
	 * \version 0.3
	 * \ingroup io
	 */
	class TextFile: public ComplexObject
	{
		public:
			/*! \brief Allowed text encoding */
			enum class Encoding { UTF_8, UTF_32 };

			/*! \brief Blank constructor */
			TextFile(Encoding enc = Encoding::UTF_8);
			/*! \brief File constructor */
			TextFile(const Path &fname, Encoding enc = Encoding::UTF_8);
			TextFile(const TextFile&) = delete;
			TextFile(TextFile&&) = default;
			/*! \brief Destructor */
			virtual ~TextFile() override;

			TextFile& operator=(const TextFile&) = delete;
			TextFile& operator=(TextFile&&) = default;

			/*! \brief Returns the name of the class */
			virtual Protocol GetClassProtocols() const noexcept override { return Protocol::ComplexObject; }
			/*! \brief Returns the name of the class */
			virtual const String& GetClassName() const override { static const String cn(U"TextFile"); return cn; }

			/*! \brief Saves a file with the same encoding as when loaded */
			void Save(const Path &fname);
			/*! \brief Saves a file with specific encoding */
			void Save(const Path &fname, Encoding enc);
			/*! \brief Overwrites the loaded file */
			void Save();
			
			/*! \brief Get text content */
			const String& GetText() const noexcept {return text;};

			/*! \brief Extract words */
			std::vector<String> SplitText() const { return text.Split(U" \r\n\t,.!?:;&\"\'({[|`_\\^@)]}=+^$£%*/<>-"); }
			/*! \brief Extract unique words */
			std::set<String> ExtractWords(bool case_sensitive = false) const;
			/*! \brief Extract unique words sorted by frequency */
			std::map<int, String> ExtractWordsByFrequency(bool case_sensitive = false) const;

			String text; /*!< the content of the file */

		private:
			Path filename;	/*!< latest filename */
			Encoding original_encoding; /*!< default encoding for saving */
	};

	CRN_ALIAS_SMART_PTR(TextFile)
}


#endif
