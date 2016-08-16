/* Copyright 2006-2016 Yann LEYDIER, INSA-Lyon, ENS-Lyon
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
 * file: CRNDocument.cpp
 * \author Yann LEYDIER
 */

#include <CRNi18n.h>
#include <CRNDocument.h>
#include <CRNException.h>
#include <CRNConfig.h>
#include <CRNUtils/CRNAtScopeExit.h>
#include <CRNUtils/CRNProgress.h>
#include <CRNUtils/CRNXml.h>
#include <CRNIO/CRNIO.h>
#ifdef CRN_USING_HARU
#	include <CRNUtils/CRNPDF.h>
#endif

using namespace crn;
using namespace crn::literals;

const Path Document::thumbdir("/thumbs/");
size_t Document::thumbWidth = 70; // almost A4
size_t Document::thumbHeight = 100;

/*****************************************************************************/
/*!
 * Default constructor
 *
 */
Document::Document():Savable(U""),
	basename(""),
	author(U""),
	date(U"")
{
}

/*****************************************************************************/
/*!
 * Destructor
 *
 */
Document::~Document()
{
}

/*****************************************************************************/
/*!
 * Adds a new image.
 *
 * \throws	ExceptionInvalidArgument	null filename
 *
 * \param[in]		fname	The filename of the image to add to the views.
 * \return	the id of the new view
 */
String Document::AddView(const Path &fname)
{
	if (!fname)
		throw ExceptionInvalidArgument(StringUTF8("void Document::AddView(const Path &fname): ") + _("null filename."));
	String id(createNewId());
	addView(fname, id);
	return id;
}

/*****************************************************************************/
/*!
 * Adds a new image.
 *
 * \param[in]		fname	The filename of the image to add to the views.
 * \param[in]  id  the id of the new view
 */
void Document::addView(const Path &fname, const String &id)
{
	views.push_back(view(fname, id));
}

/*****************************************************************************/
/*!
 * Inserts a new image.
 *
 * \throws	ExceptionInvalidArgument	null filename
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]  fname  The filename of the image to add to the views.
 * \param[in]  pos  the position where the image will be added
 * \return	the id of the new view
 */
String Document::InsertView(const Path &fname, size_t pos)
{
	if (!fname)
		throw ExceptionInvalidArgument(StringUTF8("void Document::InsertView(const Path &fname, size_t pos): ") + _("null filename."));
	if (pos > views.size())
		throw ExceptionDomain(StringUTF8("void Document::InsertView(const Path &fname, size_t pos): ") + _("index out of bounds."));
		
	if (pos == views.size())
		return AddView(fname);
	else
	{
		String id(createNewId());
		views.insert(views.begin() + pos, view(fname, id));
		return id;
	}
}

/*****************************************************************************/
/*!
 * Removes a view.
 *
 * If multiple views have the same filename, only the first is removed.
 *
 * \throws	ExceptionNotFound	filename not found
 *
 * \param[in]		fname	The filename of the image to remove from the views.
 */
void Document::RemoveView(const Path &fname)
{
	view v(fname);
	auto it(std::find(views.begin(), views.end(), v));
	if (it == views.end())
		throw ExceptionNotFound(StringUTF8("void Document::RemoveView(const Path &fname): ") + _("filename not found."));
	RemoveView(it - views.begin());
}

/*****************************************************************************/
/*!
 * Removes a view.
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]		num	The index of the view to remove.
 */
void Document::RemoveView(size_t num)
{
	if (num >= views.size())
		throw ExceptionDomain(StringUTF8("void Document::RemoveView(size_t num): ") + _("index out of bounds."));
	// remove thumbnail
	Path thumbname(basename + thumbdir + views[num].id);
	try
	{
		IO::Rm(thumbname);
	} catch (...) { }
	// remove xml
	Path xmlname(basename + "/" + views[num].id + ".xml");
	try
	{
		IO::Rm(xmlname);
	} catch (...) { }
	// remove block
	views.erase(views.begin() + num);
}
		
/*****************************************************************************/
/*!
 * Removes a view.
 *
 * \throws	ExceptionNotFound	id not found
 *
 * \param[in]		id	The id of the view to remove.
 */
void Document::RemoveView(const String &id)
{
	RemoveView(GetViewIndex(id));
}
		
/*****************************************************************************/
/*!
 * Returns a pointer to a view.
 *
 * Its counter is incremented.
 *
 * \throws	ExceptionDomain	index out of bounds
 * \throws	ExceptionIO	XML file exists but cannot be accessed or has invalid structure
 * \throws	ExceptionRuntime	the XML file does not fit the block's image
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \param[in]		num	The index of the view.
 * \return	A pointer to a block
 */
SBlock Document::GetView(size_t num) const
{
	if (num >= views.size())
	{
		throw ExceptionDomain(StringUTF8("SBlock Document::GetView(size_t num) const: ") + _("Index out of bounds."));
	}
	if (views[num].ptr.expired())
	{
		Path s;
		if (basename.IsNotEmpty())
		{
			s += basename + Path::Separator();
			s += Path(views[num].id) + ".xml";
		}
		SBlock b(Block::New(views[num].filename, s, views[num].filename));
		views[num].ptr = b;
		return b;
	}
	return views[num].ptr.lock();
}

/*! 
 * Returns a pointer to a view
 *
 * \throws	ExceptionNotFound	id not found
 * \throws	ExceptionIO	XML file exists but cannot be accessed or has invalid structure
 * \throws	ExceptionRuntime	the XML file does not fit the block's image
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \param[in]  id  the id of the view
 * \return  a pointer to the view's block
 */
SBlock Document::GetView(const String &id) const
{
	return GetView(GetViewIndex(id));
}

/*! 
 * Returns a pointer to a view
 *
 * \throws	ExceptionNotFound	filename not found
 * \throws	ExceptionIO	XML file exists but cannot be accessed or has invalid structure
 * \throws	ExceptionRuntime	the XML file does not fit the block's image
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \param[in]  fname  the file name of the view
 * \return  a pointer to the view's block
 */
SBlock Document::GetView(const Path &fname) const
{
	return GetView(GetViewIndex(fname));
}

/*! 
 * Returns the index of a view
 *
 * \throws	ExceptionNotFound	id not found
 *
 * \param[in]  id  the id the view
 * \return  the index of the view
 */
size_t Document::GetViewIndex(const String &id) const
{
	for (size_t tmp = 0; tmp < views.size(); ++tmp)
	{
		if (views[tmp].id == id)
			return tmp;
	}
	throw ExceptionNotFound(StringUTF8("size_t Document::GetViewIndex(const String &id) const: ") + _("id not found."));
}

/*! 
 * Returns the index of a view
 *
 * \throws	ExceptionNotFound	filename not found
 *
 * \param[in]  fname  the file name the view
 * \return  the index of the view
 */
size_t Document::GetViewIndex(const Path &fname) const
{
	for (size_t tmp = 0; tmp < views.size(); ++tmp)
	{
		if (views[tmp].filename == fname)
			return tmp;
	}
	throw ExceptionNotFound(StringUTF8("size_t Document::GetViewIndex(const Path &fname) const: ") + _("filename not found."));
}

/*! Returns the id of a view
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]  num  the index of the view
 * \return  the id of the view
 */
String Document::GetViewId(size_t num) const
{
	if (num >= views.size())
		throw ExceptionDomain(StringUTF8("const String Document::GetViewId(size_t num) const: ") + _("index out of bounds."));
		
	return views[num].id;
}

/*! Returns the id of a view
 *
 * \throws	ExceptionNotFound	filename not found
 *
 * \param[in]  fname  the file name of the view
 * \return  the id of the view
 */
String Document::GetViewId(const Path &fname) const
{
	for (auto & elem : views)
	{
		if (elem.filename == fname)
			return elem.id;
	}
	throw ExceptionNotFound(StringUTF8("const String Document::GetViewId(const Path &fname) const: ") + _("filename not found."));
}

/*! Returns the filename of a view
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]  num  the index of the view
 * \return  the filename of the view
 */
Path Document::GetViewFilename(size_t num) const
{
	if (num >= views.size())
		throw ExceptionDomain(StringUTF8("const Path Document::GetViewFilename(size_t num) const: ") + _("index out of bounds."));

	return views[num].filename;
}

/*! Returns the filename of a view
 *
 * \throws	ExceptionNotFound	id not found
 *
 * \param[in]  id  the id of the view
 * \return  the filename of the view
 */
Path Document::GetViewFilename(const String &id) const
{
	return GetViewFilename(GetViewIndex(id));
}

/*! Reorders the views
 *
 * \throws	ExceptionDimension	changeset is of wrong size
 * \throws	ExceptionLogic	changeset contains duplicates
 * \throws	ExceptionDomain	changeset contains values out of bounds
 *
 * \param[in]  from  a vector containing the previous index of each view
 *
 * \return  true if success, false else
 */
void Document::ReorderViewsFrom(const std::vector<size_t> &from)
{
	if (from.size() != views.size())
	{
		throw ExceptionDimension(StringUTF8("bool Document::ReorderViewsFrom(const std::vector<size_t> &from): ") + 
				_("bad changeset size."));
	}
	std::set<size_t> cont;
	for (size_t i : from)
	{
		cont.insert(i);
	}
	if (cont.size() != views.size())
	{
		throw ExceptionLogic(StringUTF8("bool Document::ReorderViewsFrom(const std::vector<size_t> &from): ") + 
				_("changeset contains duplicates."));
	}
	if ((*cont.begin() != 0) || (*cont.rbegin() != views.size() - 1))
	{
		throw ExceptionDomain(StringUTF8("bool Document::ReorderViewsFrom(const std::vector<size_t> &from): ") + 
				_("changeset contains values out of bounds."));
	}
	std::vector<view> newviews;
	for (size_t i : from)
	{
		newviews.push_back(views[i]);
	}
	views.swap(newviews);
}

/*! Reorders the views
 *
 * \throws	ExceptionDimension	changeset is of wrong size
 * \throws	ExceptionLogic	changeset contains duplicates
 * \throws	ExceptionDomain	changeset contains values out of bounds
 *
 * \param[in]  to  a vector containing the new index of each view
 *
 * \return  true if success, false else
 */
void Document::ReorderViewsTo(const std::vector<size_t> &to)
{
	if (to.size() != views.size())
	{
		throw ExceptionDimension(StringUTF8("bool Document::ReorderViewsTo(const std::vector<size_t> &to): ") + 
				_("bad changeset size."));
	}
	std::set<size_t> cont;
	for (size_t i : to)
	{
		cont.insert(i);
	}
	if (cont.size() != views.size())
	{
		throw ExceptionLogic(StringUTF8("bool Document::ReorderViewsTo(const std::vector<size_t> &to): ") + 
				_("changeset contains duplicates."));
	}
	if ((*cont.begin() != 0) || (*cont.rbegin() != views.size() - 1))
	{
		throw ExceptionDomain(StringUTF8("bool Document::ReorderViewsTo(const std::vector<size_t> &to): ") + 
				_("changeset contains values out of bounds."));
	}
	std::vector<view> newviews(views.size());
	for (size_t tmp = 0; tmp < to.size(); ++tmp)
	{
		newviews[to[tmp]] = views[tmp];
	}
	views.swap(newviews);
}

/*! Removes all views and unsets all data 
 */
void Document::Clear()
{
	views.clear();
	SetName(U"");
	SetAuthor(U"");
	SetDate(U"");
	ClearUserData();
}

/*****************************************************************************/
/*!
 * Loads a document configuration file.
 *
 * \throws	ExceptionIO	cannot read file
 * \throws	ExceptionNotFound	empty file
 * \throws	ExceptionRuntime	not a document file
 * \throws	ExceptionNotFound	not base directory found
 * \throws	ExceptionIO	cannot create directory
 *
 * \param[in]		fname	The filename of the document configuration file.
 */
void Document::load(const Path &fname)
{
	xml::Document doc(fname); // may throw
	xml::Element root(doc.GetRoot()); // may throw
	if (root.GetName() != "Document")
	{
		throw ExceptionRuntime(StringUTF8("bool Document::load(const String &fname): ") + 
				_("Not a Document file."));
	}

	std::multimap<int, std::pair<Path, String> > xmlviews;
	xml::Element vi(root.GetFirstChildElement("View"));
	while (vi)
	{
		Path fname(vi.GetAttribute<StringUTF8>("fname", false)); // may throw
		int num ;
		try { num = vi.GetAttribute<int>("num", false); } catch (...) { num = int(xmlviews.size()); } // if no index found, push back
		String id = vi.GetAttribute<StringUTF8>("id");
		if (id.IsEmpty()) // if no id found, use index
			id = num;
		xmlviews.insert(std::make_pair(num, std::make_pair(fname, id)));
		vi = vi.GetNextSiblingElement("View");
	}

	StringUTF8 bn = root.GetAttribute<StringUTF8>("basename", false); // may throw
	if (bn.IsNotEmpty())
	{
		basename = bn;
		if (!IO::Access(bn, IO::EXISTS))
		{
			IO::Mkdir(bn); // may throw
		}
	}
	bn = root.GetAttribute<StringUTF8>("author");
	if (bn.IsNotEmpty())
		author = bn;
	bn = root.GetAttribute<StringUTF8>("date");
	if (bn.IsNotEmpty())
		date = bn;

	views.clear();
	for (auto & xmlview : xmlviews)
	{
		addView(xmlview.second.first, xmlview.second.second);
	}
	/*
	xml::Element vi(root.GetFirstChildElement("View"));
	int cnt = 0;
	while (vi)
	{
		bn = vi.GetAttribute<StringUTF8>("fname");
		if (bn.IsNotEmpty())
		{
			const StringUTF8 id = vi.GetAttribute<StringUTF8>("id");
			if (id.IsNotEmpty())
				addView(bn, id);
			else // if no id was found, create one using the index
				addView(bn, String(cnt));
		}
		cnt += 1;
		vi = vi.GetNextSiblingElement("View");
	}
	*/
	deserialize_internal_data(root);
}

/*! 
 * Returns the path of the thumbnails
 * \return the path of the thumbnails
 */
Path Document::GetThumbnailPath() const
{
	return basename + thumbdir;
}

/*! Creates a thumbnail image from an image filename
 *
 * \throws	ExceptionInvalidArgument	fname is empty
 * \throws	ExceptionIO	the image could not be loaded (file not found or invalid image format)
 *
 * \param[in]  imagename  the name of the image to scale
 * \return  the new thumbnail image
 */
UImage Document::createThumbnail(const Path &imagename) const
{
	UImage img = NewImageFromFile(imagename);
	// scale the image
	size_t nw, nh;
	nh = img->GetHeight() * thumbWidth / img->GetWidth();
	if (nh <= thumbHeight)
	{
		nw = thumbWidth;
	}
	else
	{
		nw = img->GetWidth() * thumbHeight / img->GetHeight();
		nh = thumbHeight;
	}
	img->ScaleToSize(nw, nh);
	return std::forward<UImage>(img);
}

/*! Returns a thumbnail of a view (cached)
 *
 * \throws	ExceptionDomain	index out of bounds
 * \throws	ExceptionUninitialized	the document was never saved
 * \throws	ExceptionIO	the image could not be loaded (file not found or invalid image format)
 *
 * \param[in]  index  the index of the view
 * \param[in]  refresh  shall the thumbnail be recomputed?
 * \return  a thumbnail image
 */
UImage Document::GetThumbnail(size_t index, bool refresh) const
{
	Path thumbname(GetThumbnailFilename(index, refresh));
	return NewImageFromFile(thumbname);
}

/*! Returns a thumbnail of a view (cached)
 *
 * \throws	ExceptionNotFound	id not found
 * \throws	ExceptionUninitialized	the document was never saved
 * \throws	ExceptionIO	the image could not be loaded (file not found or invalid image format)
 *
 * \param[in]  id  the id of the view
 * \param[in]  refresh  shall the thumbnail be recomputed?
 *
 * \return  a thumbnail image
 */
UImage Document::GetThumbnail(const String &id, bool refresh) const
{
	return GetThumbnail(GetViewIndex(id), refresh);
}

/*! Returns the filename of a thumbnail of a view (cached)
 *
 * \throws	ExceptionDomain	index out of bounds
 * \throws	ExceptionUninitialized	the document was never saved
 * \throws	ExceptionIO	the image could not be loaded (file not found or invalid image format)
 * \throws	ExceptionIO	cannot create directory
 *
 * \param[in]  index  the index of the view
 * \param[in]  refresh  shall the thumbnail be recomputed?
 *
 * \return  the filename of a thumbnail image
 */
Path Document::GetThumbnailFilename(size_t index, bool refresh) const
{
	if (index >= GetNbViews())
		throw ExceptionDomain(StringUTF8("const Path Document::GetThumbnailFilename(size_t index, bool refresh) const: ") + _("index out of bounds."));

	if (!basename)
	{ // the document was never saved, so no thumbnail can be cached
		throw ExceptionUninitialized(StringUTF8("const Path Document::GetThumbnailFilename(size_t index, bool refresh) const: ") + _("the document was never saved."));
	}

	if (!IO::Access(basename + thumbdir, IO::EXISTS))
	{ // if the thumb directory does not exist, create it
		IO::Mkdir(basename + thumbdir);
	}

	Path thumbname(basename + thumbdir + GetViewId(index));
	if (!IO::Access(thumbname, IO::EXISTS) || refresh)
	{ // compute the thumbnail
		UImage img(createThumbnail(GetViewFilename(index)));
		// scale the image
		img->SavePNG(thumbname);
	}
	return thumbname;
}

/*! Returns the filename of a thumbnail of a view (cached)
 *
 * \throws	ExceptionNotFound	id not found
 * \throws	ExceptionUninitialized	the document was never saved
 * \throws	ExceptionIO	the image could not be loaded (file not found or invalid image format)
 *
 * \param[in]  id  the id of the view
 * \param[in]  refresh  shall the thumbnail be recomputed?
 *
 * \return  the filename of a thumbnail image
 */
Path Document::GetThumbnailFilename(const String &id, bool refresh) const
{
	return GetThumbnailFilename(GetViewIndex(id), refresh);
}

/*****************************************************************************/
/*!
 * Saves a document configuration file.
 *
 * \throws	ExceptionIO	cannot create directory
 * \throws	ExceptionIO	cannot write file
 *
 * \param[in]		fname	The filename of the document configuration file.
 */
void Document::save(const Path &fname)
{
	size_t namepos = fname.BackwardFindAnyOf("."); // strip extension if any
	if (namepos == String::NPos())
		namepos = 0;
	basename = Path(fname.SubString(0, namepos)) + "_data"; // append "_data"
	if (!IO::Access(basename, IO::EXISTS))
	{
		IO::Mkdir(basename); // may throw
	}

	xml::Document doc;
	doc.PushBackComment("libcrn Document file");
	xml::Element root(doc.PushBackElement("Document"));
	root.SetAttribute("basename", basename.CStr());
	root.SetAttribute("author", author.CStr());
	root.SetAttribute("date", date.CStr());

	// save views
	for (size_t tmp = 0; tmp < views.size(); tmp++)
	{
		xml::Element el(root.PushBackElement("View"));
		el.SetAttribute("fname", views[tmp].filename.CStr());
		el.SetAttribute("id", views[tmp].id.CStr());
		el.SetAttribute("num", int(tmp));
	}

	serialize_internal_data(root);

	doc.Save(fname); // may throw
}

/*! 
 * Returns the default directory where the documents are saved
 *
 * \return	the default directory where the documents are saved
 */
Path Document::GetDefaultDirName()
{
	static const auto dirname = Config::GetTopDataPath() / "documents";
	if (!IO::Access(dirname, IO::EXISTS))
	{
		try { IO::Mkdir(dirname); }
		catch (...) {}
	}
	return dirname;
}

/*****************************************************************************/
/*!
 * Completes a non-absolute filename
 *
 * \param[in]	fn	The name to complete
 * \return	The absolute filename
 */
Path Document::completeFilename(const Path &fn) const
{
	return GetDefaultDirName() / fn;
}

/*! 
 * Gets the list of the image files of the document
 *
 * \return  a vector containing the file names
 */
std::vector<Path> Document::GetFilenames() const
{
	std::vector<Path> flist;
	for (const view &v : views)
		flist.push_back(v.filename);
	return flist;
}

/*! 
 * Gets the list of the view ids of the document
 *
 * \return  a vector containing the view ids
 */
std::vector<String> Document::GetViewIds() const
{
	std::vector<String> flist;
	for (const view &v : views)
		flist.push_back(v.id);
	return flist;
}

/*! 
 * Creates a new unique id for views
 * \return  a new unique id
 */
String Document::createNewId() const
{
	// create a temporary file name (in std C)
	String id(String::CreateUniqueId());
	// check if the id is unique
	for (const view &v : views)
	{
		if (v.id == id)
			return createNewId();
	}
	return id;
}

#ifdef CRN_USING_HARU
/*! 
 * Exports the views to a PDF file
 *
 * \throws	ExceptionRuntime	error in libharu
 * \throws	ExceptionIO	the image could not be loaded (file not found or invalid image format)
 *
 * \param[in]	fname	the name of the file to create
 * \param[in]	attr	the attributes of the PDF file
 * \param[in]	prog	a progressbar (may be nullptr)
 */
void Document::ExportPDF(const Path &fname, const PDF::Attributes &attr, Progress *prog) const
{
	PDF::Doc pdf(attr);
	std::vector<Path> images;
	Path tmpimg(tmpnam(nullptr));
	if (prog)
		prog->SetMaxCount((int)GetNbViews());
	for (const view &v : views)
	{
		PDF::Page page = pdf.AddPage();
		PDF::Image image;
		if (attr.lossy_compression)
		{ // jpeg
			auto img = NewImageFromFile(v.filename);
			img->SaveJPEG(tmpimg, attr.jpeg_qual);
			image	= pdf.AddJPEG(tmpimg);
		}
		else
		{ // png
			auto img = NewImageFromFile(v.filename);
			img->SavePNG(images.back());
			images.push_back(tmpnam(nullptr));
			image	= pdf.AddPNG(images.back());
		}
		page.SetWidth((double)image.GetWidth());
		page.SetHeight((double)image.GetHeight());
		page.DrawImage(image, {0, 0, (int)image.GetWidth() - 1, (int)image.GetHeight() - 1});
		if (prog)
			prog->Advance();
	}
	// save
	pdf.Save(fname);

	if (attr.lossy_compression)
	{
		try
		{
			IO::Rm(tmpimg);
		} catch (...) { }
	}
	else
	{
		for (const Path &fname : images)
		{
			try
			{
				IO::Rm(fname);
			} catch (...) { }
		}
	}
}
#endif

size_t Document::GetThumbWidth() noexcept
{
	return thumbWidth;
}
size_t Document::GetThumbHeight() noexcept
{
	return thumbHeight;
}
void Document::SetThumbWidth(size_t w)
{
	if (w == 0)
		throw ExceptionDomain{ "Document::SetThumbWidth()"_s + _("Null width.") };
	thumbWidth = w;
}
void Document::SetThumbHeight(size_t h)
{
	if (h == 0)
		throw ExceptionDomain{ "Document::SetThumbHeight()"_s + _("Null height.") };
	thumbHeight = h;
}
