/* Copyright 2011 CoReNum
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
 * file: CRNPDF.h
 * \author Yann LEYDIER
 */

#ifndef CRNPDF_HEADER
#define CRNPDF_HEADER

#include <CRN.h>
#ifdef CRN_USING_HARU
#include <CRNImage/CRNPixel.h>
#include <CRNUtils/CRNPDFAttributes.h>

namespace crn
{
	class Path;
	class Rect;

	/*! \brief Wrapper on Haru PDF
	 *
	 * Wrapper on Haru PDF
	 *
	 * \author	Yann LEYDIER
	 * \date	May 2011
	 * \version	0.1
	 * \ingroup	utils
	 * \defgroup	PDF	PDF	file creation
	 */
	namespace PDF
	{
		class Page;
		class Image;

		/*! \brief PDF Document
		 *
		 * PDF Document
		 *
		 * \author	Yann LEYDIER
		 * \date	May 2011
		 * \version	0.1
		 * \ingroup	PDF
		 */
		class Doc
		{
			public:
				/*! \brief Blank document creation */
				Doc();
				/*! \brief Document from attributes */
				Doc(const Attributes &attr);
				/*! \brief Release internal data */
				~Doc();

				/*! \brief No copy construction allowed */
				Doc(const Doc&) = delete;
				/*! \brief No copy allowed */
				Doc& operator=(const Doc&) = delete;
				/*! \brief No move construction allowed */
				Doc(Doc&&) = delete;
				/*! \brief No move allowed */
				Doc& operator=(Doc&&) = delete;

				/*! \brief Exports the document */
				void Save(const Path &file_name);

				/*! \brief Sets the initial display layout */
				void SetPageLayout(Attributes::Layout layout);
				/*! \brief Gets the initial display layout */
				Attributes::Layout GetPageLayout() const;

				/*! \brief Gets a wrapper to the current page */
				Page GetCurrentPage();
				/*! \brief Adds a page to the document */
				Page AddPage();
				/*! \brief Adds a page to the document */
				Page InsertPage(const Page& target);

				/*! \brief Adds a compressed image to the PDF */
				Image AddJPEG(const Path &fname);
				/*! \brief Adds an uncompressed image to the PDF */
				Image AddPNG(const Path &fname, bool delay = true);

				/*! \brief Sets the author name */
				void SetAuthor(const StringUTF8 &s);
				/*! \brief Sets the creator name */
				void SetCreator(const StringUTF8 &s);
				/*! \brief Sets the title */
				void SetTitle(const StringUTF8 &s);
				/*! \brief Sets the subject */
				void SetSubject(const StringUTF8 &s);
				/*! \brief Sets the list of keywords */
				void SetKeywords(const StringUTF8 &s);

				/*! \brief Sets the administration and (optionally) user passwords */
				void SetPasswords(const StringUTF8 &admin, const StringUTF8 &user = "");
				/*! \brief Sets the permissions */
				void SetPermissions(bool can_copy, bool can_print);

				/*! \brief Configures the PDF */
				void SetAttributes(const Attributes &attr);

			private:
				void *doc; /*!< Internal handle */
				bool has_pass; /*!< Is the document password protected? */
		};

		/*! \brief PDF image
		 *
		 * PDF image
		 *
		 * \author	Yann LEYDIER
		 * \date	May 2011
		 * \version	0.1
		 * \ingroup	PDF
		 */
		class Image
		{
			public:
				/*! \brief Default constructor */
				Image():img(nullptr) { }
				/*! \brief Returns the width of the image */
				size_t GetWidth() const;
				/*! \brief Returns the height of the image */
				size_t GetHeight() const;

			private:
				/*! \brief Wrapper constructor */
				Image(void *obj);
				void *img; /*!< Internal handle */
				friend class Doc;
				friend class Page;
		};

		/*! \brief PDF page
		 *
		 * PDF page
		 *
		 * \author	Yann LEYDIER
		 * \date	May 2011
		 * \version	0.2
		 * \ingroup	PDF
		 */
		class Page
		{
			public:
				/*! \brief Default constructor */
				Page():page(nullptr) { }
				
				/*! \brief Gets the width of the page */
				double GetWidth();
				/*! \brief Gets the height of the page */
				double GetHeight();
				/*! \brief Sets the width of the page */
				void SetWidth(double w);
				/*! \brief Sets the height of the page */
				void SetHeight(double h);
				
				/*! \brief Draws an image on the page */
				void DrawImage(Image &img, const Rect &bbox);

				/*! \brief Sets the pen color */
				void SetPenColor(const crn::pixel::RGB8 &color);
				/*! \brief Sets the fill color */
				void SetFillColor(const crn::pixel::RGB8 &color);
				/*! \brief Draws a line on the page */
				void DrawLine(double x1, double y1, double x2, double y2);
				/*! \brief Draws a rectangle on the page */
				void DrawRect(const Rect &rect, bool fill = false);

			private:
				/*! \brief Wrapper constructor */
				Page(void *obj);
				void *page; /*!< Internal handle */
				friend class Doc;
		};
	}
}

#endif // CRN_USING_HARU

#endif


