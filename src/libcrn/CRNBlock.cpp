/* Copyright 2006-2016 INSA-Lyon, ENS-Lyon
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
 * file: CRNBlock.cpp
 * \author Yann LEYDIER
 */

#include <CRNi18n.h>
#include <CRNBlock.h>
#include <CRNImage/CRNDifferential.h>
#include <CRNImage/CRNImageRGB.h>
#include <CRNImage/CRNImageGray.h>
#include <CRNImage/CRNImageBW.h>
#include <CRNData/CRNMap.h>
#include <CRNException.h>
#include <algorithm>
#include <functional>
#include <CRNIO/CRNFileShield.h>
#include <CRNIO/CRNIO.h>
#include <CRNUtils/CRNXml.h>

using namespace crn;

/*****************************************************************************/
/*!
 * Constructor for a top Block
 *
 * \throws	ExceptionInvalidArgument	null image or unsupported image type (BW, Gray nor RGB)
 * \param[in] 	src The source image of the top block
 * \param[in]	nam	the name of the new block
 */
SBlock Block::New(const SImage &src, const String &nam)
{
	auto b = SBlock(new Block(src, nam));
	b->self = b;
	return b;
}

/*****************************************************************************/
/*!
 * Constructor for a top Block
 *
 * \throws	ExceptionInvalidArgument	null image or unsupported image type (BW, Gray nor RGB)
 * \param[in] 	src The source image of the top block
 * \param[in]	nam	the name of the new block
 */
Block::Block(const SImage &src, const String &nam):
	Savable(nam),
	child(std::make_shared<Map>()),
	srcRGB(nullptr),
	srcGray(nullptr),
	srcBW(nullptr),
	srcGradient(nullptr),
	buffRGB(nullptr),
	buffGray(nullptr),
	buffBW(nullptr),
	buffGradient(nullptr),
	grad_sigma(-1),
	grad_diffusemaxiter(0),
	grad_diffusemaxdiv(std::numeric_limits<double>::max())
{
	if (!nam)
		SetName(_("NewBlock"));
	if (!src)
	{
		throw ExceptionInvalidArgument(StringUTF8("Block::Block(SImage*) :") + _("No image."));
	}
	srcRGB = std::dynamic_pointer_cast<ImageRGB>(src);
	if (!srcRGB)
	{
		srcGray = std::dynamic_pointer_cast<ImageGray>(src);
		if (!srcGray)
		{
			srcBW = std::dynamic_pointer_cast<ImageBW>(src);
			if (!srcBW)
				srcGradient = std::dynamic_pointer_cast<ImageGradient>(src);
		}
	}
	if (!srcRGB && !srcGray && !srcBW && !srcGradient)
		throw ExceptionInvalidArgument(StringUTF8("Block::Block(SImage src, String nam): ") +
			 	_("unsupported image type."));
	image_is_open = true;
	bbox = Rect(0, 0, int(src->GetWidth()) - 1, int(src->GetHeight()) - 1);
}

/*****************************************************************************/
/*!
 * Constructor from filenames
 *
 * \throws	ExceptionIO	XML file exists but cannot be accessed or has invalid structure
 * \throws	ExceptionRuntime	the XML file does not fit the block's image
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \param[in] ifname	image filename
 * \param[in]	xfname	xml filename
 * \param[in]	nam	the name of the new block
 *
 */
SBlock Block::New(const Path &ifname, const Path &xfname, const String &nam)
{
	auto b = SBlock(new Block(ifname, xfname, nam));
	b->self = b;
	if (xfname.IsNotEmpty())
		b->Append(xfname);
	if ((!b->bbox.IsValid()) || (b->bbox.GetWidth() == 0) || (b->bbox.GetHeight() == 0))
	{
		auto irgb = b->GetRGB();
		b->bbox.SetLeft(0);
		b->bbox.SetTop(0);
		b->bbox.SetWidth(int(irgb->GetWidth()));
		b->bbox.SetHeight(int(irgb->GetHeight()));
	}
	return b;
}

/*****************************************************************************/
/*!
 * Constructor from filenames
 *
 * \throws	ExceptionIO	XML file exists but cannot be accessed or has invalid structure
 * \throws	ExceptionRuntime	the XML file does not fit the block's image
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \param[in] ifname	image filename
 * \param[in]	xfname	xml filename
 * \param[in]	nam	the name of the new block
 *
 */
Block::Block(const Path &ifname, const Path &xfname, const String &nam):
	Savable(nam),
	child(std::make_shared<Map>()),
	srcRGB(nullptr),
	srcGray(nullptr),
	srcBW(nullptr),
	srcGradient(nullptr),
	buffRGB(nullptr),
	buffGray(nullptr),
	buffBW(nullptr),
	buffGradient(nullptr),
	grad_sigma(-1),
	grad_diffusemaxiter(0),
	grad_diffusemaxdiv(std::numeric_limits<double>::max())
{
	if (!nam)
		SetName(_("NewBlock"));
	image_is_open = false;
	imagefilename = ifname;
}

/*****************************************************************************/
/*!
 * Constructor for child Block
 *
 * \throws	ExceptionInvalidArgument	parent block is null
 *
 * \param[in] 	par The parent Block
 * \param[in]	tree	the tree in which the block will be added
 * \param[in]		clip The bounding box of the new block, given in absolute coordinates
 * \param[in]	nam	the name of the new block
 */
SBlock Block::create(const WBlock &par, const String &tree, const Rect &clip, const String &nam)
{
	auto b = SBlock(new Block(par, tree, clip, nam));
	b->self = b;
	return b;
}

/*****************************************************************************/
/*!
 * Constructor for child Block
 *
 * \throws	ExceptionInvalidArgument	parent block is null
 *
 * \param[in] 	par The parent Block
 * \param[in]	tree	the tree in which the block will be added
 * \param[in]		clip The bounding box of the new block, given in absolute coordinates
 * \param[in]	nam	the name of the new block
 */
Block::Block(const WBlock &par, const String &tree, const Rect &clip, const String &nam):
	Savable(nam),
	child(std::make_shared<Map>()),
	parent(par),
	parenttree(tree),
	bbox(clip),
	buffRGB(nullptr),
	buffGray(nullptr),
	buffBW(nullptr),
	buffGradient(nullptr),
	grad_sigma(-1),
	grad_diffusemaxiter(0),
	grad_diffusemaxdiv(std::numeric_limits<double>::max())
{
	if (!nam)
		SetName(U"NewChildBlock");
	if (par.expired())
		throw ExceptionInvalidArgument(StringUTF8("Block::Block(WBlock par, const String &tree, Rect clip, String nam) : ") +
				_("No parent."));
	srcRGB = par.lock()->srcRGB;
	srcGray = par.lock()->srcGray;
	srcBW = par.lock()->srcBW;
	srcGradient = par.lock()->srcGradient;
	image_is_open = par.lock()->image_is_open;
	// clipping!
	SImage src = nullptr;
	if (srcRGB) // search for a valid source
		src = srcRGB;
	else if (srcGray)
		src = srcGray;
	else if (srcBW)
		src = srcBW;
	else if (srcGradient)
		src = srcGradient;
	bbox = bbox & par.lock()->GetAbsoluteBBox();
	if (src)
	{
		if (bbox.GetLeft() < 0)
			bbox.SetLeft(0);
		if (bbox.GetTop() < 0)
			bbox.SetTop(0);
		if (bbox.GetRight() >= int(src->GetWidth()))
			bbox.SetRight(int(src->GetWidth()) - 1);
		if (bbox.GetBottom() >= int(src->GetHeight()))
			bbox.SetBottom(int(src->GetHeight()) - 1);
	}
}

/*!
 * Sets the absolute bounding box.
 *
 * If it overflows the topmost parent's bounding box, we truncate the box. A topmost block's bounding box cannot be set.
 *
 * \throws	ExceptionLogic	the block is the topmost block
 * \throws	ExceptionInvalidArgument	uninitialized bounding box
 * \throws	ExceptionDimension	bounding box is out of parent's bounding box
 *
 * \param[in]	newbox	the new absolute bounding box
 */
void Block::SetAbsoluteBBox(const Rect &newbox)
{
	if (parent.expired())
		throw ExceptionLogic(StringUTF8("Block::SetAbsoluteBBox(): ") +
				_("this is a topmost block. Its bounding box cannot be changed."));
	if (!newbox.IsValid())
		throw ExceptionInvalidArgument(StringUTF8("Block::SetAbsoluteBBox(): ") +
				_("Uninitialized bounding box."));
	Rect nr = parent.lock()->GetAbsoluteBBox();
	nr &= newbox;
	if (!nr.IsValid())
		throw ExceptionDimension(StringUTF8("Block::SetAbsoluteBBox(): ") +
				_("bounding box out of parent's bounding box."));

	if ((nr & bbox).GetArea() < bbox.GetArea())
		for (auto & elem : *child)
		{
			SVector v(std::static_pointer_cast<Vector>(elem.second));
			for (long j = long(v->Size()) - 1 ; j >= 0 ; --j)
			{
				SBlock b(std::static_pointer_cast<Block>((*v)[size_t(j)]));
				Rect newChildBox(nr & b->GetAbsoluteBBox());
				if (newChildBox.GetArea() > 0)
					b->SetAbsoluteBBox(newChildBox);
				else
				{
					b->bbox = Rect();
					v->Remove(j);
				}
			}
		}
	FlushAll();
	bbox = nr;
}

/*!
 * Sets the relative bounding box.
 *
 * If it overflows the topmost parent's bounding box, we truncate the box. A topmost block's bounding box cannot be set.
 *
 * \throws	ExceptionLogic	the block is the topmost block
 * \throws	ExceptionInvalidArgument	uninitialized bounding box
 * \throws	ExceptionDimension	bounding box is out of parent's bounding box
 *
 * \param[in]	newbox	the new relative bounding box
 */
void Block::SetRelativeBBox(Rect newbox)
{
	if (parent.expired())
		throw ExceptionLogic(StringUTF8("Block::SetRelativeBBox(): ") +
				_("this is a topmost block. Its bounding box cannot be changed."));
	if (!newbox.IsValid())
		throw ExceptionInvalidArgument(StringUTF8("Block::SetRelativeBBox(): ") +
				_("Uninitialized bounding box."));
	newbox.Translate(parent.lock()->GetAbsoluteBBox().GetLeft(), parent.lock()->GetAbsoluteBBox().GetTop());
	SetAbsoluteBBox(newbox);
}
/*!
 * Gets the relative bounding box of the block
 *
 * \return the relative bounding box of the block
 */
Rect Block::GetRelativeBBox() const
{
	if (parent.expired())
		return bbox;
	Rect r = bbox;
	r.Translate(-parent.lock()->GetAbsoluteBBox().GetLeft(), -parent.lock()->GetAbsoluteBBox().GetTop());
	return r;
}

/*!
 * Returns an iterator on the first block of a tree
 *
 * \throws	ExceptionInvalidArgument	tree not found
 *
 * \param[in]	tree	the name of the subblock tree
 *
 * \return	an iterator on the first block of the tree
 */
Block::block_iterator Block::BlockBegin(const String &tree)
{
	if (child->Find(tree) != child->end())
	{
		return block_iterator(getChildList(tree)->begin());
	}
	else
		throw ExceptionInvalidArgument(StringUTF8("Block::block_iterator Block::BlockBegin(const String &tree): ") +
				_("tree not found."));
}

/*!
 * Returns an iterator after the last block of a tree
 *
 * \throws	ExceptionInvalidArgument	tree not found
 *
 * \param[in]	tree	the name of the subblock tree
 *
 * \return	an iterator after the last block of the tree
 */
Block::block_iterator Block::BlockEnd(const String &tree)
{
	if (child->Find(tree) != child->end())
		return block_iterator(getChildList(tree)->end());
	else
		throw ExceptionInvalidArgument(StringUTF8("Block::block_iterator Block::BlockEnd(const String &tree): ") +
				_("tree not found."));
}

/*!
 * Returns a const_iterator on the first block of a tree
 *
 * \throws	ExceptionInvalidArgument	tree not found
 *
 * \param[in]	tree	the name of the subblock tree
 *
 * \return	a const_iterator on the first block of the tree
 */
Block::const_block_iterator Block::BlockBegin(const String &tree) const
{
	if (child->Find(tree) != child->end())
	{
		SCVector v = std::static_pointer_cast<const Vector>(child->Get(tree));
		return const_block_iterator(v->begin());
	}
	else
		throw ExceptionInvalidArgument(StringUTF8("Block::const_block_iterator Block::BlockBegin(const String &tree) const: ") +
				_("tree not found."));
}

/*!
 * Returns a const_iterator after the last block of a tree
 *
 * \throws	ExceptionInvalidArgument	tree not found
 *
 * \param[in]	tree	the name of the subblock tree
 *
 * \return	a const_iterator after the last block of the tree
 */
Block::const_block_iterator Block::BlockEnd(const String &tree) const
{
	if (child->Find(tree) != child->end())
	{
		SCVector v = std::static_pointer_cast<const Vector>(child->Get(tree));
		return const_block_iterator(v->end());
	}
	else
		throw ExceptionInvalidArgument(StringUTF8("Block::const_block_iterator Block::BlockEnd(const String &tree) const: ") +
				_("tree not found."));
}

/*!
 * Loads the image corresponding to the block if it wasn't already loaded.
 *
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 */
void Block::openImage(void)
{
	if (image_is_open)
	{
		return;
	}
	if (!parent.expired())
	{
		parent.lock()->openImage();
		return;
	}

	SImage src(nullptr);
	try
	{
		src = NewImageFromFile(imagefilename);
	}
	catch (...)
	{
	}
	if (!src)
	{
		throw ExceptionIO(StringUTF8("Block::openImage(void): ") +
				_("Cannot open image."));
	}
	srcRGB = std::dynamic_pointer_cast<ImageRGB>(src);
	srcGray = std::dynamic_pointer_cast<ImageGray>(src);
	srcBW = std::dynamic_pointer_cast<ImageBW>(src);
	bbox = Rect(0, 0, int(src->GetWidth()) - 1, int(src->GetHeight()) - 1);
	image_is_open = true;
	return;
}

/*!
 * Returns the source RGB image. Loads it from file if necessary.
 *
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \return	the source RGB image or nullptr if not applicable.
 */
SImageRGB Block::get_srcRGB(void)
{
	if (!parent.expired())
		return parent.lock()->get_srcRGB();

	openImage();
	return srcRGB;
}

/*!
 * Returns the source gray image. Loads it from file if necessary.
 *
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \return	the source gray image or nullptr if not applicable.
 */
SImageGray Block::get_srcGray(void)
{
	if (!parent.expired())
		return parent.lock()->get_srcGray();

	openImage();
	return srcGray;
}

/*!
 * Returns the source BW image. Loads it from file if necessary.
 *
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \return	the source BW image or nullptr if not applicable.
 */
SImageBW Block::get_srcBW(void)
{
	if (!parent.expired())
		return parent.lock()->get_srcBW();

	openImage();
	return srcBW;
}

/*!
 * Returns the source gradient image.
 *
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \return	the source gradient image or nullptr if not applicable.
 */
SImageGradient Block::get_srcGradient(void)
{
	if (!parent.expired())
		return parent.lock()->get_srcGradient();

	openImage();
	return srcGradient;
}

/*****************************************************************************/
/*!
 * Destructor. Saves the changes on subblock trees.
 */
Block::~Block()
{
	if (GetFilename().IsNotEmpty())
	{
		Save();
	}
}

/*!
 * Returns a subblock tree. Creates it if necessary.
 * \param	name	the name of the tree
 *
 * \return a SVector containing SBlocks
 */
SVector Block::getChildList(const String &name)
{
	if (child->Find(name) == child->end())
		child->Set(name, std::make_shared<Vector>());
	return std::static_pointer_cast<Vector>(child->Get(name));
}

/*****************************************************************************/
/*!
 * Creates and adds a child to the current block
 *
 * \throws	ExceptionInvalidArgument	uninitialized rectangle
 * \throws	ExceptionDomain	rectangle is out of bounds
 *
 * \param[in]	tree	The name of the tree to add the subblock
 * \param[in]		clip The bounding box of the new block, given in absolute coordinates.
 * \return		The newly created block.
 */
SBlock Block::AddChildAbsolute(const String &tree, Rect clip)
{
	if (!clip.IsValid())
		throw ExceptionInvalidArgument(StringUTF8("SBlock Block::AddChildAbsolute(const String &tree, Rect clip): ") +
				_("Uninitialized clipping rectangle."));
	SBlock nb = create(self, tree, clip, tree);
	if (!nb->GetAbsoluteBBox().IsValid())
	{
		throw ExceptionDomain(StringUTF8("SBlock Block::AddChildAbsolute(const String &tree, Rect clip): ") +
				_("Clipping rectangle out of bounds."));
	}
	getChildList(tree)->PushBack(nb);
	return nb;
}

/*****************************************************************************/
/*!
 * Creates and adds a child to the current block with a name
 *
 * \throws	ExceptionInvalidArgument	uninitialized rectangle
 * \throws	ExceptionDomain	rectangle is out of bounds
 *
 * \param[in]	tree	The name of the tree to add the subblock
 * \param[in]		clip The bounding box of the new block, given in absolute coordinates.
 * \param[in]	name	The name of the new subblock
 * \return		The newly created block.
 */
SBlock Block::AddChildAbsolute(const String &tree, Rect clip, const String &name)
{
	if (!clip.IsValid())
		throw ExceptionInvalidArgument(StringUTF8("SBlock Block::AddChildAbsolute(const String &tree, Rect clip, const String &name): ") +
				_("Uninitialized clipping rectangle."));
	SBlock nb = create(self, tree, clip, name);
	if (!nb->GetAbsoluteBBox().IsValid())
	{
		throw ExceptionDomain(StringUTF8("SBlock Block::AddChildAbsolute(const String &tree, Rect clip, const String &name): ") +
				_("Clipping rectangle out of bounds."));
	}
	getChildList(tree)->PushBack(nb);
	return nb;
}

/*****************************************************************************/
/*!
 * Creates and adds a child to the current block at the specified position
 *
 * \throws	ExceptionInvalidArgument	uninitialized rectangle
 * \throws	ExceptionDomain	rectangle is out of bounds
 *
 * \param[in]	tree	The name of the tree to add the subblock
 * \param[in]		clip The bounding box of the new block, given in absolute coordinates.
 * \param[in]	pos	The position of the newly created child
 * \return		The newly created block.
 */
SBlock Block::AddChildAbsoluteAt(const String &tree, Rect clip, size_t pos)
{
	if (!clip.IsValid())
		throw ExceptionInvalidArgument(StringUTF8("SBlock Block::AddChildAbsoluteAt(const String &tree, Rect clip, size_t pos): ") +
				_("Uninitialized clipping rectangle."));
	if (child->Find(tree) == child->end())
		return AddChildAbsolute(tree, clip);
	else if (pos >= getChildList(tree)->Size())
		return AddChildAbsolute(tree, clip);
	else
	{
		SBlock nb = create(self, tree, clip, tree.CStr());
		if (!nb->GetAbsoluteBBox().IsValid())
		{
			throw ExceptionDomain(StringUTF8("SBlock Block::AddChildAbsoluteAt(const String &tree, Rect clip, size_t pos): ") +
					_("Clipping rectangle out of bounds."));
		}
		getChildList(tree)->Insert(nb, pos);
		return nb;
	}
}

/*****************************************************************************/
/*!
 * Creates and adds a child to the current block with a name at the specified position
 *
 * \throws	ExceptionInvalidArgument	uninitialized rectangle
 * \throws	ExceptionDomain	rectangle is out of bounds
 *
 * \param[in]	tree	The name of the tree to add the subblock
 * \param[in]		clip The bounding box of the new block, given in absolute coordinates.
 * \param[in]	name	The name of the new subblock
 * \param[in]	pos	The position of the newly created child
 * \return		The newly created block.
 */
SBlock Block::AddChildAbsoluteAt(const String &tree, Rect clip, const String &name, size_t pos)
{
	if (!clip.IsValid())
		throw ExceptionInvalidArgument(StringUTF8("SBlock Block::AddChildAbsoluteAt(const String &tree, Rect clip, const String &name, size_t pos): ") +
				_("Uninitialized clipping rectangle."));
	if (child->Find(tree) == child->end())
		return AddChildAbsolute(tree, clip, name);
	else if (pos >= getChildList(tree)->Size())
		return AddChildAbsolute(tree, clip, name);
	else
	{
		SBlock nb = create(self, tree, clip, name);
		if (!nb->GetAbsoluteBBox().IsValid())
		{
			throw ExceptionDomain(StringUTF8("SBlock Block::AddChildAbsoluteAt(const String &tree, Rect clip, const String &name, size_t pos): ") +
					_("Clipping rectangle out of bounds."));
		}
		getChildList(tree)->Insert(nb, pos);
		return nb;
	}
}

/*****************************************************************************/
/*!
 * Creates and adds a child to the current block
 *
 * \throws	ExceptionInvalidArgument	uninitialized rectangle
 * \throws	ExceptionDomain	rectangle is out of bounds
 *
 * \param[in]	tree	The name of the tree to add the subblock
 * \param[in]		clip The bounding box of the new block, given in relative coordinates.
 * \return		The newly created block.
 */
SBlock Block::AddChildRelative(const String &tree, Rect clip)
{
	if (!clip.IsValid())
		throw ExceptionInvalidArgument(StringUTF8("SBlock Block::AddChildRelative(const String &tree, Rect clip): ") +
				_("Uninitialized clipping rectangle."));
	clip.Translate(bbox.GetLeft(), bbox.GetTop());
	SBlock nb = create(self, tree, clip, tree);
	if (!nb->GetAbsoluteBBox().IsValid())
	{
		throw ExceptionDomain(StringUTF8("SBlock Block::AddChildRelative(const String &tree, Rect clip): ") +
				_("Clipping rectangle out of bounds."));
	}
	getChildList(tree)->PushBack(nb);
	return nb;
}

/*****************************************************************************/
/*!
 * Creates and adds a child to the current block with a name
 *
 * \throws	ExceptionInvalidArgument	uninitialized rectangle
 * \throws	ExceptionDomain	rectangle is out of bounds
 *
 * \param[in]	tree	The name of the tree to add the subblock
 * \param[in]		clip The bounding box of the new block, given in relative coordinates.
 * \param[in]	name	The name of the new subblock
 * \return		The newly created block.
 */
SBlock Block::AddChildRelative(const String &tree, Rect clip, const String &name)
{
	if (!clip.IsValid())
		throw ExceptionInvalidArgument(StringUTF8("SBlock Block::AddChildRelative(const String &tree, Rect clip, const String &name): ") +
				_("Uninitialized clipping rectangle."));
	clip.Translate(bbox.GetLeft(), bbox.GetTop());
	SBlock nb = create(self, tree, clip, name);
	if (!nb->GetAbsoluteBBox().IsValid())
	{
		throw ExceptionDomain(StringUTF8("SBlock Block::AddChildRelative(const String &tree, Rect clip, const String &name): ") +
				_("Clipping rectangle out of bounds."));
	}
	getChildList(tree)->PushBack(nb);
	return nb;
}

/*****************************************************************************/
/*!
 * Creates and adds a child to the current block at the specified position
 *
 * \throws	ExceptionInvalidArgument	uninitialized rectangle
 * \throws	ExceptionDomain	rectangle is out of bounds
 *
 * \param[in]	tree	The name of the tree to add the subblock
 * \param[in]		clip The bounding box of the new block, given in relative coordinates.
 * \param[in]	pos	The position of the newly created child
 * \return		The newly created block.
 */
SBlock Block::AddChildRelativeAt(const String &tree, Rect clip, size_t pos)
{
	if (!clip.IsValid())
		throw ExceptionInvalidArgument(StringUTF8("SBlock Block::AddChildRelativeAt(const String &tree, Rect clip, size_t pos): ") +
				_("Uninitialized clipping rectangle."));
	if (child->Find(tree) == child->end())
		return AddChildRelative(tree, clip);
	else if (pos >= getChildList(tree)->Size())
		return AddChildRelative(tree, clip);
	else
	{
		clip.Translate(bbox.GetLeft(), bbox.GetTop());
		SBlock nb = create(self, tree, clip, tree.CStr());
		if (!nb->GetAbsoluteBBox().IsValid())
		{
			throw ExceptionDomain(StringUTF8("SBlock Block::AddChildRelativeAt(const String &tree, Rect clip, size_t pos): ") +
					_("Clipping rectangle out of bounds."));
		}
		getChildList(tree)->Insert(nb, pos);
		return nb;
	}
}

/*****************************************************************************/
/*!
 * Creates and adds a child to the current block with a name at the specified position
 *
 * \throws	ExceptionInvalidArgument	uninitialized rectangle
 * \throws	ExceptionDomain	rectangle is out of bounds
 *
 * \param[in]	tree	The name of the tree to add the subblock
 * \param[in]		clip The bounding box of the new block, given in relative coordinates.
 * \param[in]	name	The name of the new subblock
 * \param[in]	pos	The position of the newly created child
 * \return		The newly created block.
 */
SBlock Block::AddChildRelativeAt(const String &tree, Rect clip, const String &name, size_t pos)
{
	if (!clip.IsValid())
		throw ExceptionInvalidArgument(StringUTF8("SBlock Block::AddChildRelativeAt(const String &tree, Rect clip, const String &name, size_t pos): ") +
				_("Uninitialized clipping rectangle."));
	if (child->Find(tree) == child->end())
		return AddChildRelative(tree, clip, name);
	else if (pos >= getChildList(tree)->Size())
		return AddChildRelative(tree, clip, name);
	else
	{
		clip.Translate(bbox.GetLeft(), bbox.GetTop());
		SBlock nb = create(self, tree, clip, name);
		if (!nb->GetAbsoluteBBox().IsValid())
		{
			throw ExceptionDomain(StringUTF8("SBlock Block::AddChildRelativeAt(const String &tree, Rect clip, const String &name, size_t pos): ") +
					_("Clipping rectangle out of bounds."));
		}
		getChildList(tree)->Insert(nb, pos);
		return nb;
	}
}


/*****************************************************************************/
/*!
 * Returns a pointer to the local RGB buffer.
 *
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \return		The RGB local buffer.
 */
SImageRGB Block::GetRGB()
{
	if (!buffRGB)
	{
		// 1st option: get from the RGB source
		if (get_srcRGB())
		{
			buffRGB = std::make_shared<ImageRGB>(*get_srcRGB(), bbox);
			return buffRGB;
		}
		// 2nd option: get parent RGB buffer
		if (!parent.expired())
		{
			if (parent.lock()->GetRGB())
			{
				Rect localbbox(bbox.GetLeft() - parent.lock()->GetAbsoluteBBox().GetLeft(),
						bbox.GetTop() - parent.lock()->GetAbsoluteBBox().GetTop(),
						bbox.GetRight() - parent.lock()->GetAbsoluteBBox().GetLeft(),
						bbox.GetBottom() - parent.lock()->GetAbsoluteBBox().GetTop());
				buffRGB = std::make_shared<ImageRGB>(*parent.lock()->GetRGB(), localbbox);
				return buffRGB;
			}
		}
		// 3rd option: get from the gray local buffer
		if (buffGray)
		{
			buffRGB = std::make_shared<ImageRGB>(*buffGray);
			return buffRGB;
		}
		// 4th option: get from the b&w local buffer
		if (buffBW)
		{
			buffRGB = std::make_shared<ImageRGB>(*buffBW);
			return buffRGB;
		}
		// 5th option: get from the gray source
		if (get_srcGray())
		{
			buffRGB = std::make_shared<ImageRGB>(*get_srcGray());
			return buffRGB;
		}
		// 6th option: get from the b&w source
		if (get_srcBW())
		{
			buffRGB = std::make_shared<ImageRGB>(*get_srcBW());
			return buffRGB;
		}
		//else
		return nullptr;
	}

	return buffRGB;
}



/*****************************************************************************/
/*!
 * Returns a pointer to the local gray buffer.
 * Creates the buffer if non existent.
 *
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \param[in]		create	Shall the buffer be created from a rgb one? default = true.
 * \return		The gray local buffer.
 */
SImageGray Block::GetGray(bool create)
{
	if (!buffGray)
	{
		// 1st option: get from the gray source
		if (get_srcGray())
		{
			buffGray = std::make_shared<ImageGray>(*get_srcGray(), bbox);
			return buffGray;
		}
		// 2nd option: get parent gray buffer
		if (!parent.expired())
		{
			if (parent.lock()->GetGray(create))
			{
				Rect localbbox(bbox.GetLeft() - parent.lock()->GetAbsoluteBBox().GetLeft(),
						bbox.GetTop() - parent.lock()->GetAbsoluteBBox().GetTop(),
						bbox.GetRight() - parent.lock()->GetAbsoluteBBox().GetLeft(),
						bbox.GetBottom() - parent.lock()->GetAbsoluteBBox().GetTop());
				buffGray = std::make_shared<ImageGray>(*parent.lock()->GetGray(false), localbbox);
				return buffGray;
			}
		}
		// 3rd option: get from the RGB buffer
		if (buffRGB && create)
		{
			buffGray = MoveShared(MakeImageGray(*buffRGB));
			return buffGray;
		}
		// 4th option: get from the RGB source
		if (get_srcRGB() && create)
		{
			buffGray = MoveShared(MakeImageGray(*GetRGB()));
			return buffGray;
		}
		// 5th option: get from the BW buffer
		if (buffBW)
		{
			buffGray = std::make_shared<ImageGray>(*buffBW);
			return buffGray;
		}
		// 6th option: get from the BW source
		if (get_srcBW())
		{
			buffGray = std::make_shared<ImageGray>(*get_srcBW());
			return buffGray;
		}
		//else
		if (create)
		{
			CRNWarning(String(U"SImageGray* Block::GetGray(): ") +
					_("Cannot access to any source or buffer."));
		}
		return nullptr;
	}
	return buffGray;
}


/*****************************************************************************/
/*!
 * Returns a pointer to the local b&w buffer.
 * Creates the buffer if non existent.
 *
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \param[in]		create	Shall the buffer be created from a rgb or gray one? default = true.
 * \return		The b&w local buffer.
 */
SImageBW Block::GetBW(bool create)
{
	if (!buffBW)
	{
		// 1st option: get from the b&w source
		if (get_srcBW())
		{
			buffBW = std::make_shared<ImageBW>(*get_srcBW(), bbox);
			return buffBW;
		}
		// 2nd option: get parent BW buffer
		if (!parent.expired())
		{
			if (parent.lock()->GetBW(create))
			{
				Rect localbbox(bbox.GetLeft() - parent.lock()->GetAbsoluteBBox().GetLeft(),
						bbox.GetTop() - parent.lock()->GetAbsoluteBBox().GetTop(),
						bbox.GetRight() - parent.lock()->GetAbsoluteBBox().GetLeft(),
						bbox.GetBottom() - parent.lock()->GetAbsoluteBBox().GetTop());
				buffBW = std::make_shared<ImageBW>(*parent.lock()->GetBW(false), localbbox);
				return buffBW;
			}
		}
		// 3rd option: create it
		if (create)
		{
			SImageGray tmp = GetGray(true);
			if (tmp)
			{
				buffBW = MoveShared(MakeImageBW(*tmp));
				return buffBW;
			}
			else
			{
				return nullptr;
			}
		}
		// else
		if (create)
		{
			CRNWarning(String(U"SImageBW* Block::GetBW(): ") +
					_("Cannot access to any source or buffer."));
		}
		return nullptr;
	}

	return buffBW;
}

/*!
 * Gets the list of the tree names
 *
 * \return	The list of the child trees' names
 */
std::vector<String> Block::GetTreeNames() const
{
	std::vector<String> list;
	for (Map::const_iterator it = child->begin(); it != child->end(); ++it)
	{
		list.push_back(it->first);
	}
	return list;
}

/*!
 * Saves the child trees into a file.
 *
 * \throws	ExceptionInvalidArgument	empty file name
 * \throws	ExceptionIO	cannot save XML file
 * \throws	ExceptionRuntime	error creating the file
 * \param[in]		fname	The file to create.
 */
void Block::Save(const Path &fname)
{
	std::lock_guard<std::mutex> lockf(crn::FileShield::GetMutex(fname)); // lock the file
	if (!fname)
	{
		throw ExceptionInvalidArgument(StringUTF8("void Block::Save(const Path &fname): ") +
				_("No filename given."));
	}

	xml::Document doc;
	doc.PushBackComment("libcrn Block tree file");
	addToXml(doc);
	doc.Save(fname.CStr()); // may throw
}

/*!
 * Adds the block's name, coordinates and subblock trees to an XML node.
 * \param[in]	parent	the node that will contain the block's data
 */
void Block::addToXml(xml::Document &parent)
{
	xml::Element eb(parent.PushBackElement("Block"));
	eb.SetAttribute("left", bbox.GetLeft());
	eb.SetAttribute("top", bbox.GetTop());
	eb.SetAttribute("right", bbox.GetRight());
	eb.SetAttribute("bottom", bbox.GetBottom());
	for (Map::const_iterator it = child->begin(); it != child->end(); ++it)
	{
		xml::Element el(eb.PushBackElement("BlockTree"));
		el.SetAttribute("treename", it->first.CStr());
		SVector v = std::static_pointer_cast<Vector>(it->second);
		for (size_t tmp = 0; tmp < v->Size(); tmp++)
		{
			std::static_pointer_cast<Block>(v->At(tmp))->addToXml(el);
		}
	}
	// save userdata
	serialize_internal_data(eb);
}

/*!
 * Adds the block's name, coordinates and subblock trees to an XML node.
 * \param[in]	parent	the node that will contain the block's data
 */
void Block::addToXml(xml::Element &parent)
{
	xml::Element eb(parent.PushBackElement("Block"));
	eb.SetAttribute("left", bbox.GetLeft());
	eb.SetAttribute("top", bbox.GetTop());
	eb.SetAttribute("right", bbox.GetRight());
	eb.SetAttribute("bottom", bbox.GetBottom());
	for (Map::const_iterator it = child->begin(); it != child->end(); ++it)
	{
		xml::Element el(eb.PushBackElement("BlockTree"));
		el.SetAttribute("treename", it->first.CStr());
		SVector v = std::static_pointer_cast<Vector>(it->second);
		for (size_t tmp = 0; tmp < v->Size(); tmp++)
		{
			std::static_pointer_cast<Block>(v->At(tmp))->addToXml(el);
		}
	}
	// save userdata
	serialize_internal_data(eb);
}

/*!
 * Appends child trees from a file.
 *
 * \throws	ExceptionInvalidArgument	empty file name
 * \throws	ExceptionIO	file exists but cannot be accessed or has invalid structure
 * \throws	ExceptionRuntime	the XML file does not fit the block's image
 *
 * \param[in]		fname	The file to load.
 * \return	true if success, false if file not found.
 */
bool Block::Append(const Path &fname)
{
	std::lock_guard<std::mutex> lockf(crn::FileShield::GetMutex(fname)); // lock the file

	setFilename(fname);
	if (!fname)
	{
		throw ExceptionInvalidArgument(StringUTF8("bool Block::Append(const Path &fname): ") +
				_("No filename given."));
	}
	Path fn(fname);
	fn.ToLocal();
	if (!IO::Access(fn, IO::EXISTS))
	{ // file not found, do not warn
		return false;
	}
	xml::Document doc(fname); // may throw
	xml::Element root = doc.GetRoot();
	int l = root.GetAttribute<int>("left", false); // may throw
	int t = root.GetAttribute<int>("top", false); // may throw
	int b = root.GetAttribute<int>("bottom", false); // may throw
	int r = root.GetAttribute<int>("right", false); // may throw
	if (image_is_open)
	{
		if ((l != bbox.GetLeft()) || (t != bbox.GetTop()) ||
				(r != bbox.GetRight()) || (b != bbox.GetBottom()))
		{
			throw ExceptionRuntime(StringUTF8("bool Block::Append(const Path &fname): ") +
						_("Saved block do not have the same size."));
		}
	}
	else
	{
		bbox.SetLeft(l);
		bbox.SetRight(r);
		bbox.SetTop(t);
		bbox.SetBottom(b);
	}
	addTreeFromXml(root);
	return true;
}

/*!
 * Reads block's name, coordinates and subblock trees from an XML node.
 *
 * \throws	ExceptionIO	file exists but cannot be accessed or has invalid structure
 * \throws	ExceptionNotFound	cannot find attribute
 * \throws	ExceptionDomain	wrong attribute
 *
 * \param[in]	bnode	the XML node containing the block's data
 */
void Block::addTreeFromXml(xml::Element &bnode)
{
	for (xml::Element tree = bnode.BeginElement(); tree != bnode.EndElement(); ++tree)
	{
		if (tree.GetName() != "BlockTree")
			continue;
		const StringUTF8 treename = tree.GetAttribute<StringUTF8>("treename");
		for (xml::Element block = tree.BeginElement(); block != tree.EndElement(); ++block)
		{
			if (block.GetName() != "Block")
				continue;
			int l = block.GetAttribute<int>("left", false); // may throw
			int t = block.GetAttribute<int>("top", false); // may throw
			int b = block.GetAttribute<int>("bottom", false); // may throw
			int r = block.GetAttribute<int>("right", false); // may throw
			StringUTF8 bn = block.GetAttribute<StringUTF8>("name");
			if (bn.IsEmpty()) // ancient files have a blockname in stead of a name
				bn = block.GetAttribute<StringUTF8>("blockname");
			Rect rec(l, t, r, b);
			if (!rec.IsValid())
				throw ExceptionIO(StringUTF8("void Block::addTreeFromXml(xml::Node &bnode): ") +
						_("Wrong content."));
			SBlock newblock = AddChildAbsolute(treename, rec, (bn.IsNotEmpty() ? bn : String(U"")));
			newblock->addTreeFromXml(block);
		}
	}
	// load userdata
	deserialize_internal_data(bnode);
	if (!GetName()) // ancient files have a blockname in stead of a name
	{
		const StringUTF8 bn = bnode.GetAttribute<StringUTF8>("blockname");
		if (bn.IsNotEmpty())
			SetName(bn);
	}
}

/*!
 * Checks if a child tree exists
 *
 * \param[in]		tname	The name of the tree to check
 * \return	true if exists, false else.
 */
bool Block::HasTree(const String &tname) const
{
	if (child->Find(tname) == child->end())
		return false;
	else
		return GetNbChildren(tname) != 0;
}

/*!
 * Deletes a child tree
 *
 * \throws	ExceptionNotFound	tree not found
 *
 * \param[in]		tname	The tree to erase
 */
void Block::RemoveTree(const String &tname)
{
	child->Remove(tname); // may throw
}

/*****************************************************************************/
/*!
 * Returns a pointer to the local gradient buffer.
 *
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \param[in]		create	Shall the buffer be created from a gray or b&w one? default = true.
 * \param[in]		sigma	The standard deviation of the gaussian core used to apply the gradient. default = 0.5
 * \param[in]		diffusemaxiter	maximal number of iterations of the diffusion
 * \param[in]		diffusemaxdiv	maximal divergence to allow modification for each pixel during the diffusion
 * \return		The gradient local buffer or nullptr
 */
SImageGradient Block::GetGradient(bool create, double sigma, size_t diffusemaxiter, double diffusemaxdiv)
{
	if (buffGradient)
		return buffGradient;
	if (!create)
		return nullptr;
	// need to compute the buffer
	grad_sigma = sigma;
	grad_diffusemaxiter = diffusemaxiter;
	grad_diffusemaxdiv = diffusemaxdiv;

	// is the source a gradient?
	if (get_srcGradient())
	{
		// warning all arguments are ignored!
		buffGradient = std::make_shared<ImageGradient>(*get_srcGradient(), bbox);
		buffGradient->SetMinModule(get_srcGradient()->GetMinModule());
		return buffGradient;
	}

	// autocompute sigma if needed
	if (sigma == -1)
	{
		sigma = 0.5;
		auto ig = GetGray(true);
		if (ig)
		{
			size_t sw = StrokesWidth(*ig, 50, 3);
			sigma = double(sw) / 6.0;
		}
	}

	// topmost block
	if (parent.expired())
	{
		auto diff = Differential::NewGaussian(*GetRGB(), Differential::RGBProjection::ABSMAX, sigma);
		if (diffusemaxiter)
			diff.Diffuse(diffusemaxiter, diffusemaxdiv);

		buffGradient = MoveShared(diff.MakeImageGradient());
		return buffGradient;
	}

	// not topmost
	// look for a parent with a gradient
	WBlock gpar = parent;
	while (!gpar.expired())
	{
		if (gpar.lock()->GetGradient(false) && (gpar.lock()->grad_sigma == grad_sigma) && (gpar.lock()->grad_diffusemaxiter == grad_diffusemaxiter) && (gpar.lock()->grad_diffusemaxdiv == grad_diffusemaxdiv))
			break;
		gpar = gpar.lock()->parent;
	}
	if (!gpar.expired())
	{ // some top gradient was already computed
		Rect b(bbox);
		b.Translate(-gpar.lock()->GetAbsoluteBBox().GetLeft(),
				-gpar.lock()->GetAbsoluteBBox().GetTop());
		SImageGradient topgrad(gpar.lock()->GetGradient(true, sigma, diffusemaxiter, diffusemaxdiv));
		buffGradient = std::make_shared<ImageGradient>(*topgrad, b);
		buffGradient->SetMinModule(topgrad->GetMinModule());

		return buffGradient;
	}
	else
	{ // top gradient not computed
		Rect clip = GetTop().lock()->GetAbsoluteBBox();
		// add a margin
		clip.SetLeft(Max(0, bbox.GetLeft() - 10));
		clip.SetTop(Max(0, bbox.GetTop() - 10));
		clip.SetRight(Min(clip.GetRight(), bbox.GetRight() + 10));
		clip.SetBottom(Min(clip.GetBottom(), bbox.GetBottom() + 10));
		int offsetx = bbox.GetLeft() - clip.GetLeft();
		int offsety = bbox.GetTop() - clip.GetTop();
		auto tmp = ImageRGB(*GetTop().lock()->GetRGB(), clip);
		auto diff = Differential::NewGaussian(tmp, Differential::RGBProjection::ABSMAX, sigma);
		if (diffusemaxiter)
			diff.Diffuse(diffusemaxiter, diffusemaxdiv);

		auto tmpGradient = diff.MakeImageGradient();
		Rect r;
		r.SetLeft((int)offsetx);
		r.SetTop((int)offsety);
		r.SetRight((int)(offsetx + bbox.GetWidth() - 1));
		r.SetBottom((int)(offsety + bbox.GetHeight() - 1));
		buffGradient = std::make_shared<ImageGradient>(tmpGradient, r);
		buffGradient->SetMinModule(tmpGradient.GetMinModule());

		return buffGradient;
	}
}


/*****************************************************************************/
/*!
 * Reloads the image
 *
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 */
void Block::ReloadImage()
{
	if (!parent.expired())
		parent.lock()->ReloadImage();
	else
	{
		FlushAll(true);
		image_is_open = false;
		srcRGB = nullptr;
		srcGray = nullptr;
		srcBW = nullptr;
		srcGradient = nullptr;
		openImage();
		refreshSources();
	}
}

/*****************************************************************************/
/*!
 * Frees all buffers
 *
 * \param[in]	recursive	shall childrens' buffer be flushed?
 */
void Block::FlushAll(bool recursive)
{
	FlushRGB(recursive);
	FlushGray(recursive);
	FlushBW(recursive);
	FlushGradient(recursive);
}

/*****************************************************************************/
/*!
 * Frees the local RGB buffer
 *
 * \param[in]	recursive	shall childrens' buffer be flushed?
 */
void Block::FlushRGB(bool recursive)
{
	buffRGB = nullptr;
	if (recursive)
	{
		for (crn::Map::pair p : child)
		{
			SVector l(std::static_pointer_cast<Vector>(p.second));
			for (SObject b : l)
			{
				std::static_pointer_cast<Block>(b)->FlushRGB(true);
			}
		}
	}
}

/*****************************************************************************/
/*!
 * Frees the local gray buffer
 *
 * \param[in]	recursive	shall childrens' buffer be flushed?
 */
void Block::FlushGray(bool recursive)
{
	buffGray = nullptr;
	if (recursive)
	{
		for (crn::Map::pair p : child)
		{
			SVector l(std::static_pointer_cast<Vector>(p.second));
			for (SObject b : l)
			{
				std::static_pointer_cast<Block>(b)->FlushGray(true);
			}
		}
	}
}

/*****************************************************************************/
/*!
 * Frees the local b&w buffer
 *
 * \param[in]	recursive	shall childrens' buffer be flushed?
 */
void Block::FlushBW(bool recursive)
{
	buffBW = nullptr;
	if (recursive)
	{
		for (crn::Map::pair p : child)
		{
			SVector l(std::static_pointer_cast<Vector>(p.second));
			for (SObject b : l)
			{
				std::static_pointer_cast<Block>(b)->FlushBW(true);
			}
		}
	}
}

/*****************************************************************************/
/*!
 * Frees the local gradient buffer
 *
 * \param[in]	recursive	shall childrens' buffer be flushed?
 */
void Block::FlushGradient(bool recursive)
{
	buffGradient = nullptr;
	if (recursive)
	{
		for (crn::Map::pair p : child)
		{
			SVector l(std::static_pointer_cast<Vector>(p.second));
			for (SObject b : l)
			{
				std::static_pointer_cast<Block>(b)->FlushGradient(true);
			}
		}
	}
}

/*****************************************************************************/
/*!
 * Substitutes the RGB buffer with a new image.
 *
 * If a buffer exists, it will be freed.
 *
 * If the substitution succeeds, the block will keep the reference to the new image and will free it himself.
 *
 * \throws	ExceptionDimension	the images do no have the same size
 *
 * \param[in]	img 	the new image to use
 */
void Block::SubstituteRGB(const SImageRGB &img)
{
	if (!img)
	{
		FlushRGB();
		return;
	}
	int w = GetAbsoluteBBox().GetWidth();
	int h = GetAbsoluteBBox().GetHeight();
	if ((int(img->GetWidth()) != w) || (int(img->GetHeight()) != h))
	{
		throw ExceptionDimension(StringUTF8("bool Block::SubstituteRGB(const SImageRGB &img): ") +
				_("Wrong image dimensions."));
	}
	FlushRGB();
	buffRGB = img;
}

/*****************************************************************************/
/*!
 * Substitutes the Gray buffer with a new image.
 *
 * If a buffer exists, it will be freed.
 *
 * If the substitution succeeds, the block will keep the reference to the new image and will free it himself.
 *
 * \throws	ExceptionDimension	the images do no have the same size
 *
 * \param[in]	img 	the new image to use
 */
void Block::SubstituteGray(const SImageGray &img)
{
	if (!img)
	{
		FlushGray();
		return;
	}
	int w = GetAbsoluteBBox().GetWidth();
	int h = GetAbsoluteBBox().GetHeight();
	if ((int(img->GetWidth()) != w) || (int(img->GetHeight()) != h))
	{
		throw ExceptionDimension(StringUTF8("bool Block::SubstituteGray(const SImageGray &img): ") +
				_("Wrong image dimensions."));
	}
	FlushGray();
	buffGray = img;
}

/*****************************************************************************/
/*!
 * Substitutes the BW buffer with a new image.
 *
 * If a buffer exists, it will be freed.
 *
 * If the substitution succeeds, the block will keep the reference to the new image and will free it himself.
 *
 * \throws	ExceptionDimension	the images do no have the same size
 *
 * \param[in]	img 	the new image to use
 */
void Block::SubstituteBW(const SImageBW &img)
{
	if (!img)
	{
		FlushBW();
		return;
	}
	int w = GetAbsoluteBBox().GetWidth();
	int h = GetAbsoluteBBox().GetHeight();
	if ((int(img->GetWidth()) != w) || (int(img->GetHeight()) != h))
	{
		throw ExceptionDimension(StringUTF8("bool Block::SubstituteBW(const SImageBW &img): ") +
				_("Wrong image dimensions."));
	}
	FlushBW();
	buffBW = img;
}

/*****************************************************************************/
/*!
 * Substitutes the Gradient buffer with a new image.
 *
 * If a buffer exists, it will be freed.
 *
 * If the substitution succeeds, the block will keep the reference to the new image and will free it himself.
 *
 * \throws	ExceptionDimension	the images do no have the same size
 *
 * \param[in]	img 	the new image to use
 */
void Block::SubstituteGradient(const SImageGradient &img)
{
	if (!img)
	{
		FlushGradient();
		return;
	}
	int w = GetAbsoluteBBox().GetWidth();
	int h = GetAbsoluteBBox().GetHeight();
	if ((int(img->GetWidth()) != w) || (int(img->GetHeight()) != h))
	{
		throw ExceptionDimension(StringUTF8("bool Block::SubstituteGradient(const SImageGradient &img): ") +
				_("Wrong image dimensions."));
	}
	FlushGradient();
	buffGradient = img;
}

/*! \cond Internal */
struct interResolv
{
	/*! \brief Adds a pair of equivalent labels */
	void Add(int a, int b)
	{
		int found = -1;
		int foundwhat = 0; // -1 = a, +1 = b
		for (int tmp = 0; tmp < int(inter.size()); ++tmp)
		{
			if (inter[tmp].count(a))
			{
				inter[tmp].insert(b);
				found = tmp;
				foundwhat = -1;
				break;
			}
			if (inter[tmp].count(b))
			{
				inter[tmp].insert(a);
				found = tmp;
				foundwhat = +1;
				break;
			}
		}
		if (found == -1)
		{
			inter.push_back(std::set<int>());
			inter.back().insert(a);
			inter.back().insert(b);
		}
		else
		{
			if (foundwhat == -1) // found a
			{
				for (int tmp = 0; tmp < int(inter.size()); ++tmp)
				{
					if (tmp == found)
						continue;
					if (inter[tmp].count(b))
					{
						inter[found].insert(inter[tmp].begin(), inter[tmp].end());
						inter.erase(inter.begin() + tmp);
						break;
					}
				}
			}
			else
			{ // foundwhat == +1, found b
				for (int tmp = 0; tmp < int(inter.size()); ++tmp)
				{
					if (tmp == found)
						continue;
					if (inter[tmp].count(a))
					{
						inter[found].insert(inter[tmp].begin(), inter[tmp].end());
						inter.erase(inter.begin() + tmp);
						break;
					}
				}
			}
		}
	}
	// this ought to be a list since erase() are performed on it, but sweeping a list is slower than sweeping a vector
	std::vector<std::set<int> > inter; /*!< sets of equivalent labels */
	/*! \brief computes a map of equivalence */
	void Compile()
	{
		corresp.clear();
		for (std::set<int> &s : inter)
		{
			auto it = s.begin();
			int rep = *it;
			++it;
			for (; it != s.end(); ++it)
				corresp[*it] = rep;
		}
	}
	std::map<int,int> corresp; /*!< equivalence for each label */
};
/*! \endcond */

/*****************************************************************************/
/*!
 * Creates a child tree with connected components.
 *
 * Each child block's name is the index of the connected component it represents.
 *
 * Inspired from "Fast connected-component labeling" by Lifeng He, Yuyan Chao, Kenji Suzuki and Kesheng Wu.
 *
 * \throws	ExceptionIO	cannot open image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \param[in]	tree	The tree name add the block to
 *
 * \return	a SImageIntGray containing the indexes of the connected components or nullptr if failed
 */
UImageIntGray Block::ExtractCC(const String &tree)
{
	SImageBW bwi = GetBW(true);

	int num = 1;
	size_t w = bwi->GetWidth();
	size_t h = bwi->GetHeight();
	UImageIntGray imap = std::make_unique<ImageIntGray>(w, h, 0);
	interResolv ir;
	// c2 c3 c4
	// c1  C  .
	//  .  .  .
	// 1st pixel
	if (bwi->At(0) == pixel::BWBlack)
	{ // first CC found
		imap->At(0) = num;
		num += 1;
	}
	// 1st line
	for (size_t x = 1; x < w; ++x)
	{
		if (bwi->At(x) == pixel::BWBlack)
		{
			int pv = imap->At(x - 1);
			if (pv != 0)
			{ // continue previous CC
				imap->At(x) = pv;
			}
			else
			{ // new CC
				imap->At(x) = num;
				num += 1;
			}
		}
	}
	// all other lines
	size_t yoffset = 0;
	for (size_t y = 1; y < h; ++y)
	{
		yoffset += w;
		// 1st pixel of the line
		if (bwi->At(yoffset) == pixel::BWBlack)
		{
			// c3
			size_t c3 = yoffset - w;
			int cval = imap->At(c3);
			if (cval != 0)
			{
				imap->At(yoffset) = cval;
			}
			else
			{
				// c4
				cval = imap->At(c3 + 1);
				if (cval != 0)
				{
					imap->At(yoffset) = cval;
				}
				else
				{
					// new
					imap->At(yoffset) = num;
					num += 1;
				}
			}
		}
		// middle pixels
		for (size_t x = 1; x < w - 1; ++x)
		{
			size_t offset = x + yoffset;
			if (bwi->At(offset) == pixel::BWBlack)
			{
				// c3
				size_t c3 = offset - w;
				int cval = imap->At(c3);
				if (cval != 0)
				{
					imap->At(offset) = cval;
					continue;
				}
				// c1
				size_t c1 = offset - 1;
				size_t c4 = c3 + 1;
				cval = imap->At(c1);
				int cval4 = imap->At(c4);
				if (cval != 0)
				{
					imap->At(offset) = cval;
					if ((cval4 != 0) && (cval4 != cval))
						ir.Add(cval, cval4);
					continue;
				}
				// c2
				size_t c2 = c3 - 1;
				cval = imap->At(c2);
				if (cval != 0)
				{
					imap->At(offset) = cval;
					if ((cval4 != 0) && (cval4 != cval))
						ir.Add(cval, cval4);
					continue;
				}
				// c4
				if (cval4 != 0)
				{
					imap->At(offset) = cval4;
					continue;
				}
				// new
				imap->At(offset) = num;
				num += 1;
			}
		}
		// last pixel
		size_t offset = w - 1 + yoffset;
		if (bwi->At(offset) == pixel::BWBlack)
		{
			// c3
			size_t c3 = offset - w;
			int cval = imap->At(c3);
			if (cval != 0)
			{
				imap->At(offset) = cval;
				continue;
			}
			// c1
			size_t c1 = offset - 1;
			cval = imap->At(c1);
			if (cval != 0)
			{
				imap->At(offset) = cval;
				continue;
			}
			// c2
			size_t c2 = c3 - 1;
			cval = imap->At(c2);
			if (cval != 0)
			{
				imap->At(offset) = cval;
				continue;
			}
			// new
			imap->At(offset) = num;
			num += 1;
		}
	}
	// now merge the CCs and create the boxes
	std::map<int, Rect> bboxes;
	ir.Compile();
	FOREACHPIXEL(x, y, *imap)
	{
		int v = imap->At(x, y);
		if (v != 0)
		{
			auto it = ir.corresp.find(v);
			if (it != ir.corresp.end())
			{ // has to be changed
				v = it->second;
				imap->At(x, y) = v;
			}
			if (bboxes.find(v) == bboxes.end())
			{ // create bounding box
				//bboxes[v] = Rect(x, y, x, y);
				bboxes.insert(std::make_pair(v, Rect(int(x), int(y), int(x), int(y))));
			}
			else
			{ // grow bounding box
				Rect &r = bboxes[v];
				if (r.GetLeft() > int(x))
					r.SetLeft(int(x));
				if (r.GetTop() > int(y))
					r.SetTop(int(y));
				if (r.GetRight() < int(x))
					r.SetRight(int(x));
				if (r.GetBottom() < int(y))
					r.SetBottom(int(y));
			}
		}
	}
	// add sub blocks
	for (const auto &bbox : bboxes)
	{
		if (!bbox.second.IsValid())
		{
			continue;
		}
		AddChildRelative(tree, bbox.second, String(bbox.first));
	}

	return std::forward<UImageIntGray>(imap);
}

/*****************************************************************************/
/*!
 * Returns the mean width, height and area of the subblocks
 *
 * \throws	ExceptionInvalidArgument	tree not found
 *
 * \param[in]	tree	the name of the block tree
 * \param[out]	mwidth	the mean width
 * \param[out]	mheight	the mean height
 * \param[out]	marea	the mean area
 *
 * \return	true if success, false else
 */
void Block::GetTreeMeans(const String &tree, double *mwidth, double *mheight, double *marea) const
{
	if (!HasTree(tree))
		throw ExceptionInvalidArgument(StringUTF8("void Block::GetTreeMeans(const String &tree, "
					"double *mwidth, double *mheight, double *marea) const: ") + _("tree not found."));
	long w = 0, h = 0, a = 0;
	for (size_t tmp = 0; tmp < GetNbChildren(tree); tmp++)
	{
		SCBlock b = GetChild(tree, tmp);
		w += b->GetAbsoluteBBox().GetWidth();
		h += b->GetAbsoluteBBox().GetHeight();
		a += b->GetAbsoluteBBox().GetArea();
	}
	double tot = (double)GetNbChildren(tree);
	if (mwidth)
		*mwidth = double(w) / tot;
	if (mheight)
		*mheight = double(h) / tot;
	if (marea)
		*marea = double(a) / tot;
}

/*****************************************************************************/
/*!
 * Gets the number of blocks in a child tree
 *
 * \throws	ExceptionInvalidArgument	tree not found
 *
 * \param[in]		tree	The tree name
 *
 * \return		The number of children or -1 if the tree does not exist
 */
size_t Block::GetNbChildren(const String &tree) const
{
	if (child->Find(tree) == child->end())
		throw ExceptionInvalidArgument(StringUTF8("size_t Block::GetNbChildren(const String &tree) const: ") + _("tree not found."));
	const SVector v = std::static_pointer_cast<Vector>(child->Get(tree));
	return v->Size();
}

/*****************************************************************************/
/*!
 * Gets a block of a child tree
 *
 * \throws	ExceptionNotFound	tree not found
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]		tree	The tree name
 * \param[in]		num	The index of the subblock
 *
 * \return		A pointer to the block, nullptr if the tree does not exist or the number is erroneous
 */
SBlock Block::GetChild(const String &tree, size_t num)
{
	if (child->Find(tree) == child->end())
		throw ExceptionNotFound(StringUTF8("SBlock Block::GetChild(const String &tree, size_t num): ") + _("tree not found."));
	else if (num >= getChildList(tree)->Size())
		throw ExceptionDomain(StringUTF8("SBlock Block::GetChild(const String &tree, size_t num): ") + _("index out of bounds."));
	else
		return std::static_pointer_cast<Block>(getChildList(tree)->At(num));
}

/*****************************************************************************/
/*!
 * Gets a block of a child tree
 *
 * \throws	ExceptionNotFound	tree not found
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]		tree	The tree name
 * \param[in]		num	The index of the subblock
 *
 * \return		A pointer to the block, nullptr if the tree does not exist or the number is erroneous
 */
SCBlock Block::GetChild(const String &tree, size_t num) const
{
	if (child->Find(tree) == child->end())
		throw ExceptionNotFound(StringUTF8("SCBlock Block::GetChild(const String &tree, size_t num) const: ") + _("tree not found."));
	else
	{
		SCVector v = std::static_pointer_cast<const Vector>(child->Get(tree));
		if (num >= v->Size())
			throw ExceptionDomain(StringUTF8("SCBlock Block::GetChild(const String &tree, size_t num) const: ") + _("index out of bounds."));
		else
			return std::static_pointer_cast<const Block>(v->At(num));
	}
}

/*****************************************************************************/
/*!
 * Gets a block of a child tree
 *
 * \throws	ExceptionNotFound	tree or block not found
 *
 * \param[in]		tree	The tree name
 * \param[in]		name	The name of the subblock
 *
 * \return		A pointer to the first block having that name, nullptr if the tree does not exist or the number is erroneous
 */
SBlock Block::GetChild(const String &tree, const String &name)
{
	if (child->Find(tree) == child->end())
		throw ExceptionNotFound(StringUTF8("SBlock Block::GetChild(const String &tree, const String &name): ") + _("tree not found."));
	SVector v = std::static_pointer_cast<Vector>(child->Get(tree));
	for (SObject ob : v)
	{
		SBlock b(std::static_pointer_cast<Block>(ob));
		if (b->GetName() == name)
			return b;
	}
	throw ExceptionNotFound(StringUTF8("SBlock Block::GetChild(const String &tree, const String &name): ") + _("block not found."));
}

/*****************************************************************************/
/*!
 * Gets a block of a child tree
 *
 * \throws	ExceptionNotFound	tree or block not found
 *
 * \param[in]		tree	The tree name
 * \param[in]		name	The name of the subblock
 *
 * \return		A pointer to the first block having that name, nullptr if the tree does not exist or the number is erroneous
 */
SCBlock Block::GetChild(const String &tree, const String &name) const
{
	if (child->Find(tree) == child->end())
		throw ExceptionNotFound(StringUTF8("SCBlock Block::GetChild(const String &tree, const String &name) const: ") + _("tree not found."));
	SCVector v = std::static_pointer_cast<const Vector>(child->Get(tree));
	for (SCObject ob : v)
	{
		SCBlock b(std::static_pointer_cast<const Block>(ob));
		if (b->GetName() == name)
			return b;
	}
	throw ExceptionNotFound(StringUTF8("SCBlock Block::GetChild(const String &tree, const String &name) const: ") + _("block not found."));
}


/*****************************************************************************/
/*!
 * Removes a block of a child tree. The possible shared pointers remaining on the
 * block should not be used after the removal.
 *
 * \throws	ExceptionNotFound	tree not found
 * \throws	ExceptionDomain	index ouf of bounds
 *
 * \param[in]		tree	The tree name
 * \param[in]		num	The index of the subblock
 */
void Block::RemoveChild(const String &tree, size_t num)
{
	if (child->Find(tree) == child->end())
		throw ExceptionNotFound(StringUTF8("void Block::RemoveChild(const String &tree, size_t num): ") +
				_("tree not found."));

	if (num >= getChildList(tree)->Size())
		throw ExceptionDomain(StringUTF8("void Block::RemoveChild(const String &tree, size_t num): ") +
				_("index out of bounds."));
	SBlock b(std::static_pointer_cast<Block>((*getChildList(tree))[num]));
	b->parent = WBlock();
	getChildList(tree)->Remove(num);
}

/*****************************************************************************/
/*!
 * Removes a block of a child tree. The possible shared pointers remaining on the
 * block should not be used after the removal.
 *
 * \throws	ExceptionNotFound	tree or block not found
 *
 * \param[in]		tree	The tree name
 * \param[in]		name	The name of the subblock
 */
void Block::RemoveChild(const String &tree, const String &name)
{
	RemoveChild(tree, GetChild(tree, name));
}

/*****************************************************************************/
/*!
 * Removes the current block of a child tree. The possible shared pointers remaining on the
 * block should not be used after the removal.
 *
 * \throws	ExceptionNotFound	tree or block not found
 *
 * \param[in]		tree	The tree name
 * \param[in]		b	The block
 */
void Block::RemoveChild(const String &tree, SBlock b)
{
	if (child->Find(tree) == child->end())
		throw ExceptionNotFound(StringUTF8("void Block::RemoveChild(const String &tree, SBlock b): ") +
				_("tree not found."));

	if (getChildList(tree)->Contains(b))
	{
		b->parent = WBlock();
		getChildList(tree)->Remove(b);
	}
	else
		throw ExceptionNotFound(StringUTF8("void Block::RemoveChild(const String &tree, SBlock b): ") +
				_("block not found."));
}

/*!
 * Removes a set of blocks from a child tree
 *
 * \throws	ExceptionNotFound	tree not found
 *
 * \param[in]	tree	the tree to prune
 * \param[in]	toremove	the list of blocks to remove
 */
void Block::RemoveChildren(const String &tree, const std::set<SBlock> &toremove)
{
	if (child->Find(tree) == child->end())
		throw ExceptionNotFound(StringUTF8("void Block::RemoveChildren(const String &tree, const std::set<SBlock> &toremove): ") +
				_("tree not found."));
	SVector lst(getChildList(tree));
	lst->RemoveIf(
			[&toremove](const SObject &b)
			{ return toremove.find(std::static_pointer_cast<Block>(b)) != toremove.end(); }
			);
}

/*****************************************************************************/
/*!
 * Filters a child tree. Each child block with width and height lesser than the min values is removed.
 *
 * \throws	ExceptionNotFound	tree not found
 *
 * \param[in]		tree	The tree name
 * \param[in]		minw	The minimal width
 * \param[in]		minh	The minimal height
 */
void Block::FilterMinAnd(const String &tree, size_t minw, size_t minh)
{
	if (child->Find(tree) == child->end())
	{
		throw ExceptionNotFound(StringUTF8("void Block::FilterMinAnd(const String &tree, size_t minw, size_t minh): ") +
				_("tree not found."));
	}

	// Blocks to be removed are copied in a list first...
	std::set<SBlock> v;
	for (Vector::iterator it = getChildList(tree)->begin(); it != getChildList(tree)->end(); ++it)
	{
		SBlock b(std::static_pointer_cast<Block>(*it));
		if ((b->GetAbsoluteBBox().GetWidth() < int(minw)) && (b->GetAbsoluteBBox().GetHeight() < int(minh)))
		{
			v.insert(b);
		}
	}

	// ... then removed from child tree
	RemoveChildren(tree, v);
}

/*****************************************************************************/
/*!
 * Filters a child tree. Each child block with width or height lesser than the min values is removed.
 *
 * \throws	ExceptionNotFound	tree not found
 *
 * \param[in]		tree	The tree name
 * \param[in]		minw	The minimal width
 * \param[in]		minh	The minimal height
 */
void Block::FilterMinOr(const String &tree, size_t minw, size_t minh)
{
	if (child->Find(tree) == child->end())
		throw ExceptionNotFound(StringUTF8("void Block::FilterMinOr(const String &tree, size_t minw, size_t minh): ") +
				_("tree not found."));
	std::set<SBlock> v;
	for (Vector::iterator it = getChildList(tree)->begin(); it != getChildList(tree)->end(); ++it)
	{
		SBlock b(std::static_pointer_cast<Block>(*it));
		if ((b->GetAbsoluteBBox().GetWidth() < int(minw)) || (b->GetAbsoluteBBox().GetHeight() < int(minh)))
			v.insert(b);
	}
	RemoveChildren(tree, v);
}

/*****************************************************************************/
/*!
 * Filters a child tree. Each child block with width and height greater than the min values is removed.
 *
 * \throws	ExceptionNotFound	tree not found
 *
 * \param[in]		tree	The tree name
 * \param[in]		maxw	The maximal width
 * \param[in]		maxh	The maximal height
 */
void Block::FilterMaxAnd(const String &tree, size_t maxw, size_t maxh)
{
	if (child->Find(tree) == child->end())
		throw ExceptionNotFound(StringUTF8("void Block::FilterMaxAnd(const String &tree, size_t maxw, size_t maxh): ") +
				_("tree not found."));
	std::set<SBlock> v;
	for (Vector::iterator it = getChildList(tree)->begin(); it != getChildList(tree)->end(); ++it)
	{
		SBlock b(std::static_pointer_cast<Block>(*it));
		if ((b->GetAbsoluteBBox().GetWidth() > int(maxw)) && (b->GetAbsoluteBBox().GetHeight() > int(maxh)))
			v.insert(b);
	}
	RemoveChildren(tree, v);
}

/*****************************************************************************/
/*!
 * Filters a child tree. Each child block with width or height greater than the min values is removed.
 *
 * \throws	ExceptionNotFound	tree not found
 *
 * \param[in]		tree	The tree name
 * \param[in]		maxw	The maximal width
 * \param[in]		maxh	The maximal height
 */
void Block::FilterMaxOr(const String &tree, size_t maxw, size_t maxh)
{
	if (child->Find(tree) == child->end())
		throw ExceptionNotFound(StringUTF8("void Block::FilterMaxOr(const String &tree, size_t maxw, size_t maxh): ") +
				_("tree not found."));
	std::set<SBlock> v;
	for (Vector::iterator it = getChildList(tree)->begin(); it != getChildList(tree)->end(); ++it)
	{
		SBlock b(std::static_pointer_cast<Block>(*it));
		if ((b->GetAbsoluteBBox().GetWidth() > int(maxw)) || (b->GetAbsoluteBBox().GetHeight() > int(maxh)))
			v.insert(b);
	}
	RemoveChildren(tree, v);
}

/*****************************************************************************/
/*!
 * Filters a child tree. Each child block within a margin close to the borders is removed
 *
 * \throws	ExceptionNotFound	tree not found
 *
 * \param[in]		tree	The tree name
 * \param[in]		margin	The minimal distance to the border
 */
void Block::FilterBorders(const String &tree, size_t margin)
{
	if (child->Find(tree) == child->end())
		throw ExceptionNotFound(StringUTF8("void Block::FilterBorders(const String &tree, size_t margin): ") +
				_("tree not found."));
	std::set<SBlock> v;
	for (Vector::iterator it = getChildList(tree)->begin(); it != getChildList(tree)->end(); ++it)
	{
		SBlock b(std::static_pointer_cast<Block>(*it));
		if ((b->GetAbsoluteBBox().GetLeft() < int(GetAbsoluteBBox().GetLeft() + margin)) ||
				(b->GetAbsoluteBBox().GetTop() < int(GetAbsoluteBBox().GetTop() + margin)) ||
				(b->GetAbsoluteBBox().GetRight() > int(GetAbsoluteBBox().GetRight() - margin)) ||
				(b->GetAbsoluteBBox().GetBottom() > int(GetAbsoluteBBox().GetBottom() - margin)))
			v.insert(b);
	}
	RemoveChildren(tree, v);
}

/*****************************************************************************/
/*!
 * Filters a child tree. Each child block with Width > ratio * Height is removed.
 *
 * \throws	ExceptionNotFound	tree not found
 * \throws	ExceptionDomain	ratio is negative or null
 *
 * \param[in]		tree	The tree name
 * \param[in]		ratio	The maximal Width/Height ratio allowed (must be >0).
 */
void Block::FilterWidthRatio(const String &tree, double ratio)
{
	if (child->Find(tree) == child->end())
		throw ExceptionNotFound(StringUTF8("void Block::FilterWidthRatio(const String &tree, double ratio): ") +
				_("tree not found."));
	if (ratio <= 0)
		throw ExceptionDomain(StringUTF8("bool Block::FilterWidthRatio("
					"const String &tree, double ratio): ") +
				_("ratio is null or negative."));

	getChildList(tree)->RemoveIf(
			[ratio](const SObject &b)
			{
				const Rect bbox(std::static_pointer_cast<Block>(b)->GetAbsoluteBBox());
				return bbox.GetWidth() > ratio * bbox.GetHeight();
			}
			);
}

/*****************************************************************************/
/*!
 * Filters a child tree. Each child block with Height > ratio * Width is removed.
 *
 * \throws	ExceptionNotFound	tree not found
 * \throws	ExceptionDomain	ratio is negative or null
 *
 * \param[in]		tree	The tree name
 * \param[in]		ratio	The maximal Height/Width ratio allowed (must be >0)
 */
void Block::FilterHeightRatio(const String &tree, double ratio)
{
	if (child->Find(tree) == child->end())
		throw ExceptionNotFound(StringUTF8("void Block::FilterHeightRatio(const String &tree, double ratio): ") +
				_("tree not found."));
	if (ratio <= 0)
		throw ExceptionDomain(StringUTF8("bool Block::FilterHeightRatio("
					"const String &tree, double ratio): ") +
				_("ratio is null or negative."));

	getChildList(tree)->RemoveIf(
			[ratio](const SObject &b)
			{
				const Rect bbox(std::static_pointer_cast<Block>(b)->GetAbsoluteBBox());
				return bbox.GetHeight() > ratio * bbox.GetWidth();
			}
			);
}

/*****************************************************************************/
/*!
 * Merges overlapping children in a tree. Once it is done, it's possible that
 * a merged block resulting from overlapping ones overlaps another merged block.
 * In order to make sure that the result contains no overlapping blocks, this method
 * should be run until it returns false (until the re is no more overlapping blocks).
 *
 * \throws	ExceptionNotFound	tree not found
 * \throws	ExceptionDomain	overlap is negative
 *
 * \param[in]		tree	The tree name
 * \param[in]		overlap	The maximal overlapping to avoid merging. ie: overlap=0 means all overlapping children will be merged. overlap=1 means only fully overlaid children will be merged.
 * \param[in]		imap	the map image to update (can be null)
 *
 * \return		true if some children were merged, false else
 */
bool Block::MergeChildren(const String &tree, double overlap, ImageIntGray *imap)
{
	if (child->Find(tree) == child->end())
		throw ExceptionNotFound(StringUTF8("void Block::MergeChildren(const String &tree, double overlap, ImageIntGray *imap): ") +
				_("tree not found."));
	if (overlap < 0)
		throw ExceptionDomain(StringUTF8("void Block::MergeChildren(const String &tree, double overlap, ImageIntGray *imap): ") +
				_("overlap is negative."));

	std::map<size_t, std::map<size_t, size_t> > overlaps;
	SVector vtree(GetTree(tree));
	for (size_t b1 = 0; b1 < vtree->Size(); ++b1)
	{
		SBlock cb1(std::static_pointer_cast<Block>((*vtree)[b1]));
		Rect bb1(cb1->GetAbsoluteBBox());
		double ov1 = bb1.GetArea() * overlap;
		for (size_t b2 = b1 + 1; b2 < vtree->Size(); ++b2)
		{
			SBlock cb2(std::static_pointer_cast<Block>((*vtree)[b2]));
			Rect bb2(cb2->GetAbsoluteBBox());
			Rect rov = bb1 & bb2;
			if (rov.IsValid())
			{
				double rova = rov.GetArea();
				double ov2 = bb2.GetArea() * overlap;
				size_t dist = Min(Abs(bb1.GetCenterX() - bb2.GetCenterX()), Abs(bb1.GetCenterY() - bb2.GetCenterY()));
				if ((rova >= ov1) && (rova >= ov2))
				{
					if (ov1 >= ov2)
					{
						overlaps[b2][dist] = b1;
					}
					else
					{
						overlaps[b1][dist] = b2;
						break;
					}
				} // all two overlap
				else if (rova >= ov1)
				{
					overlaps[b1][dist] = b2;
					break;
				} // b1 overlaps
				else if (rova >= ov2)
				{
					overlaps[b2][dist] = b1;
				} // b2 overlaps
			} // intersection
		} // for b2
	} // for b1
	if (overlaps.empty())
		return false;

	// propagate
	std::map<size_t, size_t> change;
	for (auto it = overlaps.begin(); it != overlaps.end(); ++it)
	{
		size_t from = it->first;
		size_t to = it->second.begin()->second;
		auto next = overlaps.find(to);
		while (next != overlaps.end())
		{
			to = next->second.begin()->second;
			next = overlaps.find(to);
		}
		change[from] = to;
	}
	// merge
	std::set<SBlock> toremove;
	SVector lst(getChildList(tree));
	for (auto & elem : change)
	{
		SBlock bfrom(std::static_pointer_cast<Block>((*vtree)[elem.first]));
		SBlock bto(std::static_pointer_cast<Block>((*vtree)[elem.second]));
		int f = bfrom->GetName().ToInt();
		int t = bto->GetName().ToInt();
		//toremove.insert(it->first);
		toremove.insert(std::static_pointer_cast<Block>((*lst)[elem.first]));
		bto->SetAbsoluteBBox(bto->GetAbsoluteBBox() | bfrom->GetAbsoluteBBox());
		if (imap)
		{
			for (const Point2DInt &p : bfrom->GetAbsoluteBBox())
			{
				if (imap->At(p.X, p.Y) == f)
					imap->At(p.X, p.Y) = t;
			}
		}
	}
	RemoveChildren(tree, toremove);

	return true;
}

/*****************************************************************************/
/*!
 * Merges two subblocks.
 *
 * \throws	ExceptionNotFound	tree not found
 * \throws	ExceptionLogic	identical indexes
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	tree	the name of the tree
 * \param[in]	index1 the index of the child that will be updated
 * \param[in]	index2 the index of the child that will be removed
 * \param[in]	imap	the map image to update (can be nullptr)
 */
void Block::MergeSiblings(const String &tree, size_t index1, size_t index2, ImageIntGray *imap)
{
	if (!HasTree(tree))
	{
		throw ExceptionNotFound(StringUTF8("bool Block::MergeSiblings("
					"const String &tree, size_t index1, size_t index1, "
					"ImageIntGray *imap): ") + _("Tree not found."));
	}
	if (index1 == index2)
	{
		throw ExceptionLogic(StringUTF8("bool Block::MergeSiblings("
					"const String &tree, size_t index1, size_t index1, "
					"ImageIntGray *imap): ") + _("Identical indexes."));
	}
	if ((index1 >= GetNbChildren(tree)) || (index2 >= GetNbChildren(tree)))
	{
		throw ExceptionDomain(StringUTF8("bool Block::MergeSiblings("
					"const String &tree, size_t index1, size_t index1, "
					"ImageIntGray *imap): ") + _("Index out of bounds."));
	}
	// update map image
	if (imap)
	{
		int v1, v2;
		v1 = (int)GetChild(tree, index1)->GetName().ToInt();
		v2 = (int)GetChild(tree, index2)->GetName().ToInt();
		for (Rect::iterator it = GetChild(tree, index2)->GetAbsoluteBBox().begin();
				it != GetChild(tree, index2)->GetAbsoluteBBox().end(); ++it)
		{
			if (imap->At(it->X, it->Y) == v2)
				imap->At(it->X, it->Y) = v1;
		}
	}
	// update child 1
	GetChild(tree, index1)->bbox |= GetChild(tree, index2)->GetAbsoluteBBox();
	// copy childrens from child2 into child1
	const std::vector<String> child2TreeNames = GetChild(tree, index2)->GetTreeNames();
	for (const auto & child2TreeName : child2TreeNames)
		for (size_t i = 0 ; i < GetChild(tree, index2)->GetNbChildren(child2TreeName) ; ++i)
		{
			SBlock child = GetChild(tree, index2)->GetChild(child2TreeName, i);
			child->parent = GetChild(tree, index1);
			GetChild(tree, index1)->getChildList(child2TreeName)->PushBack(child);
		}
	// remove child 2
	RemoveChild(tree, index2);
}

/*****************************************************************************/
/*!
 * Merges two subblocks.
 *
 * \throws	ExceptionNotFound	tree or block not found
 *
 * \param[in]	tree	the name of the tree
 * \param[in]	sb1
 * \param[in]	sb2
 * \param[in]	imap	the map image to update (can be nullptr)
 *
 * \return	true if success, false else
 */
void Block::MergeSiblings(const String &tree, Block &sb1, Block &sb2, ImageIntGray *imap)
{
	if (!HasTree(tree))
	{
		throw ExceptionNotFound(StringUTF8("bool Block::MergeSiblings("
					"const String &tree, Block &sb1, Block &sb2, "
					"ImageIntGray *imap): ") + _("Tree not found."));
	}
	// look for indexes
	int index1 = -1, index2 = -1;
	for (int tmp = 0; tmp < int(GetNbChildren(tree)); tmp++)
	{
		if (GetChild(tree, tmp).get() == &sb1)
			index1 = tmp;
		if (GetChild(tree, tmp).get() == &sb2)
			index2 = tmp;
		if ((index1 != -1) && (index2 != -1))
			break;
	}
	if ((index1 == -1) || (index2 == -1))
	{
		throw ExceptionNotFound(StringUTF8("bool Block::MergeSiblings("
					"const String &tree, Block &sb1, Block &sb2, "
					"ImageIntGray *imap): ") + _("Cannot find subblock."));
	}

	MergeSiblings(tree, index1, index2, imap);
}

/*!
 * Gets a reference to the topmost parent of the block
 *
 * \return A reference to the topmost parent of the block
 */
WBlock Block::GetTop()
{
	if (parent.expired())
		return self;
	else
		return parent.lock()->GetTop();
}

/*!
 * Checks ascendency between two blocks.
 *
 * \param[in]	b	the candidate parent
 *
 * \return true if b is a parent of the current bloc, false else
 */
bool Block::IsParent(const Block &b) const
{
	if (parent.expired())
		return false;
	if (parent.lock().get() == &b)
		return true;
	return parent.lock()->IsParent(b);
}

/*!
 * Returns an iterator on the first pixel of the block
 *
 * \throws	ExceptionInvalidArgument	tree not found
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	tree the name of the subblock tree
 * \param[in]	num	the index of the block to iterate
 *
 * \return	an iterator, invalid if the tree does not exist or the index is out of range
 */
Block::pixel_iterator Block::PixelBegin(const String &tree, size_t num) const
{
	if (child->Find(tree) == child->end())
		throw ExceptionInvalidArgument(StringUTF8("Block::pixel_iterator Block::PixelBegin(const String &tree, size_t num) const: ") +
				_("tree not found."));
	const SVector v = std::static_pointer_cast<Vector>(child->Get(tree));
	if (num >= v->Size())
		throw ExceptionDomain(StringUTF8("Block::pixel_iterator Block::PixelBegin(const String &tree, size_t num) const: ") +
				_("index out of bounds."));
	const SBlock b(std::static_pointer_cast<Block>(v->At(num)));
	Rect r = b->GetAbsoluteBBox();
	r.Translate(-bbox.GetLeft(), -bbox.GetTop());
	return r.begin();
}
/*!
 * Returns an iterator on the first pixel of the block
 *
 * \throws	ExceptionInvalidArgument	null block or block is not a child
 *
 * \param[in]	b	the subblock to iterate
 *
 * \return	an iterator, invalid if the block is not a descendant
 */
Block::pixel_iterator Block::PixelBegin(const SBlock &b) const
{
	if (!b || !b->IsParent(*this))
		throw ExceptionInvalidArgument(StringUTF8("Block::pixel_iterator Block::PixelBegin(const SBlock &b) const: ") +
				_("null block or block is not a child."));
	Rect r = b->GetAbsoluteBBox();
	r.Translate(-bbox.GetLeft(), -bbox.GetTop());
	return r.begin();
}
/*!
 * Returns an iterator after the last pixel of the block
 *
 * \throws	ExceptionInvalidArgument	tree not found
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	tree the name of the subblock tree
 * \param[in]	num	the index tof the block to iterate
 *
 * \return	an iterator, invalid if the tree does not exist or the index is out of range
 */
Block::pixel_iterator Block::PixelEnd(const String &tree, size_t num) const
{
	if (child->Find(tree) == child->end())
		throw ExceptionInvalidArgument(StringUTF8("Block::pixel_iterator Block::PixelEnd(const String &tree, size_t num) const: ") +
				_("tree not found."));
	const SVector v = std::static_pointer_cast<Vector>(child->Get(tree));
	if (num >= v->Size())
		throw ExceptionDomain(StringUTF8("Block::pixel_iterator Block::PixelEnd(const String &tree, size_t num) const: ") +
				_("index out of bounds."));
	return Block::pixel_iterator();
}
/*!
 * Returns an iterator after the last pixel of the block
 *
 * \throws	ExceptionInvalidArgument	null block or block is not a child
 *
 * \param[in]	b	the subblock to iterate
 *
 * \return	an iterator, invalid if the block is not a descendant
 */
Block::pixel_iterator Block::PixelEnd(const SBlock &b) const
{
	if (!b || !b->IsParent(*this))
		throw ExceptionInvalidArgument(StringUTF8("Block::pixel_iterator Block::PixelEnd(const SBlock &b) const: ") +
				_("null block or block is not a child."));
	return Block::pixel_iterator();
}

/*!
 * Returns an iterator on the first pixel of the block
 *
 * \throws	ExceptionInvalidArgument	tree not found
 * \throws	ExceptionDomain	index out of bounds
 * \throws	ExceptionIO	cannot open bw image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \param[in]	tree the name of the subblock tree
 * \param[in]	num	the index tof the block to iterate
 * \param[in]	mask_value	crn::pixel::BWWhite or crn::pixel::BWBlack
 *
 * \return	an iterator, invalid if the tree does not exist or the index is out of range
 */
Block::masked_pixel_iterator Block::MaskedPixelBegin(const String &tree, size_t num, pixel::BW mask_value)
{
	if (child->Find(tree) == child->end())
		throw ExceptionInvalidArgument(StringUTF8("Block::masked_pixel_iterator Block::MaskedPixelBegin(const String &tree, size_t num, pixel::BW mask_value): ") +
				_("tree not found."));
	SVector v = std::static_pointer_cast<Vector>(child->Get(tree));
	if (num >= v->Size())
		throw ExceptionDomain(StringUTF8("Block::masked_pixel_iterator Block::MaskedPixelBegin(const String &tree, size_t num, pixel::BW mask_value): ") +
				_("index out of bounds."));
	SBlock b(std::static_pointer_cast<Block>(v->At(num)));
	Rect r = b->GetAbsoluteBBox();
	r.Translate(-bbox.GetLeft(), -bbox.GetTop());
	return Block::masked_pixel_iterator(r, b->GetBW(true), r.GetLeft(), r.GetTop(), mask_value);
}

/*!
 * Returns an iterator on the first pixel of the block
 *
 * \throws	ExceptionInvalidArgument	tree not found
 * \throws	ExceptionIO	cannot open bw image
 * \throws	ExceptionRuntime	unsupported image format (not BW, Gray nor RGB)
 *
 * \param[in]	b	the subblock to iterate
 * \param[in]	mask_value	crn::pixel::BWWhite or crn::pixel::BWBlack
 *
 * \return	an iterator, invalid if the block is not a descendant
 */
Block::masked_pixel_iterator Block::MaskedPixelBegin(const SBlock &b, pixel::BW mask_value)
{
	if (!b || !b->IsParent(*this))
		throw ExceptionInvalidArgument(StringUTF8("Block::masked_pixel_iterator Block::MaskedPixelBegin(const SBlock &b, pixel::BW mask_value): ") +
				_("tree not found."));
	Rect r = b->GetAbsoluteBBox();
	r.Translate(-bbox.GetLeft(), -bbox.GetTop());
	return Block::masked_pixel_iterator(r, b->GetBW(true), r.GetLeft(), r.GetTop(), mask_value);
}

/*!
 * Returns an iterator after the last pixel of the block
 *
 * \throws	ExceptionInvalidArgument	tree not found
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	tree the name of the subblock tree
 * \param[in]	num	the index tof the block to iterate
 * \param[in]	mask_value	crn::pixel::BWWhite or crn::pixel::BWBlack
 *
 * \return	an iterator, invalid if the tree does not exist or the index is out of range
 */
Block::masked_pixel_iterator Block::MaskedPixelEnd(const String &tree, size_t num, pixel::BW mask_value)
{
	if (child->Find(tree) == child->end())
		throw ExceptionInvalidArgument(StringUTF8("Block::masked_pixel_iterator Block::MaskedPixelEnd(const String &tree, size_t num, pixel::BW mask_value): ") +
				_("tree not found."));
	SCVector v = std::static_pointer_cast<Vector>(child->Get(tree));
	if (num >= v->Size())
		throw ExceptionDomain(StringUTF8("Block::masked_pixel_iterator Block::MaskedPixelEnd(const String &tree, size_t num, pixel::BW mask_value): ") +
				_("index out of bounds."));
	return Block::masked_pixel_iterator();
}

/*!
 * Returns an iterator after the last pixel of the block
 *
 * \throws	ExceptionInvalidArgument	null block or block is not a child
 *
 * \param[in]	b	the subblock to iterate
 * \param[in]	mask_value	crn::pixel::BWWhite or crn::pixel::BWBlack
 *
 * \return	an iterator, invalid if the block is not a descendant
 */
Block::masked_pixel_iterator Block::MaskedPixelEnd(const SBlock &b, pixel::BW mask_value)
{
	if (!b || !b->IsParent(*this))
		throw ExceptionInvalidArgument(StringUTF8("Block::masked_pixel_iterator Block::MaskedPixelEnd(const SBlock &b, pixel::BW mask_value): ") +
				_("null block or block is not a child."));
	return Block::masked_pixel_iterator();
}

/*!
 * Sorts a child tree
 *
 * \throws	ExceptionInvalidArgument	tree not found
 * \throws	ExceptionDomain	invalid direction
 *
 * \param[in]	name	the name of the child tree to sort
 * \param[in]	direction	the direction (Direction::LEFT for left to right, Direction::TOP for top to bottom, Direction::RIGHT for right to left or Direction::BOTTOM for bottom to top).
 * Use only one direction at a time, don't try to combinate the directions with the | operator.
 *
 * \return	true if success, false else
 */
void Block::SortTree(const String &name, Direction direction)
{
	if (!HasTree(name))
	{
		throw ExceptionInvalidArgument(StringUTF8("void Block::SortTree(const String &name, Direction direction): ")
				+ _("Tree not found."));
	}
	if (direction == Direction::LEFT)
	{
		std::sort(BlockBegin(name), BlockEnd(name), [](const SObject &b1, const SObject &b2)
					{
						return std::static_pointer_cast<Block>(b1)->GetAbsoluteBBox().GetLeft() <
										std::static_pointer_cast<Block>(b2)->GetAbsoluteBBox().GetLeft();
					}
				);
	}
	else if (direction == Direction::RIGHT)
	{
		std::sort(BlockBegin(name), BlockEnd(name), [](const SObject &b1, const SObject &b2)
					{
						return std::static_pointer_cast<Block>(b1)->GetAbsoluteBBox().GetRight() >
										std::static_pointer_cast<Block>(b2)->GetAbsoluteBBox().GetRight();
					}
				);
	}
	else if (direction == Direction::TOP)
	{
		std::sort(BlockBegin(name), BlockEnd(name), [](const SObject &b1, const SObject &b2)
					{
						return std::static_pointer_cast<Block>(b1)->GetAbsoluteBBox().GetTop() <
										std::static_pointer_cast<Block>(b2)->GetAbsoluteBBox().GetTop();
					}
				);
	}
	else if (direction == Direction::BOTTOM)
	{
		std::sort(BlockBegin(name), BlockEnd(name), [](const SObject &b1, const SObject &b2)
					{
						return std::static_pointer_cast<Block>(b1)->GetAbsoluteBBox().GetBottom() >
										std::static_pointer_cast<Block>(b2)->GetAbsoluteBBox().GetBottom();
					}
				);
	}
	else
	{
		throw ExceptionDomain(StringUTF8("void Block::SortTree(const String &name, Direction direction): ")
				+ _("Wrong direction."));
	}
}

/*! Refreshes the source images */
void Block::refreshSources()
{
	if (!parent.expired())
	{
		srcRGB = parent.lock()->srcRGB;
		srcGray = parent.lock()->srcGray;
		srcBW = parent.lock()->srcBW;
		srcGradient = parent.lock()->srcGradient;
	}
	for (crn::Map::pair p : child)
	{
		SVector l(std::static_pointer_cast<Vector>(p.second));
		for (SObject b : l)
		{
			std::static_pointer_cast<Block>(b)->refreshSources();
		}
	}
}

