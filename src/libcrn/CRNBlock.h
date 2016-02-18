/* Copyright 2006-2016 INSA Lyon, CoReNum
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
 * file: CRNBlock.h
 * \author Yann LEYDIER, Asma OUJI
 */

#ifndef CRNBLOCK_HEADER
#define CRNBLOCK_HEADER

#include <CRNComplexObject.h>
#include <CRNGeometry/CRNRect.h>
#include <CRNData/CRNMap.h>
#include <CRNData/CRNVector.h>
#include <CRNImage/CRNImage.h>
#include <CRNImage/CRNImageGradient.h>
#include <CRNBlockPtr.h>

namespace crn
{
	namespace xml
	{
		class Document;
	}
	/****************************************************************************/
	/*! \brief A block.
	 *
	 * A block is a container of other blocks.
	 * Each block refers to a source image and has a local copy of a part of it.
	 * A top block is typically a page or a double page.
	 *
	 * \author  Yann Leydier
	 * \date    16 August 2006
	 * \version 0.1
	 * \ingroup document
	 */

	class Block: public ComplexObject
	{
		public:
			/*! \brief Top block creator */
			static SBlock New(const SImage &src, const String &nam = U"");
			/*! \brief Constructor from filenames */
			static SBlock New(const Path &ifname, const Path &xfname, const String &nam = U"");
			/*! \brief Destructor */
			virtual ~Block() override;

			Block(const Block&) = delete;
			Block& operator=(const Block&) = delete;
			Block(Block&&) = delete;
			Block& operator=(Block&&) = delete;

			/*! \brief Returns the id of the class */
			virtual const String& GetClassName() const override { static const String cn(U"Block"); return cn; }
			/*! \brief This is is a ComplexObject */
			virtual Protocol GetClassProtocols() const noexcept override { return Protocol::ComplexObject; }

			/*! \brief Sets the default filename of the block */
			void SetFilename(const Path &nam) { setFilename(nam); }

			/*! \brief Gets the absolute bounding box of the block */
			const Rect& GetAbsoluteBBox() const noexcept { return bbox; }
			/*! \brief Gets the relative bounding box of the block */
			Rect GetRelativeBBox() const;
			/*! \brief Sets the absolute bounding box of the block */
			void SetAbsoluteBBox(const Rect &newbox);
			/*! \brief Sets the absolute bounding box of the block */
			void SetRelativeBBox(Rect newbox);

			/*! \brief Gets a reference to the parent of the block */
			WBlock GetParent() { return parent; }
			/*! \brief Gets a reference to the parent of the block */
			WCBlock GetParent() const { return parent; }
			/*! \brief Gets a reference to the topmost parent of the block */
			WBlock GetTop();
			/*! \brief Gets the name of the parent tree of the block */
			const String& GetParentTree() const noexcept { return parenttree; }
			/*! \brief Checks if a block is a parent of current */
			bool IsParent(const Block &b) const;

			/*! \brief Adds a child to current block */
			SBlock AddChildAbsolute(const String &tree, Rect clip);
			/*! \brief Adds a child to current block with a name */
			SBlock AddChildAbsolute(const String &tree, Rect clip, const String &name);
			/*! \brief Adds a child to current block */
			SBlock AddChildAbsoluteAt(const String &tree, Rect clip, size_t pos);
			/*! \brief Adds a child to current block with a name */
			SBlock AddChildAbsoluteAt(const String &tree, Rect clip, const String &name, size_t pos);
			/*! \brief Adds a child to current block */
			SBlock AddChildRelative(const String &tree, Rect clip);
			/*! \brief Adds a child to current block with a name */
			SBlock AddChildRelative(const String &tree, Rect clip, const String &name);
			/*! \brief Adds a child to current block */
			SBlock AddChildRelativeAt(const String &tree, Rect clip, size_t pos);
			/*! \brief Adds a child to current block with a name */
			SBlock AddChildRelativeAt(const String &tree, Rect clip, const String &name, size_t pos);
			/*! \brief Gets the number of blocks in a child tree */
			size_t GetNbChildren(const String &tree) const;
			/*! \brief Gets a block of a child tree */
			SBlock GetChild(const String &tree, size_t num);
			/*! \brief Gets a block of a child tree */
			SCBlock GetChild(const String &tree, size_t num) const;
			/*! \brief Gets a block of a child tree */
			SBlock GetChild(const String &tree, const String &name);
			/*! \brief Gets a block of a child tree */
			SCBlock GetChild(const String &tree, const String &name) const;
			/*! \brief Removes a block of a child tree */
			void RemoveChild(const String &tree, size_t num);
			/*! \brief Removes a block of a child tree */
			void RemoveChild(const String &tree, const String &name);
			/*! \brief Removes a block of a child tree */
			void RemoveChild(const String &tree, SBlock b);
			/*! \brief Removes a set of blocks from a child tree */
			void RemoveChildren(const String &tree, const std::set<SBlock> &toremove);

			/*! \brief Gets the list of the tree names */
			std::vector<String> GetTreeNames() const;
			/*! \brief Checks if a child tree exists */
			bool HasTree(const String &tname) const;
			/*! \brief Deletes a child tree */
			void RemoveTree(const String &tname);

			/*! \brief Appends child trees from a file */
			bool Append(const Path &fname);
			/*! \brief Saves the child trees into a file */
			void Save(const Path &fname);
			/*! \brief Saves the child trees into the default file 
			 *
			 * Saves the child trees into the default file
			 *
			 * \throws	ExceptionInvalidArgument	empty file name
			 * \throws	ExceptionIO	cannot save XML file
			 */
			void Save() { Save(GetFilename()); }

			/*! \brief Returns a pointer to the local RGB buffer */
			SImageRGB GetRGB();
			/*! \brief Returns a pointer to the local gray buffer */
			SImageGray GetGray(bool create = true);
			/*! \brief Returns a pointer to the local b&w buffer */
			SImageBW GetBW(bool create = true);
			/*! \brief Returns a pointer to the local gradient buffer */
			SImageGradient GetGradient(bool create = true, double sigma = -1, size_t diffusemaxiter = 0, double diffusemaxdiv = std::numeric_limits<double>::max());
			/*! \brief Reloads the image */
			void ReloadImage();
			/*! \brief Frees the local buffers */
			void FlushAll(bool recursive = false);
			/*! \brief Frees the local RGB buffer */
			void FlushRGB(bool recursive = false);
			/*! \brief Frees the local gray buffer */
			void FlushGray(bool recursive = false);
			/*! \brief Frees the local b&w buffer */
			void FlushBW(bool recursive = false);
			/*! \brief Frees the local gradient buffer */
			void FlushGradient(bool recursive = false);
			/*! \brief Substitutes the RGB buffer with a new one */
			void SubstituteRGB(const SImageRGB &img);
			/*! \brief Substitutes the Gray buffer with a new one */
			void SubstituteGray(const SImageGray &img);
			/*! \brief Substitutes the BW buffer with a new one */
			void SubstituteBW(const SImageBW &img);
			/*! \brief Substitutes the Gradient buffer with a new one */
			void SubstituteGradient(const SImageGradient &img);

			/*! \brief Creates a child tree with connected components */
			UImageIntGray ExtractCC(const String &tree);
		
			/*! \brief Filters a child tree */
			void FilterMinAnd(const String &tree, size_t minw, size_t minh);
			/*! \brief Filters a child tree */
			void FilterMinOr(const String &tree, size_t minw, size_t minh);
			/*! \brief Filters a child tree */
			void FilterMaxAnd(const String &tree, size_t maxw, size_t maxh);
			/*! \brief Filters a child tree */
			void FilterMaxOr(const String &tree, size_t maxw, size_t maxh);
			/*! \brief Filters a child tree */
			void FilterBorders(const String &tree, size_t margin);
			/*! \brief Filters a child tree */
			void FilterWidthRatio(const String &tree, double ratio);
			/*! \brief Filters a child tree */
			void FilterHeightRatio(const String &tree, double ratio);
			/*! \brief Merges overlapping children in a tree */
			bool MergeChildren(const String &tree, double overlap, ImageIntGray *imap = nullptr);
			/*! \brief Merges two subblocks */
			void MergeSiblings(const String &tree, size_t index1, size_t index2, ImageIntGray *imap = nullptr);
			/*! \brief Merges two subblocks */
			void MergeSiblings(const String &tree, Block &sb1, Block &sb2, ImageIntGray *imap = nullptr);
			/*! \brief Returns the mean width, height and area of the subblocks */
			void GetTreeMeans(const String &tree, double *mwidth, double *mheight, double *marea) const;

#include <CRNBlockIterator.h>
			/*! \brief Returns an iterator on the first block of a tree */
			block_iterator BlockBegin(const String &tree);
			/*! \brief Returns an iterator after the last block of a tree */
			block_iterator BlockEnd(const String &tree);

			/*! \brief Returns a const iterator on the first block of a tree */
			const_block_iterator BlockBegin(const String &tree) const;
			/*! \brief Returns a const iterator after the last block of a tree */
			const_block_iterator BlockEnd(const String &tree) const;

			/*! \brief Returns a list of children. Can be used with CRN_FOREACH. */
			SVector GetTree(const String &name)
			{
				if (child->Find(name) == child->End()) return nullptr; 
				else return std::static_pointer_cast<Vector>(child->Get(name));
			}
			/*! \brief Returns a list of children. Can be used with CRN_FOREACH. */
			SCVector GetTree(const String &name) const
			{
				if (child->Find(name) == child->End()) return nullptr; 
				else return std::static_pointer_cast<const Vector>(child->Get(name));
			}

			/*! \brief Sorts a child tree */
			void SortTree(const String &name, Direction direction);

			/*! \brief Iterator on the pixels the block */
			using pixel_iterator = Rect::iterator;
			/*! \brief Returns an iterator on the first pixel of the block */
			pixel_iterator PixelBegin(const String &tree, size_t num) const;
			/*! \brief Returns an iterator on the first pixel of the block */
			pixel_iterator PixelBegin(const SBlock &b) const;
			/*! \brief Returns an iterator after the last pixel of the block */
			pixel_iterator PixelEnd(const String &tree, size_t num) const;
			/*! \brief Returns an iterator after the last pixel of the block */
			pixel_iterator PixelEnd(const SBlock &b) const;

			/*! \brief Iterator on the pixels the block with BW buffer as mask */
			class masked_pixel_iterator: public pixel_iterator
			{
				public:
					/*! \brief Invalid iterator constructor */
					masked_pixel_iterator() noexcept:pixel_iterator(),offsetx(0),offsety(0),value(0) {}
					/*! \brief Copy constructor */
					masked_pixel_iterator(const masked_pixel_iterator &it):
						pixel_iterator(it),mask(it.mask),offsetx(it.offsetx),offsety(it.offsety),value(it.value) 
					{
						if (mask)
							if (mask->At(pos.X - offsetx, pos.Y - offsety) != value)
								operator++();
					}
					/*! \brief Constructor */
					masked_pixel_iterator(const Rect &r, const SImageBW &ibw, int ox, int oy, pixel::BW val = pixel::BWBlack) noexcept:
						pixel_iterator(r),mask(ibw),offsetx(ox),offsety(oy),value(val) 
					{
						if (mask->At(pos.X - offsetx, pos.Y - offsety) != value)
							operator++();
					}
					/*! \brief Destructor */
					virtual ~masked_pixel_iterator() override {}
					/*! \brief Copy operator */
					masked_pixel_iterator& operator=(const masked_pixel_iterator &it) noexcept
					{
						pixel_iterator::operator=(it);
						mask = it.mask;
						value = it.value;
						offsetx = it.offsetx;
						offsety = it.offsety;
						return *this; 
					}
					/*! \brief Go to next pixel in mask */
					virtual const masked_pixel_iterator& operator++() noexcept override
					{ 
						pixel_iterator::operator++(); 
						while (valid) 
						{ 
							if (mask->At(pos.X - offsetx, pos.Y - offsety) == value) 
								break; 
							pixel_iterator::operator++(); 
						} 
						return *this;
					}
				private:
					SCImageBW mask; /*!< the binary mask */
					int offsetx, offsety; /*!< current position */
					pixel::BW value; /*!< mask value */
			};
			/*! \brief Returns a masked iterator on the first pixel of the block */
			masked_pixel_iterator MaskedPixelBegin(const String &tree, size_t num, pixel::BW mask_value = pixel::BWBlack);
			/*! \brief Returns a masked iterator on the first pixel of the block */
			masked_pixel_iterator MaskedPixelEnd(const String &tree, size_t num, pixel::BW mask_value = pixel::BWBlack);
			/*! \brief Returns a masked iterator after the last pixel of the block */
			masked_pixel_iterator MaskedPixelBegin(const SBlock &b, pixel::BW mask_value = pixel::BWBlack);
			/*! \brief Returns a masked iterator after the last pixel of the block */
			masked_pixel_iterator MaskedPixelEnd(const SBlock &b, pixel::BW mask_value = pixel::BWBlack);

		private:
			/*! \brief Top block creator */
			Block(const SImage &src, const String &nam = U"");
			/*! \brief Constructor from filenames */
			Block(const Path &ifname, const Path &xfname, const String &nam = U"");
			/*! \brief Internal. Child block creator */
			static SBlock create(const WBlock &par, const String &tree, const Rect &clip, const String &nam = U""); 
			/*! \brief Internal. Child block creator */
			Block(const WBlock &par, const String &tree, const Rect &clip, const String &nam = U""); 

			/*! \brief Internal. */
			void addToXml(xml::Document &parent); 
			/*! \brief Internal. */
			void addToXml(xml::Element &parent); 
			/*! \brief Internal. */
			void addTreeFromXml(xml::Element &bnode); 
			/*! \brief Loads the image corresponding to the block. */
			void openImage(void); 
			/*! \brief Refreshes the source images */
			void refreshSources();

			SMap child; /*!< The list of subblock trees */
			/*! \brief Returns a subblock tree */
			SVector getChildList(const String &name); 
			WBlock self; /*!< Shared pointer of the block. Expired if the block is not shared. */
			WBlock parent; /*!< Parent of the block. Expired if top block. */
			String parenttree; /*!< The name of the parent tree */

			Path imagefilename; /*!< File name of the image */
			bool image_is_open; /*!< Was the image already loaded? */
			SImageRGB srcRGB; /*!< Source RGB image */
			/*! \brief Returns the source RGB image */
			SImageRGB get_srcRGB(void); 
			SImageGray srcGray; /*!< Source gray image */
			/*! \brief Returns the source gray image */
			SImageGray get_srcGray(void); 
			SImageBW srcBW; /*!< Source black&white image */
			/*! \brief Returns the source BW image */
			SImageBW get_srcBW(void); 
			SImageGradient srcGradient; /*!< Source gradient image */
			/*! \brief Returns the source gradient image */
			SImageGradient get_srcGradient(void); 
			Rect bbox; /*!< Bounding box of the block */
			SImageRGB buffRGB; /*!< Local RGB buffer */
			SImageGray buffGray; /*!< Local gray buffer */
			SImageBW buffBW; /*!< Local black&white buffer */
			SImageGradient buffGradient; /*!< Local gradient buffer */
			double grad_sigma; /*!< Gradient property */
			size_t grad_diffusemaxiter; /*!< Gradient property */
			double grad_diffusemaxdiv; /*!< Gradient property */
	};
}
#endif
