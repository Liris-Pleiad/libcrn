/* Copyright 2015 INSA Lyon
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
 * file: CRNUTFConverter.h
 * \author Yann LEYDIER
 */

#ifndef CRNUTFCONVERTER__H__
#define CRNUTFCONVERTER__H__

#include <CRN.h>
#include <string>
#include <locale>

/*! \ingroup string */
namespace crn
{
	/*! \brief Utility class to convert Unicode strings
	 * \author	Yann LEYDIER
	 * \date	Jan 2015
	 * \version	0.1
	 * \warning	not supported by GCC 4.8!
	 */
	class UTFConverter
	{
		public:
			static std::u32string ToUTF32(char utf8string) { return getCv().from_bytes(utf8string); }
			static std::u32string ToUTF32(const char* utf8string) { return getCv().from_bytes(utf8string); }
			static std::u32string ToUTF32(const std::string& utf8string) { return getCv().from_bytes(utf8string); }

			static std::string ToUTF8(char32_t utf32string) { return getCv().to_bytes(utf32string); }
			static std::string ToUTF8(const char32_t* utf32string) { return getCv().to_bytes(utf32string); }
			static std::string ToUTF8(const std::u32string& utf32string) { return getCv().to_bytes(utf32string); }

		private:
			static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>& getCv()
			{
				static UTFConverter uc;
				return uc.cv;
			}

			UTFConverter()
#ifndef CRN_UTF_CONVERSION_THROWS
				:cv("Error converting UTF32 to UTF8", U"Error converting UTF8 to UTF32")
#endif
				{}
			std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cv;
	};
}

#endif

