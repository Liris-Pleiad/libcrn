/* Copyright 2006-2015 Yann LEYDIER, CoReNum, INSA-Lyon, Université Paris Descartes
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
 * file: CRNStringUTF8.h
 * \author Yann LEYDIER
 */

#ifndef CRNSTRINGUTF8_HEADER
#define CRNSTRINGUTF8_HEADER

#include <CRNObject.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <complex>
#include <stdlib.h>

namespace crn
{
	class String;

	/****************************************************************************/
	/*! \brief A character string class
	 *
	 * class SStringUTF8
	 * Convenience string class.
	 *
	 * \author 	Yann LEYDIER
	 * \date		May 2008
	 * \version 0.1
	 * \ingroup string
	 */
	class StringUTF8: public Object
	{
		public:
			/**********************************************************************/
			/* Constructors                                                       */
			/**********************************************************************/
			/*! \brief Default constructor (empty string) */
			StringUTF8() {}
			virtual ~StringUTF8() override {}
			/*! \brief Constructor from a std string */
			StringUTF8(const std::string &s):data(s) {}
			/*! \brief Constructor from a std string */
			StringUTF8(std::string &&s) noexcept:data(std::move(s)) {}
			/*! \brief Constructor from a cstring */
			StringUTF8(char *s) {if (s) data = s; else data = ""; }
			/*! \brief Constructor from a cstring */
			StringUTF8(const char *s) {if (s) data = s; else data = ""; }
			/*! \brief Copy constructor */
			StringUTF8(const StringUTF8 &s):data(s.data) {}
			/*! \brief Constructor from a UTF32 string */
			explicit StringUTF8(const String &s);
			/*! \brief Constructor from a Prop3 */
			StringUTF8(const Prop3 &p);
			/*! \brief Constructor from a char */
			StringUTF8(char c, size_t n = 1);
			/*! \brief Constructor from an int */
			StringUTF8(int i) { convertFrom(i); }
			/*! \brief Constructor from an unsigned int */
			StringUTF8(unsigned int i) { convertFrom(i); }
			/*! \brief Constructor from a long */
			StringUTF8(long i) { convertFrom(i); }
			/*! \brief Constructor from an unsigned long */
			StringUTF8(unsigned long i) { convertFrom(i); }
			/*! \brief Constructor from a float */
			StringUTF8(float f) { convertFrom(f); }
			/*! \brief Constructor from a double */
			StringUTF8(double d) { convertFrom(d); }
			/*! \brief Constructor from a long double */
			StringUTF8(long double d) { convertFrom(d); }
			/*! \brief Constructor from an long */
			StringUTF8(long long i) { convertFrom(i); }
			/*! \brief Constructor from an unsigned long */
			StringUTF8(unsigned long long i) { convertFrom(i); }
			/*! \brief Constructor from a complex */
			template<typename T> StringUTF8(const std::complex<T> &c) { *this = StringUTF8("(") + c.real() + StringUTF8(", ") + c.imag() + StringUTF8(")"); }

			StringUTF8(StringUTF8&&) = default;
			StringUTF8& operator=(StringUTF8&&) = default;

			/**********************************************************************/
			/* Protocols                                                          */
			/**********************************************************************/
			/*! \brief This a Clonable, Serializable and POSet object */
			virtual Protocol GetClassProtocols() const noexcept override { return crn::Protocol::Clonable|crn::Protocol::Serializable|crn::Protocol::POSet; }
			/*! \brief Returns the id of the class */
			virtual const String& GetClassName() const override;
			/*! \brief Copies to wide string */
			virtual String ToString() const override;
			/*! \brief Creates another string from this one */
			virtual UObject Clone() const override { return std::make_unique<StringUTF8>(*this); }
			
			/**********************************************************************/
			/* Typecast methods                                                   */
			/**********************************************************************/
			/*! \brief Conversion to std string */
			std::string& Std() & noexcept { return data; }
			/*! \brief Conversion to std string */
			const std::string& Std() const & noexcept { return data; }
			/*! \brief Conversion to std string */
			std::string Std() && { return std::move(data); }
			/*! \brief Conversion to UTF8 cstring */
			const char* CStr() const noexcept { return data.c_str(); }
			/*! \brief Conversion to Prop3 */
			Prop3 ToProp3() const;
			/*! \brief Conversion to int */
			int ToInt() const { return convertTo<int>(); }
			/*! \brief Conversion to unsigned int */
			unsigned int ToUInt() const { return convertTo<unsigned int>(); }
			/*! \brief Conversion to long */
			long ToLong() const { return convertTo<long>(); }
			/*! \brief Conversion to unsigned long */
			unsigned long ToULong() const { return convertTo<unsigned long>(); }
			/*! \brief Conversion to float */
			float ToFloat() const { return convertTo<float>(); }
			/*! \brief Conversion to double */
			double ToDouble() const { return convertTo<double>(); }
#ifndef __SYMBIAN32__
			/*! \brief Conversion to long double */
			long double ToLongDouble() const { return convertTo<long double>(); }
#endif
			/*! \brief Conversion to long long */
			long long ToLongLong() const { return convertTo<long long>(); }
			/*! \brief Conversion to unsigned long long */
			unsigned long long ToULongLong() const { return convertTo<unsigned long long>(); }
			/*! \brief Conversion to int64_t */
			int64_t Toint64_t() const { return convertTo<int64_t>(); }
			/*! \brief Conversion to uint64_t */
			uint64_t Touint64_t() const { return convertTo<uint64_t>(); }
			
			/**********************************************************************/
			/* String manipulation                                                */
			/**********************************************************************/
			/*!\brief Returns the number of bytes in the string */
			size_t Size() const noexcept { return data.size(); }
			/*!\brief Returns the number of characters the string */
			size_t Length() const noexcept;
			/*!\brief Checks if the string is empty */
			bool operator!() const noexcept { return data.empty(); }
			/*!\brief Checks if the string is empty */
			bool IsEmpty() const noexcept { return data.empty(); }
			/*!\brief Checks if the string is not empty */
			bool IsNotEmpty() const noexcept { return !data.empty(); }

			/*!\brief Access to a character */
			char& operator[](size_t index);
			/*!\brief Access to a character */
			const char& operator[](size_t index) const;

			/*!\brief Copies from another string */
			StringUTF8& operator=(const StringUTF8 &s) { data = s.data; return *this; }

			/*!\brief Appends a string */
			StringUTF8& operator+=(const StringUTF8 &s) { data += s.data; return *this; }

			/*!\brief Inserts a string */
			StringUTF8& Insert(size_t pos, const StringUTF8 &s);

			/*!\brief Crops the string */
			StringUTF8& Crop(size_t pos, size_t n = 0);
			/*!\brief Erases a part of the string */
			StringUTF8& Erase(size_t pos, size_t n = 0);

			/*!\brief Replaces a part of the string */
			StringUTF8& Replace(const StringUTF8 &s, size_t pos, size_t n = 0);

			/*!\brief Replaces all occurrences of a character with another */
			StringUTF8& Replace(char from, char to);

			/*!\brief Converts the string to uppercase */
			StringUTF8& ToUpper();
			/*!\brief Converts the first character of string to uppercase */
			StringUTF8& FirstCharacterToUpper();

			/*!\brief Converts the string to lowercase */
			StringUTF8& ToLower();

			/*!\brief Extracts a part of the string */
			StringUTF8 SubString(size_t pos, size_t n = 0) const;

			/*!\brief Check if string has a given prefix */
			bool StartsWith(const StringUTF8 &s) const;
			/*!\brief Check if string has a given suffix */
			bool EndsWith(const StringUTF8 &s) const;
		
			/*!\brief Finds the first occurrence of a string */
			size_t Find(const StringUTF8 &s, size_t from_pos = 0) const;
			/*!\brief Finds the first occurrence of character in a list */
			size_t FindAnyOf(const StringUTF8 &s, size_t from_pos = 0) const;
			/*!\brief Finds the first occurrence of character not in a list */
			size_t FindNotOf(const StringUTF8 &s, size_t from_pos = 0) const;

			/*!\brief Finds the last occurrence of a string */
			size_t BackwardFind(const StringUTF8 &s, size_t last_pos = NPos()) const;
			/*!\brief Finds the last occurrence of character in a list */
			size_t BackwardFindAnyOf(const StringUTF8 &s, size_t from_pos = NPos()) const;
			/*!\brief Finds the last occurrence of character not in a list */
			size_t BackwardFindNotOf(const StringUTF8 &s, size_t from_pos = NPos()) const;

			/*!\brief Replaces suffix by another pattern if present */
			StringUTF8& ReplaceSuffix(const StringUTF8 &old_suffix, const StringUTF8 &new_suffix);

			/*! \brief Splits the string in multiple strings delimited by a set of separators */
			std::vector<StringUTF8> Split(const StringUTF8 &sep) const;

			/*! \brief Optimizes the memory usage */
			void ShrinkToFit();

			/*! \brief Swaps two strings */
			void Swap(StringUTF8 &str) noexcept { data.swap(str.data); }

			/*! \brief Precision of the floating point conversion */
			static int& Precision() noexcept; 
			/*! \brief Last position in a string */
			static size_t NPos() noexcept; 

			/*! \brief Generates an almost unique id */
			static crn::StringUTF8 CreateUniqueId(size_t len = 8);

		private:

			// CRNPROTOCOL_SERIALIZABLE
			/*! \brief Initializes the object from an XML element. Unsafe. */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps the object to an XML element. Unsafe. */
			virtual xml::Element serialize(xml::Element &parent) const override;
			// CRNPROTOCOL_POSET
			/*! \brief UNSAFE Greater or Equal */
			virtual Prop3 ge(const Object &l) const override;
			/*! \brief UNSAFE Lower or Equal */
			virtual Prop3 le(const Object &l) const override;

			/*! \brief Internal. */
			template<typename T> T convertTo() const { std::stringstream ss(data); T val; ss >> val; return val; }
			/*! \brief Internal. */
			template<typename T> void convertFrom(T val) { std::stringstream ss; ss << std::setprecision(Precision()) << val; data = ss.str(); }


			std::string data; /*!< internal string */
		
		CRN_DECLARE_CLASS_CONSTRUCTOR(StringUTF8)
		CRN_SERIALIZATION_CONSTRUCTOR(StringUTF8)
	};

	/*! \addtogroup string */
	/*@{*/

	/*! \brief Tests equality of two strings */
	inline bool operator==(const StringUTF8 &s1, const StringUTF8 &s2) { return s1.Std() == s2.Std(); }
	/*! \brief Tests inequality of two strings */
	inline bool operator!=(const StringUTF8 &s1, const StringUTF8 &s2) { return s1.Std() != s2.Std(); }
	/*! \brief Compares two strings */
	inline bool operator<(const StringUTF8 &s1, const StringUTF8 &s2) { return s1.Std() < s2.Std(); }
	/*! \brief Adds two strings */
	inline StringUTF8 operator+(const StringUTF8 &s1, const StringUTF8 &s2) { return StringUTF8(s1.Std() + s2.Std()); }

	/*! \brief Swaps two strings */
	inline void swap(StringUTF8 &s1, StringUTF8 &s2) noexcept { s1.Swap(s2); }
	
	/*! \brief Size of a string */
	inline size_t Size(const StringUTF8 &s) noexcept { return s.Size(); }

	namespace literals
	{
		/*! \brief StringUTF8 from literal */
		inline StringUTF8 operator"" _s(const char *str, size_t len)
		{
			return StringUTF8{std::string{str, len}};
		}
	}
	/*@}*/
}

#include <CRNStringUTF8Ptr.h>

namespace std
{
	/*! \brief Swaps two strings */
	template<> inline void swap<crn::StringUTF8>(crn::StringUTF8 &s1, crn::StringUTF8 &s2) noexcept { s1.Swap(s2); }

	template<> struct hash<crn::StringUTF8>
	{
		inline size_t operator()(const crn::StringUTF8 &s) const { return hash<string>{}(s.Std()); }
	};
}

#include <ostream>
/*! \brief stream output */
template<typename traits> inline std::basic_ostream<char, traits>& operator<<(std::basic_ostream<char, traits> &strm, const crn::StringUTF8 &s)
{
	strm << s.CStr();
	return strm;
}


#endif
