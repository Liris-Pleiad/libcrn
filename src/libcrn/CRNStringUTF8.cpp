/* Copyright 2006-2015 Yann LEYDIER, CoReNum, INSA-Lyon
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
 * file: CRNStringUTF8.cpp
 * \author Yann LEYDIER
 */

#include <CRNi18n.h>
#include <CRNStringUTF8.h>
#include <CRNException.h>
#include <3rdParty/unicode/UtfConverter.h>
#include <CRNString.h>
#include <CRNMath/CRNProp3.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <random>

using namespace crn;

/*! \return	the id of the class */
const String& StringUTF8::GetClassName() const
{ // defined here to avoid cyclic dependencies with crn::String
	static const String cn(U"StringUTF8"); 
	return cn; 
}

/*! Precision of the floating point conversion
 * \return	a reference to the configuration variable
 */
int& StringUTF8::Precision() noexcept
{
	static int precision = 16;
	return precision;
}

/*! Last position in a string
 * \return	std::string::npos
 */
size_t StringUTF8::NPos() noexcept
{
	return std::string::npos;
}

/*!
 * Constructor from a character
 *
 * \param[in]	c	the character to use
 * \param[in]	n	the number of characters to add
 */
StringUTF8::StringUTF8(char c, size_t n)
{
	data = std::string(n, c);
}

/*!
 * Constructor from a wide string
 *
 * \param[in]	s	the string to convert
 */
StringUTF8::StringUTF8(const String &s)
{
	data = UtfConverter::ToUtf8(s.Std());
}

/*! Constructor from a Prop3
 *
 * \param[in]	p	the Prop3 to convert
 */
StringUTF8::StringUTF8(const Prop3 &p)
{
	if (p.IsTrue())
		data = "true";
	else if (p.IsFalse())
		data = "false";
	else
		data = "unknown";
}

/*!
 * Conversion to wide string
 * \return	a wide string
 */
String StringUTF8::ToString() const
{
	return *this;
}

/*!
 * Access to a character
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	index	the index of the character in the string
 * \return	a reference to the character
 */
char& StringUTF8::operator[](size_t index) 
{ 
	if (index >= Size())
		throw ExceptionDomain(StringUTF8("char& StringUTF8::operator[](size_t index): ") + 
				_("index out of bounds."));
	return data[index]; 
}

/*!
 * Access to a character
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	index	the index of the character in the string
 * \return	a reference to the character
 */
const char& StringUTF8::operator[](size_t index) const 
{ 
	if (index >= Size())
		throw ExceptionDomain(StringUTF8("const char& StringUTF8::operator[](size_t index) const: ") + 
				_("index out of bounds."));
	return data[index]; 
}

/*! Conversion to Prop3
 * \return	a Prop3
 */
Prop3 StringUTF8::ToProp3() const
{
	if (data == "true")
		return Prop3::True;
	else if (data == "false")
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
StringUTF8& StringUTF8::Insert(size_t pos, const StringUTF8 &s)
{
	if (pos == NPos())
	{
		*this += s;
		return *this;
	}
	if (pos >= Size())
	{
		throw ExceptionDomain(StringUTF8("StringUTF8& StringUTF8::Insert(int pos, const StringUTF8 &s): ") + 
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
 * \return	a reference to the modified string
 */
StringUTF8 StringUTF8::SubString(size_t pos, size_t n) const
{
	if ((pos >= Size()) || (pos + n > Size()))
	{
		throw ExceptionDomain(StringUTF8("StringUTF8 StringUTF8::SubString(int pos, int n): ") + 
				StringUTF8(_("index out of bounds.")) + StringUTF8(pos) + 
				StringUTF8(" + ") + StringUTF8(n) + StringUTF8(" > ") + StringUTF8(Size()));
	}
	StringUTF8 s;
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
StringUTF8& StringUTF8::Crop(size_t pos, size_t n)
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
StringUTF8& StringUTF8::Erase(size_t pos, size_t n)
{
	if ((pos >= Size()) || (pos + n > Size()))
	{
		throw ExceptionDomain(StringUTF8("StringUTF8& StringUTF8::Erase(int pos, int n): ") + 
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
StringUTF8& StringUTF8::Replace(const StringUTF8 &s, size_t pos, size_t n)
{
	if ((pos >= Size()) || (pos + n > Size()))
	{
		throw ExceptionDomain(StringUTF8("StringUTF8& StringUTF8::Replace(const StringUTF8 &s, int pos, int n): ") + 
				_("index out of bounds."));
	}
	data.replace(pos, n, s.data);
	return *this;
}

/*!
 * Replaces all occurrences of a character with another
 *
 * \param[in]	from	the character to replace
 * \param[in]	to	the replacement character
 * \return	a reference to the modified string
 */
StringUTF8& StringUTF8::Replace(char from, char to)
{
	for (size_t tmp = 0; tmp < data.size(); tmp++)
		if (data[tmp] == from)
			data[tmp] = to;
	return *this;
}

/*!
 * Converts the string to uppercase.
 *
 * \return	a reference to the modified string
 */
StringUTF8& StringUTF8::ToUpper()
{
	for (size_t tmp = 0; tmp < data.size(); tmp++)
		data[tmp] = (char)toupper(data[tmp]);
	return *this;
}

/*!
 * Converts the first character of string to uppercase.
 *
 * \return	a reference to the modified string
 */
StringUTF8& StringUTF8::FirstCharacterToUpper()
{
	data[0] = (char)toupper(data[0]);
	return *this;
}

/*!
 * Converts the string to lowercase.
 *
 * \return	a reference to the modified string
 */
StringUTF8& StringUTF8::ToLower()
{
	for (size_t tmp = 0; tmp < data.size(); tmp++)
		data[tmp] = (char)tolower(data[tmp]);
	return *this;
}


/*!
 * Check if string begins with a certain prefix
 *
 * \param[in]	s	the prefix to be searched
 * 
 * \return	true if success, false else
 */
bool StringUTF8::StartsWith(const StringUTF8 &s) const
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

bool StringUTF8::EndsWith(const StringUTF8 &s) const
{
	size_t length_main = Length();
	size_t length_suffix = s.Length();
	
	if (length_main <= length_suffix)
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
 * Finds the first occurrence of a string
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	s	the string to search
 * \param[in]	from_pos	the position of the beginning
 * \return	the index of the first occurrence, or NPos if not found.
 */
size_t StringUTF8::Find(const StringUTF8 &s, size_t from_pos) const
{
	if (s.Size() > Size())
	{
		return NPos();
	}
	if (from_pos >= Size())
	{
		throw ExceptionDomain(StringUTF8("int StringUTF8::Find(const StringUTF8 &s, int from_pos = 0) const: ") +
			 _("index out of bounds."));
	}
	return data.find(s.data, from_pos);
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
size_t StringUTF8::FindAnyOf(const StringUTF8 &s, size_t from_pos) const
{
	if (from_pos >= Size())
	{
		throw ExceptionDomain(StringUTF8("int StringUTF8::FindAnyOf(const StringUTF8 &s, int from_pos = 0) const: ") + 
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
size_t StringUTF8::FindNotOf(const StringUTF8 &s, size_t from_pos) const
{
	if (from_pos >= Size())
	{
		throw ExceptionDomain(StringUTF8("int StringUTF8NotOf::Find(const StringUTF8 &s, int from_pos = 0) const: ") + 
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
size_t StringUTF8::BackwardFind(const StringUTF8 &s, size_t last_pos) const
{
	if (s.Size() > Size())
		return NPos();
	if (last_pos == NPos())
		return data.rfind(s.data);
	if (last_pos >= Size())
	{
		throw ExceptionDomain(StringUTF8("int StringUTF8::BackwardFind(const StringUTF8 &s, int last_pos = NPos()) const: ") + 
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
size_t StringUTF8::BackwardFindAnyOf(const StringUTF8 &s, size_t last_pos) const
{
	if (last_pos == NPos())
		return data.find_last_of(s.data);
	if (last_pos >= Size())
	{
		throw ExceptionDomain(StringUTF8("int StringUTF8::BackwardFindAnyOf(const StringUTF8 &s, int last_pos = NPos()) const: ") + 
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
size_t StringUTF8::BackwardFindNotOf(const StringUTF8 &s, size_t last_pos) const
{
	if (last_pos == NPos())
		return data.find_last_not_of(s.data);
	if (last_pos >= Size())
	{
		throw ExceptionDomain(StringUTF8("int StringUTF8::BackwardFindNotOf(const StringUTF8 &s, int last_pos = NPos()) const: ") + 
				_("index out of bounds."));
	}
	return data.find_last_not_of(s.data, last_pos);
}

////////////////////////////////////////////////////////////////////////

/*! 
 * Initializes the object from an XML element. Unsafe.
 *
 * \throws	ExceptionInvalidArgument	not a StringUTF8
 * \throws	ExceptionDomain	no CDATA found
 *
 * \param[in]	el	the XML element to read
 */
void StringUTF8::deserialize(xml::Element &el)
{
	if (el.GetValue() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool StringUTF8::deserialize(xml::Element &el): ") + 
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
xml::Element StringUTF8::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	el.PushBackText(*this);
	return el;
}

/*!
 * UNSAFE Greater or Equal
 *
 * \param[in]	l	the object to compare
 * \return	true if success, false else
 */
Prop3 StringUTF8::ge(const Object &l) const
{
	const StringUTF8 &s = (const StringUTF8&)l;
	if (data.compare(s.data) >= 0)
		return Prop3::True;
	else
		return Prop3::False;
}

/*!
 * UNSAFE Lower or Equal 
 *
 * \param[in]	l	the object to compare
 * \return	true if success, false else
 */
Prop3 StringUTF8::le(const Object &l) const
{
	const StringUTF8 &s = (const StringUTF8&)l;
	if (data.compare(s.data) <= 0)
		return Prop3::True;
	else
		return Prop3::False;
}

/*! 
 * Splits the string in multiple strings delimited by a set of separators 
 * \param[in]	sep	a list of separators
 *
 * \return	a list of substrings
 */
std::vector<StringUTF8> StringUTF8::Split(const StringUTF8 &sep) const
{
	size_t n = data.length();
	auto start = data.find_first_not_of(sep.data);
	std::vector<StringUTF8> words;
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
void StringUTF8::ShrinkToFit()
{
	std::string(data.begin(), data.end()).swap(data);
}

/*!
 * Generates an almost unique id
 * \param[in]	len	the length of the string
 *
 * \return  an random combination of [0..9], [a..z] and [A..Z]
 */
StringUTF8 StringUTF8::CreateUniqueId(size_t len)
{
	static auto urnd = std::mt19937{std::random_device{}()};
	static auto gen = std::uniform_int_distribution<int>{0, 61};
	auto cod = StringUTF8{' ', len};
	for (auto i : Range(cod.data))
	{
		auto val = gen(urnd);
		if (val < 10)
			cod[i] = char('0' + val);
		else if (val < 36)
			cod[i] = char('A' + val - 10);
		else
			cod[i] = char('a' + val - 36);
	}
	return cod;
	/*
	// create a temporary file name (in std C)
	StringUTF8 id(tmpnam(nullptr));
	// cut from / to be clean
	size_t cut = id.BackwardFindAnyOf("/\\");
	if (cut != NPos())
	{
		id.Crop(cut + 1);
	}
	return id;
	*/
}

/*! Returns the number of characters the string (lower or equal to Size())
 * \return	the actual number of character (letters, numbers and symbols) in the string
 */
size_t StringUTF8::Length() const noexcept
{
	if (IsEmpty())
		return 0;
	size_t i = 0;
	size_t j = 0;
	while (i != data.size())
	{
		if ((data[i] & 0xC0) != 0x80)
			j++;
		i++;
	}
	return j;
}

/*!
 * Replaces suffix by another pattern if present
 * \throws	ExceptionInvalidArgument	null suffix to search
*
 * \param[in]	old_suffix	the suffix to be repaced
 * \param[in]	new_suffix	the new pattern
 * \return	a reference to the modified string
 */
StringUTF8& StringUTF8::ReplaceSuffix(const StringUTF8 &old_suffix, const StringUTF8 &new_suffix)
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

CRN_BEGIN_CLASS_CONSTRUCTOR(StringUTF8)
	CRN_DATA_FACTORY_REGISTER(U"StringUTF8", StringUTF8)
CRN_END_CLASS_CONSTRUCTOR(StringUTF8)

