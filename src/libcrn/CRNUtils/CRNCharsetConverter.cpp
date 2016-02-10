/* Copyright 2012-2014 CoReNum
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
 * file: CRNCharsetConverter.cpp
 * \author Yann LEYDIER
 */

#include <CRNUtils/CRNCharsetConverter.h>
#include <errno.h>
#include <CRNStringUTF8.h>
#include <CRNi18n.h>

using namespace crn;

/*! Constructor 
 * \throws	crn::ExceptionInvalidArgument	unsupported charset
 * \param[in]	to_code	the character set to convert
 * \param[in]	translit	shall incompatible characters be transliterated (default: yes)
 * \param[in]	throw_exceptions	shall the converter throw exceptions when an error occurred
 */
CharsetConverter::CharsetConverter(const std::string &to_code, bool translit, bool throw_exceptions):
	silent(true),
	throws(throw_exceptions)
{
	Reset(to_code, translit);
}

/*! Destructor */
CharsetConverter::~CharsetConverter()
{
	if (!silent)
	{
		iconv_close(fromutf);
		iconv_close(toutf);
	}
}

/*! Changes the charset to convert
 * \throws	crn::ExceptionInvalidArgument	unsupported charset
 * \param[in]	to_code	the character set to convert
 * \param[in]	translit	shall incompatible characters be transliterated (default: yes)
 */
void CharsetConverter::Reset(const std::string &to_code, bool translit)
{
	if (!silent)
	{
		iconv_close(fromutf);
		iconv_close(toutf);
	}
	if (to_code.empty())
		throw crn::ExceptionInvalidArgument(_("Null charset"));
	if (to_code == "utf-8")
	{
		silent = true;
		current_code = to_code;
		return;
	}
	else
	{
		silent = false;
	}
	std::string to(to_code), from("utf-8");
	if (translit)
		to += "//TRANSLIT";
	fromutf = iconv_open(to.c_str(), from.c_str());
	if (fromutf == (iconv_t)(-1))
		throw crn::ExceptionInvalidArgument(_("Cannot convert from ") + to_code);
	to = "utf-8";
	from = to_code;
	if (translit)
		to += "//TRANSLIT";
	toutf = iconv_open(to.c_str(), from.c_str());
	if (toutf == (iconv_t)(-1))
	{
		iconv_close(fromutf);
		throw crn::ExceptionInvalidArgument(_("Cannot convert to ") + to_code);
	}
	current_code = to_code;
}

/*! Converts from unicode to the selected charset
 * \throws	ExceptionInvalidCharacter	invalid character
 * \throws	ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	str	the UTF-8 string to convert
 * \param[in]	stat	a status variable (may be nullptr): if no error occurred, the status is left unchanged
 * \return	the converted string
 */
std::string CharsetConverter::FromUTF8(const StringUTF8 &str, Status *stat) const
{
	if (silent)
		return str.Std();
	if (str.IsEmpty())
		return std::string();
	return fromUTF8(str, str.Size() * 2, stat);
}

/*! Converts from unicode to the selected charset
 * \throws	ExceptionInvalidCharacter	invalid character
 * \throws	ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	str	the UTF-8 string to convert
 * \param[in]	buff	the size of the output buffer
 * \param[in]	stat	a status variable (may be nullptr): if no error occurred, the status is left unchanged
 * \return	the converted string
 */
std::string CharsetConverter::fromUTF8(const StringUTF8 &str, size_t buff, Status *stat) const
{
	if (buff == 0)
		return std::string();
	const char *in(&str[0]);
	size_t ins = str.Size();
	if (ins == 0)
		return std::string();
	size_t outs = buff;
	std::vector<char> ret(outs, '\0');
	char *out = &ret.front();
	size_t ans = iconv(fromutf, (char**)&in, &ins, &out, &outs);
	if (outs == 0)
		return fromUTF8(str, buff * 2, stat);
	if (ans == (size_t)(-1))
	{
		switch (errno)
		{
			case E2BIG:
				return fromUTF8(str, buff * 2, stat);
			case EILSEQ:
				if (throws)
					throw ExceptionInvalidCharacter(_("Invalid character."));
				if (stat)
					*stat = Status::INVALID; // modify after exception throw to preserve state
				break;
			case EINVAL:
				if (throws)
					throw ExceptionIncompleteCode(_("Incomplete multibyte character."));
				if (stat)
					*stat = Status::INCOMPLETE;
				break;
		}
	}
	return std::string(&ret.front());
}

/*! Converts to unicode
 * \throws	ExceptionInvalidCharacter	invalid character
 * \throws	ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	str	the string to convert
 * \param[in]	stat	a status variable (may be nullptr): if no error occurred, the status is left unchanged
 * \return	the UTF-8 string
 */
StringUTF8 CharsetConverter::ToUTF8(const std::string &str, Status *stat) const
{
	if (silent)
		return str;
	if (str.empty())
		return StringUTF8();
	return toUTF8(str, str.size() * 2, stat);
}

/*! Converts to unicode
 * \throws	ExceptionInvalidCharacter	invalid character
 * \throws	ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	str	the string to convert
 * \param[in]	buff	the size of the output buffer
 * \param[in]	stat	a status variable (may be nullptr): if no error occurred, the status is left unchanged
 * \return	the UTF-8 string
 */
StringUTF8 CharsetConverter::toUTF8(const std::string &str, size_t buff, Status *stat) const
{
	if (buff == 0)
		return StringUTF8();
	const char *in(str.c_str());
	size_t ins = str.size();
	if (ins == 0)
		return StringUTF8();
	size_t outs = buff;
	std::vector<char> ret(outs, '\0');
	char *out = &ret.front();
	size_t ans = iconv(toutf, (char**)&in, &ins, &out, &outs);
	if (outs == 0)
		return toUTF8(str, buff * 2, stat);
	if (ans == (size_t)(-1))
	{
		switch (errno)
		{
			case E2BIG:
				return toUTF8(str, buff * 2, stat);
			case EILSEQ:
				if (throws)
					throw ExceptionInvalidCharacter(_("Invalid character in: ") + str);
				if (stat)
					*stat = Status::INVALID;
				break;
			case EINVAL:
				if (throws)
					throw ExceptionIncompleteCode(_("Incomplete multibyte character in: ") + str);
				if (stat)
					*stat = Status::INCOMPLETE;
				break;
		}
	}
	return StringUTF8(&ret.front());
}

/*! Default constructor */
CharsetConverter::Exception::Exception() throw() { }
/*! Constructor with a message */
CharsetConverter::Exception::Exception(const StringUTF8 &msg) throw():crn::Exception(msg) { }
/*! Constructor with a message */
CharsetConverter::Exception::Exception(const char *msg) throw():crn::Exception(msg) { }

/*! Default constructor */
CharsetConverter::ExceptionInvalidCharacter::ExceptionInvalidCharacter() throw() { }
/*! Constructor with a message */
CharsetConverter::ExceptionInvalidCharacter::ExceptionInvalidCharacter(const StringUTF8 &msg) throw():CharsetConverter::Exception(msg) { }
/*! Constructor with a message */
CharsetConverter::ExceptionInvalidCharacter::ExceptionInvalidCharacter(const char *msg) throw():CharsetConverter::Exception(msg) { }

/*! Default constructor */
CharsetConverter::ExceptionIncompleteCode::ExceptionIncompleteCode() throw() { }
/*! Constructor with a message */
CharsetConverter::ExceptionIncompleteCode::ExceptionIncompleteCode(const StringUTF8 &msg) throw():CharsetConverter::Exception(msg) { }
/*! Constructor with a message */
CharsetConverter::ExceptionIncompleteCode::ExceptionIncompleteCode(const char *msg) throw():CharsetConverter::Exception(msg) { }


