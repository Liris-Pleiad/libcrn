/* Copyright 2008-2012 INSA Lyon
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
 * file: UtfConverter.h
 * \author Yann LEYDIER
 */

#ifndef UTFCONVERTER__H__
#define UTFCONVERTER__H__
#include <string>
#include <CRNString.h>

/*! \ingroup string */
namespace UtfConverter
{
	std::basic_string<char32_t> FromUtf8(const std::string& utf8string);
	std::string ToUtf8(const std::basic_string<char32_t>& widestring);
}

#endif
 
