/* Copyright 2010-2016 CoReNum, ENS-Lyon
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
 * file: CRNPath.cpp
 * \author Yann LEYDIER
 */

#include <CRNIO/CRNPath.h>
#include <CRNString.h>
#include <CRNData/CRNForeach.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNException.h>
#include <CRNi18n.h>

using namespace crn;

/*! Invalid drive or no drive found
 * \return	a character for an invalid drive (win32)
 */
char Path::NoDrive()
{
	return '?';
}

/*! Local directory separator
 * \return	the character used to separate directories on the host OS
 */
char Path::Separator() noexcept
{
#ifdef CRN_PF_WIN32
	return '\\';
#else
	return '/';
#endif
}

/*!
 * Appends a path with conversion to the local format
 * \param[in]	s	the string to convert and append
 * \return	the modified string
 */
const Path& Path::operator+=(const Path &s)
{
	Path tmp(s);
	tmp.ConvertTo(GetFormat());
	if (tmp.IsURI())
	{ // remove the file:// before concatenating!
		size_t sep(tmp.Find("://"));
		if (sep == NPos())
			StringUTF8::operator+=(tmp);
		else if (sep + 3 < tmp.Size())
			StringUTF8::operator+=(tmp.CStr() + sep + 3);
	}
	else
	{
		StringUTF8::operator+=(tmp);
	}
	return *this;
}

/*! 
 * Appends a string after adding directory separator if needed
 * \param[in]	s	the string to convert and append
 * \return	the modified string
 */
const Path& Path::operator/=(const Path &s)
{
	// append separator
	char sep('/');
	if (IsWindows() && !IsURI() && !IsUnix())
		sep = '\\';
	bool needsep = true;
	if (IsNotEmpty() && ((*this)[Size() - 1] == sep))
		needsep = false;
	else if (s.IsNotEmpty() && (s[0] == sep))
		needsep = false;
	if (needsep)
		StringUTF8::operator+=(sep);

	// append string
	Path tmp(s);
	tmp.ConvertTo(GetFormat());
	operator+=(tmp);
	return *this;
}

/*!
 * Returns the format
 * \return URI, WINDOWS or UNIX
 */
Path::Format Path::GetFormat() const
{
	if (format != Format::AUTO)
		return format;
	if (IsURI())
		return Format::URI;
	if (IsWindows() && !IsUnix())
		return Format::WINDOWS;
	return Format::UNIX;
}

/*! Is the path a URI?
 * \return  true if the path is a URI
 */
bool Path::IsURI() const
{ // a URI begins with "scheme://"
	return Find("://") != NPos();
}

/*! Is the path in Unix format?
 * \return  true if the path is in Unix format
 */
bool Path::IsUnix() const
{ // unix paths do not begin with "scheme://" nor "X:\"
	return !IsURI() && (Find(":\\") == NPos());
}

/*! Is the path in Windows format?
 * \return  true if the path is in Windows format
 */
bool Path::IsWindows() const
{ // Windows paths begin with "X:\" or do not contain any "/"
	if (Find(":\\") == 1)
		return true;
	return Find("/") == NPos();
}

/*! Converts to a specific format 
 * \param[in]	fmt	the destination format
 * \return	the updated path
 */
Path& Path::ConvertTo(Path::Format fmt)
{
	if (fmt == format)
		return *this;
	switch (fmt)
	{
		case Format::AUTO:
			if (IsWindows())
				ToWindows(); // XXX is this necessary?
			format = fmt;
			break;
		case Format::URI:
			ToURI();
			break;
		case Format::UNIX:
			ToUnix();
			break;
		case Format::WINDOWS:
			ToWindows();
			break;
	}
	return *this;
}

/*! Is the path absolute?
 * \return  true if the path is absolute
 */
bool Path::IsAbsolute() const
{
	if (IsEmpty())
		return true; // XXX well this is technically not absolute…
	if ((*this)[0] == '.')
		return true; // XXX well this is technically not absolute…
	if (IsURI())
	{
		size_t sep(Find("://"));
		if ((sep != NPos()) && (sep + 3 < Size()))
		{
			if ((*this)[sep + 3] == '/')
				return true;
		}
	}
	else if (IsWindows())
	{
		if (Size() > 1)
			if ((*this)[1] == ':')
				return true;
	}
	else if (IsUnix())
	{
		if ((*this)[0] == '/')
			return true;
	}
	return false;
}

/*! Returns the filename
 * \return  the filename
 */
Path Path::GetFilename() const
{
	size_t pos;
	if (IsWindows())
	{
		Path tmp(*this);
		tmp.ToWindows(); // ensure that there are no trailing '/'
		pos = tmp.BackwardFind("\\");
	}
	else
	{
		pos = BackwardFind("/");
	}
	if (pos == NPos())
	{ // not found
		return *this;
	}
	else if (pos == Size() - 1)
	{ // the separator is the last character
		return Path("", format);
	}
	else
		return Path(SubString(pos + 1), format);
}

/*! Returns the base of the filename
 *
 * \throws	ExceptionLogic	path does not contain a filename
 * \return  the filename's base (i.e.: filename minus extension)
 */
Path Path::GetBase() const
{
	Path fname(GetFilename());
	if (!fname)
		throw ExceptionLogic(StringUTF8("const Path Path::GetBase() const: ") + _("the path does not contain a filename."));
	size_t ppos = fname.BackwardFind(".");
	if ((ppos != NPos()) && (ppos != 0))
		return Path(fname.SubString(0, ppos), format);
	else
		return Path("", format);
}

/*! Returns the extension
 *
 * \throws	ExceptionLogic	path does not contain a filename
 * \return  the filename's extension
 */
Path Path::GetExtension() const
{
	Path fname(GetFilename());
	if (!fname)
		throw ExceptionLogic(StringUTF8("const Path Path::GetBase() const: ") + _("the path does not contain a filename."));
	size_t ppos = fname.BackwardFind(".");
	if ((ppos != NPos()) && (ppos != fname.Size() - 1))
		return Path(fname.SubString(ppos + 1), format);
	else
		return Path("", format);
}

/*! Returns the full directory path
 * \return  the directory path
 */
Path Path::GetDirectory() const
{
	size_t pos;
	if (IsWindows())
	{
		pos = BackwardFind("\\");
	}
	else
	{
		pos = BackwardFind("/");
	}
	if ((pos == NPos()) || (pos == Size() - 1))
	{ // not found or the / is the last character
		if (IsURI())
		{
			size_t beg = Find("://") + 3;
			return Path(SubString(beg), format);
		}
		else
			return *this;
	}
	else
	{
		if (IsURI())
		{
			size_t beg = Find("://") + 3;
			return Path(SubString(beg, pos + 1 - beg), format);
		}
		else
			return Path(SubString(0, pos + 1), format);
	}
}

/*! Returns the scheme of the URI
 * \return the scheme of the URI
 */
StringUTF8 Path::GetScheme() const
{
	if (IsURI())
	{
		return SubString(0, Find("://"));
	}
	return "";
}

/*! Converts the path to URI
 * \return  a reference to the modified path
 */
Path& Path::ToURI()
{
	if (IsURI())
		return *this;
	format = Format::URI;
	Path uri("file://", Format::URI);
	if (IsEmpty())
	{
		Swap(uri);
		return *this;
	}

	if (IsWindows())
	{
		size_t beg = 0;
		if (Size() >= 2)
		{
			if ((*this)[1] == ':')
			{
				uri.Std() += "/";
				uri.Std() += (*this)[0];
				uri.Std() += (*this)[1];
				uri.Std() += "/";
				beg = 2; // C:toto
				if (Size() >= 3)
				{
					if ((*this)[2] == '\\') // C:\toto
						beg = 3;
				}
			}
		}
		for (size_t tmp = beg; tmp < Size(); ++tmp)
		{
			if ((*this)[tmp] == '\\')
				uri.Std() += '/';
			else
				uri.Std() += (*this)[tmp];
		}
	}
	else if (IsUnix())
	{
		uri.Std() += this->Std(); // convert to char* to avoid multiple conversions
	}
	// we do not encode, this is too shitty
	Swap(uri);
	return *this;
}

/*! Converts the path to Unix format
 * \return  a reference to the modified path
 */
Path& Path::ToUnix()
{
	if (IsUnix() || IsEmpty())
		return *this;
	format = Format::UNIX;
	Path uri(Format::UNIX);
	if (IsWindows())
	{
		size_t beg = 0;
		if (Size() >= 2)
		{
			if ((*this)[1] == ':')
			{
				uri.Std() += "/";
				uri.Std() += (*this)[0];
				uri.Std() += "/";
				beg = 2; // C:toto
				if (Size() >= 3)
				{
					if ((*this)[2] == '\\') // C:\toto
						beg = 3;
				}
			}
		}
		for (size_t tmp = beg; tmp < Size(); ++tmp)
		{
			if ((*this)[tmp] == '\\')
				uri.Std() += '/';
			else
				uri.Std() += (*this)[tmp];
		}
	}
	else if (IsURI())
	{
		uri.Std() += SubString(Find("://") + 3).Std();
		uri.Decode();
	}
	Swap(uri);
	return *this;
}

/*! Converts the path to Windows format 
 * \return  a reference to the modified path
 */
Path& Path::ToWindows()
{
	if (IsEmpty())
		return *this;
	if (IsWindows())
	{
		for (size_t tmp = 0; tmp < Size(); ++tmp)
		{
			if ((*this)[tmp] == '/')
				(*this)[tmp] = '\\';
		}
		return *this;
	}
	format = Format::WINDOWS;
	Path uri(Format::UNIX);
	if (IsURI())
	{
		uri += SubString(Find("://") + 3);
		uri.Decode();
	}
	else 
	{
		uri = *this;
	}
	Path uri2(Format::WINDOWS);
	size_t beg = 0;
	if (Size() >= 2)
	{
		if ((uri[1] == '/') && (uri[0] != '.'))
		{
			uri2.Std() += uri[0];
			uri2.Std() += ":\\";
			beg = 2;
		}
	}
	if ((Size() >= 3) && (beg == 0))
	{
		if ((uri[0] == '/') && (uri[2] == '/'))
		{
			uri2.Std() += uri[1];
			uri2.Std() += ":\\";
			beg = 3;
		}
	}
	if ((Size() >= 4) && (beg == 0))
	{
		if ((uri[0] == '/') && (uri[2] == ':') && (uri[3] == '/'))
		{
			uri2.Std() += uri[1];
			uri2.Std() += ":\\";
			beg = 4;
		}
	}
	for (size_t tmp = beg; tmp < uri.Size(); ++tmp)
	{
		if (uri[tmp] == '/')
			uri2.Std() += '\\';
		else
			uri2.Std() += uri[tmp];
	}
	Swap(uri2);
	return *this;
}

/*! Gets the drive letter 
 * \return  the drive letter or NoDrive if not applicable
 */
char Path::GetDrive() const
{
	if (IsWindows() && IsAbsolute())
	{
		return (*this)[0];
	}
	return NoDrive();
}

/*! 
 * Replaces % codes with the corresponding character
 * \return	a reference to the modified string
 */
Path& Path::Decode()
{
	Path dec;
	for (size_t tmp = 0; tmp < Size(); ++tmp)
	{
		if ((*this)[tmp] == '%')
		{
			// read the hex code
			int val = 0;
			tmp += 1;
			if (((*this)[tmp] >= '0') && ((*this)[tmp] <= '9'))
			{
				val = ((*this)[tmp] - '0') * 16;
			}
			else if (((*this)[tmp] >= 'a') && ((*this)[tmp] <= 'f'))
			{
				val = ((*this)[tmp] - 'a' + 10) * 16;
			}
			else if (((*this)[tmp] >= 'A') && ((*this)[tmp] <= 'F'))
			{
				val = ((*this)[tmp] - 'A' + 10) * 16;
			}
			tmp += 1;
			if (((*this)[tmp] >= '0') && ((*this)[tmp] <= '9'))
			{
				val += (*this)[tmp] - '0';
			}
			else if (((*this)[tmp] >= 'a') && ((*this)[tmp] <= 'f'))
			{
				val += (*this)[tmp] - 'a' + 10;
			}
			else if (((*this)[tmp] >= 'A') && ((*this)[tmp] <= 'F'))
			{
				val += (*this)[tmp] - 'A' + 10;
			}
			dec.Std() += char(val);
		}
		else
			dec.Std() += (*this)[tmp];
	}
	*this = dec;
	return *this;
}

Path& Path::ToLocal()
{
	return ConvertTo(Format::LOCAL);
}

/*! 
 * Splits the string in multiple strings delimited by a set of separators 
 * \param[in]	sep	a list of separators
 *
 * \return	a list of substrings
 */
std::vector<Path> Path::Split(const StringUTF8 &sep) const
{
	const std::vector<StringUTF8> w(StringUTF8::Split(sep));
	std::vector<Path> words;
	for (const StringUTF8 &s : w)
		words.push_back(s);
	return words;
}

/*! 
 * Initializes the object from an XML element. Unsafe.
 *
 * \throws	ExceptionInvalidArgument	not a StringUTF8
 * \throws	ExceptionDomain	no CDATA found
 *
 * \param[in]	el	the XML element to read
 */
void Path::Deserialize(xml::Element &el)
{
	if (el.GetValue() != "Path")
	{
		throw ExceptionInvalidArgument(StringUTF8("void Path::deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	xml::Node c(el.GetFirstChild());
	if (!c)
		return; // no content
	xml::Text t(c.AsText()); // may throw
	*this = t.GetValue();
	ShrinkToFit();
}

/*! 
 * Dumps the object to an XML element. Unsafe. 
 *
 * \param[in]	parent	the parent element to which we will add the new element
 * \return	The newly created element, nullptr if failed.
 */
xml::Element Path::Serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement("Path"));
	el.PushBackText(*this);
	return el;
}


CRN_BEGIN_CLASS_CONSTRUCTOR(Path)
	CRN_DATA_FACTORY_REGISTER(U"Path", Path)
CRN_END_CLASS_CONSTRUCTOR(Path)

