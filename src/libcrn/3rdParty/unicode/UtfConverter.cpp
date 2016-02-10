/* Copyright 2008-2009 INSA Lyon
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
 * file: UtfConverter.cpp
 * \author Yann LEYDIER
 */

#include "UtfConverter.h"
#include "ConvertUTF.h"
#include <CRNException.h>
#include <CRNIO/CRNIO.h>
#include <CRNi18n.h>

namespace UtfConverter
{

	std::basic_string<char32_t> FromUtf8(const std::string& utf8string)
	{
#ifdef CRN_NO_WSTRING
		return utf8string;
#endif
		if (utf8string.empty())
			return std::basic_string<char32_t>();
		size_t widesize = utf8string.length();
		if (sizeof(char32_t) == 2)
		{
			std::basic_string<char32_t> resultstring;
			resultstring.resize(widesize+1, U'\0');
			const UTF8* sourcestart = reinterpret_cast<const UTF8*>(utf8string.c_str());
			const UTF8* sourceend = sourcestart + widesize;
			UTF16* targetstart = reinterpret_cast<UTF16*>(&resultstring[0]);
			UTF16* ts = targetstart;
			UTF16* targetend = targetstart + widesize;
			ConversionResult res = ConvertUTF8toUTF16(&sourcestart, sourceend, &targetstart, targetend, strictConversion);
			switch (res)
			{
				case conversionOK:
					break;
				case sourceExhausted:
					CRNWarning(U"FromUtf8: source exhausted.");
					break;
				case targetExhausted:
					CRNWarning(U"FromUtf8: target exhausted.");
					break;
				case sourceIllegal:
					CRNWarning(U"FromUtf8: source illegal.");
					break;
			}
			*targetstart = 0;
			return std::basic_string<char32_t>(resultstring, 0, targetstart - ts);
		}
		else if (sizeof(char32_t) == 4)
		{
			std::basic_string<char32_t> resultstring;
			resultstring.resize(widesize+1, U'\0');
			const UTF8* sourcestart = reinterpret_cast<const UTF8*>(utf8string.c_str());
			const UTF8* sourceend = sourcestart + widesize;
			UTF32* targetstart = reinterpret_cast<UTF32*>(&resultstring[0]);
			UTF32* ts = targetstart;
			UTF32* targetend = targetstart + widesize;
			ConversionResult res = ConvertUTF8toUTF32(&sourcestart, sourceend, &targetstart, targetend, strictConversion);
			switch (res)
			{
				case conversionOK:
					break;
				case sourceExhausted:
					CRNWarning(U"FromUtf8: source exhausted.");
					break;
				case targetExhausted:
					CRNWarning(U"FromUtf8: target exhausted.");
					break;
				case sourceIllegal:
					CRNWarning(U"FromUtf8: source illegal.");
					break;
			}
			*targetstart = 0;
			return std::basic_string<char32_t>(resultstring, 0, targetstart - ts);
		}
		else
		{
			throw crn::ExceptionRuntime(crn::StringUTF8("FromUtf8: ") + _("UTF not supported."));
		}
		return U"";
	}

	std::string ToUtf8(const std::basic_string<char32_t>& widestring)
	{
#ifdef CRN_NO_WSTRING
		return widestring;
#endif
		if (widestring.empty())
			return std::string();

		size_t widesize = widestring.length();

		if (sizeof(char32_t) == 2)
		{
			size_t utf8size = 3 * widesize + 1;
			std::string resultstring;
			resultstring.resize(utf8size, '\0');
			const UTF16* sourcestart = reinterpret_cast<const UTF16*>(widestring.c_str());
			const UTF16* sourceend = sourcestart + widesize;
			UTF8* targetstart = reinterpret_cast<UTF8*>(&resultstring[0]);
			UTF8* ts = targetstart;
			UTF8* targetend = targetstart + utf8size;
			ConversionResult res = ConvertUTF16toUTF8(&sourcestart, sourceend, &targetstart, targetend, strictConversion);
			switch (res)
			{
				case conversionOK:
					break;
				case sourceExhausted:
					CRNWarning(U"ToUtf8: source exhausted.");
					break;
				case targetExhausted:
					CRNWarning(U"ToUtf8: target exhausted.");
					break;
				case sourceIllegal:
					CRNWarning(U"ToUtf8: source illegal.");
					break;
			}
			*targetstart = 0;
			return std::string(resultstring, 0, targetstart - ts);
		}
		else if (sizeof(char32_t) == 4)
		{
			size_t utf8size = 4 * widesize + 1;
			std::string resultstring;
			resultstring.resize(utf8size, '\0');
			const UTF32* sourcestart = reinterpret_cast<const UTF32*>(widestring.c_str());
			const UTF32* sourceend = sourcestart + widesize;
			UTF8* targetstart = reinterpret_cast<UTF8*>(&resultstring[0]);
			UTF8* ts = targetstart;
			UTF8* targetend = targetstart + utf8size;
			ConversionResult res = ConvertUTF32toUTF8(&sourcestart, sourceend, &targetstart, targetend, strictConversion);
			switch (res)
			{
				case conversionOK:
					break;
				case sourceExhausted:
					CRNWarning(U"ToUtf8: source exhausted.");
					break;
				case targetExhausted:
					CRNWarning(U"ToUtf8: target exhausted.");
					break;
				case sourceIllegal:
					CRNWarning(U"ToUtf8: source illegal.");
					break;
			}
			*targetstart = 0;
			return std::string(resultstring, 0, targetstart - ts);
		}
		else
		{
			throw crn::ExceptionRuntime(crn::StringUTF8("FromUtf8: ") + _("UTF not supported."));
		}
		return "";
	}
}

