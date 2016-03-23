/* Copyright 2008-2016 INSA Lyon, CoReNum, Université Paris Descartes, ENS-Lyon
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
 * file: CRNString.h
 * \author Yann LEYDIER
 */

#ifndef CRNSTRING_Header
#define CRNSTRING_Header

#include <CRNObject.h>
#include <string>
#include <vector>
#include <sstream>
#include <complex>
#include <iomanip>
#include <stdlib.h>

/*! \defgroup string Strings 
 * \ingroup	base */
namespace crn
{
	class StringUTF8;
	class Prop3;

	/*! \addtogroup string */
	/*@{*/
	/*! \brief Transforms a character to its upper case */
	inline wint_t ToWUpper(wint_t c) { return towupper(c); }
	/*! \brief Transforms a character to its lower case */
	inline wint_t ToWLower(wint_t c) { return towlower(c); }
	/*@}*/

	/****************************************************************************/
	/*! \brief A UTF32 character string class
	 *
	 * class String
	 * Convenience UTF32 string class.
	 *
	 * \author 	Yann LEYDIER
	 * \date		May 2008
	 * \version 0.3
	 * \ingroup string
	 */
	class String: public Object
	{
		public:
			/**********************************************************************/
			/* Constructors                                                       */
			/**********************************************************************/
			/*! \brief Default constructor (empty string) */
			String() = default;
			virtual ~String() override = default;
			/*! \brief Constructor from a std wide string */
			String(const std::u32string &s):data(s) {}
			/*! \brief Constructor from a std wide string */
			String(std::u32string &&s) noexcept:data(std::move(s)) {}
			/*! \brief Constructor from a wide cstring */
			String(char32_t *s) {if (s) data = s; else data = U""; }
			/*! \brief Constructor from a wide cstring */
			String(const char32_t *s) {if (s) data = s; else data = U""; }
			/*! \brief Copy constructor */
			String(const String &s):data(s.data) {}
			/*! \brief Constructor from a wide char */
			String(char32_t c, size_t n = 1);
			/*! \brief Constructor from a UTF8 string */
			String(const StringUTF8 &s);
			/*! \brief Constructor from a Prop3 */
			String(const Prop3 &p);
			/*! \brief Constructor from a cstring */
			String(char *s);
			/*! \brief Constructor from a cstring */
			String(const char *s);
			/*! \brief Constructor from a std string */
			String(const std::string &s);
			/*! \brief Constructor from a char */
			String(char c, size_t n = 1);
			/*! \brief Constructor from an int */
			String(int i) { convertFrom(i); }
			/*! \brief Constructor from an unsigned int */
			String(unsigned int i) { convertFrom(i); }
			/*! \brief Constructor from an long */
			String(long i) { convertFrom(i); }
			/*! \brief Constructor from an unsigned long */
			String(unsigned long i) { convertFrom(i); }
			/*! \brief Constructor from a float */
			String(float f) { convertFrom(f); }
			/*! \brief Constructor from a double */
			String(double d) { convertFrom(double(d)); }
			/*! \brief Constructor from a long double */
			String(long double d) { convertFrom(d); }
			/*! \brief Constructor from an long */
			String(long long i) { convertFrom(i); }
			/*! \brief Constructor from an unsigned long */
			String(unsigned long long i) { convertFrom(i); }
			/*! \brief Constructor from a complex */
			template<typename T> String(const std::complex<T> &c) { *this = String(U"(") + c.real() + String(U", ") + c.imag() + String(U")"); }

			String(String&&) = default;
			String& operator=(String&&) = default;

			/**********************************************************************/
			/* Typecast methods                                                   */
			/**********************************************************************/
			/*! \brief Conversion to std u32string */
			std::u32string& Std() & noexcept { return data; }
			/*! \brief Conversion to std u32string */
			const std::u32string& Std() const & noexcept { return data; }
			/*! \brief Conversion to std u32string */
			std::u32string Std() && { return std::move(data); }
			/*! \brief Conversion to wide cstring */
			const char32_t* CWStr() const noexcept { return data.c_str(); }
			/*! \brief Conversion to UTF8 cstring */
			const char* CStr() const;
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
			/*! \brief Conversion to long double */
			long double ToLongDouble() const { return convertTo<long double>(); }
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
			/*!\brief Returns the length of the string */
			size_t Size() const noexcept { return data.length(); }
			/*!\brief Returns the length of the string */
			size_t Length() const noexcept { return data.length(); }
			/*!\brief Checks if the string is empty */
			bool operator!() const noexcept { return data.empty(); }
			/*!\brief Checks if the string is empty */
			bool IsEmpty() const noexcept { return data.empty(); }
			/*!\brief Checks if the string is not empty */
			bool IsNotEmpty() const noexcept { return !data.empty(); }

			/*!\brief Access to a character */
			char32_t& operator[](size_t index);
			/*!\brief Access to a character */
			const char32_t& operator[](size_t index) const;

			/*!\brief Copies from another string */
			String& operator=(const String &s) { data = s.data; return *this; }

			/*!\brief Appends a string */
			String& operator+=(const String &s) { data += s.data; return *this; }

			/*!\brief Inserts a string */
			String& Insert(size_t pos, const String &s);

			/*!\brief Crops the string */
			String& Crop(size_t pos, size_t n = 0);
			/*!\brief Erases a part of the string */
			String& Erase(size_t pos, size_t n = 0);

			/*!\brief Replaces a part of the string */
			String& Replace(const String &s, size_t pos, size_t n = 0);

			/*!\brief Replaces suffix by another pattern if present */
			String& ReplaceSuffix(const String &old_suffix, const String &new_suffix);
			/*!\brief Replaces last characters of the string by a given pattern */
			String& ReplaceSuffix(const size_t n, const String &new_suffix);
			/*!\brief Delete suffix if found */
			String& DeleteSuffix(const String &suffix);			
			/*!\brief Returns suffixes found within a collection */
			std::vector<String> WhichSuffixes(const std::vector<String> &suffixes) const;
			/*!\brief Returns prefixes found within a collection */
			std::vector<String> WhichPrefixes(const std::vector<String> &prefixes) const;
			/*!\brief Returns substring found within a collection */
			std::vector<String> WhichSubstrings(const std::vector<String> &substrings) const;

			/*!\brief Replaces all occurrences of a character with another */
			String& Replace(char32_t from, char32_t to);

			/*!\brief Converts the string to uppercase */
			String& ToUpper();
			/*!\brief Converts the first character of string to uppercase */
			String& FirstCharacterToUpper();

			/*!\brief Converts the string to lowercase */
			String& ToLower();

			/*!\brief Extracts a part of the string */
			String SubString(size_t pos, size_t n = 0) const;

			/*!\brief Finds the first occurrence of a string */
			size_t Find(const String &s, size_t from_pos = 0) const;
			/*!\brief Finds the first occurrence of character in a list */
			size_t FindAnyOf(const String &s, size_t from_pos = 0) const;
			/*!\brief Finds the first occurrence of character not in a list */
			size_t FindNotOf(const String &s, size_t from_pos = 0) const;

			/*!\brief Finds the last occurrence of a string */
			size_t BackwardFind(const String &s, size_t last_pos = NPos()) const;
			/*!\brief Finds the last occurrence of character in a list */
			size_t BackwardFindAnyOf(const String &s, size_t from_pos = NPos()) const;
			/*!\brief Finds the last occurrence of character not in a list */
			size_t BackwardFindNotOf(const String &s, size_t from_pos = NPos()) const;

			/*!\brief Check if string has a given prefix */
			bool StartsWith(const String &s) const;
			
			/*!\brief Check if string has a given suffix */
			bool EndsWith(const String &s) const;
			/*!\brief Check if string has suffix among some given ones */
			bool EndsWith(const std::vector<String> &suffixes) const;

			/*! \brief Splits the string in multiple strings delimited by a set of separators */
			std::vector<String> Split(const String &sep) const;

			/*! \brief Edit distance */
			int EditDistance(const String &s) const;

			/*! \brief Precision of the floating point conversion */
			static int& Precision() noexcept;

			/*! \brief Optimizes the memory usage */
			void ShrinkToFit();

			/*! \brief Swaps two strings */
			void Swap(String &str) noexcept { data.swap(str.data); }

			/*! \brief Last position in a string */
			static size_t NPos() noexcept;

			/*! \brief Generates an almost unique id */
			static String CreateUniqueId(size_t len = 8);

			/*! \brief Initializes the object from an XML element. Unsafe. */
			void Deserialize(xml::Element &el);
			/*! \brief Dumps the object to an XML element. Unsafe. */
			xml::Element Serialize(xml::Element &parent) const;

		private:
			/*! \brief Internal. */
			template<typename T> T convertTo() const { std::stringstream ss(CStr()); T val; ss >> val; return val; }
			/*! \brief Internal. */
			template<typename T> void convertFrom(T val) { std::stringstream ss; ss << std::setprecision(Precision()) << val; *this = ss.str(); }

			std::u32string data; /*!< internal string */
			mutable std::string cdata; /*!< temporary narrow string */
		
		CRN_DECLARE_CLASS_CONSTRUCTOR(String)
		CRN_SERIALIZATION_CONSTRUCTOR(String)
	};
	template<> struct IsClonable<String> : public std::true_type {};
	template<> struct IsSerializable<String> : public std::true_type {};

	/* \addtogroup string */
	/*@{*/

	/*! \brief Tests equality of two strings */
	inline bool operator==(const String &s1, const String &s2) { return s1.Std() == s2.Std(); }
	/*! \brief Tests inequality of two strings */
	inline bool operator!=(const String &s1, const String &s2) { return s1.Std() != s2.Std(); }
	/*! \brief Compares two strings */
	inline bool operator<(const String &s1, const String &s2) { return s1.Std() < s2.Std(); }
	/*! \brief Compares two strings */
	inline bool operator>(const String &s1, const String &s2) { return s1.Std() > s2.Std(); }
	/*! \brief Compares two strings */
	inline bool operator<=(const String &s1, const String &s2) { return s1.Std() <= s2.Std(); }
	/*! \brief Compares two strings */
	inline bool operator>=(const String &s1, const String &s2) { return s1.Std() >= s2.Std(); }
	/*! \brief Adds two strings */
	inline String operator+(const String &s1, const String &s2) { return String(s1.Std() + s2.Std()); }
	
	/*! \brief (almost) Universal conversion to String */
	template<typename T> String ToString(const T &obj) { return obj.ToString(); }
	/*! \brief (almost) Universal conversion to String */
	inline String ToString(const String &obj) { return obj; }

	/*! \brief Distance between two strings */
	inline double Distance(const String &s1, const String &s2) { return s1.EditDistance(s2); }

	/*! \brief Swaps two strings */
	inline void swap(String &s1, String &s2) noexcept { s1.Swap(s2); }

	/*! \brief Size of a string */
	inline size_t Size(const String &s) noexcept { return s.Size(); }

	namespace literals
	{
		/*! \brief String from literal */
		inline String operator"" _s(const char32_t *str, size_t len)
		{
			return String{std::u32string{str, len}};
		}
	}
	/*@}*/
}

#include <CRNStringPtr.h>

namespace std
{
	/*! \brief Swaps two strings */
	inline void swap(crn::String &s1, crn::String &s2) noexcept { s1.Swap(s2); }

	template<> struct hash<crn::String>
	{
		inline size_t operator()(const crn::String&s) const { return hash<u32string>{}(s.Std()); }
	};
}

#include <ostream>
/*! \brief stream output */
template<typename traits> inline std::basic_ostream<char, traits>& operator<<(std::basic_ostream<char, traits> &strm, const crn::String &s)
{
	strm << s.CStr();
	return strm;
}

#endif
