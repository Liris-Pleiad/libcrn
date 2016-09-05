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
 * file: CRNAltoSpace.cpp
 * \author Yann LEYDIER
 */

#include <CRNXml/CRNAlto.h>
#include <CRNException.h>
#include <algorithm>
#include <CRNi18n.h>

using namespace crn;
using namespace xml;

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \throws	ExceptionNotFound	a mandatory element or attribute was not found
 * \param[in]	conv	a charset converter
 * \param[in]	el	an XML element
 */
Alto::Layout::Page::Space::Space(const Element &el):
	Element(el)
{
	if (!el)
		throw ExceptionInvalidArgument(_("Null node."));
	GetAttribute<double>("HEIGHT", false); // may throw
	GetAttribute<double>("WIDTH", false); // may throw
	GetAttribute<double>("VPOS", false); // may throw
	GetAttribute<double>("HPOS", false); // may throw

	update_subelements();
}

/*! Updates the block cache */
void Alto::Layout::Page::Space::update_subelements()
{
	blocks.clear();
	id_blocks.clear();
	textBlocks.clear();
	id_textBlocks.clear();
	illustrations.clear();
	graphicalElements.clear();
	composedBlocks.clear();

	for (Element cel = BeginElement(); cel != EndElement(); ++cel)
	{
		std::shared_ptr<Block> newblock;
		StringUTF8 elname(cel.GetName());
		if (elname == "TextBlock")
		{
			newblock.reset(new TextBlock(cel));
			textBlocks.push_back(std::static_pointer_cast<TextBlock>(newblock));
			id_textBlocks[newblock->GetId()] = textBlocks.back();
		}
		else if (elname == "Illustration")
		{
			newblock.reset(new Illustration(cel));
			illustrations.push_back(std::static_pointer_cast<Illustration>(newblock));
		}
		else if (elname == "GraphicalElement")
		{
			newblock.reset(new GraphicalElement(cel));
			graphicalElements.push_back(std::static_pointer_cast<GraphicalElement>(newblock));
		}
		else if (elname == "ComposedBlock")
		{
			newblock.reset(new ComposedBlock(cel));
			composedBlocks.push_back(std::static_pointer_cast<ComposedBlock>(newblock));
		}
		if (newblock)
		{
			blocks.push_back(newblock);
			id_blocks[newblock->GetId()] = newblock;
		}
	}
}

/*! Constructor
 * \param[in]	id	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 * \param[in]	conv	a charset converter
 */
Alto::Layout::Page::Space::Space(const Element &el, const Id &id_, double x, double y, double w, double h):
	Element(el)
{
	SetAttribute("ID", id_);
	SetAttribute("HPOS", x);
	SetAttribute("VPOS", y);
	SetAttribute("WIDTH", w);
	SetAttribute("HEIGHT", h);
}

/*! \brief Returns the id of the element */
Option<Id> Alto::Layout::Page::Space::GetId() const
{
	Option<Id> id;
	StringUTF8 str = GetAttribute<StringUTF8>("ID");
	if (str.IsNotEmpty())
		id = str;
	return id;
}

/*! \return	the height of the element */
double Alto::Layout::Page::Space::GetHeight() const
{
	return GetAttribute<double>("HEIGHT", false); // should not throw
}

/*! \param[in]	d	the new height of the element */
void Alto::Layout::Page::Space::SetHeight(double d)
{
	SetAttribute("HEIGHT", d);
}

/*! \return	the width of the element */
double Alto::Layout::Page::Space::GetWidth() const
{
	return GetAttribute<double>("WIDTH", false); // should not throw
}

/*! \param[in]	d	the new width of the element */
void Alto::Layout::Page::Space::SetWidth(double d)
{
	SetAttribute("WIDTH", d);
}

/*! \return	the abscissa of the element */
double Alto::Layout::Page::Space::GetHPos() const
{
	return GetAttribute<double>("HPOS", false); // should throw
}

/*! \param[in]	d	the new abscissa of the element */
void Alto::Layout::Page::Space::SetHPos(double d)
{
	SetAttribute("HPOS", d);
}

/*! \return	the ordinate of the element */
double Alto::Layout::Page::Space::GetVPos() const
{
	return GetAttribute<double>("VPOS", false); // should throw
}

/*! \param[in]	d	the new ordinate of the element */
void Alto::Layout::Page::Space::SetVPos(double d)
{
	SetAttribute("VPOS", d);
}

/*! \return	the list of style references */
std::vector<Id> Alto::Layout::Page::Space::GetStyles() const
{
	return GetStyleRefs(*this);
}

/*! Adds a reference to a style
 * \param[in]	styleid	the reference to add
 */
void Alto::Layout::Page::Space::AddStyle(const Id &styleid)
{
	AddStyleRef(*this, styleid);
}

/*! Removes a reference to a style
 * \param[in]	styleid	the reference to remove
 */
void Alto::Layout::Page::Space::RemoveStyle(const Id &styleid)
{
	RemoveStyleRef(*this, styleid);
}

/*!
 * \throws	ExceptionNotFound	no block with this id
 * \param[in]	bid	the id of the block to retrieve
 * \return	a weak pointer on the block
 */
Alto::Layout::Page::Space::Block& Alto::Layout::Page::Space::GetBlock(const Id &bid)
{
	if (GetNbSubelements() != blocks.size()) 
		const_cast<Space*>(this)->update_subelements(); 
	std::map<Id, std::weak_ptr<Block> >::iterator it(id_blocks.find(bid));
	if ((it != id_blocks.end()) && !it->second.expired())
		return *(it->second.lock());
	for (const BlockPtr &tb : blocks) // probably useless
	{
		const std::shared_ptr<Block> stb(tb.lock());
		if (tb.lock()->GetId() == bid)
		{
			id_blocks[bid] = tb;
			return *tb.lock();
		}
	}
	throw ExceptionNotFound(_("The space contains no block with this id."));
}

/*! \returns the list of blocks in the print space */
std::vector<Alto::Layout::Page::Space::BlockPtr> Alto::Layout::Page::Space::GetBlocks() const 
{ 
	if (GetNbSubelements() != blocks.size()) 
		const_cast<Space*>(this)->update_subelements();
	return std::vector<BlockPtr>(blocks.begin(), blocks.end());
}

/*! Removes a block
 * \throws	crn::ExceptionNotFound	block not found
 * \param[in]	bid	the block id
 */
void Alto::Layout::Page::Space::RemoveBlock(const Id &bid)
{
	Block &b(GetBlock(bid)); // may throw
	RemoveChild(b);
	textBlocks.erase(std::remove_if(textBlocks.begin(), textBlocks.end(), [&bid](const TextBlockPtr &p){ return p.lock()->GetId() == bid; }), textBlocks.end());
	id_textBlocks.erase(bid);
	illustrations.erase(std::remove_if(illustrations.begin(), illustrations.end(), [&bid](const IllustrationPtr &p){ return p.lock()->GetId() == bid; }), illustrations.end());
	graphicalElements.erase(std::remove_if(graphicalElements.begin(), graphicalElements.end(), [&bid](const GraphicalElementPtr &p){ return p.lock()->GetId() == bid; }), graphicalElements.end());
	composedBlocks.erase(std::remove_if(composedBlocks.begin(), composedBlocks.end(), [&bid](const ComposedBlockPtr &p){ return p.lock()->GetId() == bid; }), composedBlocks.end());
	blocks.erase(std::remove_if(blocks.begin(), blocks.end(), [&bid](const BlockPtr &p){ return p.lock()->GetId() == bid; }), blocks.end());
	id_blocks.erase(bid);
}

/*! \return	the list of style references */
std::vector<Id> Alto::Layout::Page::Space::Block::GetStyles() const
{
	return GetStyleRefs(*this);
}

/*! Adds a reference to a style
 * \param[in]	styleid	the reference to add
 */
void Alto::Layout::Page::Space::Block::AddStyle(const Id &styleid)
{
	AddStyleRef(*this, styleid);
}

/*! Removes a reference to a style
 * \param[in]	styleid	the reference to remove
 */
void Alto::Layout::Page::Space::Block::RemoveStyle(const Id &styleid)
{
	RemoveStyleRef(*this, styleid);
}

/*!
 * \throws	ExceptionNotFound	no text block with this id
 * \param[in]	id	the id of the block to retrieve
 * \return	a weak pointer on the text block
 */
Alto::Layout::Page::Space::TextBlock& Alto::Layout::Page::Space::GetTextBlock(const Id &id)
{
	if (GetNbSubelements() != blocks.size()) 
		const_cast<Space*>(this)->update_subelements(); 
	std::map<Id, TextBlockPtr>::iterator it(id_textBlocks.find(id));
	if ((it != id_textBlocks.end()) && !it->second.expired())
		return *(it->second.lock());
	for (const TextBlockPtr &tb : textBlocks)
	{
		const std::shared_ptr<TextBlock> stb(tb.lock());
		if (stb->GetId() == id)
		{
			id_textBlocks[id] = tb;
			return *stb;
		}
	}
	throw ExceptionNotFound(_("The space contains no text block with this id."));
}

/*! Adds a text block in the print space
 * \param[in]	id	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::TextBlock& Alto::Layout::Page::Space::AddTextBlock(const Id &id_, int x, int y, int w, int h)
{
	std::shared_ptr<TextBlock> bl(new TextBlock(PushBackElement("TextBlock"), id_, x, y, w, h));
	blocks.push_back(bl);
	id_blocks[id_] = bl;
	textBlocks.push_back(bl);
	id_textBlocks[id_] = textBlocks.back();
	return *bl;
}

/*! Adds a text block in the print space
 * \throws	ExceptionNotFound	previous block not found
 * \param[in]	pred id of the previous block
 * \param[in]	id	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::TextBlock& Alto::Layout::Page::Space::AddTextBlockAfter(const Id &pred, const Id &id_, int x, int y, int w, int h)
{
	for (std::vector<std::shared_ptr<Block> >::iterator it = blocks.begin(); it != blocks.end(); ++it)
	{
		if ((*it)->GetId() == pred)
		{
			Element pel(**it);
			++it;
			if (it == blocks.end())
				return AddTextBlock(id_, x, y, w, h);
			else
			{
				std::shared_ptr<TextBlock> bl(new TextBlock(InsertElement(pel, "TextBlock"), id_, x, y, w, h));
				blocks.insert(it, bl);
				id_blocks[id_] = bl;
				for (; it != blocks.end(); ++it)
				{
					if (std::dynamic_pointer_cast<TextBlock>(*it))
						break;
				}
				if (it != blocks.end())
					textBlocks.insert(std::find_if(textBlocks.begin(), textBlocks.end(), [&it](const TextBlockPtr &p){ return p.lock() == *it; }), bl);
				else
					textBlocks.push_back(bl);
				id_textBlocks[id_] = bl;
				return *bl;
			}
		}
	}
	throw ExceptionNotFound(_("Cannot find block."));
}

/*! Adds a text block in the print space
 * \throws	ExceptionNotFound	next block not found
 * \param[in]	next id of the next block
 * \param[in]	id	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::TextBlock& Alto::Layout::Page::Space::AddTextBlockBefore(const Id &next, const Id &id_, int x, int y, int w, int h)
{
	for (std::vector<std::shared_ptr<Block> >::iterator it = blocks.begin(); it != blocks.end(); ++it)
	{
		if ((*it)->GetId() == next)
		{
			std::shared_ptr<TextBlock> bl;
			if (it == blocks.begin())
				bl.reset(new TextBlock(PushFrontElement("TextBlock"), id_, x, y, w, h));
			else
				bl.reset(new TextBlock(InsertElement(**(it - 1), "TextBlock"), id_, x, y, w, h));
			blocks.insert(it, bl);
			id_blocks[id_] = bl;
			for (; it != blocks.end(); ++it)
			{
				if (std::dynamic_pointer_cast<TextBlock>(*it))
					break;
			}
			if (it != blocks.end())
				textBlocks.insert(std::find_if(textBlocks.begin(), textBlocks.end(), [&it](const TextBlockPtr &p){ return p.lock() == *it; }), bl);
			else
				textBlocks.push_back(bl);
			id_textBlocks[id_] = bl;
			return *bl;
		}
	}
	throw ExceptionNotFound(_("Cannot find block."));
}

/*! \return the list of illustrations in the print space */
const std::vector<Alto::Layout::Page::Space::IllustrationPtr>& Alto::Layout::Page::Space::GetIllustrations() const
{
	if (GetNbSubelements() != blocks.size()) 
		const_cast<Space*>(this)->update_subelements(); 
	return illustrations;
}

/*! Adds an illustration in the print space
 * \param[in]	id	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::Illustration& Alto::Layout::Page::Space::AddIllustration(const Id &id_, int x, int y, int w, int h)
{
	std::shared_ptr<Illustration> bl(new Illustration(PushBackElement("Illustration"), id_, x, y, w, h));
	blocks.push_back(bl);
	id_blocks[id_] = bl;
	illustrations.push_back(bl);
	return *bl;
}

/*! Adds an illustration in the print space
 * \throws	ExceptionNotFound	previous block not found
 * \param[in]	pred	the id of the previous block
 * \param[in]	id	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::Illustration& Alto::Layout::Page::Space::AddIllustrationAfter(const Id &pred, const Id &id_, int x, int y, int w, int h)
{
	for (std::vector<std::shared_ptr<Block> >::iterator it = blocks.begin(); it != blocks.end(); ++it)
	{
		if ((*it)->GetId() == pred)
		{
			Element pel(**it);
			++it;
			if (it == blocks.end())
				return AddIllustration(id_, x, y, w, h);
			else
			{
				std::shared_ptr<Illustration> bl(new Illustration(InsertElement(pel, "Illustration"), id_, x, y, w, h));
				blocks.insert(it, bl);
				id_blocks[id_] = bl;
				for (; it != blocks.end(); ++it)
				{
					if (std::dynamic_pointer_cast<Illustration>(*it))
						break;
				}
				if (it != blocks.end())
					illustrations.insert(std::find_if(illustrations.begin(), illustrations.end(), [&it](const IllustrationPtr &p){ return p.lock() == *it; }), bl);
				else
					illustrations.push_back(bl);
				return *bl;
			}
		}
	}
	throw ExceptionNotFound(_("Cannot find block."));
}

/*! Adds an illustration in the print space
 * \throws	ExceptionNotFound	next block not found
 * \param[in]	next id of the next block
 * \param[in]	id	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::Illustration& Alto::Layout::Page::Space::AddIllustrationBefore(const Id &next, const Id &id_, int x, int y, int w, int h)
{
	for (std::vector<std::shared_ptr<Block> >::iterator it = blocks.begin(); it != blocks.end(); ++it)
	{
		if ((*it)->GetId() == next)
		{
			std::shared_ptr<Illustration> bl;
			if (it == blocks.begin())
				bl.reset(new Illustration(PushFrontElement("Illustration"), id_, x, y, w, h));
			else
				bl.reset(new Illustration(InsertElement(**(it - 1), "Illustration"), id_, x, y, w, h));
			blocks.insert(it, bl);
			id_blocks[id_] = bl;
			for (; it != blocks.end(); ++it)
			{
				if (std::dynamic_pointer_cast<Illustration>(*it))
					break;
			}
			if (it != blocks.end())
				illustrations.insert(std::find_if(illustrations.begin(), illustrations.end(), [&it](const IllustrationPtr &p){ return p.lock() == *it; }), bl);
			else
				illustrations.push_back(bl);
			return *bl;
		}
	}
	throw ExceptionNotFound(_("Cannot find block."));
}

/*! \return the list of graphical elements in the print space */
const std::vector<Alto::Layout::Page::Space::GraphicalElementPtr>& Alto::Layout::Page::Space::GetGraphicalElements() const 
{
	if (GetNbSubelements() != blocks.size()) 
		const_cast<Space*>(this)->update_subelements(); 
	return graphicalElements;
}

/*! Adds an graphical element in the print space
 * \param[in]	id	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::GraphicalElement& Alto::Layout::Page::Space::AddGraphicalElement(const Id &id_, int x, int y, int w, int h)
{
	std::shared_ptr<GraphicalElement> bl(new GraphicalElement(PushBackElement("GraphicalElement"), id_, x, y, w, h));
	blocks.push_back(bl);
	id_blocks[id_] = bl;
	graphicalElements.push_back(bl);
	return *bl;
}

/*! Adds an graphical element in the print space
 * \throws	ExceptionNotFound	previous block not found
 * \param[in]	pred	the id of the previous block
 * \param[in]	id	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::GraphicalElement& Alto::Layout::Page::Space::AddGraphicalElementAfter(const Id &pred, const Id &id_, int x, int y, int w, int h)
{
	for (std::vector<std::shared_ptr<Block> >::iterator it = blocks.begin(); it != blocks.end(); ++it)
	{
		if ((*it)->GetId() == pred)
		{
			Element pel(**it);
			++it;
			if (it == blocks.end())
				return AddGraphicalElement(id_, x, y, w, h);
			else
			{
				std::shared_ptr<GraphicalElement> bl(new GraphicalElement(InsertElement(pel, "GraphicalElement"), id_, x, y, w, h));
				blocks.insert(it, bl);
				id_blocks[id_] = bl;
				for (; it != blocks.end(); ++it)
				{
					if (std::dynamic_pointer_cast<GraphicalElement>(*it))
						break;
				}
				if (it != blocks.end())
					graphicalElements.insert(std::find_if(graphicalElements.begin(), graphicalElements.end(), [&it](const GraphicalElementPtr &p){ return p.lock() == *it; }), bl);
				else
					graphicalElements.push_back(bl);
				return *bl;
			}
		}
	}
	throw ExceptionNotFound(_("Cannot find block."));
}

/*! Adds an graphical element in the print space
 * \throws	ExceptionNotFound	next block not found
 * \param[in]	next id of the next block
 * \param[in]	id	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::GraphicalElement& Alto::Layout::Page::Space::AddGraphicalElementBefore(const Id &next, const Id &id_, int x, int y, int w, int h)
{
	for (std::vector<std::shared_ptr<Block> >::iterator it = blocks.begin(); it != blocks.end(); ++it)
	{
		if ((*it)->GetId() == next)
		{
			std::shared_ptr<GraphicalElement> bl;
			if (it == blocks.begin())
				bl.reset(new GraphicalElement(PushFrontElement("GraphicalElement"), id_, x, y, w, h));
			else
				bl.reset(new GraphicalElement(InsertElement(**(it - 1), "GraphicalElement"), id_, x, y, w, h));
			blocks.insert(it, bl);
			id_blocks[id_] = bl;
			for (; it != blocks.end(); ++it)
			{
				if (std::dynamic_pointer_cast<GraphicalElement>(*it))
					break;
			}
			if (it != blocks.end())
				graphicalElements.insert(std::find_if(graphicalElements.begin(), graphicalElements.end(), [&it](const GraphicalElementPtr &p){ return p.lock() == *it; }), bl);
			else
				graphicalElements.push_back(bl);
			return *bl;
		}
	}
	throw ExceptionNotFound(_("Cannot find block."));
}

/*! \brief Returns the list of composed blocks in the print space */
const std::vector<Alto::Layout::Page::Space::ComposedBlockPtr>& Alto::Layout::Page::Space::GetComposedBlocks() const
{
	if (GetNbSubelements() != blocks.size()) 
		const_cast<Space*>(this)->update_subelements(); 
	return composedBlocks;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Block
///////////////////////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \param[in]	nam	the name of the element
 */
Alto::Layout::Page::Space::Block::Block(const Element &el):
	Element(el)
{
	if (!el)
		throw ExceptionInvalidArgument(_("Null node."));
	id = GetAttribute<StringUTF8>("ID", false); // may throw
	GetAttribute<int>("HEIGHT", false); // may throw
	GetAttribute<int>("WIDTH", false); // may throw
	GetAttribute<int>("HPOS", false); // may throw
	GetAttribute<int>("VPOS", false); // may throw
}

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \param[in]	nam	the name of the element
 * \param[in]	id	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 */
Alto::Layout::Page::Space::Block::Block(const Element &el, const Id &id_, int x, int y, int w, int h):
	Element(el),
	id(id_)
{
	if (!el)
		throw ExceptionInvalidArgument(_("Null node."));
	SetAttribute("ID", id);
	SetAttribute("HPOS", x);
	SetAttribute("VPOS", y);
	SetAttribute("WIDTH", w);
	SetAttribute("HEIGHT", h);
}

/*! \return	the height of the element */
int Alto::Layout::Page::Space::Block::GetHeight() const
{
	return GetAttribute<int>("HEIGHT", false); // should not throw
}

/*! \param[in]	i	the new height of the element */
void Alto::Layout::Page::Space::Block::SetHeight(int i)
{
	SetAttribute("HEIGHT", i);
}

/*! \return	the width of the element */
int Alto::Layout::Page::Space::Block::GetWidth() const
{
	return GetAttribute<int>("WIDTH", false); // should not throw
}

/*! \param[in]	i	the new width of the element */
void Alto::Layout::Page::Space::Block::SetWidth(int i)
{
	SetAttribute("WIDTH", i);
}

/*! \return	the abscissa of the element */
int Alto::Layout::Page::Space::Block::GetHPos() const
{
	return GetAttribute<int>("HPOS", false); // should not throw
}

/*! \param[in]	i	the new abscissa of the element */
void Alto::Layout::Page::Space::Block::SetHPos(int i)
{
	SetAttribute("HPOS", i);
}

/*! \return	the ordinate of the element */
int Alto::Layout::Page::Space::Block::GetVPos() const
{
	return GetAttribute<int>("VPOS", false); // should not throw
}

/*! \param[in]	i	the new ordinate of the element */
void Alto::Layout::Page::Space::Block::SetVPos(int i)
{
	SetAttribute("VPOS", i);
}

/*! \return	the rotation of the object, in degree, counterclockwise */
Option<double> Alto::Layout::Page::Space::Block::GetRotation() const
{
	Option<double> rotation;
	try { rotation = GetAttribute<double>("ROTATION", false); } catch (...) { }
	return rotation;
}

/*! \return	the id of the next block */
Option<Id> Alto::Layout::Page::Space::Block::GetNextId() const
{
	Option<Id> next;
	StringUTF8 str = GetAttribute<StringUTF8>("IDNEXT");
	if (str.IsNotEmpty())
		next = str;
	return next;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Illustration
///////////////////////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \throws	ExceptionNotFound	a mandatory element or attribute was not found
 * \param[in]	el	an XML element
 * \param[in]	conv	a charset converter
 */
Alto::Layout::Page::Space::Illustration::Illustration(const Element &el):
	Block(el)
{ }

/*! Constructor
 * \param[in]	id	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 * \param[in]	conv	a charset converter
 */
Alto::Layout::Page::Space::Illustration::Illustration(const Element &el, const Id &id_, int x, int y, int w, int h):
	Block(el, id_, x, y, w, h)
{ }

/*! \return	the type of illustration (photo, map, drawing, chart…) */
Option<StringUTF8> Alto::Layout::Page::Space::Illustration::GetType() const
{
	Option<StringUTF8> type;
	StringUTF8 str = GetAttribute<StringUTF8>("TYPE");
	if (str.IsNotEmpty())
		type = str;
	return type;
}

/*! \return	a link to an image containing only this illustration */
Option<StringUTF8> Alto::Layout::Page::Space::Illustration::GetFileId() const
{
	Option<StringUTF8> fileId;
	StringUTF8 str = GetAttribute<StringUTF8>("FILEID");
	if (str.IsNotEmpty())
		fileId = str;
	return fileId;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// GraphicalElement
///////////////////////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \throws	ExceptionNotFound	a mandatory element or attribute was not found
 * \param[in]	el	an XML element
 */
Alto::Layout::Page::Space::GraphicalElement::GraphicalElement(const Element &el):
	Block(el)
{
	if (!el)
		throw ExceptionInvalidArgument(_("Null node."));
}

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \param[in]	el	an XML element
 * \param[in]	id	the id of the element
 * \param[in]	x	the horizontal position
 * \param[in]	y	the vertical position
 * \param[in]	w	the width
 * \param[in]	h	the height
 * \param[in]	conv	a charset converter
 */
Alto::Layout::Page::Space::GraphicalElement::GraphicalElement(const Element &el, const Id &id_, int x, int y, int w, int h):
	Block(el, id_, x, y, w, h)
{ 
	if (!el)
		throw ExceptionInvalidArgument(_("Null node."));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ComposedBlock
///////////////////////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \throws	ExceptionInvalidArgument	null node
 * \throws	ExceptionNotFound	a mandatory element or attribute was not found
 * \param[in]	el	an XML element
 */
Alto::Layout::Page::Space::ComposedBlock::ComposedBlock(const Element &el):
	Block(el)
{
	if (!el)
		throw ExceptionInvalidArgument(_("Null node."));

	for (Element cel = BeginElement(); cel != EndElement(); ++cel)
	{
		std::shared_ptr<Block> newblock;
		StringUTF8 elname(cel.GetName());
		if (elname == "TextBlock")
		{
			newblock.reset(new TextBlock(cel));
			textBlocks.push_back(std::static_pointer_cast<TextBlock>(newblock));
		}
		else if (elname == "Illustration")
		{
			newblock.reset(new Illustration(cel));
			illustrations.push_back(std::static_pointer_cast<Illustration>(newblock));
		}
		else if (elname == "GraphicalElement")
		{
			newblock.reset(new GraphicalElement(cel));
			graphicalElements.push_back(std::static_pointer_cast<GraphicalElement>(newblock));
		}
		else if (elname == "ComposedBlock")
		{
			newblock.reset(new ComposedBlock(cel));
			composedBlocks.push_back(std::static_pointer_cast<ComposedBlock>(newblock));
		}
		if (newblock)
			blocks.push_back(newblock);
	}
}

/*! \return	the type of composed block (photo, map, drawing, chart…) */
Option<StringUTF8> Alto::Layout::Page::Space::ComposedBlock::GetType() const
{
	Option<StringUTF8> type;
	StringUTF8 str = GetAttribute<StringUTF8>("TYPE");
	if (str.IsNotEmpty())
		type = str;
	return type;
}

/*! \return	a link to an image containing only this composed block */
Option<StringUTF8> Alto::Layout::Page::Space::ComposedBlock::GetFileId() const
{
	Option<StringUTF8> fileId;
	StringUTF8 str = GetAttribute<StringUTF8>("FILEID");
	if (str.IsNotEmpty())
		fileId = str;
	return fileId;
}

