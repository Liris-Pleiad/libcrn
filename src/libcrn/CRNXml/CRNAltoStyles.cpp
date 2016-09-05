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
 * file: CRNAltoStyles.cpp
 * \author Yann LEYDIER
 */

#include <CRNXml/CRNAlto.h>
#include <CRNException.h>
#include <CRNString.h>
#include <CRNi18n.h>

using namespace crn;
using namespace xml;

/*! Constructor from file
 * \param[in]	el	the Xml element to read
 */
Alto::Styles::Styles(const Element &el):
	Element(el)
{
	for (Element el = BeginElement(); el != EndElement(); ++el)
	{
		StringUTF8 elname = el.GetName();
		if (elname == "TextStyle")
		{
			std::unique_ptr<Text> t(new Text(el));
			textStyles.insert(std::make_pair(t->GetId(), std::move(t)));
		}
		else if (elname == "ParagraphStyle")
		{
			std::unique_ptr<Paragraph> p(new Paragraph(el));
			parStyles.insert(std::make_pair(p->GetId(), std::move(p)));
		}
	}
}

/*! \returns the ids of the text styles */
std::vector<Id> Alto::Styles::GetTextStyles() const
{
	std::vector<Id> styles;
	for (std::map<Id, std::unique_ptr<Text> >::const_iterator it = textStyles.begin(); it != textStyles.end(); ++it)
	{
		styles.push_back(it->first);
	}
	return styles;
}

/*! \returns the ids of the paragraph styles */
std::vector<Id> Alto::Styles::GetParagraphStyles() const
{
	std::vector<Id> styles;
	for (std::map<Id, std::unique_ptr<Paragraph> >::const_iterator it = parStyles.begin(); it != parStyles.end(); ++it)
	{
		styles.push_back(it->first);
	}
	return styles;
}

/*! 
 * \throws	crn::ExceptionNotFound	cannot find text style
 * \param[in]	id_	the id of the text style
 * \returns the text style
 */
const Alto::Styles::Text& Alto::Styles::GetTextStyle(const Id &id_) const
{
	std::map<Id, std::unique_ptr<Text> >::const_iterator it = textStyles.find(id_);
	if (it != textStyles.end())
		return *(it->second);
	else
		throw ExceptionNotFound(_("Text style not found."));
}

/*! 
 * \throws	crn::ExceptionNotFound	cannot find text style
 * \param[in]	id_	the id of the text style
 * \returns the text style
 */
Alto::Styles::Text& Alto::Styles::GetTextStyle(const Id &id_)
{
	std::map<Id, std::unique_ptr<Text> >::iterator it = textStyles.find(id_);
	if (it != textStyles.end())
		return *(it->second);
	else
		throw ExceptionNotFound(_("Text style not found."));
}

/*! Adds a text style
 * \param[in]	style	the text style to be added
 * \param[in]	size	the size of the font
 */
Alto::Styles::Text& Alto::Styles::AddTextStyle(const Id &id_, double size)
{
	Element el(PushBackElement("TextStyle"));
	el.SetAttribute("ID", id_);
	el.SetAttribute("FONTSIZE", size);
	auto res = textStyles.insert(std::make_pair(id_, std::unique_ptr<Text>(new Text(el))));
	return *res.first->second;
}

/*! 
 * \throws	crn::ExceptionNotFound	cannot find paragraph style
 * \param[in]	id_	the id of the paragraph style
 * \returns the paragraph style
 */
const Alto::Styles::Paragraph& Alto::Styles::GetParagraphStyle(const Id &id_) const
{
	std::map<Id, std::unique_ptr<Paragraph> >::const_iterator it = parStyles.find(id_);
	if (it != parStyles.end())
		return *(it->second);
	else
		throw ExceptionNotFound(_("Paragraph style not found."));
}

/*! 
 * \throws	crn::ExceptionNotFound	cannot find paragraph style
 * \param[in]	id_	the id of the paragraph style
 * \returns the paragraph style
 */
Alto::Styles::Paragraph& Alto::Styles::GetParagraphStyle(const Id &id_)
{
	std::map<Id, std::unique_ptr<Paragraph> >::iterator it = parStyles.find(id_);
	if (it != parStyles.end())
		return *(it->second);
	else
		throw ExceptionNotFound(_("Paragraph style not found."));
}

/*! Adds a paragraph style
 * \param[in]	style	the paragraph style to be added
 */
Alto::Styles::Paragraph& Alto::Styles::AddParagraphStyle(const Id &id_)
{
	Element el(PushBackElement("ParagraphStyle"));
	el.SetAttribute("ID", id_);
	auto res = parStyles.insert(std::make_pair(id_, std::unique_ptr<Paragraph>(new Paragraph(el))));
	return *res.first->second;
}

/////////////////////////////////////////////////////////////////////////////////////
// Text
/////////////////////////////////////////////////////////////////////////////////////

Alto::Styles::Text::Text(const Element &el):
	Element(el)
{
	id = GetAttribute<StringUTF8>("ID", false); // may throw
	GetAttribute<double>("FONTSIZE", false); // may throw
}

Option<StringUTF8> Alto::Styles::Text::GetFontFamily() const
{
	Option<StringUTF8> ff;
	StringUTF8 str = GetAttribute<StringUTF8>("FONTFAMILY");
	if (str.IsNotEmpty())
		ff = str;
	return ff;
}

void Alto::Styles::Text::SetFontFamily(const StringUTF8 &ff)
{
	SetAttribute("FONTFAMILY", ff);
}

void Alto::Styles::Text::UnsetFontFamily()
{
	RemoveAttribute("FONTFAMILY");
}

Option<Alto::Styles::Text::FontType> Alto::Styles::Text::GetFontType() const
{
	Option<FontType> fontType;
	StringUTF8 str = GetAttribute<StringUTF8>("FONTTYPE");
	if (str.IsNotEmpty())
	{
		if (str == "serif")
			fontType = Text::FontType::Serif;
		else if (str == "sans-serif")
			fontType = Text::FontType::SansSerif;
	}
	return fontType;
}

void Alto::Styles::Text::SetFontType(Alto::Styles::Text::FontType ft)
{
	switch (ft)
	{
		case Text::FontType::Serif:
			SetAttribute("FONTTYPE", "Serif");
			break;
		case Text::FontType::SansSerif:
			SetAttribute("FONTTYPE", "Sans-Serif");
			break;
	}
}

void Alto::Styles::Text::UnsetFontType()
{
	RemoveAttribute("FONTTYPE");
}

Option<Alto::Styles::Text::FontWidth> Alto::Styles::Text::GetFontWidth() const
{
	Option<FontWidth> fontWidth;
	StringUTF8 str = GetAttribute<StringUTF8>("FONTWIDTH");
	if (str.IsNotEmpty())
	{
		if (str == "proporitional")
			fontWidth = Text::FontWidth::Proportional;
		else if (str == "fixed")
			fontWidth = Text::FontWidth::Fixed;
	}
	return fontWidth;
}

void Alto::Styles::Text::SetFontWidth(Alto::Styles::Text::FontWidth fw)
{
	switch (fw)
	{
		case Text::FontWidth::Proportional:
			SetAttribute("FONTWIDTH", "proportional");
			break;
		case Text::FontWidth::Fixed:
			SetAttribute("FONTWIDTH", "fixed");
			break;
	}
}

void Alto::Styles::Text::UnsetFontWidth()
{
	RemoveAttribute("FONTWIDTH");
}

double Alto::Styles::Text::GetFontSize() const
{
	return GetAttribute<double>("FONTSIZE", false); // should not throw…
}

void Alto::Styles::Text::SetFontSize(double fs)
{
	SetAttribute("FONTSIZE", fs);
}

Option<uint32_t> Alto::Styles::Text::GetFontColor() const
{
	Option<uint32_t> fontColor;
	try { fontColor = GetAttribute<unsigned int>("FONTCOLOR", false); } catch (...) { }
	return fontColor;
}

void Alto::Styles::Text::SetFontColor(uint32_t fc)
{
	SetAttribute("FONTCOLOR", fc);
}

void Alto::Styles::Text::UnsetFontColor()
{
	RemoveAttribute("FONTCOLOR");
}

Option<Alto::Styles::Text::FontStyle> Alto::Styles::Text::GetFontStyle() const
{
	Option<FontStyle> fontStyle;
	StringUTF8 str = GetAttribute<StringUTF8>("FONTSTYLE");
	if (str.IsNotEmpty())
	{
		FontStyle val = FontStyle::Undef;
		if (str.Find("bold") != String::NPos())
			val |= Text::FontStyle::Bold;
		if (str.Find("italics") != String::NPos())
			val |= Text::FontStyle::Italics;
		if (str.Find("subscript") != String::NPos())
			val |= Text::FontStyle::Subscript;
		if (str.Find("superscript") != String::NPos())
			val |= Text::FontStyle::Superscript;
		if (str.Find("smallcaps") != String::NPos())
			val |= Text::FontStyle::SmallCaps;
		if (str.Find("underline") != String::NPos())
			val |= Text::FontStyle::Underline;
		if (val != FontStyle::Undef)
			fontStyle = val;
	}
	return fontStyle;
}

void Alto::Styles::Text::SetFontStyle(Alto::Styles::Text::FontStyle fs)
{
	StringUTF8 av;
	if (!!(fs & Text::FontStyle::Bold))
		av += "bold ";
	if (!!(fs & Text::FontStyle::Italics))
		av += "italics ";
	if (!!(fs & Text::FontStyle::Subscript))
		av += "subscript ";
	if (!!(fs & Text::FontStyle::Superscript))
		av += "superscript ";
	if (!!(fs & Text::FontStyle::SmallCaps))
		av += "smallcaps ";
	if (!!(fs & Text::FontStyle::Underline))
		av += "undunderline ";
	SetAttribute("FONTSTYLE", av);
}

void Alto::Styles::Text::UnsetFontStyle()
{
	RemoveAttribute("FontStyle");
}

/////////////////////////////////////////////////////////////////////////////////////
// Paragraph
/////////////////////////////////////////////////////////////////////////////////////

Alto::Styles::Paragraph::Paragraph(const Element &el):
	Element(el)
{
	id = GetAttribute<StringUTF8>("ID", false); // may throw
}

Option<Alto::Styles::Paragraph::Align> Alto::Styles::Paragraph::GetAlign() const
{
	Option<Align> align;
	StringUTF8 str(GetAttribute<StringUTF8>("ALIGN"));
	if (str.IsNotEmpty())
	{
		if (str == "Left")
			align = Paragraph::Align::Left;
		else if (str == "Right")
			align = Paragraph::Align::Right;
		else if (str == "Center")
			align = Paragraph::Align::Center;
		else if (str == "Block")
			align = Paragraph::Align::Block;
	}
	return align;
}

void Alto::Styles::Paragraph::SetAlign(Alto::Styles::Paragraph::Align a)
{
	switch (a)
	{
		case Paragraph::Align::Left:
			SetAttribute("ALIGN", "Left");
			break;
		case Paragraph::Align::Right:
			SetAttribute("ALIGN", "Right");
			break;
		case Paragraph::Align::Center:
			SetAttribute("ALIGN", "Center");
			break;
		case Paragraph::Align::Block:
			SetAttribute("ALIGN", "Block");
			break;
	}
}

void Alto::Styles::Paragraph::UnsetAlign()
{
	RemoveAttribute("ALIGN");
}

Option<double> Alto::Styles::Paragraph::GetLeftIndent() const
{
	Option<double> leftIndent;
	try { leftIndent = GetAttribute<double>("LEFT", false); } catch (...) { }
	return leftIndent;
}

void Alto::Styles::Paragraph::UnsetLeftIndent()
{
	RemoveAttribute("LEFT");
}

void Alto::Styles::Paragraph::SetLeftIndent(double i)
{
	SetAttribute("LEFT", i);
}

Option<double> Alto::Styles::Paragraph::GetRightIndent() const
{
	Option<double> rightIndent;
	try { rightIndent = GetAttribute<double>("RIGHT", false); } catch (...) { }
	return rightIndent;
}

void Alto::Styles::Paragraph::SetRightIndent(double i)
{
	SetAttribute("RIGHT", i);
}

void Alto::Styles::Paragraph::UnsetRightIndent()
{
	RemoveAttribute("RIGHT");
}

Option<double> Alto::Styles::Paragraph::GetLineSpace() const
{
	Option<double> ls;
	try { ls = GetAttribute<double>("LINESPACE", false); } catch (...) { }
	return ls;
}

void Alto::Styles::Paragraph::SetLineSpace(double i)
{
	SetAttribute("LINESPACE", i);
}

void Alto::Styles::Paragraph::UnsetLineSpace()
{
	RemoveAttribute("LINESPACE");
}

Option<double> Alto::Styles::Paragraph::GetFirstLineIndent() const
{
	Option<double> fli;
	try { fli = GetAttribute<double>("FIRSTLINE", false); } catch (...) { }
	return fli;
}

void Alto::Styles::Paragraph::SetFirstLineIndent(double i)
{
	SetAttribute("FIRSTLINE", i);
}

void Alto::Styles::Paragraph::UnsetFirstLineIndent()
{
	RemoveAttribute("FIRSTLINE");
}

