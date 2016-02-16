/* Copyright 2010-2015 CoReNum, INSA-Lyon, Université Paris-Descartes
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
 * file: CRNData.cpp
 * \author Yann LEYDIER
 */

#include <CRNData/CRNData.h>
#include <CRNi18n.h>
#include <array>

using namespace crn;

/*!
 * Converts an int to CRN data
 * \param[in]  i  an integer
 * \return  a CRNInt that can be stored in crn::containers and serialized
 */
UInt crn::Data::ToCRN(int i)
{
	return std::make_unique<Int>(i);
}

/*!
 * Converts a double to CRN data
 * \param[in]  d  a double
 * \return  a CRNDouble that can be stored in crn::containers and serialized
 */
UReal crn::Data::ToCRN(double d)
{
	return std::make_unique<Real>(d);
}

/*!
 * Converts a String to CRN data
 * \param[in]  str  a String
 * \return  a SString that can be stored in crn::containers and serialized
 */
UString crn::Data::ToCRN(const String &str)
{
	return str.CloneAs<String>();
}

/*!
 * Converts a StringUTF8 to CRN data
 * \param[in]  str  a StringUTF8
 * \return  a SStringUTF8 that can be stored in crn::containers and serialized
 */
UStringUTF8 crn::Data::ToCRN(const StringUTF8 &str)
{
	return str.CloneAs<StringUTF8>();
}

/*!
 * Converts a Path to CRN data
 * \param[in]  str  a Path
 * \return  a Path that can be stored in crn::containers and serialized
 */
UPath crn::Data::ToCRN(const Path &str)
{
	return str.CloneAs<Path>();
}

/*!
 * Converts a Prop3 to CRN data
 * \param[in]  val	a Prop3
 * \return  a Prop3 that can be stored in crn::containers and serialized
 */
UProp3 crn::Data::ToCRN(const Prop3 &val)
{
	return val.CloneAs<Prop3>();
}

static const auto asciiOffset = 33;
static const auto pow85 = std::vector<unsigned int>{ 85*85*85*85, 85*85*85, 85*85, 85, 1 };
static void encodeBlock(size_t count, unsigned int &_tuple, StringUTF8 &s)
{
	auto block = std::array<uint8_t, 5>{};
	for (auto it = block.rbegin(); it != block.rend(); ++it)
	{
		*it = uint8_t((_tuple % 85) + asciiOffset);
		_tuple /= 85;
	}
	for (auto i = size_t(0); i < count; ++i)
	{
		s += char(block[i]);
	}
}

/*! Encodes any data into a printable string
 * \param[in]	data	a string of bytes
 * \param[in]	len	the number of bytes to convert
 * \return	an ASCII85-encoded printable string
 */
StringUTF8 crn::Data::ASCII85Encode(const uint8_t * const data, size_t len)
{
	auto count = 0;
	auto _tuple = 0u;
	auto s = StringUTF8{};

	for (auto tmp = size_t(0); tmp < len; ++tmp)
	{
		if (count >= 3)
		{
			_tuple |= data[tmp];
			if (_tuple == 0)
				s += 'z';
			else
				encodeBlock(size_t(5), _tuple, s);
			_tuple = 0;
			count = 0;
		}
		else
		{
			_tuple |= (data[tmp] << (24 - (count * 8)));
			count += 1;
		}
	}

	if (count > 0)
		encodeBlock(size_t(count + 1), _tuple, s);

	return s;
}


/*! \brief Decodes an ASCII85 string to a vector of bytes
 * \throws	ExceptionInvalidArgument	invalid ASCII85 string
 * \param[in]	s	an ASCII85 string
 * \return	a vector of data
 */
std::vector<uint8_t> crn::Data::ASCII85Decode(const StringUTF8 &s)
{
	auto data = std::vector<uint8_t>{};
	auto count = 0;
	auto processChar = false;
	auto block = std::array<uint8_t, 4>{};
	auto _tuple = 0u;
	
	for (auto tmp : crn::Range(s))
	{
		const auto c = s[tmp];
		switch (c)
		{
			case 'z':
				if (count != 0)
					throw crn::ExceptionInvalidArgument{StringUTF8{"ASCII85Decode()::"} + _("the character 'z' is invalid inside an ASCII85 block.")};
				std::fill(block.begin(), block.end(), 0);
				data.insert(data.end(), block.begin(), block.end());
				processChar = false;
				break;
			case '\n': case '\r': case '\t': case '\0': case '\f': case '\b': 
				processChar = false;
				break;
			default:
				if (c < '!' || c > 'u')
					throw crn::ExceptionInvalidArgument{StringUTF8{"ASCII85Decode()::"} + _("ASCII85 only allows characters '!' to 'u', but found a: ") + StringUTF8{c}};
				processChar = true;
				break;
		}

		if (processChar)
		{
			_tuple += (c - asciiOffset) * pow85[count];
			count += 1;
			if (count == 5)
			{
				for (auto i : crn::Range(block))
					block[i] = uint8_t(_tuple >> 24 - (int(i) * 8));
				data.insert(data.end(), block.begin(), block.end());
				_tuple = 0;
				count = 0;
			}
		}
	}

	if (count != 0)
	{
		if (count == 1)
			throw crn::ExceptionInvalidArgument{StringUTF8{"ASCII85Decode()::"} + _("the last block of ASCII85 data cannot be a single byte.")};
		count -= 1;
		_tuple += pow85[count];
		for (auto i = 0; i < count; ++i)
			block[i] = uint8_t(_tuple >> 24 - (i * 8));
		data.insert(data.end(), block.begin(), block.begin() + count);
	}
	return data;
}

