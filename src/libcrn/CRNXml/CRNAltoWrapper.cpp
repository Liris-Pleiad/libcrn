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
 * file: CRNAltoWrapper.cpp
 * \author Yann LEYDIER
 */

#include <CRNXml/CRNAltoWrapper.h>
#include <CRNIO/CRNIO.h>
#include <CRNi18n.h>

using namespace crn;
using namespace xml;

const crn::String& PagePath::Separator()
{
	static const crn::String p(U" ");
	return p;
}

PagePath::PagePath(const String &p)
{
	std::vector<String> pl(p.Split(Separator()));
	if (pl.size() < 2)
		throw ExceptionInvalidArgument(_("Malformed path string."));
	view_id = pl[0];
	page_id = pl[1].CStr();
}

const PagePath& PagePath::NullPath()
{
	static const PagePath p;
	return p;
}

SpacePath::SpacePath(const String &p)
{
	std::vector<String> pl(p.Split(Separator()));
	if (pl.size() < 3)
		throw ExceptionInvalidArgument(_("Malformed path string."));
	view_id = pl[0];
	page_id = pl[1].CStr();
	space_id = pl[2].CStr();
}

const SpacePath& SpacePath::NullPath()
{
	static const SpacePath p;
	return p;
}


BlockPath::BlockPath(const String &p)
{
	std::vector<String> pl(p.Split(Separator()));
	if (pl.size() < 4)
		throw ExceptionInvalidArgument(_("Malformed path string."));
	view_id = pl[0];
	page_id = pl[1].CStr();
	space_id = pl[2].CStr();
	block_id = pl[3].CStr();
}

const BlockPath& BlockPath::NullPath()
{
	static const BlockPath p;
	return p;
}

TextLinePath::TextLinePath(const String &p)
{
	std::vector<String> pl(p.Split(Separator()));
	if (pl.size() < 5)
		throw ExceptionInvalidArgument(_("Malformed path string."));
	view_id = pl[0];
	page_id = pl[1].CStr();
	space_id = pl[2].CStr();
	block_id = pl[3].CStr();
	textline_id = pl[4].CStr();
}

const TextLinePath& TextLinePath::NullPath()
{
	static const TextLinePath p;
	return p;
}

WordPath::WordPath(const String &p)
{
	std::vector<String> pl(p.Split(Separator()));
	if (pl.size() < 6)
		throw ExceptionInvalidArgument(_("Malformed path string."));
	view_id = pl[0];
	page_id = pl[1].CStr();
	space_id = pl[2].CStr();
	block_id = pl[3].CStr();
	textline_id = pl[4].CStr();
	word_id = pl[5].CStr();
}

const WordPath& WordPath::NullPath()
{
	static const WordPath p;
	return p;
}

const String& AltoWrapper::AltoPathKey()
{
	static const crn::String k(U"nimrod::AltoPath");
	return k;
}
const String& AltoWrapper::PageKey()
{
	static const crn::String k(U"nimrod::Page");
	return k;
}
const String& AltoWrapper::SpaceKey()
{
	static const crn::String k(U"nimrod::Space");
	return k;
}
const String& AltoWrapper::TextBlockKey()
{
	static const crn::String k(U"nimrod::TextBlock");
	return k;
}
const String& AltoWrapper::IllustrationKey()
{
	static const crn::String k(U"nimrod::Illustration");
	return k;
}
const String& AltoWrapper::GraphicalElementKey()
{
	static const crn::String k(U"nimrod::GraphicalElement");
	return k;
}
const String& AltoWrapper::ComposedBlockKey()
{
	static const crn::String k(U"nimrod::ComposedBlock");
	return k;
}
const String& AltoWrapper::TextLineKey()
{
	static const crn::String k(U"nimrod::TextLine");
	return k;
}
const String& AltoWrapper::WordKey()
{
	static const crn::String k(U"nimrod::Word");
	return k;
}

/*! Constructor
 * \param[in]	throw_exceptions	shall an exception be thrown on character encoding conversion error?
 */
AltoWrapper::AltoWrapper(bool throw_exceptions):
	doc(std::make_shared<crn::Document>()),
	throws(throw_exceptions)
{
	doc->SetUserData(AltoPathKey(), std::make_shared<crn::Map>());
}

struct keepXML
{
	bool operator()(const Path &p) { Path ext(p.GetExtension()); ext.ToLower(); return ext != "xml"; }
};
/*! Creates a wrapper from a directory containing Altos
 * \throws	ExceptionIO	cannot open directory
 * \throws	ExceptionIO	the image could not be loaded (file not found or invalid image format)
 * \throws	ExceptionInvalidArgument	null node in alto 
 * \throws	ExceptionNotFound	an mandatory alto element or attribute was not found
 * \throws	ExceptionIO	cannot read alto file
 * \param[in]	directory	the path to the XML Alto files
 * \param[in]	documentname	the file name of the crn::Document to create
 * \param[in]	imagedirectory	[Optional] the path to the images if it's different from the XML path
 * \param[in]	prog	a progress object
 * \param[in]	throw_exceptions	shall an exception be thrown on character encoding conversion error?
 * \return	a new Alto Wrapper
 */
std::unique_ptr<AltoWrapper> AltoWrapper::NewFromDir(const crn::Path &directory, const crn::Path &documentname, const crn::Path &imagedirectory, crn::Progress *prog, bool throw_exceptions)
{
	// create data
	std::unique_ptr<AltoWrapper> wrapper(new AltoWrapper(throw_exceptions));
	// compute paths
	const Path xpath(directory);
	//std::cout << xpath << std::endl;
	const Path ipath = imagedirectory.IsEmpty() ? directory : imagedirectory;
	//std::cout << ipath << std::endl;

	// get list of XML files
	IO::Directory xdir(xpath);
	std::vector<Path> xfiles(xdir.GetFiles());
	xfiles.erase(std::remove_if(xfiles.begin(), xfiles.end(), keepXML()), xfiles.end()); // filter files
	std::sort(xfiles.begin(), xfiles.end()); // sort files

	// read altos and add images to document
	if (prog)
		prog->SetMaxCount(int(xfiles.size()));
	for (const Path &altofile : xfiles)
	{
		//std::cout << altofile.CStr()<< std::endl;
		try
		{
			Alto xml(altofile, throw_exceptions);
			Alto::Description &desc(xml.GetDescription());
			Path imgfile(desc.GetFilename().Get());
			if (imgfile.IsRelative())
				imgfile = ipath / imgfile;
			wrapper->AddView(imgfile, altofile);
		}
		catch (std::exception &e)
		{ // cannot read the alto its image name is wrong
			CRNdout << altofile.CStr() << " : " << e.what() << std::endl;
		}
		if (prog)
			prog->Advance();
	}

	// save document
	wrapper->doc->Save(documentname);

	// compute links between Blocks and Altos
	wrapper->Synchronize(false);

	return std::move(wrapper);
}

/*! Creates a wrapper from two directories containing images and Altos with the same base names
 * \throws	ExceptionIO	cannot open directory
 * \throws	ExceptionIO	the image could not be loaded (file not found or invalid image format)
 * \throws	ExceptionInvalidArgument	null node in alto 
 * \throws	ExceptionNotFound	an mandatory alto element or attribute was not found
 * \throws	ExceptionIO	cannot read alto file
 * \param[in]	image_directory	the path to the images
 * \param[in]	xml_directory	the path to the alto files
 * \param[in]	documentname	the file name of the crn::Document to create
 * \param[in]	prog	a progress object
 * \param[in]	throw_exceptions	shall an exception be thrown on character encoding conversion error?
 * \return	a new Alto Wrapper
 */
std::unique_ptr<AltoWrapper> AltoWrapper::NewFromDirs(const crn::Path &image_directory, const crn::Path &xml_directory, const crn::Path &documentname, crn::Progress *prog, bool throw_exceptions)
{
	// create data
	std::unique_ptr<AltoWrapper> wrapper(new AltoWrapper(throw_exceptions));
	
	// get list of XML files
	IO::Directory idir(image_directory);
	std::vector<Path> ifiles(idir.GetFiles());
	std::sort(ifiles.begin(), ifiles.end()); // sort files

	// read altos and add images to document
	if (prog)
		prog->SetMaxCount(int(ifiles.size()));
	for (const Path &imgfile : ifiles)
	{
		try
		{
			Path altofile(xml_directory);
			altofile += Path::Separator();
			altofile += imgfile.GetBase();
			if (IO::Access(altofile + ".xml", IO::EXISTS))
				altofile += ".xml";
			else if (IO::Access(altofile + ".Xml", IO::EXISTS))
				altofile += ".Xml";
			else if (IO::Access(altofile + ".XML", IO::EXISTS))
				altofile += ".XML";
			else
			{ // no alto corresponds to the image
				throw crn::ExceptionNotFound(StringUTF8(imgfile) + _(": no xml match."));
			}
			wrapper->AddView(imgfile, altofile);
		}
		catch (std::exception &e)
		{ // cannot read the alto its image name is wrong
			CRNWarning(e.what());
		}
		if (prog)
			prog->Advance();
	}

	// save document
	wrapper->doc->Save(documentname);

	// compute links between Blocks and Altos
	wrapper->Synchronize(false);

	return std::move(wrapper);
}

/*! Creates a wrapper from a crn::Document that was created by Nimrod
 * \throws	crn::ExceptionInvalidArgument	not a Nimrod document
 * \param[in]	document	the indexed document
 * \param[in]	create_altos	shall empty altos be created if the needed?
 * \param[in]	throw_exceptions	shall an exception be thrown on character encoding conversion error?
 * \return	a new Alto Wrapper
 */
std::unique_ptr<AltoWrapper> AltoWrapper::NewFromDocument(const crn::SDocument &document, bool create_altos, bool throw_exceptions)
{
	if (!document->IsUserData(AltoPathKey()) && !create_altos)
		throw crn::ExceptionInvalidArgument(_("The document is not associated to any Alto data."));

	UAltoWrapper wrapper(new AltoWrapper(throw_exceptions));
	wrapper->doc = document;

	if (create_altos)
		wrapper->createAltos();

	return std::move(wrapper);
}

/*! Creates a wrapper from a crn::Document that was created by Nimrod
 * \throws	ExceptionIO	cannot read file
 * \throws	ExceptionUninitialized	empty file
 * \throws	ExceptionRuntime	not a document file
 * \throws	ExceptionNotFound	not base directory found
 * \throws	ExceptionIO	cannot create directory
 * \param[in]	documentname	the name of the indexed document
 * \param[in]	create_altos	shall empty altos be created if the needed?
 * \param[in]	throw_exceptions	shall an exception be thrown on character encoding conversion error?
 * \return	a new Alto Wrapper
 */
std::unique_ptr<AltoWrapper> AltoWrapper::NewFromDocument(const crn::Path &documentname, bool create_altos, bool throw_exceptions)
{
	UAltoWrapper wrapper(new AltoWrapper(throw_exceptions));
	wrapper->doc->Load(documentname);

	if (!wrapper->doc->IsUserData(AltoPathKey()) && !create_altos)
		throw crn::ExceptionInvalidArgument(_("The document is not associated to any Alto data."));

	if (create_altos)
		wrapper->createAltos();

	return std::move(wrapper);
}

/*! Creates a wrapper from list of images and Alto paths
 * \throws	ExceptionInvalidArgument	null filename
 * \throws	ExceptionIO	the image could not be loaded (file not found or invalid image format)
 * \throws	ExceptionInvalidArgument	null node in alto 
 * \throws	ExceptionNotFound	an mandatory alto element or attribute was not found
 * \throws	ExceptionIO	cannot read alto file
 * \param[in]	filelist	the list of image/xml couples
 * \param[in]	documentname	the path of the crn::Document to create
 * \param[in]	prog	a progress object
 * \param[in]	throw_exceptions	shall an exception be thrown on character encoding conversion error?
 * \return	a new Alto Wrapper
 */
std::unique_ptr<AltoWrapper> AltoWrapper::newFromList(const std::vector<std::pair<crn::Path, crn::Path> > &filelist, const crn::Path &documentname, crn::Progress *prog, bool throw_exceptions)
{
	// create data
	UAltoWrapper wrapper(new AltoWrapper(throw_exceptions));
	
	// save document
	wrapper->doc->Save(documentname);
	
	// read altos and add images to document
	if (prog)
		prog->SetMaxCount(int(filelist.size()));
	for (const std::pair<crn::Path, crn::Path> &p : filelist)
	{
		wrapper->AddView(p.first, p.second);
		if (prog)
			prog->Advance();
	}

	// compute links between Blocks and Altos
	wrapper->Synchronize(false);

	// save document
	wrapper->doc->Save();
	
	return std::move(wrapper);
}

/*! Adds a view to the document 
 * \throws	ExceptionInvalidArgument	null filename
 * \throws	ExceptionIO	the image could not be loaded (file not found or invalid image format)
 * \throws	ExceptionInvalidArgument	null node in alto 
 * \throws	ExceptionNotFound	an mandatory alto element or attribute was not found
 * \throws	ExceptionIO	cannot read alto file
 * \param[in]	imagename	the image to add
 * \param[in]	altoname	the path to the associated alto or "" to create a new alto
 * \return	the id of the new view
 */
const String AltoWrapper::AddView(const Path &imagename, const Path &altoname)
{
	{ // check image
		auto img = NewImageFromFile(imagename); // may throw
	}
	if (altoname.IsNotEmpty())
	{ // check alto
		Alto a(altoname); // may throw
	}
	// add view
	String vid = doc->AddView(imagename); // may throw
	if (altoname.IsEmpty())
	{
		createAltos();
	}
	else
	{
		SMap altomap(std::static_pointer_cast<Map>(doc->GetUserData(AltoPathKey())));
		altomap->Set(vid, Clone(altoname));
	}
	if (doc->GetFilename().IsNotEmpty())
		doc->Save();
	return vid;
}

/*! Creates ids where there is none and builds the CRNBlock trees
 * \param[in]	reset	shall the blocks be rebuild
 */
void AltoWrapper::Synchronize(bool reset)
{
	SMap altomap(std::static_pointer_cast<Map>(doc->GetUserData(AltoPathKey())));
	const std::vector<String> vids(doc->GetViewIds());
	for(const String &id : vids)
	{
		SBlock block(doc->GetView(id));
		// cleanup
		if (block->HasTree(PageKey()))
		{
			if (reset)
				block->RemoveTree(PageKey());
			else 
				continue;
		}

		// load alto
		Alto alto(*std::static_pointer_cast<Path>((*altomap)[id]));

		// pages
		std::vector<std::weak_ptr<Alto::Layout::Page> > pages(alto.GetLayout().GetPages());
		int refx = 0;
		for (const std::weak_ptr<Alto::Layout::Page> &page : pages)
		{
			const std::shared_ptr<Alto::Layout::Page> spage(page.lock());
			try { spage->GetId(); } catch (...) { alto.AddId(*spage); }
			crn::Rect pagearea;
			const Option<Alto::Layout::Page::Position> pos(spage->GetPosition());
			int pagew = spage->GetWidth() ? spage->GetWidth().Get() : block->GetAbsoluteBBox().GetWidth() / int(pages.size());
			int pageh = spage->GetHeight() ? spage->GetHeight().Get() : block->GetAbsoluteBBox().GetHeight();
			if (pos)
			{
				if (*pos == Alto::Layout::Page::Position::Right)
				{
					pagearea = crn::Rect(block->GetAbsoluteBBox().GetRight() - pagew, 0, block->GetAbsoluteBBox().GetRight(), pageh);
				}
				else // considered to be left
				{
					pagearea = crn::Rect(refx, 0, refx + pagew, pageh);
					refx = pagearea.GetRight();
				}
			}
			else
			{
				pagearea = crn::Rect(refx, 0, refx + pagew, pageh);
				refx = pagearea.GetRight();
			}
			SBlock pageblock(block->AddChildAbsolute(PageKey(), pagearea, spage->GetId()));
			// spaces
			std::vector<std::weak_ptr<Alto::Layout::Page::Space> > spaces(spage->GetSpaces());
			for (const std::weak_ptr<Alto::Layout::Page::Space> space : spaces)
			{
				const std::shared_ptr<Alto::Layout::Page::Space> sspace(space.lock());
				try { sspace->GetId(); } catch (...) { alto.AddId(*sspace); }
				int spacev = int(sspace->GetVPos());
				int spaceh = int(sspace->GetHPos());
				crn::Rect spacearea(spaceh, spacev, spaceh + int(sspace->GetWidth()), spacev + int(sspace->GetHeight()));
				String sname(sspace->GetName());
				sname.ToLower();
				SBlock spaceblock(pageblock->AddChildAbsolute(SpaceKey(), spacearea, sname));
				// text blocks
				std::vector<std::weak_ptr<Alto::Layout::Page::Space::TextBlock> > textblocks(sspace->GetTextBlocks());
				for (const std::weak_ptr<Alto::Layout::Page::Space::TextBlock> &tb : textblocks)
				{
					const std::shared_ptr<Alto::Layout::Page::Space::TextBlock> stb(tb.lock());
					try { stb->GetId(); } catch (...) { alto.AddId(*stb); }
					int tbv = int(stb->GetVPos());
					int tbh = int(stb->GetHPos());
					crn::Rect tbarea(tbh, tbv, tbh + int(stb->GetWidth()), tbv + int(stb->GetHeight()));
					SBlock tbblock(spaceblock->AddChildAbsolute(TextBlockKey(), tbarea, stb->GetId()));
					// lines
					std::vector<std::weak_ptr<Alto::Layout::Page::Space::TextBlock::TextLine> > textlines(stb->GetTextLines());
					for (const std::weak_ptr<Alto::Layout::Page::Space::TextBlock::TextLine> &tl : textlines)
					{
						const std::shared_ptr<Alto::Layout::Page::Space::TextBlock::TextLine> stl(tl.lock());
						try { stl->GetId(); } catch (...) { alto.AddId(*stl); }
						int tlv = int(stl->GetVPos());
						int tlh = int(stl->GetHPos());
						crn::Rect tlarea(tlh, tlv, tlh + int(stl->GetWidth()), tlv + int(stl->GetHeight()));
						SBlock tlblock(tbblock->AddChildAbsolute(TextLineKey(), tlarea, stl->GetId()));
						// words
						std::vector<std::weak_ptr<Alto::Layout::Page::Space::TextBlock::TextLine::Word> > words(stl->GetWords());
						for (const std::weak_ptr<Alto::Layout::Page::Space::TextBlock::TextLine::Word> &word : words)
						{
							const std::shared_ptr<Alto::Layout::Page::Space::TextBlock::TextLine::Word> sword(word.lock());
							try { sword->GetId(); } catch (...) { alto.AddId(*sword); }
							if (sword->GetVPos() && sword->GetHPos() && sword->GetWidth())
							{
								int wx = int(sword->GetHPos().Get());
								int wy = int(sword->GetVPos().Get());
								int ww = int(sword->GetWidth().Get());
								int wh = sword->GetHeight() ? int(sword->GetHeight().Get()) : tlarea.GetHeight();
								crn::Rect wordarea(wx, wy, wx + ww, wy + wh);
								tlblock->AddChildAbsolute(WordKey(), wordarea, sword->GetId().Get());
							}
						} // words
					} // lines
				} // text blocks
				// illustrations
				std::vector<std::weak_ptr<Alto::Layout::Page::Space::Illustration> > illustrations(sspace->GetIllustrations());
				for (const std::weak_ptr<Alto::Layout::Page::Space::Illustration> &illus : illustrations)
				{
					const std::shared_ptr<Alto::Layout::Page::Space::Illustration> sillus(illus.lock());
					try { sillus->GetId(); } catch (...) { alto.AddId(*sillus); }
					int ilv = int(sillus->GetVPos());
					int ilh = int(sillus->GetHPos());
					crn::Rect ilarea(ilh, ilv, ilh + int(sillus->GetWidth()), ilv + int(sillus->GetHeight()));
					spaceblock->AddChildAbsolute(IllustrationKey(), ilarea);
				} // illustrations
				// illustrations
				std::vector<std::weak_ptr<Alto::Layout::Page::Space::GraphicalElement> > graphelems(sspace->GetGraphicalElements());
				for (const std::weak_ptr<Alto::Layout::Page::Space::GraphicalElement> &gel : graphelems)
				{
					const std::shared_ptr<Alto::Layout::Page::Space::GraphicalElement> sgel(gel.lock());
					try { sgel->GetId(); } catch (...) { alto.AddId(*sgel); }
					int gev = int(sgel->GetVPos());
					int geh = int(sgel->GetHPos());
					crn::Rect gearea(geh, gev, geh + int(sgel->GetWidth()), gev + int(sgel->GetHeight()));
					spaceblock->AddChildAbsolute(GraphicalElementKey(), gearea);
				} // illustrations
				// composed blocks
				// TODO
			} // spaces
		} // pages
		alto.Save();
	} // views
}

/*! Creates empty altos where needed 
 */
void AltoWrapper::createAltos()
{
	SMap altomap;
	if (!doc->IsUserData(AltoPathKey()))
	{
		altomap = std::make_shared<crn::Map>();
		doc->SetUserData(AltoPathKey(), altomap);
	}
	else
		altomap = std::static_pointer_cast<Map>(doc->GetUserData(AltoPathKey()));
	const std::vector<String> vids(doc->GetViewIds());
	for (const String &id : vids)
	{
		if (altomap->Find(id) == altomap->end())
		{ // create alto
			Alto a(doc->GetViewFilename(id), StringUTF8("nimrod"));
			Path altoname(doc->GetBasename() / id.CStr() + "_alto.xml");
			a.Save(altoname);
			altomap->Set(id, Clone(altoname));
		}
	}
	doc->Save();
}

/*! Gets a view by id 
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionNotFound	id not found
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 * \throws	ExceptionInvalidArgument	null node in alto
 * \throws	ExceptionNotFound	an mandatory element or attribute was not found
 * \throws	ExceptionIO	cannot read alto file
 * \param[in]	view_id	the id of the view to get
 * \return	a wrapper on the view and its alto
 */
AltoWrapper::View AltoWrapper::GetView(const String &view_id)
{
	return View(getLock(view_id), view_id);
}

/*! Gets a view by id 
 * \throws	ExceptionDomain	index out of bounds
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionNotFound	id not found
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 * \throws	ExceptionInvalidArgument	null node in alto
 * \throws	ExceptionNotFound	an mandatory element or attribute was not found
 * \throws	ExceptionIO	cannot read alto file
 * \param[in]	index	the index of the view to get
 * \return	a wrapper on the view and its alto
 */
AltoWrapper::View AltoWrapper::GetView(size_t index)
{
	String id(doc->GetViewId(index));
	return View(getLock(id), id);
}

/*! Gets a lock on a view
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionNotFound	id not found
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 * \throws	ExceptionInvalidArgument	null node in alto
 * \throws	ExceptionNotFound	an mandatory element or attribute was not found
 * \throws	ExceptionIO	cannot read alto file
 * \param[in]	view_id	the id of the view to get
 * \return	a lock on the view and its alto
 */
std::shared_ptr<AltoWrapper::ViewLock> AltoWrapper::getLock(const String &view_id) const
{
	std::map<String, std::weak_ptr<ViewLock> >::iterator it(viewLocks.find(view_id));
	if (it == viewLocks.end())
	{
		SCMap altomap(std::static_pointer_cast<const Map>(doc->GetUserData(AltoPathKey())));
		std::shared_ptr<ViewLock> vl(new ViewLock(doc->GetView(view_id), std::make_shared<Alto>(*std::static_pointer_cast<const Path>(altomap->Get(view_id)), throws))); // may throw
		//it = viewLocks.insert(std::make_pair(view_id, vl)).first;
		viewLocks[view_id] = vl;
		return vl;
	}
	else
	{
		if (it->second.expired())
		{
			SCMap altomap(std::static_pointer_cast<const Map>(doc->GetUserData(AltoPathKey())));
			std::shared_ptr<ViewLock> vl(new ViewLock(doc->GetView(view_id), std::make_shared<Alto>(*std::static_pointer_cast<const Path>(altomap->Get(view_id)), throws))); // may throw
			//it = viewLocks.insert(std::make_pair(view_id, vl)).first;
			viewLocks[view_id] = vl;
			return vl;
		}
		else
		{
			return it->second.lock();
		}
	}
}

/*! Gets a Word by path
 * \throws	ExceptionDomain	index out of bounds
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionNotFound	id not found
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 * \throws	ExceptionInvalidArgument	null node in alto
 * \throws	ExceptionNotFound	an mandatory element or attribute was not found
 * \throws	ExceptionIO	cannot read alto file
 * \param[in]	p	the path to the word
 * \return	a wrapper on the word
 */
AltoWrapper::Word AltoWrapper::GetWord(const WordPath &p)
{
	return GetView(p.view_id).GetPage(p.page_id).GetSpace(p.space_id).GetTextBlock(p.block_id).GetTextLine(p.textline_id).GetWord(p.word_id);
}

/*! Gets a TextLine by path
 * \throws	ExceptionDomain	index out of bounds
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionNotFound	id not found
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 * \throws	ExceptionInvalidArgument	null node in alto
 * \throws	ExceptionNotFound	an mandatory element or attribute was not found
 * \throws	ExceptionIO	cannot read alto file
 * \param[in]	p	the path to the line
 * \return	a wrapper on the line
 */
AltoWrapper::TextLine AltoWrapper::GetTextLine(const TextLinePath &p)
{
	return GetView(p.view_id).GetPage(p.page_id).GetSpace(p.space_id).GetTextBlock(p.block_id).GetTextLine(p.textline_id);
}

/*! Gets a TextBlock by path
 * \throws	ExceptionDomain	index out of bounds
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionNotFound	id not found
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 * \throws	ExceptionInvalidArgument	null node in alto
 * \throws	ExceptionNotFound	an mandatory element or attribute was not found
 * \throws	ExceptionIO	cannot read alto file
 * \param[in]	p	the path to the text block
 * \return	a wrapper on the text block
 */
AltoWrapper::TextBlock AltoWrapper::GetTextBlock(const BlockPath &p)
{
	return GetView(p.view_id).GetPage(p.page_id).GetSpace(p.space_id).GetTextBlock(p.block_id);
}

/*! Gets a Space by path
 * \throws	ExceptionDomain	index out of bounds
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionNotFound	id not found
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 * \throws	ExceptionInvalidArgument	null node in alto
 * \throws	ExceptionNotFound	an mandatory element or attribute was not found
 * \throws	ExceptionIO	cannot read alto file
 * \param[in]	p	the path to the space
 * \return	a wrapper on the space
 */
AltoWrapper::Space AltoWrapper::GetSpace(const SpacePath &p)
{
	return GetView(p.view_id).GetPage(p.page_id).GetSpace(p.space_id);
}

/*! Gets a Page by path
 * \throws	ExceptionDomain	index out of bounds
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionNotFound	id not found
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 * \throws	ExceptionInvalidArgument	null node in alto
 * \throws	ExceptionNotFound	an mandatory element or attribute was not found
 * \throws	ExceptionIO	cannot read alto file
 * \param[in]	p	the path to the page
 * \return	a wrapper on the page
 */
AltoWrapper::Page AltoWrapper::GetPage(const PagePath &p)
{
	return GetView(p.view_id).GetPage(p.page_id);
}

/*! Changes the size of a word and all its parents if needed
 * \throws	ExceptionNotFound	invalid path
 * \param[in]	p	the path to the word
 * \param[in]	r	the new bounding box
 */
void AltoWrapper::ResizeWord(const WordPath &p, const crn::Rect &r)
{
	GetView(p.view_id).ResizeWord(p, r);
}

/*! Changes the size of a text line and all its parents if needed
 * \throws	ExceptionNotFound	invalid path
 * \throws	crn::ExceptionDomain	bounding box is too small to hold the content
 * \param[in]	p	the path to the word
 * \param[in]	r	the new bounding box
 * \param[in]	erase_oob	shall the out of bounds contained elements be erased?
 */
void AltoWrapper::ResizeTextLine(const TextLinePath &p, const crn::Rect &r, bool erase_oob)
{
	GetView(p.view_id).ResizeTextLine(p, r, erase_oob);
}

/*! Changes the size of a text block and all its parents if needed
 * \throws	ExceptionNotFound	invalid path
 * \throws	crn::ExceptionDomain	bounding box is too small to hold the content
 * \param[in]	p	the path to the block
 * \param[in]	r	the new bounding box
 * \param[in]	erase_oob	shall the out of bounds contained elements be erased?
 */
void AltoWrapper::ResizeTextBlock(const BlockPath &p, const crn::Rect &r, bool erase_oob)
{
	GetView(p.view_id).ResizeTextBlock(p, r, erase_oob);
}

/*! Changes the size of a space and all its parents if needed
 * \throws	ExceptionNotFound	invalid path
 * \throws	crn::ExceptionDomain	bounding box is too small to hold the content
 * \param[in]	p	the path to the space
 * \param[in]	r	the new bounding box
 * \param[in]	erase_oob	shall the out of bounds contained elements be erased?
 */
void AltoWrapper::ResizeSpace(const SpacePath &p, const crn::Rect &r, bool erase_oob)
{
	GetView(p.view_id).ResizeSpace(p, r, erase_oob);
}

///////////////////////////////////////////////////////////////////////////////////
// View
///////////////////////////////////////////////////////////////////////////////////

/*! \return	the list of page ids */
std::vector<Id> AltoWrapper::View::GetPages()
{
	std::vector<Id> ids;
	const std::vector<std::weak_ptr<AltoPage> > &pages(lock->GetAlto()->GetLayout().GetPages());
	for (const std::weak_ptr<AltoPage> &p : pages)
		ids.push_back(p.lock()->GetId());
	return ids;
}

/*! Gets a page
 * \throws	ExceptionNotFound	page not found
 * \param[in]	pageId	the id of the page to get
 * \return	a wrapper on the page
 */
AltoWrapper::Page AltoWrapper::View::GetPage(const Id &pageId)
{
	SBlock b(lock->GetBlock()->GetChild(PageKey(), pageId)); // may throw
	AltoPage &p(lock->GetAlto()->GetLayout().GetPage(pageId)); // may throw
	return AltoWrapper::Page(b, p, lock, id);
}

/*! Adds a page
 * \throws	crn::ExceptionDomain	invalid page position
 * \param[in]	image_number	the physical image number
 * \param[in]	w the page width
 * \param[in]	h	the page height
 * \param[in]	pos	the page position (optional)
 * \return	a wrapper on the new page
 */
AltoWrapper::Page AltoWrapper::View::AddPage(int image_number, int w, int h, Option<AltoPage::Position> pos)
{
	Id pageId(lock->GetAlto()->CreateId());
	AltoPage &page(lock->GetAlto()->GetLayout().AddPage(pageId, image_number, w, h, pos));

	int pagew = page.GetWidth() ? page.GetWidth().Get() : lock->GetBlock()->GetAbsoluteBBox().GetWidth() / int(lock->GetAlto()->GetLayout().GetPages().size());
	int pageh = page.GetHeight() ? page.GetHeight().Get() : lock->GetBlock()->GetAbsoluteBBox().GetHeight();
	auto pagearea = crn::Rect(0, 0, pagew, pageh);
	if (pos)
	{
		if (*pos == Alto::Layout::Page::Position::Right)
		{
			pagearea = crn::Rect(lock->GetBlock()->GetAbsoluteBBox().GetRight() - pagew, 0, lock->GetBlock()->GetAbsoluteBBox().GetRight(), pageh);
		}
		// else considered to be left
	}
	auto b = lock->GetBlock()->AddChildAbsolute(PageKey(), pagearea, pageId);

	return Page(b, page, lock, id);
}

/*! Adds a page
 * \throws	crn::ExceptionNotFound cannot find previous page
 * \throws	crn::ExceptionDomain	invalid page position
 * \param[in]	next	id of the previous page
 * \param[in]	image_number	the physical image number
 * \param[in]	w the page width
 * \param[in]	h	the page height
 * \param[in]	pos	the page position (optional)
 * \return	a wrapper on the new page
 */
AltoWrapper::Page AltoWrapper::View::AddPageAfter(const Id &pred, int image_number, int w, int h, Option<AltoPage::Position> pos)
{
	Id pageId(lock->GetAlto()->CreateId());
	AltoPage &page(lock->GetAlto()->GetLayout().AddPageAfter(pred, pageId, image_number, w, h, pos)); // may throw

	int pagew = page.GetWidth() ? page.GetWidth().Get() : lock->GetBlock()->GetAbsoluteBBox().GetWidth() / int(lock->GetAlto()->GetLayout().GetPages().size());
	int pageh = page.GetHeight() ? page.GetHeight().Get() : lock->GetBlock()->GetAbsoluteBBox().GetHeight();
	auto pagearea = crn::Rect(0, 0, pagew, pageh);
	if (pos)
	{
		if (*pos == Alto::Layout::Page::Position::Right)
		{
			pagearea = crn::Rect(lock->GetBlock()->GetAbsoluteBBox().GetRight() - pagew, 0, lock->GetBlock()->GetAbsoluteBBox().GetRight(), pageh);
		}
		// else considered to be left
	}
	auto b = lock->GetBlock()->AddChildAbsolute(PageKey(), pagearea, pageId);

	return Page(b, page, lock, id);
}

/*! Adds a page
 * \throws	crn::ExceptionNotFound cannot find next page
 * \throws	crn::ExceptionDomain	invalid page position
 * \param[in]	next	id of the next page
 * \param[in]	image_number	the physical image number
 * \param[in]	w the page width
 * \param[in]	h	the page height
 * \param[in]	pos	the page position (optional)
 * \return	a wrapper on the new page
 */
AltoWrapper::Page AltoWrapper::View::AddPageBefore(const Id &next, int image_number, int w, int h, Option<AltoPage::Position> pos)
{
	Id pageId(lock->GetAlto()->CreateId());
	AltoPage &page(lock->GetAlto()->GetLayout().AddPageBefore(next, pageId, image_number, w, h, pos)); // may throw

	int pagew = page.GetWidth() ? page.GetWidth().Get() : lock->GetBlock()->GetAbsoluteBBox().GetWidth() / int(lock->GetAlto()->GetLayout().GetPages().size());
	int pageh = page.GetHeight() ? page.GetHeight().Get() : lock->GetBlock()->GetAbsoluteBBox().GetHeight();
	auto pagearea = crn::Rect(0, 0, pagew, pageh);
	if (pos)
	{
		if (*pos == Alto::Layout::Page::Position::Right)
		{
			pagearea = crn::Rect(lock->GetBlock()->GetAbsoluteBBox().GetRight() - pagew, 0, lock->GetBlock()->GetAbsoluteBBox().GetRight(), pageh);
		}
		// else considered to be left
	}
	auto b = lock->GetBlock()->AddChildAbsolute(PageKey(), pagearea, pageId);

	return Page(b, page, lock, id);
}

/*! Removes a page
 * \throws	crn::ExceptionNotFound	page not found
 * \param[in]	pid	the page id
 */
void AltoWrapper::View::RemovePage(const Id &pageId)
{
	lock->GetAlto()->GetLayout().RemovePage(pageId);
	lock->GetBlock()->RemoveChild(PageKey(), pageId);
}

/*! Changes the size of a word and all its parents if needed
 * \throws	ExceptionNotFound	invalid path
 * \param[in]	p	the path to the word
 * \param[in]	r	the new bounding box
 */
void AltoWrapper::View::ResizeWord(const WordPath &p, const crn::Rect &r)
{
	Page page(GetPage(p.page_id)); // may throw
	Space space(page.GetSpace(p.space_id)); // may throw
	TextBlock tb(space.GetTextBlock(p.block_id)); // may throw
	TextLine tl(tb.GetTextLine(p.textline_id)); // may throw
	Word w(tl.GetWord(p.word_id)); // may throw

	// page
	crn::Rect box(page.GetBBox() | r);
	if (box != page.GetBBox())
		page.SetBBox(box, false); // grow
	// space
	box = space.GetBBox() | r;
	if (box != space.GetBBox())
		space.SetBBox(box, false); // grow
	// block
	box = tb.GetBBox() | r;
	if (box != tb.GetBBox())
		tb.SetBBox(box, false); // grow
	// line
	box = tl.GetBBox() | r;
	if (box != tl.GetBBox())
		tl.SetBBox(box, false); // grow
	// word
	w.SetBBox(r);
}

/*! Changes the size of a text line and all its parents if needed
 * \throws	ExceptionNotFound	invalid path
 * \throws	crn::ExceptionDomain	bounding box is too small to hold the content
 * \param[in]	p	the path to the line
 * \param[in]	r	the new bounding box
 * \param[in]	erase_oob	shall the out of bounds contained elements be erased?
 */
void AltoWrapper::View::ResizeTextLine(const TextLinePath &p, const crn::Rect &r, bool erase_oob)
{
	Page page(GetPage(p.page_id)); // may throw
	Space space(page.GetSpace(p.space_id)); // may throw
	TextBlock tb(space.GetTextBlock(p.block_id)); // may throw
	TextLine tl(tb.GetTextLine(p.textline_id)); // may throw

	// page
	crn::Rect box(page.GetBBox() | r);
	if (box != page.GetBBox())
		page.SetBBox(box, false); // grow
	// space
	box = space.GetBBox() | r;
	if (box != space.GetBBox())
		space.SetBBox(box, false); // grow
	// block
	box = tb.GetBBox() | r;
	if (box != tb.GetBBox())
		tb.SetBBox(box, false); // grow
	// line
	tl.SetBBox(r, erase_oob); // may throw only if shrinking
}

/*! Changes the size of a text block and all its parents if needed
 * \throws	ExceptionNotFound	invalid path
 * \throws	crn::ExceptionDomain	bounding box is too small to hold the content
 * \param[in]	p	the path to the block
 * \param[in]	r	the new bounding box
 * \param[in]	erase_oob	shall the out of bounds contained elements be erased?
 */
void AltoWrapper::View::ResizeTextBlock(const BlockPath &p, const crn::Rect &r, bool erase_oob)
{
	Page page(GetPage(p.page_id)); // may throw
	Space space(page.GetSpace(p.space_id)); // may throw
	TextBlock tb(space.GetTextBlock(p.block_id)); // may throw

	// page
	crn::Rect box(page.GetBBox() | r);
	if (box != page.GetBBox())
		page.SetBBox(box, false); // grow
	// space
	box = space.GetBBox() | r;
	if (box != space.GetBBox())
		space.SetBBox(box, false); // grow
	// block
	tb.SetBBox(r, erase_oob); // may throw only if shrinking
}

/*! Changes the size of a space and all its parents if needed
 * \throws	ExceptionNotFound	invalid path
 * \throws	crn::ExceptionDomain	bounding box is too small to hold the content
 * \param[in]	p	the path to the space
 * \param[in]	r	the new bounding box
 * \param[in]	erase_oob	shall the out of bounds contained elements be erased?
 */
void AltoWrapper::View::ResizeSpace(const SpacePath &p, const crn::Rect &r, bool erase_oob)
{
	Page page(GetPage(p.page_id)); // may throw
	Space space(page.GetSpace(p.space_id)); // may throw

	// page
	crn::Rect box(page.GetBBox() | r);
	if (box != page.GetBBox())
		page.SetBBox(box, false); // grow
	// space
	space.SetBBox(r, erase_oob); // may throw only if shrinking
}

///////////////////////////////////////////////////////////////////////////////////
// Page
///////////////////////////////////////////////////////////////////////////////////

/*! Sets the coordinates of the page. The coordinates are cropped to the view's bounding box. If the new bounding box is smaller than the previous one, the spaces, text blocks, text lines and words in the page are cropped.
 * \throws	crn::ExceptionInvalidArgument	uninitialized bounding box
 * \throws	crn::ExceptionDimension	bounding box is out of parent's bounding box
 * \throws	crn::ExceptionDomain	bounding box is too small to hold the content
 * \param[in]	r	the new bounding box of the page
 * \param[in]	erase_oob	shall the out of bounds contained elements be erased?
 */
void AltoWrapper::Page::SetBBox(const crn::Rect &r, bool erase_oob)
{
	// check inner elements
	const std::vector<Id> spaces(GetSpaces());
	for (const Id &sid : spaces)
	{
		Space s(GetSpace(sid));
		if ((s.GetBBox() & r).GetArea() == 0)
		{
			if (erase_oob)
				RemoveSpace(sid);
			else
				throw crn::ExceptionDomain(_("Resizing the element would invalidate its content."));
		}
		if (!erase_oob)
		{
			const std::vector<Id> blocks(s.GetTextBlocks());
			for (const Id &bid : blocks)
			{
				TextBlock tb(s.GetTextBlock(bid));
				if ((tb.GetBBox() & r).GetArea() == 0)
					throw crn::ExceptionDomain(_("Resizing the element would invalidate its content."));
				const std::vector<Id> lines(tb.GetTextLines());
				for (const Id &lid : lines)
				{
					TextLine tl(tb.GetTextLine(lid));
					if ((tl.GetBBox() & r).GetArea() == 0)
						throw crn::ExceptionDomain(_("Resizing the element would invalidate its content."));
					const std::vector<Id> words(tl.GetWords());
					for (const Id &wid : words)
					{
						Word w(tl.GetWord(wid));
						if ((w.GetBBox() & r).GetArea() == 0)
							throw crn::ExceptionDomain(_("Resizing the element would invalidate its content."));
					}
				}
			}
		}
	}
	block->SetAbsoluteBBox(r); // adjusts the coordinates of the words in the CRNBlock
	const crn::Rect crop(block->GetAbsoluteBBox());
	//page->SetHPos(crop.GetLeft());
	//page->SetVPos(crop.GetTop());
	page->SetWidth(crop.GetWidth());
	page->SetHeight(crop.GetHeight());
	for (const Id &sid : spaces)
	{
		Space s(GetSpace(sid));
		s.SetBBox(s.GetBBox(), erase_oob); // GetBBox() uses the CRNBlock's coordinates that are up to date
	}
}

/*! \return	the text styles */
std::vector<Alto::Styles::Text> AltoWrapper::Page::GetTextStyles()
{
	std::vector<Alto::Styles::Text> sty;
	const std::vector<Id> &sids(GetStyles());
	for (const Id &sid : sids)
	{
		try { sty.push_back(lock->GetAlto()->GetStyles().GetTextStyle(sid)); } catch (...) { }
	}
	return sty;
}

/*! \return	the paragraph styles */
std::vector<Alto::Styles::Paragraph> AltoWrapper::Page::GetParagraphStyles()
{
	std::vector<Alto::Styles::Paragraph> sty;
	const std::vector<Id> &sids(GetStyles());
	for (const Id &sid : sids)
	{
		try { sty.push_back(lock->GetAlto()->GetStyles().GetParagraphStyle(sid)); } catch (...) { }
	}
	return sty;
}

/*! \return	the ids of the spaces in the page */
std::vector<Id> AltoWrapper::Page::GetSpaces() const
{
	const std::vector<std::weak_ptr<AltoSpace> > spaces(page->GetSpaces());
	std::vector<Id> ids(spaces.size());
	for (size_t tmp = 0; tmp < spaces.size(); ++tmp)
		ids[tmp] = spaces[tmp].lock()->GetId().Get(); // the id should have been allocated if it was originally missing
	return ids;
}

/*! Gets a space proxy 
 * \throws	ExceptionNotFound	no space has this id
 * \param[in]	the id of the space to get
 * \return	the space proxy
 */
AltoWrapper::Space AltoWrapper::Page::GetSpace(const Id &spaceid)
{
	AltoSpace &sp(page->GetSpace(spaceid)); // may throw
	String n(sp.GetName());
	n.ToLower();
	SBlock b(block->GetChild(SpaceKey(), n)); // may throw
	return AltoWrapper::Space(b, sp, lock, path);
}

/*! Gets the space proxy on the top margin
 * \throws	ExceptionNotFound	no top margin
 * \return	the space proxy on the top margin
 */
AltoWrapper::Space AltoWrapper::Page::GetTopMargin()
{
	std::weak_ptr<AltoSpace> sp(page->GetTopMargin());
	if (sp.expired())
		throw ExceptionNotFound(_("No top margin on this page."));
	SBlock b(block->GetChild(SpaceKey(), U"topmargin")); // may throw
	return AltoWrapper::Space(b, *sp.lock(), lock, path);
}

/*! Adds a top margin
 * \throws	crn::ExceptionLogic	the page already has a top margin
 * \param[in]	bbox	the bounding box of the magin
 * \return	a wrapper on the margin
 */
AltoWrapper::Space AltoWrapper::Page::AddTopMargin(const crn::Rect &bbox)
{
	AltoSpace &sp(page->AddTopMargin(lock->GetAlto()->CreateId(), bbox.GetLeft(), bbox.GetTop(), bbox.GetWidth(), bbox.GetHeight())); // may throw
	SBlock b(block->AddChildAbsolute(SpaceKey(), bbox, U"topmargin"));
	return AltoWrapper::Space(b, sp, lock, path);
}

/*! Gets the space proxy on the left margin
 * \throws	ExceptionNotFound	no left margin
 * \return	the space proxy on the left margin
 */
AltoWrapper::Space AltoWrapper::Page::GetLeftMargin()
{
	std::weak_ptr<AltoSpace> sp(page->GetLeftMargin());
	if (sp.expired())
		throw ExceptionNotFound(_("No left margin on this page."));
	SBlock b(block->GetChild(SpaceKey(), U"leftmargin")); // may throw
	return AltoWrapper::Space(b, *sp.lock(), lock, path);
}

/*! Adds a left margin
 * \throws	crn::ExceptionLogic	the page already has a left margin
 * \param[in]	bbox	the bounding box of the magin
 * \return	a wrapper on the margin
 */
AltoWrapper::Space AltoWrapper::Page::AddLeftMargin(const crn::Rect &bbox)
{
	AltoSpace &sp(page->AddLeftMargin(lock->GetAlto()->CreateId(), bbox.GetLeft(), bbox.GetTop(), bbox.GetWidth(), bbox.GetHeight())); // may throw
	SBlock b(block->AddChildAbsolute(SpaceKey(), bbox, U"leftmargin"));
	return AltoWrapper::Space(b, sp, lock, path);
}

/*! Gets the space proxy on the bottom margin
 * \throws	ExceptionNotFound	no bottom margin
 * \return	the space proxy on the bottom margin
 */
AltoWrapper::Space AltoWrapper::Page::GetBottomMargin()
{
	std::weak_ptr<AltoSpace> sp(page->GetBottomMargin());
	if (sp.expired())
		throw ExceptionNotFound(_("No bottom margin on this page."));
	SBlock b(block->GetChild(SpaceKey(), U"bottommargin")); // may throw
	return AltoWrapper::Space(b, *sp.lock(), lock, path);
}

/*! Adds a bottom margin
 * \throws	crn::ExceptionLogic	the page already has a bottom margin
 * \param[in]	bbox	the bounding box of the magin
 * \return	a wrapper on the margin
 */
AltoWrapper::Space AltoWrapper::Page::AddBottomMargin(const crn::Rect &bbox)
{
	AltoSpace &sp(page->AddBottomMargin(lock->GetAlto()->CreateId(), bbox.GetLeft(), bbox.GetTop(), bbox.GetWidth(), bbox.GetHeight())); // may throw
	SBlock b(block->AddChildAbsolute(SpaceKey(), bbox, U"bottommargin"));
	return AltoWrapper::Space(b, sp, lock, path);
}

/*! Gets the space proxy on the right margin
 * \throws	ExceptionNotFound	no right margin
 * \return	the space proxy on the right margin
 */
AltoWrapper::Space AltoWrapper::Page::GetRightMargin()
{
	std::weak_ptr<AltoSpace> sp(page->GetRightMargin());
	if (sp.expired())
		throw ExceptionNotFound(_("No right margin on this page."));
	SBlock b(block->GetChild(SpaceKey(), U"rightmargin")); // may throw
	return AltoWrapper::Space(b, *sp.lock(), lock, path);
}

/*! Adds a right margin
 * \throws	crn::ExceptionLogic	the page already has a right margin
 * \param[in]	bbox	the bounding box of the magin
 * \return	a wrapper on the margin
 */
AltoWrapper::Space AltoWrapper::Page::AddRightMargin(const crn::Rect &bbox)
{
	AltoSpace &sp(page->AddRightMargin(lock->GetAlto()->CreateId(), bbox.GetLeft(), bbox.GetTop(), bbox.GetWidth(), bbox.GetHeight())); // may throw
	SBlock b(block->AddChildAbsolute(SpaceKey(), bbox, U"rightmargin"));
	return AltoWrapper::Space(b, sp, lock, path);
}

/*! Gets the space proxy on the print space
 * \throws	ExceptionNotFound	no print space
 * \return	the space proxy on the print space
 */
AltoWrapper::Space AltoWrapper::Page::GetPrintSpace()
{
	std::weak_ptr<AltoSpace> sp(page->GetPrintSpace());
	if (sp.expired())
		throw ExceptionNotFound(_("No print space on this page."));
	SBlock b(block->GetChild(SpaceKey(), U"printspace")); // may throw
	return AltoWrapper::Space(b, *sp.lock(), lock, path);
}

/*! Adds a print space
 * \throws	crn::ExceptionLogic	the page already has a print space
 * \param[in]	bbox	the bounding box of the print space
 * \return	a wrapper on the print space
 */
AltoWrapper::Space AltoWrapper::Page::AddPrintSpace(const crn::Rect &bbox)
{
	AltoSpace &sp(page->AddPrintSpace(lock->GetAlto()->CreateId(), bbox.GetLeft(), bbox.GetTop(), bbox.GetWidth(), bbox.GetHeight())); // may throw
	SBlock b(block->AddChildAbsolute(SpaceKey(), bbox, U"printspace"));
	return AltoWrapper::Space(b, sp, lock, path);
}

/*! Removes a space
 * \throws	crn::ExceptionNotFound	space not found
 * \param[in]	sid	the space id
 */
void AltoWrapper::Page::RemoveSpace(const Id &sid)
{
	page->RemoveSpace(sid);
	block->RemoveChild(SpaceKey(), sid);
}

/*! Adds a copy of another page's content 
 * \param[in]	other	the page to import
 * \param[in]	crop	the bounding box of the page (can be an uninitialized rectangle for no cropping)
 */
/*
	 void AltoWrapper::Page::ImportContent(AltoWrapper::Page &other, const crn::Rect &crop)
	 {
	 try
	 {
	 crn::xml::AltoWrapper::Space sp(other.GetTopMargin());
	 crn::Rect sbbox(sp.GetBBox());
	 if (crop.IsValid())
	 sbbox &= crop;
	 if (sbbox.IsValid())
	 {
// TODO!!
}
}
catch (crn::ExceptionNotFound&) { } // no top margin

}
*/

///////////////////////////////////////////////////////////////////////////////////
// Space
///////////////////////////////////////////////////////////////////////////////////

/*! Sets the coordinates of the space. The coordinates are cropped to the page's bounding box. If the new bounding box is smaller than the previous one, the text blocks, text lines and words in the space are cropped.
 * \throws	crn::ExceptionInvalidArgument	uninitialized bounding box
 * \throws	crn::ExceptionDimension	bounding box is out of parent's bounding box
 * \throws	crn::ExceptionDomain	bounding box is too small to hold the content
 * \param[in]	r	the new bounding box of the space
 * \param[in]	erase_oob	shall the out of bounds contained elements be erased?
 */
void AltoWrapper::Space::SetBBox(const crn::Rect &r, bool erase_oob)
{
	// check inner elements
	const std::vector<Id> blocks(GetTextBlocks());
	for (const Id &bid : blocks)
	{
		TextBlock tb(GetTextBlock(bid));
		if ((tb.GetBBox() & r).GetArea() == 0)
		{
			if (erase_oob)
				RemoveBlock(bid);
			else
				throw crn::ExceptionDomain(_("Resizing the element would invalidate its content."));
		}
		if (!erase_oob)
		{
			const std::vector<Id> lines(tb.GetTextLines());
			for (const Id &lid : lines)
			{
				TextLine tl(tb.GetTextLine(lid));
				if ((tl.GetBBox() & r).GetArea() == 0)
					throw crn::ExceptionDomain(_("Resizing the element would invalidate its content."));
				const std::vector<Id> words(tl.GetWords());
				for (const Id &wid : words)
				{
					Word w(tl.GetWord(wid));
					if ((w.GetBBox() & r).GetArea() == 0)
						throw crn::ExceptionDomain(_("Resizing the element would invalidate its content."));
				}
			}
		}
	}
	block->SetAbsoluteBBox(r); // adjusts the coordinates of the words in the CRNBlock
	const crn::Rect crop(block->GetAbsoluteBBox());
	space->SetHPos(crop.GetLeft());
	space->SetVPos(crop.GetTop());
	space->SetWidth(crop.GetWidth());
	space->SetHeight(crop.GetHeight());
	for (const Id &bid : blocks)
	{
		TextBlock tb(GetTextBlock(bid));
		tb.SetBBox(tb.GetBBox(), erase_oob); // GetBBox() uses the CRNBlock's coordinates that are up to date
	} // TODO XXX resize the illustrations and other non-text blocks
}

/*! \return	the text styles */
std::vector<Alto::Styles::Text> AltoWrapper::Space::GetTextStyles()
{
	std::vector<Alto::Styles::Text> sty;
	const std::vector<Id> &sids(GetStyles());
	for (const Id &sid : sids)
	{
		try { sty.push_back(lock->GetAlto()->GetStyles().GetTextStyle(sid)); } catch (...) { }
	}
	return sty;
}

/*! \return	the paragraph styles */
std::vector<Alto::Styles::Paragraph> AltoWrapper::Space::GetParagraphStyles()
{
	std::vector<Alto::Styles::Paragraph> sty;
	const std::vector<Id> &sids(GetStyles());
	for (const Id &sid : sids)
	{
		try { sty.push_back(lock->GetAlto()->GetStyles().GetParagraphStyle(sid)); } catch (...) { }
	}
	return sty;
}

/*! \return	the ids of the text blocks in the page space */
std::vector<Id> AltoWrapper::Space::GetTextBlocks() const
{
	const std::vector<std::weak_ptr<AltoTextBlock> > tbs(space->GetTextBlocks());
	std::vector<Id> ids(tbs.size());
	for (size_t tmp = 0; tmp < tbs.size(); ++tmp)
		ids[tmp] = tbs[tmp].lock()->GetId();
	return ids;
}

/*! Gets a text block proxy
 * \throws	ExceptionNotFound	no text block with this id
 * \param[in]	id	the id of the block to retrieve
 * \return	a proxy on the text block
 */
AltoWrapper::TextBlock AltoWrapper::Space::GetTextBlock(const Id &id)
{
	AltoTextBlock &tb(space->GetTextBlock(id)); // may throw
	SBlock b(block->GetChild(TextBlockKey(), id)); // may throw
	return AltoWrapper::TextBlock(b, tb, lock, path);
}

/*! Adds a text block to the space
 * \param[in]	bbox	the bounding box of the text block
 * \return	a wrapper on the text block
 */
AltoWrapper::TextBlock AltoWrapper::Space::AddTextBlock(const crn::Rect &bbox)
{
	AltoTextBlock &tb(space->AddTextBlock(lock->GetAlto()->CreateId(), bbox.GetLeft(), bbox.GetTop(), bbox.GetWidth(), bbox.GetHeight()));
	SBlock b(block->AddChildAbsolute(TextBlockKey(), bbox, tb.GetId()));
	return AltoWrapper::TextBlock(b, tb, lock, path);
}

/*! Adds a text block to the space
 * \throws	crn::ExceptionNotFound cannot find previous block
 * \param[in]	pred	id of the previous block
 * \param[in]	bbox	the bounding box of the text block
 * \return	a wrapper on the text block
 */
AltoWrapper::TextBlock AltoWrapper::Space::AddTextBlockAfter(const Id &pred, const crn::Rect &bbox)
{
	AltoTextBlock &tb(space->AddTextBlockAfter(pred, lock->GetAlto()->CreateId(), bbox.GetLeft(), bbox.GetTop(), bbox.GetWidth(), bbox.GetHeight())); // may throw
	SBlock b(block->AddChildAbsolute(TextBlockKey(), bbox, tb.GetId()));
	return AltoWrapper::TextBlock(b, tb, lock, path);
}

/*! Adds a text block to the space
 * \throws	crn::ExceptionNotFound cannot find next block
 * \param[in]	next	id of the next block
 * \param[in]	bbox	the bounding box of the text block
 * \return	a wrapper on the text block
 */
AltoWrapper::TextBlock AltoWrapper::Space::AddTextBlockBefore(const Id &next, const crn::Rect &bbox)
{
	AltoTextBlock &tb(space->AddTextBlockBefore(next, lock->GetAlto()->CreateId(), bbox.GetLeft(), bbox.GetTop(), bbox.GetWidth(), bbox.GetHeight())); // may throw
	SBlock b(block->AddChildAbsolute(TextBlockKey(), bbox, tb.GetId()));
	return AltoWrapper::TextBlock(b, tb, lock, path);
}

/*! Removes a block
 * \throws	crn::ExceptionNotFound	block not found
 * \param[in]	bid	the block id
 */
void AltoWrapper::Space::RemoveBlock(const Id &bid)
{
	space->RemoveBlock(bid);
	block->RemoveChild(TextBlockKey(), bid);
}

///////////////////////////////////////////////////////////////////////////////////
// TextBlock
///////////////////////////////////////////////////////////////////////////////////

/*! Sets the coordinates of the text block. The coordinates are cropped to the space's bounding box. If the new bounding box is smaller than the previous one, the text lines and words in the text block are cropped.
 * \throws	crn::ExceptionInvalidArgument	uninitialized bounding box
 * \throws	crn::ExceptionDimension	bounding box is out of parent's bounding box
 * \throws	crn::ExceptionDomain	bounding box is too small to hold the content
 * \param[in]	r	the new bounding box of the space
 * \param[in]	erase_oob	shall the out of bounds contained elements be erased?
 */
void AltoWrapper::TextBlock::SetBBox(const crn::Rect &r, bool erase_oob)
{
	// check inner elements
	const std::vector<Id> lines(GetTextLines());
	for (const Id &lid : lines)
	{
		TextLine tl(GetTextLine(lid));
		if ((tl.GetBBox() & r).GetArea() == 0)
		{
			if (erase_oob)
				RemoveTextLine(lid);
			else
				throw crn::ExceptionDomain(_("Resizing the element would invalidate its content."));
		}
		if (!erase_oob)
		{
			const std::vector<Id> words(tl.GetWords());
			for (const Id &wid : words)
			{
				Word w(tl.GetWord(wid));
				if ((w.GetBBox() & r).GetArea() == 0)
					throw crn::ExceptionDomain(_("Resizing the element would invalidate its content."));
			}
		}
	}
	block->SetAbsoluteBBox(r); // adjusts the coordinates of the words in the CRNBlock
	const crn::Rect crop(block->GetAbsoluteBBox());
	textblock->SetHPos(crop.GetLeft());
	textblock->SetVPos(crop.GetTop());
	textblock->SetWidth(crop.GetWidth());
	textblock->SetHeight(crop.GetHeight());
	for (const Id &lid : lines)
	{
		TextLine tl(GetTextLine(lid));
		tl.SetBBox(tl.GetBBox(), erase_oob); // GetBBox() uses the CRNBlock's coordinates that are up to date
	}
}

/*! \return	the text styles */
std::vector<Alto::Styles::Text> AltoWrapper::TextBlock::GetTextStyles()
{
	std::vector<Alto::Styles::Text> sty;
	const std::vector<Id> &sids(GetStyles());
	for (const Id &sid : sids)
	{
		try { sty.push_back(lock->GetAlto()->GetStyles().GetTextStyle(sid)); } catch (...) { }
	}
	return sty;
}

/*! \return	the paragraph styles */
std::vector<Alto::Styles::Paragraph> AltoWrapper::TextBlock::GetParagraphStyles()
{
	std::vector<Alto::Styles::Paragraph> sty;
	const std::vector<Id> &sids(GetStyles());
	for (const Id &sid : sids)
	{
		try { sty.push_back(lock->GetAlto()->GetStyles().GetParagraphStyle(sid)); } catch (...) { }
	}
	return sty;
}

/*! \return	the ids of the text lines in the block */
std::vector<Id> AltoWrapper::TextBlock::GetTextLines() const
{
	const std::vector<std::weak_ptr<AltoTextLine> > tls(textblock->GetTextLines());
	std::vector<Id> ids(tls.size());
	for (size_t tmp = 0; tmp < tls.size(); ++tmp)
		ids[tmp] = tls[tmp].lock()->GetId();
	return ids;
}

/*! Gets a text line proxy
 * \throws	ExceptionNotFound	no text line with this id
 * \param[in]	id	the id of the line to retrieve
 * \return	a proxy on the text line
 */
AltoWrapper::TextLine AltoWrapper::TextBlock::GetTextLine(const Id &id)
{
	AltoTextLine &tl(textblock->GetTextLine(id)); // may throw
	SBlock b(block->GetChild(TextLineKey(), id)); // may throw
	return AltoWrapper::TextLine(b, tl, lock, path);
}

/*! Adds a text line to the block
 * \param[in]	bbox	the bounding box of the text line
 * \return	a wrapper on the text line
 */
AltoWrapper::TextLine AltoWrapper::TextBlock::AddTextLine(const crn::Rect &bbox)
{
	AltoTextLine &tl(textblock->AddTextLine(lock->GetAlto()->CreateId(), bbox.GetLeft(), bbox.GetTop(), bbox.GetWidth(), bbox.GetHeight()));
	SBlock b(block->AddChildAbsolute(TextLineKey(), bbox, tl.GetId()));
	return AltoWrapper::TextLine(b, tl, lock, path);
}

/*! Adds a text line to the block
 * \throws	crn::ExceptionNotFound cannot find previous line
 * \param[in]	pred	id of the previous line
 * \param[in]	bbox	the bounding box of the text line
 * \return	a wrapper on the text line
 */
AltoWrapper::TextLine AltoWrapper::TextBlock::AddTextLineAfter(const Id &pred, const crn::Rect &bbox)
{
	AltoTextLine &tl(textblock->AddTextLineAfter(pred, lock->GetAlto()->CreateId(), bbox.GetLeft(), bbox.GetTop(), bbox.GetWidth(), bbox.GetHeight()));
	SBlock b(block->AddChildAbsolute(TextLineKey(), bbox, tl.GetId()));
	return AltoWrapper::TextLine(b, tl, lock, path);
}

/*! Adds a text line to the block
 * \throws	crn::ExceptionNotFound cannot find next line
 * \param[in]	next	id of the next line
 * \param[in]	bbox	the bounding box of the text line
 * \return	a wrapper on the text line
 */
AltoWrapper::TextLine AltoWrapper::TextBlock::AddTextLineBefore(const Id &next, const crn::Rect &bbox)
{
	AltoTextLine &tl(textblock->AddTextLineBefore(next, lock->GetAlto()->CreateId(), bbox.GetLeft(), bbox.GetTop(), bbox.GetWidth(), bbox.GetHeight()));
	SBlock b(block->AddChildAbsolute(TextLineKey(), bbox, tl.GetId()));
	return AltoWrapper::TextLine(b, tl, lock, path);
}

/*! Removes a text line
 * \throws	crn::ExceptionNotFound	text line not found
 * \param[in]	tid	the text line id
 */
void AltoWrapper::TextBlock::RemoveTextLine(const Id &tid)
{
	textblock->RemoveTextLine(tid);
	block->RemoveChild(TextLineKey(), tid);
}

///////////////////////////////////////////////////////////////////////////////////
// TextLine
///////////////////////////////////////////////////////////////////////////////////

/*! \return	the text styles */
std::vector<Alto::Styles::Text> AltoWrapper::TextLine::GetTextStyles()
{
	std::vector<Alto::Styles::Text> sty;
	const std::vector<Id> &sids(GetStyles());
	for (const Id &sid : sids)
	{
		try { sty.push_back(lock->GetAlto()->GetStyles().GetTextStyle(sid)); } catch (...) { }
	}
	return sty;
}

/*! \return	the paragraph styles */
std::vector<Alto::Styles::Paragraph> AltoWrapper::TextLine::GetParagraphStyles()
{
	std::vector<Alto::Styles::Paragraph> sty;
	const std::vector<Id> &sids(GetStyles());
	for (const Id &sid : sids)
	{
		try { sty.push_back(lock->GetAlto()->GetStyles().GetParagraphStyle(sid)); } catch (...) { }
	}
	return sty;
}

/*! Sets the coordinates of the text line. The coordinates are cropped to the text block's bounding box. If the new bounding box is smaller than the previous one, the words in the text line are cropped.
 * \throws	crn::ExceptionInvalidArgument	uninitialized bounding box
 * \throws	crn::ExceptionDimension	bounding box is out of parent's bounding box
 * \throws	crn::ExceptionDomain	bounding box is too small to hold the content
 * \param[in]	r	the new bounding box of the space
 * \param[in]	erase_oob	shall the out of bounds contained elements be erased?
 */
void AltoWrapper::TextLine::SetBBox(const crn::Rect &r, bool erase_oob)
{
	// check inner elements
	const std::vector<Id> words(GetWords());
	for (const Id &wid : words)
	{
		Word w(GetWord(wid));
		if ((w.GetBBox() & r).GetArea() == 0)
		{
			if (erase_oob)
				RemoveWord(wid);
			else
				throw crn::ExceptionDomain(_("Resizing the element would invalidate its content."));
		}
	}
	block->SetAbsoluteBBox(r); // adjusts the coordinates of the words in the CRNBlock
	const crn::Rect crop(block->GetAbsoluteBBox());
	textline->SetHPos(crop.GetLeft());
	textline->SetVPos(crop.GetTop());
	textline->SetWidth(crop.GetWidth());
	textline->SetHeight(crop.GetHeight());
	for (const Id &wid : words)
	{
		Word w(GetWord(wid));
		w.SetBBox(w.GetBBox()); // GetBBox() uses the CRNBlock's coordinates that are up to date
	}
}

/*! \return	the ids of the words in the line */
std::vector<Id> AltoWrapper::TextLine::GetWords() const
{
	const std::vector<std::weak_ptr<AltoWord> > words(textline->GetWords());
	std::vector<Id> ids(words.size());
	for (size_t tmp = 0; tmp < words.size(); ++tmp)
		ids[tmp] = words[tmp].lock()->GetId().Get();
	return ids;
}

/*! Gets a word proxy
 * \throws	ExceptionNotFound	no word with this id
 * \param[in]	id	the id of the word to retrieve
 * \return	a proxy on the word
 */
AltoWrapper::Word AltoWrapper::TextLine::GetWord(const Id &id)
{
	AltoWord &word(textline->GetWord(id)); // may throw
	SBlock b(block->GetChild(WordKey(), id)); // may throw
	return AltoWrapper::Word(b, word, lock, path);
}

/*! Adds a word to the line
 * \param[in]	bbox	the bounding box of the word
 * \return	a wrapper on the word
 */
AltoWrapper::Word AltoWrapper::TextLine::AddWord(const StringUTF8 &text, const crn::Rect &bbox)
{
	AltoWord &w(textline->AddWord(lock->GetAlto()->CreateId(), text, bbox.GetLeft(), bbox.GetTop(), bbox.GetWidth(), bbox.GetHeight()));
	SBlock b(block->AddChildAbsolute(WordKey(), bbox, w.GetId().Get()));
	return AltoWrapper::Word(b, w, lock, path);
}

/*! Adds a word to the line
 * \throws	crn::ExceptionNotFound cannot find previous word
 * \param[in]	pred	id of the previous word
 * \param[in]	bbox	the bounding box of the word
 * \return	a wrapper on the word
 */
AltoWrapper::Word AltoWrapper::TextLine::AddWordAfter(const Id &pred, const StringUTF8 &text, const crn::Rect &bbox)
{
	AltoWord &w(textline->AddWordAfter(pred, lock->GetAlto()->CreateId(), text, bbox.GetLeft(), bbox.GetTop(), bbox.GetWidth(), bbox.GetHeight()));
	SBlock b(block->AddChildAbsolute(WordKey(), bbox, w.GetId().Get()));
	return AltoWrapper::Word(b, w, lock, path);
}

/*! Adds a word to the line
 * \throws	crn::ExceptionNotFound cannot find next word
 * \param[in]	next	id of the next word
 * \param[in]	bbox	the bounding box of the word
 * \return	a wrapper on the word
 */
AltoWrapper::Word AltoWrapper::TextLine::AddWordBefore(const Id &next, const StringUTF8 &text, const crn::Rect &bbox)
{
	AltoWord &w(textline->AddWordBefore(next, lock->GetAlto()->CreateId(), text, bbox.GetLeft(), bbox.GetTop(), bbox.GetWidth(), bbox.GetHeight()));
	SBlock b(block->AddChildAbsolute(WordKey(), bbox, w.GetId().Get()));
	return AltoWrapper::Word(b, w, lock, path);
}

/*! Removes a word
 * \throws	crn::ExceptionNotFound	word not found
 * \param[in]	wid	the word id
 */
void AltoWrapper::TextLine::RemoveWord(const Id &wid)
{
	textline->RemoveWord(wid);
	block->RemoveChild(WordKey(), wid);
}

///////////////////////////////////////////////////////////////////////////////////
// Word
///////////////////////////////////////////////////////////////////////////////////

/*! \return	the text styles */
std::vector<Alto::Styles::Text> AltoWrapper::Word::GetTextStyles()
{
	std::vector<Alto::Styles::Text> sty;
	const std::vector<Id> &sids(GetStyles());
	for (const Id &sid : sids)
	{
		try { sty.push_back(lock->GetAlto()->GetStyles().GetTextStyle(sid)); } catch (...) { }
	}
	return sty;
}

/*! \return	the paragraph styles */
std::vector<Alto::Styles::Paragraph> AltoWrapper::Word::GetParagraphStyles()
{
	std::vector<Alto::Styles::Paragraph> sty;
	const std::vector<Id> &sids(GetStyles());
	for (const Id &sid : sids)
	{
		try { sty.push_back(lock->GetAlto()->GetStyles().GetParagraphStyle(sid)); } catch (...) { }
	}
	return sty;
}

/*! Sets the coordinates of the word. The coordinates are cropped to the line's bounding box.
 * \throws	crn::ExceptionInvalidArgument	uninitialized bounding box
 * \throws	crn::ExceptionDimension	bounding box is out of parent's bounding box
 * \param[in]	r	the new bounding box of the word
 */
void AltoWrapper::Word::SetBBox(const crn::Rect &r)
{
	block->SetAbsoluteBBox(r);
	const crn::Rect crop(block->GetAbsoluteBBox());
	word->SetHPos(crop.GetLeft());
	word->SetVPos(crop.GetTop());
	word->SetWidth(crop.GetWidth());
	word->SetHeight(crop.GetHeight());
}

