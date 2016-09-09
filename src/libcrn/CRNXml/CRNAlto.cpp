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
 * file: CRNAltoUtils.h
 * \author Yann LEYDIER
 */

#include <CRNXml/CRNAlto.h>
#include <CRNException.h>
#include <CRNi18n.h>

using namespace crn;
using namespace xml;

/*! Constructor from a file
 * \throws	crn::ExceptionIO	cannot read file
 * \throws	ExceptionInvalidArgument	null node
 * \throws	ExceptionNotFound	an mandatory element or attribute was not found
 * \param[in]	fname	the path to the file to read
 * \param[in]	char_conversion_throws	shall an exception be thrown on character conversion error?
 */
Alto::Alto(const Path &fname, bool char_conversion_throws):
	Document(fname, char_conversion_throws)
{
	root.reset(new Root(GetRoot()));
	register_ids();
}

/*! Constructor from image
 * \throws	crn::ExceptionInvalidArgument	null filename or namespace
 * \param[in]	imagename	the path the image
 * \param[in]	ns	the namespace
 * \param[in]	charset	the character encoding
 * \param[in]	throw_exceptions	shall an exception be thrown on character conversion error?
 */
Alto::Alto(const Path &imagename, const StringUTF8 &ns, const StringUTF8 &encoding, const StringUTF8 &version, bool char_conversion_throws):
	Document(encoding, version, char_conversion_throws)
{
	PushBackComment("Created by CoReNum Nimrod Alto engine");
	root.reset(new Root(PushBackElement("Alto"), imagename, ns));
}

/*! Returns a page
 * \warning	slow
 * \throws	ExceptionNotFound	page not found
 * \param[in]	id	the id of the page to get
 * \return	the page
 */
Alto::Layout::Page& Alto::GetPage(const Id &id)
{
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		const std::shared_ptr<Layout::Page> sp(p.lock());
		if (sp->GetId() == id)
			return *sp;
	} // pages
	throw ExceptionNotFound(_("Page not found."));
}

/*! Returns a page
 * \warning	slow
 * \throws	ExceptionNotFound	page not found
 * \param[in]	id	the id of the page to get
 * \return	the page
 */
const Alto::Layout::Page& Alto::GetPage(const Id &id) const
{
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		const std::shared_ptr<const Layout::Page> sp(p.lock());
		if (sp->GetId() == id)
			return *sp;
	} // pages
	throw ExceptionNotFound(_("Page not found."));
}

/*! Returns a space
 * \warning	slow
 * \throws	ExceptionNotFound	space not found
 * \param[in]	id	the id of the space to get
 * \return	the space
 */
Alto::Layout::Page::Space& Alto::GetSpace(const Id &id)
{
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		std::vector<Layout::Page::SpacePtr> spaces(p.lock()->GetSpaces());
		for (const Layout::Page::SpacePtr &s : spaces)
		{
			const std::shared_ptr<Layout::Page::Space> ss(s.lock());
			if (ss->GetId())
			 if (ss->GetId().Get() == id)
				return *ss;
		} // spaces
	} // pages
	throw ExceptionNotFound(_("Space not found."));
}

/*! Returns a space
 * \warning	slow
 * \throws	ExceptionNotFound	space not found
 * \param[in]	id	the id of the space to get
 * \return	the space
 */
const Alto::Layout::Page::Space& Alto::GetSpace(const Id &id) const
{
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		std::vector<Layout::Page::SpacePtr> spaces(p.lock()->GetSpaces());
		for (const Layout::Page::SpacePtr &s : spaces)
		{
			const std::shared_ptr<const Layout::Page::Space> ss(s.lock());
			if (ss->GetId())
			 if (ss->GetId().Get() == id)
				return *ss;
		} // spaces
	} // pages
	throw ExceptionNotFound(_("Space not found."));
}

/*! Returns a block
 * \warning	very slow
 * \throws	ExceptionNotFound	block not found
 * \param[in]	id	the id of the block to get
 * \return	the block
 */
Alto::Layout::Page::Space::Block& Alto::GetBlock(const Id &id)
{
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		std::vector<Layout::Page::SpacePtr> spaces(p.lock()->GetSpaces());
		for (const Layout::Page::SpacePtr &s : spaces)
		{
			std::vector<Layout::Page::Space::BlockPtr> blocks(s.lock()->GetBlocks());
			for (const Layout::Page::Space::BlockPtr &b : blocks)
			{
				const std::shared_ptr<Layout::Page::Space::Block> sb(b.lock());
				if (sb->GetId() == id)
					return *sb;
			} // blocks
		} // spaces
	} // pages
	throw ExceptionNotFound(_("Block not found."));
}

/*! Returns a block
 * \warning	very slow
 * \throws	ExceptionNotFound	block not found
 * \param[in]	id	the id of the block to get
 * \return	the block
 */
const Alto::Layout::Page::Space::Block& Alto::GetBlock(const Id &id) const
{
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		std::vector<Layout::Page::SpacePtr> spaces(p.lock()->GetSpaces());
		for (const Layout::Page::SpacePtr &s : spaces)
		{
			std::vector<Layout::Page::Space::BlockPtr> blocks(s.lock()->GetBlocks());
			for (const Layout::Page::Space::BlockPtr &b : blocks)
			{
				const std::shared_ptr<const Layout::Page::Space::Block> sb(b.lock());
				if (sb->GetId() == id)
					return *sb;
			} // blocks
		} // spaces
	} // pages
	throw ExceptionNotFound(_("Block not found."));
}

/*! Returns a textblock
 * \warning	very slow
 * \throws	ExceptionNotFound	textblock not found
 * \param[in]	id	the id of the textblock to get
 * \return	the textblock
 */
Alto::Layout::Page::Space::TextBlock& Alto::GetTextBlock(const Id &id)
{
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		std::vector<Layout::Page::SpacePtr> spaces(p.lock()->GetSpaces());
		for (const Layout::Page::SpacePtr &s : spaces)
		{
			std::vector<Layout::Page::Space::BlockPtr> blocks(s.lock()->GetBlocks());
			for (const Layout::Page::Space::BlockPtr &b : blocks)
			{
				const std::shared_ptr<Layout::Page::Space::TextBlock> tb(std::dynamic_pointer_cast<Layout::Page::Space::TextBlock>(b.lock()));
				if (tb)
				{
					if (tb->GetId() == id)
						return *tb;
				} // text block
			} // blocks
		} // spaces
	} // pages
	throw ExceptionNotFound(_("Text block not found."));
}

/*! Returns a textblock
 * \warning	very slow
 * \throws	ExceptionNotFound	textblock not found
 * \param[in]	id	the id of the textblock to get
 * \return	the textblock
 */
const Alto::Layout::Page::Space::TextBlock& Alto::GetTextBlock(const Id &id) const
{
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		std::vector<Layout::Page::SpacePtr> spaces(p.lock()->GetSpaces());
		for (const Layout::Page::SpacePtr &s : spaces)
		{
			std::vector<Layout::Page::Space::BlockPtr> blocks(s.lock()->GetBlocks());
			for (const Layout::Page::Space::BlockPtr &b : blocks)
			{
				const std::shared_ptr<const Layout::Page::Space::TextBlock> tb(std::dynamic_pointer_cast<Layout::Page::Space::TextBlock>(b.lock()));
				if (tb)
				{
					if (tb->GetId() == id)
						return *tb;
				} // text block
			} // blocks
		} // spaces
	} // pages
	throw ExceptionNotFound(_("Text block not found."));
}

/*! Returns a line
 * \warning	very slow
 * \throws	ExceptionNotFound	line not found
 * \param[in]	id	the id of the line to get
 * \return	the line
 */
Alto::Layout::Page::Space::TextBlock::TextLine& Alto::GetTextLine(const Id &id)
{
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		std::vector<Layout::Page::SpacePtr> spaces(p.lock()->GetSpaces());
		for (const Layout::Page::SpacePtr &s : spaces)
		{
			std::vector<Layout::Page::Space::BlockPtr> blocks(s.lock()->GetBlocks());
			for (const Layout::Page::Space::BlockPtr &b : blocks)
			{
				const std::shared_ptr<Layout::Page::Space::TextBlock> tb(std::dynamic_pointer_cast<Layout::Page::Space::TextBlock>(b.lock()));
				if (tb)
				{
					std::vector<Layout::Page::Space::TextBlock::TextLinePtr> lines(tb->GetTextLines());
					for (const Layout::Page::Space::TextBlock::TextLinePtr &l : lines)
					{
						const std::shared_ptr<Layout::Page::Space::TextBlock::TextLine> sl(l.lock());
						if (sl->GetId() == id)
							return *sl;
					} // lines
				} // text block
			} // blocks
		} // spaces
	} // pages
	throw ExceptionNotFound(_("Line not found."));
}

/*! Returns a line
 * \warning	very slow
 * \throws	ExceptionNotFound	line not found
 * \param[in]	id	the id of the line to get
 * \return	the line
 */
const Alto::Layout::Page::Space::TextBlock::TextLine& Alto::GetTextLine(const Id &id) const
{
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		std::vector<Layout::Page::SpacePtr> spaces(p.lock()->GetSpaces());
		for (const Layout::Page::SpacePtr &s : spaces)
		{
			std::vector<Layout::Page::Space::BlockPtr> blocks(s.lock()->GetBlocks());
			for (const Layout::Page::Space::BlockPtr &b : blocks)
			{
				const std::shared_ptr<const Layout::Page::Space::TextBlock> tb(std::dynamic_pointer_cast<Layout::Page::Space::TextBlock>(b.lock()));
				if (tb)
				{
					std::vector<Layout::Page::Space::TextBlock::TextLinePtr> lines(tb->GetTextLines());
					for (const Layout::Page::Space::TextBlock::TextLinePtr &l : lines)
					{
						const std::shared_ptr<const Layout::Page::Space::TextBlock::TextLine> sl(l.lock());
						if (sl->GetId() == id)
							return *sl;
					} // lines
				} // text block
			} // blocks
		} // spaces
	} // pages
	throw ExceptionNotFound(_("Line not found."));
}

/*! Returns a word
 * \warning	very slow
 * \throws	ExceptionNotFound	word not found
 * \param[in]	id	the id of the word to get
 * \return	the word
 */
Alto::Layout::Page::Space::TextBlock::TextLine::Word& Alto::GetWord(const Id &id)
{
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		std::vector<Layout::Page::SpacePtr> spaces(p.lock()->GetSpaces());
		for (const Layout::Page::SpacePtr &s : spaces)
		{
			std::vector<Layout::Page::Space::BlockPtr> blocks(s.lock()->GetBlocks());
			for (const Layout::Page::Space::BlockPtr &b : blocks)
			{
				const std::shared_ptr<Layout::Page::Space::TextBlock> tb(std::dynamic_pointer_cast<Layout::Page::Space::TextBlock>(b.lock()));
				if (tb)
				{
					std::vector<Layout::Page::Space::TextBlock::TextLinePtr> lines(tb->GetTextLines());
					for (const Layout::Page::Space::TextBlock::TextLinePtr &l : lines)
					{
						std::vector<Layout::Page::Space::TextBlock::TextLine::LineElementPtr> lels(l.lock()->GetLineElements());
						for (const Layout::Page::Space::TextBlock::TextLine::LineElementPtr lel : lels)
						{
							const std::shared_ptr<Layout::Page::Space::TextBlock::TextLine::Word> w(std::dynamic_pointer_cast<Layout::Page::Space::TextBlock::TextLine::Word>(lel.lock()));
							if (w)
							{
								if (w->GetId())
									if (w->GetId().Get() == id)
										return *w;
							} // word
						} // line elements
					} // lines
				} // text block
			} // blocks
		} // spaces
	} // pages
	throw ExceptionNotFound(_("Word not found."));
}

/*! Returns a word
 * \warning	very slow
 * \throws	ExceptionNotFound	word not found
 * \param[in]	id	the id of the word to get
 * \return	the word
 */
const Alto::Layout::Page::Space::TextBlock::TextLine::Word& Alto::GetWord(const Id &id) const
{
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		std::vector<Layout::Page::SpacePtr> spaces(p.lock()->GetSpaces());
		for (const Layout::Page::SpacePtr &s : spaces)
		{
			std::vector<Layout::Page::Space::BlockPtr> blocks(s.lock()->GetBlocks());
			for (const Layout::Page::Space::BlockPtr &b : blocks)
			{
				const std::shared_ptr<const Layout::Page::Space::TextBlock> tb(std::dynamic_pointer_cast<Layout::Page::Space::TextBlock>(b.lock()));
				if (tb)
				{
					std::vector<Layout::Page::Space::TextBlock::TextLinePtr> lines(tb->GetTextLines());
					for (const Layout::Page::Space::TextBlock::TextLinePtr &l : lines)
					{
						std::vector<Layout::Page::Space::TextBlock::TextLine::LineElementPtr> lels(l.lock()->GetLineElements());
						for (const Layout::Page::Space::TextBlock::TextLine::LineElementPtr lel : lels)
						{
							const std::shared_ptr<const Layout::Page::Space::TextBlock::TextLine::Word> w(std::dynamic_pointer_cast<const Layout::Page::Space::TextBlock::TextLine::Word>(lel.lock()));
							if (w)
							{
								if (w->GetId())
									if (w->GetId().Get() == id)
										return *w;
							} // word
						} // line elements
					} // lines
				} // text block
			} // blocks
		} // spaces
	} // pages
	throw ExceptionNotFound(_("Word not found."));
}

/*! Returns an element
 * \warning	very slow
 * \throws	ExceptionNotFound	element not found
 * \param[in]	id	the id of the element to get
 * \return	the element
 */
Element& Alto::GetElement(const Id &id)
{
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		const std::shared_ptr<Layout::Page> sp(p.lock());
		if (sp->GetId() == id)
			return *sp;
		std::vector<Layout::Page::SpacePtr> spaces(sp->GetSpaces());
		for (const Layout::Page::SpacePtr &s : spaces)
		{
			const std::shared_ptr<Layout::Page::Space> ss(s.lock());
			if (ss->GetId())
				if (ss->GetId().Get() == id)
					return *ss;
			std::vector<Layout::Page::Space::BlockPtr> blocks(ss->GetBlocks());
			for (Layout::Page::Space::BlockPtr &b : blocks)
			{
				const std::shared_ptr<Layout::Page::Space::Block> sb(b.lock());
				if (sb->GetId() == id)
					return *sb;
				const std::shared_ptr<Layout::Page::Space::TextBlock> tb(std::dynamic_pointer_cast<Layout::Page::Space::TextBlock>(sb));
				if (tb)
				{
					std::vector<Layout::Page::Space::TextBlock::TextLinePtr> lines(tb->GetTextLines());
					for (const Layout::Page::Space::TextBlock::TextLinePtr &l : lines)
					{
						const std::shared_ptr<Layout::Page::Space::TextBlock::TextLine> sl(l.lock());
						if (sl->GetId() == id)
							return *sl;
						std::vector<Layout::Page::Space::TextBlock::TextLine::LineElementPtr> lels(sl->GetLineElements());
						for (const Layout::Page::Space::TextBlock::TextLine::LineElementPtr lel : lels)
						{
							const std::shared_ptr<Layout::Page::Space::TextBlock::TextLine::Word> w(std::dynamic_pointer_cast<Layout::Page::Space::TextBlock::TextLine::Word>(lel.lock()));
							if (w)
							{
								if (w->GetId())
									if (w->GetId().Get() == id)
										return *w;
							} // word
							else
							{
								const std::shared_ptr<Layout::Page::Space::TextBlock::TextLine::WhiteSpace> s(std::dynamic_pointer_cast<Layout::Page::Space::TextBlock::TextLine::WhiteSpace>(lel.lock()));
								if (s)
								{
									if (s->GetId())
										if (s->GetId().Get() == id)
											return *s;
								} // white space
							}
						} // line elements
					} // lines
				} // text block
			} // blocks
		} // spaces
	} // pages
	throw ExceptionNotFound(_("Element not found."));
}

/*! Returns an element
 * \warning	very slow
 * \throws	ExceptionNotFound	element not found
 * \param[in]	id	the id of the element to get
 * \return	the element
 */
const Element& Alto::GetElement(const Id &id) const
{
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		const std::shared_ptr<const Layout::Page> sp(p);
		if (sp->GetId() == id)
			return *sp;
		std::vector<Layout::Page::SpacePtr> spaces(sp->GetSpaces());
		for (const Layout::Page::SpacePtr &s : spaces)
		{
			const std::shared_ptr<const Layout::Page::Space> ss(s.lock());
			if (ss->GetId())
				if (ss->GetId().Get() == id)
					return *ss;
			std::vector<Layout::Page::Space::BlockPtr> blocks(ss->GetBlocks());
			for (const Layout::Page::Space::BlockPtr &b : blocks)
			{
				const std::shared_ptr<const Layout::Page::Space::Block> sb(b.lock());
				if (sb->GetId() == id)
					return *sb;
				const std::shared_ptr<const Layout::Page::Space::TextBlock> tb(std::dynamic_pointer_cast<const Layout::Page::Space::TextBlock>(sb));
				if (tb)
				{
					std::vector<Layout::Page::Space::TextBlock::TextLinePtr> lines(tb->GetTextLines());
					for (const Layout::Page::Space::TextBlock::TextLinePtr &l : lines)
					{
						const std::shared_ptr<const Layout::Page::Space::TextBlock::TextLine> sl(l.lock());
						if (sl->GetId() == id)
							return *sl;
						std::vector<Layout::Page::Space::TextBlock::TextLine::LineElementPtr> lels(sl->GetLineElements());
						for (const Layout::Page::Space::TextBlock::TextLine::LineElementPtr lel : lels)
						{
							const std::shared_ptr<const Layout::Page::Space::TextBlock::TextLine::Word> w(std::dynamic_pointer_cast<Layout::Page::Space::TextBlock::TextLine::Word>(lel.lock()));
							if (w)
							{
								if (w->GetId())
									if (w->GetId().Get() == id)
										return *w;
							} // word
							else
							{
								const std::shared_ptr<const Layout::Page::Space::TextBlock::TextLine::WhiteSpace> s(std::dynamic_pointer_cast<Layout::Page::Space::TextBlock::TextLine::WhiteSpace>(lel.lock()));
								if (s)
								{
									if (s->GetId())
										if (s->GetId().Get() == id)
											return *s;
								} // white space
							}
						} // line elements
					} // lines
				} // text block
			} // blocks
		} // spaces
	} // pages
	throw ExceptionNotFound(_("Element not found."));
}

/*! Creates a table of the used ids */
void Alto::register_ids()
{
	std::vector<Id> lst(GetStyles().GetTextStyles());
	ids.insert(lst.begin(), lst.end());
	lst = GetStyles().GetParagraphStyles();
	ids.insert(lst.begin(), lst.end());
	std::vector<Layout::PagePtr> pages(GetLayout().GetPages());
	for (const Layout::PagePtr &p : pages)
	{
		const std::shared_ptr<const Layout::Page> sp(p.lock());
		ids.insert(sp->GetId());
		std::vector<Layout::Page::SpacePtr> spaces(sp->GetSpaces());
		for (const Layout::Page::SpacePtr &s : spaces)
		{
			const std::shared_ptr<const Layout::Page::Space> ss(s.lock());
			if (ss->GetId())
				ids.insert(ss->GetId().Get());
			std::vector<Layout::Page::Space::BlockPtr> blocks(ss->GetBlocks());
			for (const Layout::Page::Space::BlockPtr &b : blocks)
			{
				const std::shared_ptr<const Layout::Page::Space::Block> sb(b.lock());
				ids.insert(sb->GetId());
				const std::shared_ptr<const Layout::Page::Space::TextBlock> tb(std::dynamic_pointer_cast<const Layout::Page::Space::TextBlock>(sb));
				if (tb)
				{
					std::vector<Layout::Page::Space::TextBlock::TextLinePtr> lines(tb->GetTextLines());
					for (const Layout::Page::Space::TextBlock::TextLinePtr &l : lines)
					{
						const std::shared_ptr<const Layout::Page::Space::TextBlock::TextLine> sl(l.lock());
						ids.insert(sl->GetId());
						std::vector<Layout::Page::Space::TextBlock::TextLine::LineElementPtr> lels(sl->GetLineElements());
						for (const Layout::Page::Space::TextBlock::TextLine::LineElementPtr lel : lels)
						{
							const std::shared_ptr<const Layout::Page::Space::TextBlock::TextLine::Word> w(std::dynamic_pointer_cast<Layout::Page::Space::TextBlock::TextLine::Word>(lel.lock()));
							if (w)
							{
								if (w->GetId())
									ids.insert(w->GetId().Get());
							} // word
							else
							{
								const std::shared_ptr<const Layout::Page::Space::TextBlock::TextLine::WhiteSpace> s(std::dynamic_pointer_cast<Layout::Page::Space::TextBlock::TextLine::WhiteSpace>(lel.lock()));
								if (s)
								{
									if (s->GetId())
										ids.insert(s->GetId().Get());
								} // white space
							}
						} // line elements
					} // lines
				} // text block
			} // blocks
		} // spaces
	} // pages
}

/*! Creates a new id for the document
 * \return	an id not already used
 */
Id Alto::CreateId()
{
	bool ok = false;
	Id id;
	while (!ok)
	{
		id = StringUTF8::CreateUniqueId();
		if (ids.find(id) == ids.end())
		{
			ids.insert(id);
			ok = true;
		}
	}
	return id;
}

/*! Checks if an id already exists in the document
 * \param[in]	id	the id to check
 * \return	true if the id is free for use
 */
bool Alto::CheckId(const Id &id) const
{
	return ids.find(id) == ids.end();
}

/*! Adds an Id to an element and registers it
 * \warning	does not check if the element is really contained by the alto object
 * \throws	crn::ExceptionInvalidArgument	the element already has an id
 * \param[in]	el	the element to register
 * \return	the new id
 */
Id Alto::AddId(Element &el)
{
	try
	{
		el.GetAttribute<StringUTF8>("ID", false);
		throw crn::ExceptionInvalidArgument(_("The element already has an id."));
	}
	catch (crn::ExceptionNotFound&)
	{ }
	Id id(CreateId());
	el.SetAttribute("ID", id);
	return id;
}

//////////////////////////////////////////////////////////////////
// Root
//////////////////////////////////////////////////////////////////

/*!
 * \throws	ExceptionInvalidArgument	null node
 * \throws	ExceptionNotFound	no layout found
 * \param[in]	el	the element to read
 */
Alto::Root::Root(const Element &el):
	Element(el)
{
	if (!el)
		throw ExceptionInvalidArgument(_("Null node."));
	for (Element cel = BeginElement(); cel != EndElement(); ++cel)
	{
		StringUTF8 elname(cel.GetName());
		if (elname == "Description")
		{
			description.reset(new Description(cel));
		}
		else if (elname == "Styles")
		{
			styles.reset(new Styles(cel));
		}
		else if (elname == "Layout")
		{
			layout.reset(new Layout(cel));
		}
	}
	if (!layout) // mandatory element!
		throw crn::ExceptionNotFound(_("Cannot find layout."));
	init("file:///dev/null");
}

/*! Initializes an empty root
 * \throws	crn::ExceptionInvalidArgument	null filename or namespace
 * \param[in]	el	the empty element to fill
 * \param[in]	imagename	the path the image
 * \param[in]	ns	the namespace
 */
Alto::Root::Root(const Element &el, const Path &imagename, const StringUTF8 &ns):
	Element(el)
{
	if (ns.IsEmpty())
		throw crn::ExceptionInvalidArgument(_("Null namespace."));
	SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	SetAttribute("xmlns", ns);
	StringUTF8 nsloc(ns);
	nsloc += " ";
	nsloc += "http://www.loc.gov/standards/alto/alto-v2.0.xsd";
	SetAttribute("xsi:schemaLocation", nsloc);
	SetAttribute("xmlns:xlink", "http://www.w3.org/TR/xlink");
	init(imagename);
}

/*! Creates the inner elements if needed
 * \param[in]	imgname	the name of the image
 */
void Alto::Root::init(const StringUTF8 &imgname)
{
	if (!description)
		description.reset(new Description(PushBackElement("Description"), imgname));
	if (!styles)
		styles.reset(new Styles(PushBackElement("Styles")));
	if (!layout)
		layout.reset(new Layout(PushBackElement("Layout")));
}

