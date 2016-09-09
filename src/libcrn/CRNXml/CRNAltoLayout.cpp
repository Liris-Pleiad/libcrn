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
 * file: CRNAltoLayout.h
 * \author Yann LEYDIER
 */

#include <CRNXml/CRNAlto.h>
#include <CRNException.h>
#include <algorithm>
#include <CRNi18n.h>

using namespace crn;
using namespace xml;

/*! Constructor from file
 * \throws	ExceptionInvalidArgument	null node
 * \throws	ExceptionNotFound	a mandatory element or attribute was not found
 * \param[in]	el	an XML element
 */
Alto::Layout::Layout(const Element &el):
	Element(el)
{
	if (!el)
		throw ExceptionInvalidArgument(_("Null node."));
	update_subelements();
}

/*! Updates the page cache */
void Alto::Layout::update_subelements()
{
	pages.clear();
	id_pages.clear();

	Element pel = GetFirstChildElement("Page");
	while (pel)
	{
		pages.push_back(std::shared_ptr<Page>(new Page(pel)));
		id_pages[pages.back()->GetId()] = pages.back();
		pel = pel.GetNextSiblingElement("Page");
	}
}

/*! \return	the list of style references */
std::vector<Id> Alto::Layout::GetStyles() const
{
	return GetStyleRefs(*this);
}

/*! Adds a reference to a style
 * \param[in]	styleid	the reference to add
 */
void Alto::Layout::AddStyle(const Id &styleid)
{
	AddStyleRef(*this, styleid);
}

/*! Removes a reference to a style
 * \param[in]	styleid	the reference to remove
 */
void Alto::Layout::RemoveStyle(const Id &styleid)
{
	RemoveStyleRef(*this, styleid);
}

/*! Returns a page
 * \throws	crn::ExceptionNotFound	page not found
 * \param[in]	pid	the page id
 * \return	a page handle
 */
Alto::Layout::Page& Alto::Layout::GetPage(const Id &pid)
{
	if (GetNbSubelements() != pages.size()) 
		const_cast<Layout*>(this)->update_subelements(); 
	std::map<Id, std::weak_ptr<Page>>::iterator it(id_pages.find(pid));
	if ((it != id_pages.end()) && !it->second.expired())
		return *(it->second.lock());
	for (const PagePtr &p : pages) // not that useful… still, can't stop thinking it might save our life…
	{
		const std::shared_ptr<Layout::Page> sp(p.lock());
		if (sp->GetId() == pid)
		{
			id_pages[pid] = p;
			return *sp;
		}
	}
	throw crn::ExceptionNotFound(_("Page not found."));
}

/*! \return the list of all pages */
std::vector<Alto::Layout::PagePtr> Alto::Layout::GetPages() const
{
	if (GetNbSubelements() != pages.size()) 
		const_cast<Layout*>(this)->update_subelements(); 
	return std::vector<PagePtr>(pages.begin(), pages.end());
}

/*! Adds a page
 * \throws	crn::ExceptionDomain	invalid page position
 * \param[in]	id_	the id of the page
 * \param[in]	image_number	the physical image number
 * \param[in]	width_	the page width (optional)
 * \param[in]	height_	the page height (optional)
 * \param[in]	pos	the page position (optional)
 */
Alto::Layout::Page& Alto::Layout::AddPage(const Id &id_, int image_number, Option<int> width_, Option<int> height_, Option<Alto::Layout::Page::Position> pos)
{
	pages.push_back(std::shared_ptr<Page>(new Page(PushBackElement("Page"), id_, image_number, width_, height_, pos)));
	id_pages[id_] = pages.back();
	return *pages.back();
}

/*! Inserts a page
 * \throws	crn::ExceptionNotFound	page not found
 * \throws	crn::ExceptionDomain	invalid page position
 * \param[in]	pred	the id of the previous page
 * \param[in]	id_	the id of the page
 * \param[in]	image_number	the physical image number
 * \param[in]	width_	the page width (optional)
 * \param[in]	height_	the page height (optional)
 * \param[in]	pos	the page position (optional)
 */
Alto::Layout::Page& Alto::Layout::AddPageAfter(const Id &pred, const Id &id_, int image_number, Option<int> width_, Option<int> height_, Option<Alto::Layout::Page::Position> pos)
{
	for (std::vector<std::shared_ptr<Page> >::iterator it = pages.begin(); it != pages.end(); ++it)
	{
		if ((*it)->GetId() == pred)
		{
			Element pel(**it);
			++it;
			if (it == pages.end())
				return AddPage(id_, image_number, width_, height_, pos);
			else
			{
				std::shared_ptr<Page> p(new Page(InsertElement(pel, "Page"), id_, image_number, width_, height_, pos));
				pages.insert(it, p);
				id_pages[id_] = p;
				return *p;
			}
		}
	}
	throw crn::ExceptionNotFound(_("Page not found."));
}

/*! Inserts a page
 * \throws	crn::ExceptionNotFound	page not found
 * \throws	crn::ExceptionDomain	invalid page position
 * \param[in]	next	the id of the next page
 * \param[in]	id_	the id of the page
 * \param[in]	image_number	the physical image number
 * \param[in]	width_	the page width (optional)
 * \param[in]	height_	the page height (optional)
 * \param[in]	pos	the page position (optional)
 */
Alto::Layout::Page& Alto::Layout::AddPageBefore(const Id &next, const Id &id_, int image_number, Option<int> width_, Option<int> height_, Option<Alto::Layout::Page::Position> pos)
{
	for (std::vector<std::shared_ptr<Page> >::iterator it = pages.begin(); it != pages.end(); ++it)
	{
		if ((*it)->GetId() == next)
		{
			std::shared_ptr<Page> newpage;
			if (it == pages.begin())
				newpage = std::shared_ptr<Page>(new Page(PushFrontElement("Page"), id_, image_number, width_, height_, pos));
			else
				newpage = std::shared_ptr<Page>(new Page(InsertElement(**(it - 1), "Page"), id_, image_number, width_, height_, pos));
			pages.insert(it, newpage);
			id_pages[id_] = newpage;
			return *newpage;
		}
	}
	throw crn::ExceptionNotFound(_("Page not found."));
}

/*! Removes a page
 * \throws	crn::ExceptionNotFound	page not found
 * \param[in]	pid	the page id
 */
void Alto::Layout::RemovePage(const Id &pid)
{
	for (std::vector<std::shared_ptr<Page> >::iterator it = pages.begin(); it != pages.end(); ++it)
	{
		if ((*it)->GetId() == pid)
		{
			RemoveChild(**it);
			pages.erase(it);
			id_pages.erase(pid);
		}
	}
	throw crn::ExceptionNotFound(_("Page not found."));
}

//////////////////////////////////////////////////////////////////////////////////
// Page
//////////////////////////////////////////////////////////////////////////////////

/*! Constructor from file
 * \throws	ExceptionNotFound	a mandatory element or attribute was not found
 * \param[in]	el	an XML element
 */
Alto::Layout::Page::Page(const Element &el):
	Element(el)
{
	id = el.GetAttribute<StringUTF8>("ID", false); // may throw
	el.GetAttribute<int>("PHYSICAL_IMG_NR", false); // may throw

	update_subelements();
}

/*! Updates the page cache */
void Alto::Layout::Page::update_subelements()
{
	spaces.clear();
	id_spaces.clear();
	topMargin = leftMargin = rightMargin = bottomMargin = printSpace = SpacePtr();

	for (Element cel = BeginElement(); cel != EndElement(); ++cel)
	{
		StringUTF8 elname(cel.GetName());
		std::shared_ptr<Space> sp;
		if (elname == "TopMargin")
		{
			sp = std::shared_ptr<Space>(new Space(cel));
			topMargin = sp;
		}
		else if (elname == "LeftMargin")
		{
			sp = std::shared_ptr<Space>(new Space(cel));
			leftMargin = sp;
		}
		else if (elname == "RightMargin")
		{
			sp = std::shared_ptr<Space>(new Space(cel));
			rightMargin = sp;
		}
		else if (elname == "BottomMargin")
		{
			sp = std::shared_ptr<Space>(new Space(cel));
			bottomMargin = sp;
		}
		else if (elname == "PrintSpace")
		{
			sp = std::shared_ptr<Space>(new Space(cel));
			printSpace = sp;
		}
		if (sp)
		{
			spaces.push_back(sp);
			if (sp->GetId())
				id_spaces[sp->GetId().Get()] = sp;
		}
	}
}

/*! Constructor
 * \throws	crn::ExceptionDomain	invalid page position
 * \param[in]	id_	the id of the page
 * \param[in]	image_number	the physical image number
 * \param[in]	width_	the page width (optional)
 * \param[in]	height_	the page height (optional)
 * \param[in]	pos	the page position (optional)
 */
Alto::Layout::Page::Page(const Element &el, const Id &id_, int image_number, Option<int> width, Option<int> height, Option<Alto::Layout::Page::Position> position):
	Element(el),
	id(id_)
{
	SetAttribute("ID", id);
	SetAttribute("PHYSICAL_IMG_NR", image_number);
	if (width)
		SetAttribute("WIDTH", *width);
	if (height)
		SetAttribute("HEIGHT", *height);
	if (position)
	{
		StringUTF8 av;
		switch (*position)
		{
			case Position::Left:
				av = "Left";
				break;
			case Position::Right:
				av = "Right";
				break;
			case Position::Foldout:
				av = "Foldout";
				break;
			case Position::Single:
				av = "Single";
				break;
			case Position::Cover:
				av = "Cover";
				break;
			default:
				throw crn::ExceptionDomain(_("Invalid page position."));
		}
		SetAttribute("POSITION", av);
	}
}

/*! \return	the list of style references */
std::vector<Id> Alto::Layout::Page::GetStyles() const
{
	return GetStyleRefs(*this);
}

/*! Adds a reference to a style
 * \param[in]	styleid	the reference to add
 */
void Alto::Layout::Page::AddStyle(const Id &styleid)
{
	AddStyleRef(*this, styleid);
}

/*! Removes a reference to a style
 * \param[in]	styleid	the reference to remove
 */
void Alto::Layout::Page::RemoveStyle(const Id &styleid)
{
	RemoveStyleRef(*this, styleid);
}

/*! \return	the class of the page (user defined class such as "title") */
Option<StringUTF8> Alto::Layout::Page::GetPageClass() const
{
	Option<StringUTF8> pageClass;
	StringUTF8 str = GetAttribute<StringUTF8>("PAGECLASS");
	if (str.IsNotEmpty())
		pageClass = str;
	return pageClass;
}

/*! Sets the class of the page (user defined class such as "title")
 * \param[in]	s	the new page class
 */
void Alto::Layout::Page::SetPageClass(const StringUTF8 &s)
{
	SetAttribute("PAGECLASS", s);
}

/*! \return	the height of the page */
Option<int> Alto::Layout::Page::GetHeight() const
{
	Option<int> height;
	try { height = GetAttribute<int>("HEIGHT", false); } catch (...) {}
	return height;
}

/*! \param[in]	i	the new height of the page */
void Alto::Layout::Page::SetHeight(int i)
{
	SetAttribute("HEIGHT", i);
}

/*! \return	the width of the page */
Option<int> Alto::Layout::Page::GetWidth() const
{
	Option<int> width;
	try { width = GetAttribute<int>("WIDTH", false); } catch (...) {}
	return width;
}

/*! \param[in]	i	the new width of the page */
void Alto::Layout::Page::SetWidth(int i)
{
	SetAttribute("WIDTH", i);
}

/*! \return	the number of the page within the document */
int Alto::Layout::Page::GetPhysicalImageNumber() const
{
	return GetAttribute<int>("PHYSICAL_IMG_NR", false); // should not throw
}

/*! \return	the page number that is printed on the document */
Option<StringUTF8> Alto::Layout::Page::GetPrintedImageNumber() const
{
	Option<StringUTF8> printedImageNumber;
	StringUTF8 str = GetAttribute<StringUTF8>("PRINTED_IMG_NR");
	if (str.IsNotEmpty())
		printedImageNumber = str;
	return printedImageNumber;
}

/*! Sets the number of the page within the document
 * \throws	crn::ExceptionDomain	negative physical image number
 * \param[in]	pnum	the new physical image number
 */
void Alto::Layout::Page::SetPhysicalImageNumber(int pnum)
{
	if (pnum < 0)
		throw crn::ExceptionDomain(_("Negative physical image number."));
	SetAttribute("PHYSICAL_IMG_NR", pnum);
}

/*! Sets the page number that is printed on the document
 * \param[in]	s	the new printed image number
 */
void Alto::Layout::Page::SetPrintedImageNumber(const StringUTF8 &s)
{
	SetAttribute("PRINTED_IMG_NR", s);
}

/*! \return	the quality of the original page */
Option<Alto::Layout::Page::Quality> Alto::Layout::Page::GetQuality() const
{
	Option<Quality> quality;
	StringUTF8 str = GetAttribute<StringUTF8>("QUALITY");
	if (str.IsNotEmpty())
	{
		str.ToLower();
		if (str == "OK")
			quality = Quality::Ok;
		else if (str == "Missing")
			quality = Quality::Missing;
		else if (str == "Missing in original")
			quality = Quality::MissingInOriginal;
		else if (str == "Damaged")
			quality = Quality::Damaged;
		else if (str == "Retained")
			quality = Quality::Retained;
		else if (str == "Target")
			quality = Quality::Target;
		else if (str == "As in original")
			quality = Quality::AsInOriginal;
	}
	return quality;
}

/*! Sets the quality of the original page
 * \throws	crn::ExceptionDomain	invalid quality
 * \param[in]	q	the new quality
 */
void Alto::Layout::Page::SetQuality(Quality q)
{
	StringUTF8 attr;
	switch (q)
	{
		case Quality::Ok:
			attr = "OK";
			break;
		case Quality::Missing:
			attr = "Missing";
			break;
		case Quality::MissingInOriginal:
			attr = "Missing in original";
			break;
		case Quality::Damaged:
			attr = "Damaged";
			break;
		case Quality::Retained:
			attr = "Retained";
			break;
		case Quality::Target:
			attr = "Target";
			break;
		case Quality::AsInOriginal:
			attr = "As in original";
			break;
		default:
			throw crn::ExceptionDomain(_("Invalid quality."));
	}
	SetAttribute("QUALITY", attr);
}

/*! \return	details on the quality of the original page */
Option<StringUTF8> Alto::Layout::Page::GetQualityDetail() const
{
	Option<StringUTF8> qualityDetail;
	StringUTF8 str = GetAttribute<StringUTF8>("QUALITY_DETAIL");
	if (str.IsNotEmpty())
		qualityDetail = str;
	return qualityDetail;
}

/*! Sets the details on the quality of the original page
 * \param[in]	q	the new quality details
 */
void Alto::Layout::Page::SetQualityDetail(const StringUTF8 &s)
{
	SetAttribute("QUALITY_DETAIL", s);
}

/*! \return	the position of the page */
Option<Alto::Layout::Page::Position> Alto::Layout::Page::GetPosition() const
{
	Option<Position> position;
	StringUTF8 str = GetAttribute<StringUTF8>("POSITION");
	if (str.IsNotEmpty())
	{
		str.ToLower();
		if (str == "Left")
			position = Position::Left;
		else if (str == "Right")
			position = Position::Right;
		else if (str == "Foldout")
			position = Position::Foldout;
		else if (str == "Single")
			position = Position::Single;
		else if (str == "Cover")
			position = Position::Cover;
	}
	return position;
}

/*! \return	the id of the processing applied to the page */
Option<Id> Alto::Layout::Page::GetProcessing() const
{
	Option<Id> processing;
	StringUTF8 str = GetAttribute<StringUTF8>("PROCESSING");
	if (str.IsNotEmpty())
		processing = str;
	return processing;
}

/*! \return	the estimated % of OCR accuracy on the page [0, 100] */
Option<double> Alto::Layout::Page::GetAccuracy() const
{
	Option<double> accuracy;
	try { accuracy = GetAttribute<double>("ACCURACY", false); } catch (...) {}
	return accuracy;
}

/*! Sets the estimated % of OCR accuracy on the page [0, 100]
 * \throws	crn::ExceptionDomain	page accuracy must be in [0, 100]
 * \param[in]	acc	the new accuracy
 */
void Alto::Layout::Page::SetAccuracy(double acc)
{
	if ((acc < 0.0) || (acc > 100.0))
		throw crn::ExceptionDomain(_("The page accuracy must be in [0, 100]"));
	SetAttribute("ACCURACY", acc);
}

/*! \return	the confidence of OCR on the page [0, 1] */
Option<double> Alto::Layout::Page::GetPageConfidence() const
{
	Option<double> pageConfidence;
	try { pageConfidence = GetAttribute<double>("PC", false); } catch (...) {}
	return pageConfidence;
}

/*! Sets the confidence of OCR on the page [0, 1]
 * \throws	crn::ExceptionDomain	page confidence must be in [0, 1]
 * \param[in]	c	the new confidence
 */
void Alto::Layout::Page::SetPageConfidence(double c)
{
	if ((c < 0.0) || (c > 100.0))
		throw crn::ExceptionDomain(_("The page confidence must be in [0, 1]"));
	SetAttribute("PC", c);
}

/*! Adds a top margin
 * \throws	crn::ExceptionLogic	the page already has a top margin
 * \param[in]	id_	the id of the new space
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space& Alto::Layout::Page::AddTopMargin(const Id &id_, double x, double y, double w, double h)
{
	if (!topMargin.expired())
		throw crn::ExceptionLogic(_("The page already has a top margin."));
	spaces.push_back(std::shared_ptr<Space>(new Space(PushBackElement("TopMargin"), id_, x, y, w, h)));
	id_spaces[id_] = spaces.back();
	topMargin = spaces.back();
	return *topMargin.lock();
}

/*! Adds a left margin
 * \throws	crn::ExceptionLogic	the page already has a left margin
 * \param[in]	id_	the id of the new space
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space& Alto::Layout::Page::AddLeftMargin(const Id &id_, double x, double y, double w, double h)
{
	if (!leftMargin.expired())
		throw crn::ExceptionLogic(_("The page already has a left margin."));
	spaces.push_back(std::shared_ptr<Space>(new Space(PushBackElement("LeftMargin"), id_, x, y, w, h)));
	id_spaces[id_] = spaces.back();
	leftMargin = spaces.back();
	return *leftMargin.lock();
}

/*! Adds a right margin
 * \throws	crn::ExceptionLogic	the page already has a right margin
 * \param[in]	id_	the id of the new space
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space& Alto::Layout::Page::AddRightMargin(const Id &id_, double x, double y, double w, double h)
{
	if (!rightMargin.expired())
		throw crn::ExceptionLogic(_("The page already has a right margin."));
	spaces.push_back(std::shared_ptr<Space>(new Space(PushBackElement("RightMargin"), id_, x, y, w, h)));
	id_spaces[id_] = spaces.back();
	rightMargin = spaces.back();
	return *rightMargin.lock();
}

/*! Adds a bottom margin
 * \throws	crn::ExceptionLogic	the page already has a bottom margin
 * \param[in]	id_	the id of the new space
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space& Alto::Layout::Page::AddBottomMargin(const Id &id_, double x, double y, double w, double h)
{
	if (!bottomMargin.expired())
		throw crn::ExceptionLogic(_("The page already has a bottom margin."));
	spaces.push_back(std::shared_ptr<Space>(new Space(PushBackElement("BottomMargin"), id_, x, y, w, h)));
	id_spaces[id_] = spaces.back();
	bottomMargin = spaces.back();
	return *bottomMargin.lock();
}

/*! Adds the print space
 * \throws	crn::ExceptionLogic	the page already has a print space
 * \param[in]	id_	the id of the new space
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space& Alto::Layout::Page::AddPrintSpace(const Id &id_, double x, double y, double w, double h)
{
	if (!printSpace.expired())
		throw crn::ExceptionLogic(_("The page already has a print space."));
	spaces.push_back(std::shared_ptr<Space>(new Space(PushBackElement("PrintSpace"), id_, x, y, w, h)));
	id_spaces[id_] = spaces.back();
	printSpace = spaces.back();
	return *printSpace.lock();
}

/*! Returns a space
 * \throws	crn::ExceptionNotFound	space not found
 * \param[in]	sid	the space id
 * \return	a space handle
 */
Alto::Layout::Page::Space& Alto::Layout::Page::GetSpace(const Id &sid)
{
	if (GetNbSubelements() != spaces.size()) 
		const_cast<Page*>(this)->update_subelements(); 
	std::map<Id, std::weak_ptr<Space> >::iterator it(id_spaces.find(sid));
	if ((it != id_spaces.end()) && !it->second.expired())
		return *(it->second.lock());
	for (const SpacePtr &s : spaces)
	{
		const std::shared_ptr<Layout::Page::Space> ss(s.lock());
		if (ss->GetId().Get() == sid)
		{
			id_spaces[sid] = s;
			return *ss;
		}
	}
	throw crn::ExceptionNotFound(_("Space not found."));
}

/*! \return the list of all spaces */
std::vector<Alto::Layout::Page::SpacePtr> Alto::Layout::Page::GetSpaces() const
{
	if (GetNbSubelements() != spaces.size()) 
		const_cast<Page*>(this)->update_subelements(); 

	return std::vector<SpacePtr>(spaces.begin(), spaces.end());
}

/*! Removes a space
 * \throws	crn::ExceptionNotFound	space not found
 * \param[in]	sid	the space id
 */
void Alto::Layout::Page::RemoveSpace(const Id &sid)
{
	for (std::vector<std::shared_ptr<Space> >::iterator it = spaces.begin(); it != spaces.end(); ++it)
		if ((*it)->GetId().Get() == sid)
		{
			RemoveChild(**it);
			spaces.erase(it);
			id_spaces.erase(sid);
			return;
		}
	throw crn::ExceptionNotFound(_("Space not found."));
}

