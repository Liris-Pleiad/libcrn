/* Copyright 2011-2016 CoReNum
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
 * file: CRNAltoTextBlock.cpp
 * \author Yann LEYDIER
 */

#include <CRNXml/CRNAlto.h>
#include <CRNException.h>
#include <CRNString.h>
#include <algorithm>
#include <CRNi18n.h>

using namespace crn;
using namespace xml;


//////////////////////////////////////////////////////////////////////////////////
// TextBlock
//////////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \throws	ExceptionNotFound	a mandatory element or attribute was not found
 * \param[in]	el	an XML element
 */
Alto::Layout::Page::Space::TextBlock::TextBlock(const Element &el):
	Block(el)
{
	update_subelements();
}

/*! \brief Rebuilds the textline cache */
void Alto::Layout::Page::Space::TextBlock::update_subelements()
{
	lines.clear();
	id_lines.clear();
	Element cel = GetFirstChildElement("TextLine");
	while (cel)
	{
		lines.push_back(std::shared_ptr<TextLine>(new TextLine(cel)));
		id_lines[lines.back()->GetId()] = lines.back();
		cel = cel.GetNextSiblingElement("TextLine");
	}
}

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \param[in]	el	an XML element
 * \param[in]	id_	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::TextBlock::TextBlock(const Element &el, const Id &id_, int x, int y, int w, int h):
	Block(el, id_, x, y, w, h)
{ }

/*! \return	the language of the text inside the block */
Option<StringUTF8> Alto::Layout::Page::Space::TextBlock::GetLanguage() const
{
	Option<StringUTF8> language;
	StringUTF8 str = GetAttribute<StringUTF8>("language");
	if (str.IsNotEmpty())
		language = str;
	return language;
}

/*! \return the list of text lines in the block */
std::vector<Alto::Layout::Page::Space::TextBlock::TextLinePtr> Alto::Layout::Page::Space::TextBlock::GetTextLines() const 
{ 
	if (GetNbSubelements() != lines.size()) 
		const_cast<TextBlock*>(this)->update_subelements(); 
	return std::vector<TextLinePtr>(lines.begin(), lines.end());
}

/*!
 * \throws	ExceptionNotFound	no text line with this id
 * \param[in]	id_	the id of the line to retrieve
 * \return	a text line
 */
Alto::Layout::Page::Space::TextBlock::TextLine& Alto::Layout::Page::Space::TextBlock::GetTextLine(const Id &id_)
{
	if (GetNbSubelements() != lines.size()) 
		const_cast<TextBlock*>(this)->update_subelements(); 

	std::map<Id, TextLinePtr>::iterator it(id_lines.find(id_));
	if ((it != id_lines.end()) && !it->second.expired())
		return *(it->second.lock());
	for (const std::shared_ptr<TextLine> &tl : lines)
	{
		if (tl->GetId() == id_)
		{
			id_lines[id_] = tl;
			return *tl;
		}
	}
	throw ExceptionNotFound(_("The block contains no text line with this id."));
}

/*! Adds a text line in the block
 * \param[in]	id_	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::TextBlock::TextLine& Alto::Layout::Page::Space::TextBlock::AddTextLine(const Id &id_, double x, double y, double w, double h)
{
	lines.push_back(std::shared_ptr<TextLine>(new TextLine(PushBackElement("TextLine"), id_, x, y, w, h)));
	id_lines[id_] = lines.back();
	return *lines.back();
}

/*! Adds a text line in the block
 * \throws	ExceptionNotFound	previous line not found
 * \param[in]	pred	the id of the previous line
 * \param[in]	id_	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::TextBlock::TextLine& Alto::Layout::Page::Space::TextBlock::AddTextLineAfter(const Id &pred, const Id &id_, double x, double y, double w, double h)
{
	for (std::vector<std::shared_ptr<TextLine> >::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		if ((*it)->GetId() == pred)
		{
			Element pel(**it);
			++it;
			if (it == lines.end())
				return AddTextLine(id_, x, y, w, h);
			else
			{
				std::shared_ptr<TextLine> tl(new TextLine(InsertElement(pel, "TextLine"), id_, x, y, w, h));
				lines.insert(it, tl);
				id_lines[id_] = tl;
				return *tl;
			}
		}
	}
	throw crn::ExceptionNotFound(_("Page not found."));
}

/*! Adds a text line in the block
 * \throws	ExceptionNotFound	next line not found
 * \param[in]	next id of the next line
 * \param[in]	id_	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::TextBlock::TextLine& Alto::Layout::Page::Space::TextBlock::AddTextLineBefore(const Id &next, const Id &id_, double x, double y, double w, double h)
{
	for (std::vector<std::shared_ptr<TextLine> >::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		if ((*it)->GetId() == next)
		{
			std::shared_ptr<TextLine> newline;
			if (it == lines.begin())
				newline.reset(new TextLine(PushFrontElement("TextLine"), id_, x, y, w, h));
			else
				newline.reset(new TextLine(InsertElement(**(it - 1), "TextLine"), id_, x, y, w, h));
			lines.insert(it, newline);
			id_lines[id_] = newline;
			return *newline;
		}
	}
	throw crn::ExceptionNotFound(_("Page not found."));
}

/*! Removes a text line
 * \throws	crn::ExceptionNotFound	text line not found
 * \param[in]	tid	the text line id
 */
void Alto::Layout::Page::Space::TextBlock::RemoveTextLine(const Id &tid)
{
	for (std::vector<std::shared_ptr<TextLine> >::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		if ((*it)->GetId() == tid)
		{
			RemoveChild(**it);
			lines.erase(it);
			id_lines.erase(tid);
		}
	}
	throw crn::ExceptionNotFound(_("Page not found."));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// TextLine
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \throws	ExceptionNotFound	a mandatory element or attribute was not found
 * \param[in]	el	an XML element
 */
Alto::Layout::Page::Space::TextBlock::TextLine::TextLine(const Element &el):
	Element(el)
{
	id = GetAttribute<StringUTF8>("ID", false); // may throw;
	GetAttribute<double>("HEIGHT", false); // may throw
	GetAttribute<double>("WIDTH", false); // may throw
	GetAttribute<double>("HPOS", false); // may throw
	GetAttribute<double>("VPOS", false); // may throw

	update_subelements();
}

/*! \brief Reads the sub-elements */
void Alto::Layout::Page::Space::TextBlock::TextLine::update_subelements()
{
	lineElements.clear();
	words.clear();
	id_words.clear();

	for (Element cel = BeginElement(); cel != EndElement(); ++cel)
	{
		std::shared_ptr<LineElement> newnode(NULL);
		String elname(cel.GetName());
		if (elname == "String")
		{
			newnode.reset(new Word(cel));
			words.push_back(std::static_pointer_cast<Word>(newnode));
			if (words.back().lock()->GetId())
				id_words[words.back().lock()->GetId().Get()] = words.back();
		}
		else if (elname == "SP")
		{
			newnode.reset(new WhiteSpace(cel));
		}
		else if (elname == "HYP")
		{
			newnode.reset(new Hyphen(cel));
		}
		if (newnode)
			lineElements.push_back(newnode);
	}
}

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \param[in]	el	an XML element
 * \param[in]	id_	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::TextBlock::TextLine::TextLine(const Element &el, const Id &id_, double x, double y, double w, double h):
	Element(el),
	id(id_)
{
	SetAttribute("ID", id);
	SetAttribute("HPOS", x);
	SetAttribute("VPOS", y);
	SetAttribute("WIDTH", w);
	SetAttribute("HEIGHT", h);
}

/*! \return	the list of style references */
std::vector<Id> Alto::Layout::Page::Space::TextBlock::TextLine::GetStyles() const
{
	return GetStyleRefs(*this);
}

/*! Adds a reference to a style
 * \param[in]	styleid	the reference to add
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::AddStyle(const Id &styleid)
{
	AddStyleRef(*this, styleid);
}

/*! Removes a reference to a style
 * \param[in]	styleid	the reference to remove
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::RemoveStyle(const Id &styleid)
{
	RemoveStyleRef(*this, styleid);
}

/*! \return	the width of the line */
double Alto::Layout::Page::Space::TextBlock::TextLine::GetWidth() const
{
	return GetAttribute<double>("WIDTH", false); // should not throw
}

/*! Sets the width of the line
 * \param[in]	d	the new width
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::SetWidth(double d)
{
	SetAttribute("WIDTH", d);
}

/*! \return	the height of the line */
double Alto::Layout::Page::Space::TextBlock::TextLine::GetHeight() const
{
	return GetAttribute<double>("HEIGHT", false); // should not throw
}

/*! Sets the height of the line
 * \param[in]	d	the new height
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::SetHeight(double d)
{
	SetAttribute("HEIGHT", d);
}

/*! \return	the abscissa of the line */
double Alto::Layout::Page::Space::TextBlock::TextLine::GetHPos() const
{
	return GetAttribute<double>("HPOS", false); // should not throw
}

/*! Sets the x pos of the line
 * \param[in]	d	the new abscissa
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::SetHPos(double d)
{
	SetAttribute("HPOS", d);
}

/*! \return	the ordinate of the line */
double Alto::Layout::Page::Space::TextBlock::TextLine::GetVPos() const
{
	return GetAttribute<double>("VPOS", false); // should not throw
}

/*! Sets the y pos of the line
 * \param[in]	d	the new ordinate
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::SetVPos(double d)
{
	SetAttribute("VPOS", d);
}

/*! \return	the ordinate of the baseline */
Option<double> Alto::Layout::Page::Space::TextBlock::TextLine::GetBaseline() const
{
	Option<double> baseline;
	try { baseline = GetAttribute<double>("BASELINE", false); } catch (...) { }
	return baseline;
}

/*! Sets the baseline ordinate
 * \throws	ExceptionDomain	the baseline is out of the line's bounds
 * \param[in]	d	the baseline ordinate
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::SetBaseline(double d, bool check_bounds)
{
	if (check_bounds && ((d < GetVPos()) || (d > GetVPos() + GetHeight())))
		throw ExceptionDomain(_("The baseline is out of the line's bounds"));
	SetAttribute("BASELINE", d);
}

/*! Unsets the baseline ordinate */
void Alto::Layout::Page::Space::TextBlock::TextLine::UnsetBaseline()
{
	RemoveAttribute("BASELINE");
}

/*! \return	whether the line was manually corrected or not */
Option<bool> Alto::Layout::Page::Space::TextBlock::TextLine::GetManuallyCorrected() const
{
	Option<bool> manuallyCorrected;
	try { manuallyCorrected = GetAttribute<bool>("CS", false); } catch (...) { }
	return manuallyCorrected;
}

/*! Sets whether the line was manually corrected or not
 * \param[in]	c	true if the line was manually corrected, false else
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::SetManuallyCorrected(bool c)
{
	SetAttribute("CS", c);
}

/*! Unsets whether the line was manually corrected or not */
void Alto::Layout::Page::Space::TextBlock::TextLine::UnsetManuallyCorrected()
{
	RemoveAttribute("CS");
}

/*! \return	the list of elements in the line */
std::vector<Alto::Layout::Page::Space::TextBlock::TextLine::LineElementPtr> Alto::Layout::Page::Space::TextBlock::TextLine::GetLineElements() const 
{ 
	if (GetNbSubelements() != lineElements.size()) 
		const_cast<TextLine*>(this)->update_subelements(); 
	return std::vector<LineElementPtr>(lineElements.begin(), lineElements.end()); 
}

/*! \return the list of words in the line */
const std::vector<Alto::Layout::Page::Space::TextBlock::TextLine::WordPtr>& Alto::Layout::Page::Space::TextBlock::TextLine::GetWords() const 
{ 
	if (GetNbSubelements() != lineElements.size()) 
		const_cast<TextLine*>(this)->update_subelements(); 
	return words; 
}

/*!
 * \throws	ExceptionNotFound	no word with this id
 * \param[in]	id_	the id of the word to retrieve
 * \return	a weak pointer on the word
 */
Alto::Layout::Page::Space::TextBlock::TextLine::Word& Alto::Layout::Page::Space::TextBlock::TextLine::GetWord(const Id &id_)
{
	if (GetNbSubelements() != lineElements.size())
		update_subelements();
	std::map<Id, WordPtr>::iterator it(id_words.find(id_));
	if ((it != id_words.end()) && !it->second.expired())
		return *(it->second.lock());
	for (const WordPtr &word : words)
	{
		const std::shared_ptr<Word> sword(word.lock());
		if (sword->GetId() && (sword->GetId().Get() == id_))
		{
			id_words[id_] = word;
			return *sword;
		}
	}
	throw ExceptionNotFound(_("The line contains no word with this id."));
}

/*! Adds a word in the line 
 * \param[in]	id_	the id of the element
 * \param[in]	text	the transcription of the word
 * \param[in]	x	the horizontal position (optional)
 * \param[in]	y	the vertical position (optional)
 * \param[in]	w	the width (optional)
 * \param[in]	h	the height (optional)
 */
Alto::Layout::Page::Space::TextBlock::TextLine::Word& Alto::Layout::Page::Space::TextBlock::TextLine::AddWord(const Id &id_, const StringUTF8 &text, const Option<double> &x, const Option<double> &y, const Option<double> &w, const Option<double> &h)
{
	std::shared_ptr<Word> word(new Word(PushBackElement("String"), id_, text, x, y, w, h));
	words.push_back(word);
	id_words[id_] = word;
	lineElements.push_back(word);
	return *word;
}

/*! Adds a word in the line 
 * \throws	ExceptionNotFound	previous word not found
 * \param[in]	pred	the id of the previous word
 * \param[in]	id_	the id of the element
 * \param[in]	text	the transcription of the word
 * \param[in]	x	the horizontal position (optional)
 * \param[in]	y	the vertical position (optional)
 * \param[in]	w	the width (optional)
 * \param[in]	h	the height (optional)
 */
Alto::Layout::Page::Space::TextBlock::TextLine::Word& Alto::Layout::Page::Space::TextBlock::TextLine::AddWordAfter(const Id &pred, const Id &id_, const StringUTF8 &text, const Option<double> &x, const Option<double> &y, const Option<double> &w, const Option<double> &h)
{
	for (std::vector<WordPtr>::iterator it = words.begin(); it != words.end(); ++it)
	{
		std::shared_ptr<Word> sw(it->lock());
		if (sw->GetId())
		{
			if (sw->GetId().Get() == pred)
			{
				Element &pw(*sw);
				++it;
				if (it == words.end())
					return AddWord(id_, text, x, y, w, h);
				else
				{
					std::shared_ptr<Word> nw(new Word(InsertElement(pw, "String"), id_, text, x, y, w, h));
					words.insert(it, nw);
					id_words[id_] = nw;
					sw = it->lock();
					std::vector<std::shared_ptr<LineElement> >::iterator lit = std::find(lineElements.begin(), lineElements.end(), sw);
					if (lit == lineElements.end())
						lineElements.push_back(nw);
					else
					{
						++lit;
						if (lit == lineElements.end())
							lineElements.push_back(nw);
						else
							lineElements.insert(lit, nw);
					}
					return *nw;
				}
			} // found predecessor
		} // has an Id
	}
	throw ExceptionNotFound(_("Cannot find element."));
}

/*! Adds a word in the line 
 * \throws	ExceptionNotFound	next word not found
 * \param[in]	next id of the next word
 * \param[in]	id_	the id of the element
 * \param[in]	text	the transcription of the word
 * \param[in]	x	the horizontal position (optional)
 * \param[in]	y	the vertical position (optional)
 * \param[in]	w	the width (optional)
 * \param[in]	h	the height (optional)
 */
Alto::Layout::Page::Space::TextBlock::TextLine::Word& Alto::Layout::Page::Space::TextBlock::TextLine::AddWordBefore(const Id &next, const Id &id_, const StringUTF8 &text, const Option<double> &x, const Option<double> &y, const Option<double> &w, const Option<double> &h)
{
	for (std::vector<WordPtr>::iterator it = words.begin(); it != words.end(); ++it)
	{
		const std::shared_ptr<Word> sw(it->lock());
		if (sw->GetId())
		{
			if (sw->GetId().Get() == next)
			{
				std::shared_ptr<Word> nw;
				if (it == words.begin())
					nw.reset(new Word(PushFrontElement("String"), id_, text, x, y, w, h));
				else
					nw.reset(new Word(InsertElement(*(it - 1)->lock(), "String"), id_, text, x, y, w, h));
				words.insert(it, nw);
				id_words[id_] = nw;
				std::vector<std::shared_ptr<LineElement> >::iterator lit = std::find(lineElements.begin(), lineElements.end(), sw);
				if (lit == lineElements.end())
					lineElements.push_back(nw);
				else
					lineElements.insert(lit, nw);
				return *nw;
			} // found successor
		} // has an Id
	}
	throw ExceptionNotFound(_("Cannot find element."));
}

/*! Removes a word
 * \throws	crn::ExceptionNotFound	word not found
 * \param[in]	wid	the word id
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::RemoveWord(const Id &wid)
{
	for (std::vector<WordPtr>::iterator it = words.begin(); it != words.end(); ++it)
	{
		const std::shared_ptr<Word> sw(it->lock());
		if (sw->GetId())
		{
			if (sw->GetId().Get() == wid)
			{
				RemoveChild(*sw);
				std::vector<std::shared_ptr<LineElement> >::iterator lit = std::find(lineElements.begin(), lineElements.end(), sw);
				words.erase(it);
				id_words.erase(wid);
				if (lit != lineElements.end())
					lineElements.erase(lit);
				return;
			}
		}
	}
	throw ExceptionNotFound(_("Cannot find word."));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// LineElement
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \param[in]	el	an xml element to read
 */
Alto::Layout::Page::Space::TextBlock::TextLine::LineElement::LineElement(const Element &el):
	Element(el)
{ 
	if (!el)
		throw ExceptionInvalidArgument(_("Null node."));
}

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \param[in]	el	the xml element to configure
 * \param[in]	x	the horizontal position (optional)
 * \param[in]	y	the vertical position (optional)
 * \param[in]	w	the width (optional)
 */
Alto::Layout::Page::Space::TextBlock::TextLine::LineElement::LineElement(const Element &el, const Option<double> &x, const Option<double> &y, const Option<double> &w):
	Element(el)
{
	if (!el)
		throw ExceptionInvalidArgument(_("Null node."));
	if (x)
		SetAttribute("HPOS", *x);
	if (y)
		SetAttribute("VPOS", *y);
	if (w)
		SetAttribute("WIDTH", *w);
}

/*! \return	the width of the element */
Option<double> Alto::Layout::Page::Space::TextBlock::TextLine::LineElement::GetWidth() const
{
	Option<double> width;
	try { width = GetAttribute<double>("WIDTH", false); } catch (...) { }
	return width;
}

/*! Sets the width of the element
 * \param[in]	d	the new witdh
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::LineElement::SetWidth(double d)
{
	SetAttribute("WIDTH", d);
}

/*! \return	the abscissa of the element */
Option<double> Alto::Layout::Page::Space::TextBlock::TextLine::LineElement::GetHPos() const
{
	Option<double> hpos;
	try { hpos = GetAttribute<double>("HPOS", false); } catch (...) { }
	return hpos;
}

/*! Sets the abscissa of the element
 * \param[in]	d	the new x coordinate
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::LineElement::SetHPos(double d)
{
	SetAttribute("HPOS", d);
}

/*! \return	the ordinate of the element */
Option<double> Alto::Layout::Page::Space::TextBlock::TextLine::LineElement::GetVPos() const
{
	Option<double> vpos;
	try { vpos = GetAttribute<double>("VPOS", false); } catch (...) { }
	return vpos;
}

/*! Sets the ordinate of the element
 * \param[in]	d	the new y coordinate
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::LineElement::SetVPos(double d)
{
	SetAttribute("VPOS", d);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Word
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \throws	ExceptionNotFound	a mandatory element or attribute was not found
 * \param[in]	el	an XML element
 */
Alto::Layout::Page::Space::TextBlock::TextLine::Word::Word(const Element &el):
	LineElement(el)
{
	StringUTF8 str = GetAttribute<StringUTF8>("ID");
	if (str.IsNotEmpty())
		id = str;
	GetAttribute<StringUTF8>("CONTENT", false); // may throw

	/*
	for (Element cel = BeginElement(); cel != EndElement(); ++cel)
	{
		// TODO alternatives
	}
	*/
}

/*! \brief Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \param[in]	id_	the id of the element
 * \param[in]	text	the transcription of the word
 * \param[in]	x	the horizontal position (optional)
 * \param[in]	y	the vertical position (optional)
 * \param[in]	w	the width (optional)
 * \param[in]	h	the height (optional)
 */
Alto::Layout::Page::Space::TextBlock::TextLine::Word::Word(const Element &el, const Id &id_, const StringUTF8 &text, const Option<double> &x, const Option<double> &y, const Option<double> &w, const Option<double> &h):
	LineElement(el, x, y, w),
	id(id_)
{
	SetAttribute("ID", *id);
	SetAttribute("CONTENT", text);
	if (h)
		SetAttribute("HEIGHT", *h);
}

/*! \return	the list of style references */
std::vector<Id> Alto::Layout::Page::Space::TextBlock::TextLine::Word::GetStyles() const
{
	return GetStyleRefs(*this);
}

/*! Adds a reference to a style
 * \param[in]	styleid	the reference to add
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::Word::AddStyle(const Id &styleid)
{
	AddStyleRef(*this, styleid);
}

/*! Removes a reference to a style
 * \param[in]	styleid	the reference to remove
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::Word::RemoveStyle(const Id &styleid)
{
	RemoveStyleRef(*this, styleid);
}

/*! \return	the transcription of the word */
StringUTF8 Alto::Layout::Page::Space::TextBlock::TextLine::Word::GetContent() const
{
	return GetAttribute<StringUTF8>("CONTENT", false); // should not throw
}

/*! Sets the transcription of the word
 * \param[in]	s	the new transcription
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::Word::SetContent(const StringUTF8 &s)
{
	SetAttribute("CONTENT", s);
}

/*! \return	the height of the word */
Option<double> Alto::Layout::Page::Space::TextBlock::TextLine::Word::GetHeight() const
{
	Option<double> height;
	try { height = GetAttribute<double>("HEIGHT", false); } catch (...) { }
	return height;
}

/*! Sets the height of the word
 * \param[in]	d	the new height of the word
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::Word::SetHeight(double d)
{
	SetAttribute("HEIGHT", d);
}

/*! see Alto::Styles::Text::FontStyle
 * \return	the font style of the word */
Option<Alto::Styles::Text::FontStyle> Alto::Layout::Page::Space::TextBlock::TextLine::Word::GetFontStyle() const
{
	Option<Alto::Styles::Text::FontStyle> fontStyle;
	StringUTF8 str = GetAttribute<StringUTF8>("STYLE");
	if (str.IsNotEmpty())
	{
		Alto::Styles::Text::FontStyle val = Alto::Styles::Text::FontStyle::Undef;
		if (str.Find("bold") != StringUTF8::NPos())
			val |= Alto::Styles::Text::FontStyle::Bold;
		if (str.Find("italics") != StringUTF8::NPos())
			val |= Alto::Styles::Text::FontStyle::Italics;
		if (str.Find("subscript") != StringUTF8::NPos())
			val |= Alto::Styles::Text::FontStyle::Subscript;
		if (str.Find("superscript") != StringUTF8::NPos())
			val |= Alto::Styles::Text::FontStyle::Superscript;
		if (str.Find("smallcaps") != StringUTF8::NPos())
			val |= Alto::Styles::Text::FontStyle::SmallCaps;
		if (str.Find("underline") != StringUTF8::NPos())
			val |= Alto::Styles::Text::FontStyle::Underline;
		if (val != Alto::Styles::Text::FontStyle::Undef)
			fontStyle = val;
	}
	return fontStyle;
}

/*! Sets the font style of the word
 * \param[in]	fs	a combination of Alto::Styles::Text::FontStyle
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::Word::SetFontStyle(Alto::Styles::Text::FontStyle fs)
{
	StringUTF8 attr;
	if (!!(fs & Alto::Styles::Text::FontStyle::Bold))
		attr += "bold ";
	if (!!(fs & Alto::Styles::Text::FontStyle::Italics))
		attr += "italics ";
	if (!!(fs & Alto::Styles::Text::FontStyle::Subscript))
		attr += "subscript ";
	if (!!(fs & Alto::Styles::Text::FontStyle::Superscript))
		attr += "superscript ";
	if (!!(fs & Alto::Styles::Text::FontStyle::SmallCaps))
		attr += "smallcaps ";
	if (!!(fs & Alto::Styles::Text::FontStyle::Underline))
		attr += "underline ";
	SetAttribute("STYLE", attr);
}

/*! Unsets the font style of the word
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::Word::UnsetFontStyle()
{
	RemoveAttribute("STYLE");
}

/*! \return	the substitution type of the word */
Option<Alto::Layout::Page::Space::TextBlock::TextLine::Word::SubstitutionType> Alto::Layout::Page::Space::TextBlock::TextLine::Word::GetSubstitutionType() const
{
	Option<SubstitutionType> substitutionType;
	StringUTF8 str = GetAttribute<StringUTF8>("SUBS_TYPE");
	if (str.IsNotEmpty())
	{
		if (str == "HypPart1")
			substitutionType = SubstitutionType::HypPart1;
		else if (str == "HypPart2")
			substitutionType = SubstitutionType::HypPart2;
		else if (str == "Abbreviation")
			substitutionType = SubstitutionType::Abbreviation;
	}
	return substitutionType;
}

/*! \return	the substitution of the word */
Option<StringUTF8> Alto::Layout::Page::Space::TextBlock::TextLine::Word::GetSubstitutionContent() const
{
	Option<StringUTF8> substitutionContent;
	StringUTF8 str = GetAttribute<StringUTF8>("SUBS_CONTENT");
	if (str.IsNotEmpty())
		substitutionContent = str;
	return substitutionContent;
}

/*! Sets the substitution of the word
 * \throws	crn::ExceptionInvalidArgument	wrong type of substitution
 * \param[in]	stype	the type of substitution
 * \param[in]	scontent	the substitution's text
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::Word::SetSubstitution(SubstitutionType stype, const StringUTF8 &scontent)
{
	StringUTF8 atyp;
	switch (stype)
	{
		case SubstitutionType::HypPart1:
			atyp = "HypPart1";
			break;
		case SubstitutionType::HypPart2:
			atyp = "HypPart2";
			break;
		case SubstitutionType::Abbreviation:
			atyp = "Abbreviation";
			break;
		default:
			throw crn::ExceptionInvalidArgument(_("Wrong type of substitution."));
	}
	SetAttribute("SUBS_TYPE", atyp);
	SetAttribute("SUBS_CONTENT", scontent);
}

/*! \return	the OCR confidence of the word [0, 1] */
Option<double> Alto::Layout::Page::Space::TextBlock::TextLine::Word::GetWC() const
{
	Option<double> wordConfidence;
	try { wordConfidence = GetAttribute<double>("WC", false); } catch (...) { }
	return wordConfidence;
}

/*!
 * \throws	ExceptionDomain	confidence must be in [0, 1]
 * \param[in]	conf	the new word confidence in [0, 1]
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::Word::SetWC(double conf)
{
	if ((conf < 0.0) || (conf > 1.0))
		throw crn::ExceptionDomain(_("The word confidence must be in [0, 1]."));
	SetAttribute("WC", conf);
}

/*!
 * Unsets the word confidence
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::Word::UnsetWC()
{
	RemoveAttribute("WC");
}


/*! \return	the OCR confidence of the characters */
Option<StringUTF8> Alto::Layout::Page::Space::TextBlock::TextLine::Word::GetCC() const
{
	Option<StringUTF8> charactersConfidence;
	StringUTF8 str = GetAttribute<StringUTF8>("CC");
	if (str.IsNotEmpty())
		charactersConfidence = str;
	return charactersConfidence;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// WhiteSpace
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \throws	ExceptionNotFound	a mandatory element or attribute was not found
 * \param[in]	el	an XML element
 */
Alto::Layout::Page::Space::TextBlock::TextLine::WhiteSpace::WhiteSpace(const Element &el):
	LineElement(el)
{ 
	StringUTF8 str = GetAttribute<StringUTF8>("ID");
	if (str.IsNotEmpty())
		id = str;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Hyphen
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \throws	ExceptionNotFound	a mandatory element or attribute was not found
 * \param[in]	el	an XML element
 */
Alto::Layout::Page::Space::TextBlock::TextLine::Hyphen::Hyphen(const Element &el):
	LineElement(el)
{
	GetAttribute<StringUTF8>("CONTENT", false); // may throw
}

/*! \return	the hyphenation marker */
StringUTF8 Alto::Layout::Page::Space::TextBlock::TextLine::Hyphen::GetContent() const
{
	return GetAttribute<StringUTF8>("CONTENT", false); // should not throw
}

/*! Sets the hyphenation marker
 * \param[in]	s	the hyphenation marker
 */
void Alto::Layout::Page::Space::TextBlock::TextLine::Hyphen::SetContent(const StringUTF8 &s)
{
	SetAttribute("CONTENT", s);
}

