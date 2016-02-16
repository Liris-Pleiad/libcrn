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
 * file: CRNPath.h
 * \author Yann LEYDIER
 */

#ifndef CRNPath_HEADER
#define CRNPath_HEADER

#include <CRNStringUTF8.h>

namespace crn
{
	/*! \brief A convenience class for file paths
	 *
	 * A class to store a file or directory path
	 *
	 * \date Jul. 2010
	 * \author Yann LEYDIER
	 * \version 0.1
	 * \ingroup string
	 * \ingroup io
	 */
	class Path: public StringUTF8
	{
		public:
			enum class Format
			{
				AUTO = 0, URI = 1, UNIX = 2, WINDOWS = 3,
#ifdef CRN_PF_WIN32
				LOCAL = 3
#else
				LOCAL = 2
#endif
			};
			//////////////////////////////////////////////////////////////////////
			// Constructors
			//////////////////////////////////////////////////////////////////////

			/*! \brief Default constructor (empty string) */
			Path(Format fmt = Format::LOCAL):format(fmt) {}
			/*! \brief Constructor from a std string */
			Path(const std::string &s, Format fmt = Format::LOCAL):StringUTF8(s),format(Format::AUTO) { ConvertTo(fmt); }
			/*! \brief Constructor from a std string */
			Path(std::string &s, Format fmt = Format::LOCAL):StringUTF8(s),format(Format::AUTO) { ConvertTo(fmt); }
			/*! \brief Constructor from a cstring */
			Path(char *s, Format fmt = Format::LOCAL):StringUTF8(s),format(Format::AUTO) { ConvertTo(fmt); }
			/*! \brief Constructor from a cstring */
			Path(const char *s, Format fmt = Format::LOCAL):StringUTF8(s),format(Format::AUTO) { ConvertTo(fmt); }
			/*! \brief Copy constructor */
			Path(const Path &s, Format fmt = Format::AUTO):StringUTF8(s),format(s.format) { ConvertTo(fmt); }
			Path(Path&&) = default;
			/*! \brief Constructor from a UTF8 string */
			Path(const StringUTF8 &s, Format fmt = Format::LOCAL):StringUTF8(s),format(Format::AUTO) { ConvertTo(fmt); }
			/*! \brief Constructor from a UTF32 string */
			explicit Path(const String &s, Format fmt = Format::LOCAL):StringUTF8(s),format(Format::AUTO) { ConvertTo(fmt); }
			/*! \brief Constructor from a char */
			Path(char c, size_t n = 1, Format fmt = Format::LOCAL):StringUTF8(c, n),format(Format::AUTO) { ConvertTo(fmt); }
			/*! \brief Constructor from an int */
			Path(int i, Format fmt = Format::LOCAL):StringUTF8(i),format(fmt) { }	
			/*! \brief Constructor from an xml element */
			Path(xml::Element &el):format(Format::AUTO) { Deserialize(el); }
			virtual ~Path() override {}

			Path& operator=(const Path&) = default;
			Path& operator=(Path&&) = default;

			/*! \brief Returns the id of the class */
			virtual const String& GetClassName() const override;
			/*! \brief Creates another string from this one */
			virtual UObject Clone() const override { return std::make_unique<Path>(*this); }

			/*! \brief Splits the string in multiple strings delimited by a set of separators */
			std::vector<Path> Split(const StringUTF8 &sep) const;

			//////////////////////////////////////////////////////////////////////
			// Format info
			//////////////////////////////////////////////////////////////////////
			/*! \brief Returns the format */
			Format GetFormat() const;
			/*! \brief Is the path a URI? */
			bool IsURI() const;
			/*! \brief Is the path in Unix format? */
			bool IsUnix() const;
			/*! \brief Is the path in Windows format? */
			bool IsWindows() const;
			/*! \brief Converts to a specific format */
			Path& ConvertTo(Format fmt);
			
			//////////////////////////////////////////////////////////////////////
			// Generic methods
			//////////////////////////////////////////////////////////////////////
			
			/*!\brief Appends a string */
			const Path& operator+=(const Path &s);
			/*!\brief Appends a string after adding directory separator if needed */
			const Path& operator/=(const Path &s);

			/*! \brief Is the path absolute? */
			bool IsAbsolute() const;
			/*! \brief Is the path relative? */
			bool IsRelative() const { return !IsAbsolute(); }
			/*! \brief Returns the filename */
			Path GetFilename() const;
			/*! \brief Returns the base of the filename */
			Path GetBase() const;
			/*! \brief Returns the extension */
			Path GetExtension() const;
			/*! \brief Returns the full directory path */
			Path GetDirectory() const;

			/*! \brief Local directory separator */
			static char Separator() noexcept;

			/*! \brief Swaps two strings */
			void Swap(Path &str) noexcept { StringUTF8::Swap(str); std::swap(format, str.format); }

			//////////////////////////////////////////////////////////////////////
			// URI methods
			//////////////////////////////////////////////////////////////////////
	
			/*! \brief Returns the scheme of the URI */
			StringUTF8 GetScheme() const;
			/*! \brief Converts the path to URI */
			Path& ToURI();
			/*! \brief Replaces % codes with the corresponding character */
			Path& Decode();

			//////////////////////////////////////////////////////////////////////
			// Unix methods
			//////////////////////////////////////////////////////////////////////

			/*! \brief Converts the path to Unix format */
			Path& ToUnix();

			//////////////////////////////////////////////////////////////////////
			// Windows methods
			//////////////////////////////////////////////////////////////////////

			/*! \brief Gets the drive letter */
			char GetDrive() const;
			/*! \brief Invalid drive or no drive found */
			static char NoDrive(); 
			/*! \brief Converts the path to Windows format */
			Path& ToWindows();

			/*! \brief Converts the path to the local format */
			Path& ToLocal();
		private:
			Format format;

		CRN_DECLARE_CLASS_CONSTRUCTOR(Path)
	};

	/*! \addtogroup string */
	/*@{*/
	/*! \brief Adds two paths */
	inline Path operator+(const Path &s1, const Path &s2) { Path tmp(s1); return tmp += s2; }
	/*! \brief Adds two paths and insert directory separator between them if needed */
	inline Path operator/(const Path &s1, const Path &s2) { Path tmp(s1); return tmp /= s2; }
	/*! \brief Size of a path */
	inline size_t Size(Path &p) noexcept { return p.Size(); }
	/*! \brief Swaps two paths */
	inline void swap(Path &p1, Path &p2) noexcept { p1.Swap(p2); }

	namespace literals
	{
		/*! \brief Path from a literal */
		inline Path operator"" _p(const char *str, size_t len)
		{
			return Path{std::string{str, len}};
		}
	}	
	/*@}*/
}

namespace std
{
	/*! \brief Swaps two strings */
	inline void swap(crn::Path &p1, crn::Path &p2) noexcept { p1.Swap(p2); }

	template<> struct hash<crn::Path>
	{
		inline size_t operator()(const crn::Path &p) { return hash<string>{}(p.Std()); }
	};
}

#include <CRNIO/CRNPathPtr.h>
#endif


