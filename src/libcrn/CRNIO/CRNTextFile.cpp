/* Copyright 2006-2014 Yann LEYDIER, CoReNum, INSA-Lyon
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
 * file: CRNTextFile.cpp
 * \author Yann LEYDIER
 */

#include <CRNi18n.h>
#include <CRNIO/CRNTextFile.h>
#include <fstream>
#include <CRNData/CRNForeach.h>
#include <CRNException.h>

using namespace crn;

/*****************************************************************************/
/*! Blank constructor
 *
 * \param[in]	enc	the default encoding for writting
 */
TextFile::TextFile(Encoding enc):
	original_encoding(enc)
{
}

/*****************************************************************************/
/*! File constructor
 *
 * \throws	ExceptionIO	cannot open file
 *
 * \param[in]	fname	the file to load
 * \param[in]	enc	the encoding of the file
 */
TextFile::TextFile(const Path &fname, Encoding enc):
	filename(fname),
	original_encoding(enc)
{
	std::ifstream in;
	in.open(fname.CStr());
	if (!in.is_open())
		throw ExceptionIO(StringUTF8("TextFile::TextFile(const Path &fname, Encoding enc): ") + _("Cannot open file ") + StringUTF8(fname));
	std::filebuf *pbuf = in.rdbuf();
	size_t size = size_t(pbuf->pubseekoff(0, std::ios::end, std::ios::in));
	pbuf->pubseekpos(0, std::ios::in);
	if (enc == Encoding::UTF_32)
	{
		size_t wsize = size / sizeof(char32_t);
		std::vector<char32_t> buffer(wsize + 1);
		pbuf->sgetn((char*)&(buffer.front()), size);
		buffer[wsize] = U'\0'; // ensure that the string as a terminal 0
		text = &(buffer.front());
	}
	else
	{
		std::vector<char> buffer(size + 1);
		pbuf->sgetn(&(buffer.front()), size);
		buffer[size] = '\0'; // ensure that the string as a terminal 0
		text = &(buffer.front());
	}
	in.close();
}

/*****************************************************************************/
/*!
 * Saves to file
 *
 * \throws	ExceptionIO	cannot open file
 *
 * \param[in]  fname  the file name
 */
void TextFile::Save(const Path &fname)
{
	std::ofstream out;
	out.open(fname.CStr());
	if (!out.is_open())
	{
		throw ExceptionIO(StringUTF8("void TextFile::Save(const Path &fname): ") + _("cannot open file ") + StringUTF8(fname));
	}
	if (original_encoding == Encoding::UTF_32)
		out.write((char*)text.CWStr(), text.Size() * sizeof(char32_t));
	else
		out << text.CStr();
	out.close();
	filename = fname;
}

/*****************************************************************************/
/*!
 * Saves to file
 *
 * \throws	ExceptionIO	cannot open file
 *
 * \param[in]  fname  the file name
 * \param[in]	enc	the encoding for saving
 */
void TextFile::Save(const Path &fname, Encoding enc)
{
	std::ofstream out;
	out.open(fname.CStr());
	if (!out.is_open())
	{
		throw ExceptionIO(StringUTF8("void TextFile::Save(const Path &fname, Encoding enc): ") + _("cannot open file ") + StringUTF8(fname));
	}
	original_encoding = enc;
	if (enc == Encoding::UTF_32)
		out.write((char*)text.CWStr(), text.Size() * sizeof(char32_t));
	else
		out << text.CStr();
	out.close();
	filename = fname;
}

/*****************************************************************************/
/*!
 * Saves to file
 *
 * \throws	ExceptionIO	cannot open file
 */
void TextFile::Save()
{
	std::ofstream out;
	out.open(filename.CStr());
	if (!out.is_open())
	{
		throw ExceptionIO(StringUTF8("void TextFile::Save(): ") + _("cannot open file ") + StringUTF8(filename));
	}
	if (original_encoding == Encoding::UTF_32)
		out.write((char*)text.CWStr(), text.Size() * sizeof(char32_t));
	else
		out << text.CStr();
	out.close();
}


/*****************************************************************************/
/*!
 * Extract unique words
 *
 * \param[in]	case_sensitive	if false, all words are converted to lower case
 * \return  a set of words, sorted in alphabetical order
 */
std::set<String> TextFile::ExtractWords(bool case_sensitive) const
{
	std::set<String> words;

	std::vector<String> stxt(SplitText());
	for (String &s : stxt)
	{
		if (!case_sensitive)
		{
			s.ToLower();
		}
		words.insert(s);
	}
	return words;
}

/*****************************************************************************/
/*!
 * Extract unique words and sort them from least frequent to most frequent.
 *
 * If you want to sweep to words from the most frequent to the least frequent, use a reverse_iterator.
 *
 * \param[in]	case_sensitive	if true, all words are converted to lower case
 * \return  a map of occurrences and words, sorted from least frequent to most frequent
 */
std::map<int, String> TextFile::ExtractWordsByFrequency(bool case_sensitive) const
{
	std::map<String, int> occs;

	std::vector<String> stxt(SplitText());
	for (String &s : stxt)
	{
		if (!case_sensitive)
		{
			s.ToLower();
		}
		occs[s] += 1;
	}
	std::map<int, String> words;
	for (std::pair<const String, int>&p : occs)
	{
		words[p.second] = p.first;
	}
	return words;
}

