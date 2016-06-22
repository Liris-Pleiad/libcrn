/* Copyright 2006-2016 Yann LEYDIER, CoReNum, INSA-Lyon, ENS-Lyon
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
 * file: CRNDocument.h
 * \author Yann LEYDIER
 */

#ifndef CRNDOCUMENT_HEADER
#define CRNDOCUMENT_HEADER

#include <vector>
#include <CRNComplexObject.h>
#include <CRNBlock.h>
#include <CRNDocumentPtr.h>
#include <CRNData/CRNForeach.h>
#include <CRNIO/CRNPath.h>
#ifdef CRN_USING_HARU
#	include <CRNUtils/CRNPDFAttributes.h>
#endif

/*! \defgroup document Document images
 * \ingroup	imaging */

namespace crn
{
	class Progress;

	/****************************************************************************/
	/*! \brief Document utility class.
	 *
	 * This class represents a document (book, volume, etc.).
	 *
	 * \author 	Yann LEYDIER
	 * \date		25 August 2006
	 * \version 0.2
	 * \ingroup document
	 */
	class Document: public Savable
	{
		public:
			/*! \brief Constructor */
			Document();
			/*! \brief Destructor */
			virtual ~Document() override;

			Document(const Document &) = delete;
			Document& operator=(const Document &) = delete;
			Document(Document &&) = default;
			Document& operator=(Document &&) = default;

			/*! \brief Sets the base name of the document if any */
			void SetBasename(const Path &s) { basename = s; }
			/*! \brief Sets the author of the document */
			void SetAuthor(const String &s) { author = s; }
			/*! \brief Sets the date of the document */
			void SetDate(const String &s) { date = s; }

			/*! \brief Gets the file base name of the document */
			const Path& GetBasename() const noexcept { return basename; }
			/*! \brief Gets the author of the document */
			const String& GetAuthor() const noexcept { return author; }
			/*! \brief Gets the date of the document */
			const String& GetDate() const noexcept { return date; }
			/*! \brief Gets the list of the image files of the document */
			std::vector<Path> GetFilenames() const;
			/*! \brief Gets the list of the view ids of the document */
			std::vector<String> GetViewIds() const;

			/*! \brief Adds a new image */
			String AddView(const Path &fname);
			/*! \brief Inserts a new image */
			String InsertView(const Path &fname, size_t pos);
			/*! \brief Removes a view */
			void RemoveView(const Path &fname);
			/*! \brief Removes a view */
			void RemoveView(size_t num);
			/*! \brief Removes a view */
			void RemoveView(const String &id);
			/*! \brief Returns a pointer to a view */
			SBlock GetView(size_t num) const;
			/*! \brief Returns a pointer to a view */
			SBlock GetView(const String &id) const;
			/*! \brief Returns a pointer to a view */
			SBlock GetView(const Path &fname) const;
			/*! \brief Returns the index of a view */
			size_t GetViewIndex(const String &id) const;
			/*! \brief Returns the index of a view */
			size_t GetViewIndex(const Path &fname) const;
			/*! \brief Returns the id of a view */
			String GetViewId(size_t num) const;
			/*! \brief Returns the id of a view */
			String GetViewId(const Path &fname) const;
			/*! \brief Returns the filename of a view */
			Path GetViewFilename(size_t num) const;
			/*! \brief Returns the filename of a view */
			Path GetViewFilename(const String &id) const;
			/*! \brief Returns the number of views */
			size_t GetNbViews() const noexcept { return views.size(); }
			/*! \brief Reorders the views */
			void ReorderViewsFrom(const std::vector<size_t> &from);
			/*! \brief Reorders the views */
			void ReorderViewsTo(const std::vector<size_t> &to);
			/*! \brief Removes all views and unsets all data */
			void Clear();

			/*! \brief Returns the path of the thumbnails */
			Path GetThumbnailPath() const;
			/*! \brief Returns a thumbnail of a view (cached) */
			UImage GetThumbnail(size_t index, bool refresh = false) const;
			/*! \brief Returns a thumbnail of a view (cached) */
			UImage GetThumbnail(const String &id, bool refresh = false) const;
			/*! \brief Returns the filename of a thumbnail of a view (cached) */
			Path GetThumbnailFilename(size_t index, bool refresh = false) const;
			/*! \brief Returns the filename of a thumbnail of a view (cached) */
			Path GetThumbnailFilename(const String &id, bool refresh = false) const;

			/*! \brief Iterator on the blocks of the document */
			class iterator: public std::iterator<std::input_iterator_tag, SBlock>
			{
				public:
					iterator(const Document *d, size_t n):doc(d),num(n) { init_block(); }
					iterator(const iterator &it):doc(it.doc),num(it.num) { init_block(); }
					iterator(iterator&&) = default;
					iterator& operator=(const iterator &it) { doc = it.doc; num = it.num;  init_block(); return *this; }
					iterator& operator=(iterator&&) = default;
					iterator& operator++() { num += 1; init_block(); return *this; }
					iterator operator++(int nouse) { iterator it = *this; ++*this; return it; }
					bool operator==(const iterator &it) const noexcept { return (doc == it.doc) && (num == it.num); }
					bool operator!=(const iterator &it) const noexcept { return (doc != it.doc) || (num != it.num); }
					value_type operator*() const noexcept { return b; }
					value_type operator->() const noexcept { return b; }
					size_t GetIndex() const noexcept { return num; }
					String GetId() const { return doc->GetViewId(num); }
					Path GetFilename() const { return doc->GetViewFilename(num); }
				private:
					void init_block() { if (num < doc->views.size()) b = doc->GetView(num); else b = nullptr; }
					const Document *doc;
					SBlock b;
					size_t num;
			};
			/*! \brief Returns an iterator to the first block */
			iterator begin() const { return iterator(this, 0); }
			/*! \brief Returns an iterator after the last block */
			iterator end() const { return iterator(this, views.size()); }

			/*! \brief Returns the default directory where the documents are saved */
			static Path GetDefaultDirName();

#ifdef CRN_USING_HARU
			/*! \brief Exports the views to a PDF file */
			void ExportPDF(const Path &fname, const PDF::Attributes &attr = PDF::Attributes(), Progress *prog = nullptr) const;
#endif

			static size_t GetThumbWidth() noexcept;
			static size_t GetThumbHeight() noexcept;
			static void SetThumbWidth(size_t w);
			static void SetThumbHeight(size_t h);

		private:
			/*! \brief Adds a new image with a given id */
			void addView(const Path &fname, const String &id);

			/*! \brief Completes a relative file name with the default path */
			virtual Path completeFilename(const Path &fn) const override;
			/*! \brief Loads the object from an XML file (Unsafe) */
			virtual void load(const Path &fname) override;
			/*! \brief Saves the object to an XML file (Unsafe) */
			virtual void save(const Path &fname) override;

			/*! \brief Creates a thumbnail image from an image filename */
			UImage createThumbnail(const Path &imagename) const;

			/*! \brief Creates a new unique id for views */
			String createNewId() const;
			/*! \internal
			 */
			struct view
			{
				/*! \brief Constructor with no pointer */
				view(const Path &fname = "", const String &unique_id = U""):filename(fname),id(unique_id) {}
				/* using default copy constructor */
				/*! \brief Compares only the file names */
				bool operator==(const view &other) const noexcept { return filename == other.filename; }
				Path filename; /*!< file name */
				String id; /*!< unique id */
				mutable WBlock ptr; /*!< weak pointer to the block */
			};

			std::vector<view> views; /*!< The views */

			Path basename; /*!< The base directory to save the views XML files */
			String author; /*!< The author of the document */
			String date; /*!< The date of the document */

			static const Path thumbdir; /*!< Relative to the thumbnails */
			static size_t thumbWidth; /*!< Global setting for new thumbnails' width */
			static size_t thumbHeight; /*!< Global setting for new thumbnails' height */
	};

	/*! \brief Number of views in a document */
	inline size_t Size(const Document &doc) noexcept { return doc.GetNbViews(); }
}
CRN_ADD_RANGED_FOR_TO_POINTERS(crn::Document)
#endif
