/* Copyright 2008-2016 INSA Lyon, CoReNum, ENS-Lyon
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
 * file: CRNString.cpp
 * \author Yann LEYDIER
 */

#include <CRNi18n.h>
#include <CRNString.h>
#include <CRNStringUTF8.h>
#include <CRNException.h>
#include <3rdParty/unicode/UtfConverter.h>
#include <CRNStringUTF8.h>
#include <CRNMath/CRNProp3.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <algorithm> // for min & max

using namespace crn;

/*! Precision of the floating point conversion
 * \return	a reference to the configuration variable
 */
int& String::Precision() noexcept
{
	static int precision = 16;
	return precision;
}

/*! Last position in a string
 * \return	std::string::npos
 */
size_t String::NPos() noexcept
{
	return std::u32string::npos;
}

/*!
 * Constructor from a character
 *
 * \param[in]	c	the character to use
 * \param[in]	n	the number of characters to add
 */
String::String(char32_t c, size_t n)
{
	data = std::u32string(n, c);
}

/*!
 * Constructor from an UTF8 string
 *
 * \param[in]	s	the string to convert
 */
String::String(const StringUTF8 &s)
{
	data = UtfConverter::FromUtf8(s.Std());
}

/*! Constructor from a Prop3
 *
 * \param[in]	p	the Prop3 to convert
 */
String::String(const Prop3 &p)
{
	if (p.IsTrue())
		data = U"true";
	else if (p.IsFalse())
		data = U"false";
	else
		data = U"unknown";
}

/*!
 * Constructor from a cstring
 *
 * \param[in]	s	the string to convert
 */
String::String(char *s)
{
	data = UtfConverter::FromUtf8(s);
}

/*!
 * Constructor from a cstring
 *
 * \param[in]	s	the string to convert
 */
String::String(const char *s)
{
	data = UtfConverter::FromUtf8(s);
}

/*!
 * Constructor from a std string
 *
 * \param[in]	s	the string to convert
 */
String::String(const std::string &s)
{
	data = UtfConverter::FromUtf8(s);
}

/*!
 * Constructor from a character
 *
 * \param[in]	c	the character to use
 * \param[in]	n	the number of characters to add
 */
String::String(char c, size_t n)
{
	data = UtfConverter::FromUtf8(std::string(n, c));
}

/*!
 * Access to a character
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	index	the index of the character in the string
 *
 * \return	a reference to the character
 */
char32_t& String::operator[](size_t index)
{
	if (index >= Size())
		throw ExceptionDomain(StringUTF8("char& String::operator[](size_t index): ") +
				_("index out of bounds."));
	return data[index];
}

/*!
 * Access to a character
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	index	the index of the character in the string
 *
 * \return	a reference to the character
 */
const char32_t& String::operator[](size_t index) const
{
	if (index >= Size())
		throw ExceptionDomain(StringUTF8("const char& String::operator[](size_t index) const: ") +
				_("index out of bounds."));
	return data[index];
}

/*!
 * Conversion to a cstring
 *
 * \return	a cstring (do not free!)
 */
const char* String::CStr() const
{
	cdata = UtfConverter::ToUtf8(data);
	return cdata.c_str();
}

/*! Conversion to Prop3
 * \return	a Prop3
 */
Prop3 String::ToProp3() const
{
	if (data == U"true")
		return Prop3::True;
	else if (data == U"false")
		return Prop3::False;
	else
		return Prop3::Unknown;
}

/*!
 * Inserts a string
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	pos	the position of the insertion. 0 for begining, NPos for end.
 * \param[in]	s	the string to insert
 * \return	a reference to the modified string
 */
String& String::Insert(size_t pos, const String &s)
{
	if (pos == NPos())
	{
		*this += s;
		return *this;
	}
	if (pos >= Size())
	{
		throw ExceptionDomain(StringUTF8("String& String::Insert(int pos, const String &s): ") +
				_("index out of bounds."));
	}
	data.insert(pos, s.data);
	return *this;
}

/*!
 * Extracts a part of the string
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	pos	the position of the beginning
 * \param[in]	n	the number of characters to keep. 0 for all remaining characters.
 * \return	a new string
 */
String String::SubString(size_t pos, size_t n) const
{
	if ((pos >= Size()) || (pos + n > Size()))
	{
		throw ExceptionDomain(StringUTF8("String String::SubString(int pos, int n): ") +
				StringUTF8(_("index out of bounds.")) + StringUTF8(pos) +
				StringUTF8(" + ") + StringUTF8(n) + StringUTF8(" > ") + StringUTF8(Size()));
	}
	String s;
	if (n == 0)
	{
		s = data.substr(pos);
	}
	else
	{
		s = data.substr(pos, n);
	}
	return s;
}

/*!
 * Crops the string
 *
 * \param[in]	pos	the position of the beginning
 * \param[in]	n	the number of characters to keep. 0 for all remaining characters.
 * \return	a reference to the modified string
 */
String& String::Crop(size_t pos, size_t n)
{
	*this = SubString(pos, n);
	return *this;
}

/*!
 * Erases a part of the string
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	pos	the position of the beginning
 * \param[in]	n	the number of characters to erase. 0 for all remaining characters.
 * \return	a reference to the modified string
 */
String& String::Erase(size_t pos, size_t n)
{
	if ((pos >= Size()) || (pos + n > Size()))
	{
		throw ExceptionDomain(StringUTF8("String& String::Erase(int pos, int n): ") +
				_("index out of bounds."));
	}
	if (n <= 0)
		data.erase(pos);
	else
		data.erase(pos, n);
	return *this;
}

/*!
 * Replaces a part of the string
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	s	the string to insert
 * \param[in]	pos	the position of the beginning
 * \param[in]	n	the number of characters to erase. 0 for all remaining characters.
 * \return	a reference to the modified string
 */
String& String::Replace(const String &s, size_t pos, size_t n)
{
	if ((pos >= Size()) || (pos + n > Size()))
	{
		throw ExceptionDomain(StringUTF8("String& String::Replace(const String &s, int pos, int n): ") +
				_("index out of bounds."));
	}
	data.replace(pos, n, s.data);
	return *this;
}

/*!
 * Replaces suffix by another pattern if present
 * \throws	ExceptionInvalidArgument	null suffix to search
 * \param[in]	old_suffix	the suffix to be repaced
 * \param[in]	new_suffix	the new pattern
 * \return	a reference to the modified string
 */
String& String::ReplaceSuffix(const String &old_suffix, const String &new_suffix)
{
	size_t old_suffix_length = old_suffix.Length();
	if (!old_suffix_length)
		throw ExceptionInvalidArgument(_("Null suffix to search."));
	size_t word_length = Length();

	if (old_suffix_length <= word_length)
	{
		size_t pivot = word_length - old_suffix_length;

		if (EndsWith(old_suffix))
		{
			data.erase(pivot);
			data.append(new_suffix.Std());
		}
	}

	return *this;
}

/*!
 * Replaces last characters by a given pattern
*
 * \param[in]	n	the number of character to erase at the end of the word
 * \param[in]	new_suffix	the pattern to append
 * \return	a reference to the modified string
 */
String& String::ReplaceSuffix(const size_t n, const String &new_suffix)
{
	size_t word_length = Length();

	if (n <= word_length)
	{
		size_t pivot = word_length - n;

		data.erase(pivot);
		data.append(new_suffix.Std());
	}

	return *this;
}

/*!
 * Delete suffix if found
 *
 * \throws	ExceptionInvalidArgument	null suffix to delete
 * \param[in]	suffix	the suffix to be dropped
 * \return	a reference to the modified string
 */
String& String::DeleteSuffix(const String &suffix)
{
	size_t suffix_length = suffix.Length();
	if (!suffix_length)
		throw ExceptionInvalidArgument(_("Null suffix to delete."));
	size_t word_length = Length();

	if (suffix_length <= word_length)
	{
		size_t pivot = word_length - suffix_length;

		if (EndsWith(suffix))
		{
			data.erase(pivot);
		}
	}

	return *this;
}

/*!
 * Returns suffix within a collection (void String if not found)
 *
 * \param[in]	suffixes	collection of suffixes to try
 * \return	a vector of candidates found as suffixes of the string
 */
std::vector<String> String::WhichSuffixes(const std::vector<String> &suffixes) const
{
	std::vector<String> found;

	size_t n = suffixes.size();

	for (size_t k = 0; k < n; k++)
	{
		String sf = suffixes[k];

		if (EndsWith(sf))
		{
			found.push_back(sf);
		}
	}

	return found;
}

/*!
 * Returns prefix within a collection (void String if not found)
 *
 * \param[in]	prefixes	collection of prefixes to try
 * \return	a vector of candidates found as prefixes of the string
 */
std::vector<String> String::WhichPrefixes(const std::vector<String> &prefixes) const
{
	std::vector<String> found;

	size_t n = prefixes.size();

	for (size_t k = 0; k < n; k++)
	{
		String pf = prefixes[k];

		if (StartsWith(pf))
		{
			found.push_back(pf);
		}
	}

	return found;
}

/*!
 * Returns substrings within a collection (void String if not found)
 *
 * \param[in]	substrings	collection of substrings to try
 * \return	a vector of candidates found as subpatterns of the string
 */
std::vector<String> String::WhichSubstrings(const std::vector<String> &substrings) const
{
	std::vector<String> found;

	for (auto str : substrings)
	{
		

		if (Find(str) != String::NPos())
		{
			found.push_back(str);
		}
	}

	return found;
}



/*!
 * Replaces all occurrences of a character with another
 *
 * \param[in]	from	the character to replace
 * \param[in]	to	the replacement character
 * \return	a reference to the modified string
 */
String& String::Replace(char32_t from, char32_t to)
{
	for (auto & elem : data)
		if (elem == from)
			elem = to;
	return *this;
}

/*!
 * Converts the string to uppercase.
 *
 * \return	a reference to the modified string
 */
String& String::ToUpper()
{
	for (auto & elem : data)
		elem = char32_t(ToWUpper(wint_t(elem)));
	return *this;
}

/*!
 * Converts first character of the string to uppercase.
 *
 * \return	a reference to the modified string
 */
String& String::FirstCharacterToUpper()
{
	data[0] = char32_t(ToWUpper(wint_t(data[0])));

	return *this;
}

/*!
 * Converts the string to lowercase.
 *
 * \return	a reference to the modified string
 */
String& String::ToLower()
{
	for (auto & elem : data)
		elem = char32_t(ToWLower(wint_t(elem)));
	return *this;
}

/*!
 * Finds the first occurrence of a string
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	s	the string to search
 * \param[in]	from_pos	the position of the beginning
 * \return	the index of the first occurrence, or NPos if not found.
 */
size_t String::Find(const String &s, size_t from_pos) const
{
	if (IsEmpty())
		return NPos();
	if (s.Size() > Size())
	{
		return NPos();
	}
	if (from_pos >= Size())
	{
		throw ExceptionDomain(StringUTF8("int String::Find(const String &s, int from_pos = 0) const: ") +
				_("index out of bounds."));
	}
	return data.find(s.data, from_pos);
}


/*!
 * Check if string begins with a certain prefix
 *
 * \param[in]	s	the prefix to be searched
 *
 * \return	true if success, false else
 */
bool String::StartsWith(const String &s) const
{
	size_t idx = Find(s, 0);

	if (idx == NPos())
	{
		return false;
	}
	else
	{
		if (idx == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

/*!
 * Check if string ends with a certain suffix
 *
 * \param[in]	s	the suffix to be searched
 *
 * \return	true if success, false else
 */

bool String::EndsWith(const String &s) const
{
	size_t length_main = Length();
	size_t length_suffix = s.Length();

	if (length_main < length_suffix)
	{
		return false;
	}
	else
	{
		size_t pos = length_main - length_suffix;
		size_t idx = Find(s, pos);

		return (idx == pos);
	}
}

/*!
 * Check if string ends with a suffix in a given collection
 *
 * \param[in]	suffixes	the suffix to be searched
 *
 * \return	true if success, false else
 */
bool String::EndsWith(const std::vector<String> &suffixes) const
{
	size_t nb_suffixes = suffixes.size();
	size_t k = 0;
	bool found = false;

	while (!found && (k < nb_suffixes))
	{
		found = EndsWith(suffixes[k]);
		k++;
	}

	return found;
}

/*!
 * Finds the first occurrence of a character in a list
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	s	the list
 * \param[in]	from_pos	the position of the beginning
 * \return	the index of the first occurrence, or NPos if not found.
 */
size_t String::FindAnyOf(const String &s, size_t from_pos) const
{
	if (from_pos >= Size())
	{
		throw ExceptionDomain(StringUTF8("int String::FindAnyOf(const String &s, int from_pos = 0) const: ") +
				_("index out of bounds."));
	}

	return data.find_first_of(s.data, from_pos);
}

/*!
 * Finds the first occurrence of a character not in a list
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	s	the list
 * \param[in]	from_pos	the position of the beginning
 * \return	the index of the first occurrence, or NPos if not found.
 */
size_t String::FindNotOf(const String &s, size_t from_pos) const
{
	if (from_pos >= Size())
	{
		throw ExceptionDomain(StringUTF8("int String::NotOf::FindNotOf(const String &s, int from_pos = 0) const: ") +
				_("index out of bounds."));
	}

	return data.find_first_not_of(s.data, from_pos);
}

/*!
 * Finds the last occurrence of a string
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	s	the string to search
 * \param[in]	last_pos	the position of the beginning, NPos for the end.
 * \return	the index of the first occurrence, or NPos if not found.
 */
size_t String::BackwardFind(const String &s, size_t last_pos) const
{
	if (s.Size() > Size())
		return NPos();
	if (last_pos == NPos())
		return data.rfind(s.data);
	if (last_pos >= Size())
	{
		throw ExceptionDomain(StringUTF8("int String::BackwardFind(const String &s, int last_pos = -1) const: ") +
				_("index out of bounds."));
	}
	return data.rfind(s.data, last_pos);
}

/*!
 * Finds the last occurrence of a character in a list
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	s	the list
 * \param[in]	last_pos	the position of the beginning, NPos for the end.
 * \return	the index of the first occurrence, or NPos if not found.
 */
size_t String::BackwardFindAnyOf(const String &s, size_t last_pos) const
{
	if (last_pos == NPos())
		return data.find_last_of(s.data);
	if (last_pos >= Size())
	{
		throw ExceptionDomain(StringUTF8("int String::BackwardFindAnyOf(const String &s, int last_pos = -1) const: ") +
				_("index out of bounds."));
	}
	return data.find_last_of(s.data, last_pos);
}

/*!
 * Finds the last occurrence of a character not in a list
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	s	the list
 * \param[in]	last_pos	the position of the beginning, NPos for the end.
 * \return	the index of the first occurrence, or NPos if not found.
 */
size_t String::BackwardFindNotOf(const String &s, size_t last_pos) const
{
	if (last_pos == NPos())
		return data.find_last_not_of(s.data);
	if (last_pos >= Size())
	{
		throw ExceptionDomain(StringUTF8("int String::BackwardFindNotOf(const String &s, int last_pos = -1) const: ") +
				_("index out of bounds."));
	}
	return data.find_last_not_of(s.data, last_pos);
}

/*!
 * Edit distance
 *
 * \author Wikibooks
 * \param[in]	s	the string to compare
 * \return	the amount of suppression, addition and change to morph from one string to the other
 */
int String::EditDistance(const String &s) const
{
	unsigned int s1 = (unsigned int)Size();
	unsigned int s2 = (unsigned int)s.Size();
	std::vector<std::vector<unsigned int> > d(s1 + 1, std::vector<unsigned int>(s2 + 1));

	for (unsigned int i = 1; i <= s1; ++i) d[i][0] = i;
	for (unsigned int i = 1; i <= s2; ++i) d[0][i] = i;

	for (unsigned int i = 1; i <= s1; ++i)
		for (unsigned int j = 1; j <= s2; ++j)
			d[i][j] = std::min(std::min(d[i - 1][j] + 1, d[i][j - 1] + 1),
					d[i - 1][j - 1] + (data[i - 1] == s[j - 1] ? 0 : 1));
	return d[s1][s2];
}

/*!
 * Initializes the object from an XML element. Unsafe.
 *
 * \throws	ExceptionInvalidArgument	not a String
 * \throws	ExceptionDomain	no CDATA found
 *
 * \param[in]	el	the XML element to read
 */
void String::Deserialize(xml::Element &el)
{
	if (el.GetName() != "String")
	{
		throw ExceptionInvalidArgument(StringUTF8("void String::Deserialize(xml::Element &el): ") +
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
xml::Element String::Serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement("String"));
	el.PushBackText(CStr());
	return el;
}

/*!
 * Splits the string in multiple strings delimited by a set of separators
 * \param[in]	sep	a list of separators
 *
 * \return	a list of substrings
 */
std::vector<String> String::Split(const String &sep) const
{
	size_t n = data.length();
	auto start = data.find_first_not_of(sep.data);
	std::vector<String> words;
	while (start < n)
	{
		auto stop = data.find_first_of(sep.data, start);
		if (stop > n) stop = n;
			words.push_back(data.substr(start, stop - start));
		start = data.find_first_not_of(sep.data, stop + 1);
	}
	return words;
}

/*! Optimizes the memory usage */
void String::ShrinkToFit()
{
	data.shrink_to_fit();
}

/*!
 * Generates an almost unique id
 * \param[in]	len	the length of the string
 *
 * \return  an random combination of [0..9], [a..z] and [A..Z]
 */
crn::String String::CreateUniqueId(size_t len)
{
	return StringUTF8::CreateUniqueId(len);
}

CRN_BEGIN_CLASS_CONSTRUCTOR(String)
	CRN_DATA_FACTORY_REGISTER(U"String", String)
CRN_END_CLASS_CONSTRUCTOR(String)

