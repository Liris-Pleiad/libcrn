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
 * file: CRNPDF.cpp
 * \author Yann LEYDIER
 */

#include <CRN.h>
#ifdef CRN_USING_HARU

#include <CRNException.h>
#include <CRNUtils/CRNPDF.h>
#include <CRNIO/CRNPath.h>
#include <CRNGeometry/CRNRect.h>
#include <hpdf_error.h>
#include <hpdf.h>

using namespace crn;

static void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
	switch (error_no)
	{
		case HPDF_ARRAY_COUNT_ERR	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_ARRAY_ITEM_NOT_FOUND	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_ARRAY_ITEM_UNEXPECTED_TYPE	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_BINARY_LENGTH_ERR	:
			throw ExceptionRuntime("Data length > HPDF_LIMIT_MAX_STRING_LEN.");
		case HPDF_CANNOT_GET_PALLET	:
			throw ExceptionRuntime("Cannot get pallet data from PNG image.");
		case HPDF_DICT_COUNT_ERR	:
			throw ExceptionRuntime("Dictionary elements > HPDF_LIMIT_MAX_DICT_ELEMENT");
		case HPDF_DICT_ITEM_NOT_FOUND	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_DICT_ITEM_UNEXPECTED_TYPE	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_DICT_STREAM_LENGTH_NOT_FOUND	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_DOC_ENCRYPTDICT_NOT_FOUND	:
			throw ExceptionRuntime("HPDF_SetEncryptMode() or HPDF_SetPermission() called before password set.");
		case HPDF_DOC_INVALID_OBJECT	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_DUPLICATE_REGISTRATION	:
			throw ExceptionRuntime("Tried to re-register a registered font.");
		case HPDF_EXCEED_JWW_CODE_NUM_LIMIT	:
			throw ExceptionRuntime("Cannot register a character to the Japanese word wrap characters list.");
		case HPDF_ENCRYPT_INVALID_PASSWORD	:
			throw ExceptionRuntime("1. Tried to set the owner password to NULL. 2. Owner and user password are the same.");
		case HPDF_ERR_UNKNOWN_CLASS	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_EXCEED_GSTATE_LIMIT	:
			throw ExceptionRuntime("Stack depth > HPDF_LIMIT_MAX_GSTATE.");
		case HPDF_FAILD_TO_ALLOC_MEM	:
			throw ExceptionRuntime("Memory allocation failed.");
		case HPDF_FILE_IO_ERROR	:
			throw ExceptionRuntime("File processing failed. (Detailed code is set.)");
		case HPDF_FILE_OPEN_ERROR	:
			throw ExceptionRuntime("Cannot open a file. (Detailed code is set.)");
		case HPDF_FONT_EXISTS	:
			throw ExceptionRuntime("Tried to load a font that has been registered.");
		case HPDF_FONT_INVALID_WIDTHS_TABLE	:
			throw ExceptionRuntime("1. Font-file format is invalid. 2. Internal error. Data consistency was lost.");
		case HPDF_INVALID_AFM_HEADER	:
			throw ExceptionRuntime("Cannot recognize header of afm file.");
		case HPDF_INVALID_ANNOTATION	:
			throw ExceptionRuntime("Specified annotation handle is invalid.");
		case HPDF_INVALID_BIT_PER_COMPONENT	:
			throw ExceptionRuntime("Bit-per-component of a image which was set as mask-image is invalid.");
		case HPDF_INVALID_CHAR_MATRICS_DATA	:
			throw ExceptionRuntime("Cannot recognize char-matrics-data of afm file.");
		case HPDF_INVALID_COLOR_SPACE	:
			throw ExceptionRuntime("1. Invalid color_space parameter of HPDF_LoadRawImage.  2. Color-space of a image which was set as mask-image is invalid.  3. Invoked function invalid in present color-space.");
		case HPDF_INVALID_COMPRESSION_MODE	:
			throw ExceptionRuntime("Invalid value set when invoking HPDF_SetCommpressionMode().");
		case HPDF_INVALID_DATE_TIME	:
			throw ExceptionRuntime("An invalid date-time value was set.");
		case HPDF_INVALID_DESTINATION	:
			throw ExceptionRuntime("An invalid destination handle was set.");
		case HPDF_INVALID_DOCUMENT	:
			throw ExceptionRuntime("An invalid document handle was set.");
		case HPDF_INVALID_DOCUMENT_STATE	:
			throw ExceptionRuntime("Function invalid in the present state was invoked.");
		case HPDF_INVALID_ENCODER	:
			throw ExceptionRuntime("An invalid encoder handle was set.");
		case HPDF_INVALID_ENCODER_TYPE	:
			throw ExceptionRuntime("Combination between font and encoder is wrong.");
		case HPDF_INVALID_ENCODING_NAME	:
			throw ExceptionRuntime("An Invalid encoding name is specified.");
		case HPDF_INVALID_ENCRYPT_KEY_LEN	:
			throw ExceptionRuntime("Encryption key length is invalid.");
		case HPDF_INVALID_FONTDEF_DATA	:
			throw ExceptionRuntime("1. An invalid font handle was set.  2. Unsupported font format.");
		case HPDF_INVALID_FONTDEF_TYPE	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_INVALID_FONT_NAME	:
			throw ExceptionRuntime("Font with the specified name is not found.");
		case HPDF_INVALID_IMAGE	:
			throw ExceptionRuntime("Unsupported image format.");
		case HPDF_INVALID_JPEG_DATA	:
			throw ExceptionRuntime("Unsupported image format.");
		case HPDF_INVALID_N_DATA	:
			throw ExceptionRuntime("Cannot read a postscript-name from an afm file.");
		case HPDF_INVALID_OBJECT	:
			throw ExceptionRuntime("1. An invalid object is set.  2. Internal error. Data consistency was lost.");
		case HPDF_INVALID_OBJ_ID	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_INVALID_OPERATION	:
			throw ExceptionRuntime("Invoked HPDF_Image_SetColorMask() against the image-object which was set a mask-image.");
		case HPDF_INVALID_OUTLINE	:
			throw ExceptionRuntime("An invalid outline-handle was specified.");
		case HPDF_INVALID_PAGE	:
			throw ExceptionRuntime("An invalid page-handle was specified.");
		case HPDF_INVALID_PAGES	:
			throw ExceptionRuntime("An invalid pages-handle was specified. (internal error)");
		case HPDF_INVALID_PARAMETER	:
			throw ExceptionRuntime("An invalid value is set.");
		case HPDF_INVALID_PNG_IMAGE	:
			throw ExceptionRuntime("Invalid PNG image format.");
		case HPDF_INVALID_STREAM	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_MISSING_FILE_NAME_ENTRY	:
			throw ExceptionRuntime("Internal error. _FILE_NAME entry for delayed loading is missing.");
		case HPDF_INVALID_TTC_FILE	:
			throw ExceptionRuntime("Invalid .TTC file format.");
		case HPDF_INVALID_TTC_INDEX	:
			throw ExceptionRuntime("Index parameter > number of included fonts.");
		case HPDF_INVALID_WX_DATA	:
			throw ExceptionRuntime("Cannot read a width-data from an afm file.");
		case HPDF_ITEM_NOT_FOUND	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_LIBPNG_ERROR	:
			throw ExceptionRuntime("Error returned from PNGLIB while loading image.");
		case HPDF_NAME_INVALID_VALUE	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_NAME_OUT_OF_RANGE	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_PAGES_MISSING_KIDS_ENTRY	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_PAGE_CANNOT_FIND_OBJECT	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_PAGE_CANNOT_GET_ROOT_PAGES	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_PAGE_CANNOT_RESTORE_GSTATE	:
			throw ExceptionRuntime("There are no graphics-states to be restored.");
		case HPDF_PAGE_CANNOT_SET_PARENT	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_PAGE_FONT_NOT_FOUND	:
			throw ExceptionRuntime("The current font is not set.");
		case HPDF_PAGE_INVALID_FONT	:
			throw ExceptionRuntime("An invalid font-handle was specified.");
		case HPDF_PAGE_INVALID_FONT_SIZE	:
			throw ExceptionRuntime("An invalid font-size was set.");
		case HPDF_PAGE_INVALID_GMODE	:
			throw ExceptionRuntime("See Graphics mode.");
		case HPDF_PAGE_INVALID_INDEX	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_PAGE_INVALID_ROTATE_VALUE	:
			throw ExceptionRuntime("Specified value is not multiple of 90.");
		case HPDF_PAGE_INVALID_SIZE	:
			throw ExceptionRuntime("An invalid page-size was set.");
		case HPDF_PAGE_INVALID_XOBJECT	:
			throw ExceptionRuntime("An invalid image-handle was set.");
		case HPDF_PAGE_OUT_OF_RANGE	:
			throw ExceptionRuntime("The specified value is out of range.");
		case HPDF_REAL_OUT_OF_RANGE	:
			throw ExceptionRuntime("The specified value is out of range.");
		case HPDF_STREAM_EOF	:
			throw ExceptionRuntime("Unexpected EOF marker was detected.");
		case HPDF_STREAM_READLN_CONTINUE	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_STRING_OUT_OF_RANGE	:
			throw ExceptionRuntime("The length of the text is too long.");
		case HPDF_THIS_FUNC_WAS_SKIPPED	:
			throw ExceptionRuntime("Function not executed because of other errors.");
		case HPDF_TTF_CANNOT_EMBEDDING_FONT	:
			throw ExceptionRuntime("Font cannot be embedded. (license restriction)");
		case HPDF_TTF_INVALID_CMAP	:
			throw ExceptionRuntime("Unsupported ttf format. (cannot find unicode cmap)");
		case HPDF_TTF_INVALID_FOMAT	:
			throw ExceptionRuntime("Unsupported ttf format.");
		case HPDF_TTF_MISSING_TABLE	:
			throw ExceptionRuntime("Unsupported ttf format. (cannot find a necessary table)");
		case HPDF_UNSUPPORTED_FONT_TYPE	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_UNSUPPORTED_FUNC	:
			throw ExceptionRuntime("1. Library not configured to use PNGLIB.  2. Internal error. Data consistency was lost.");
		case HPDF_UNSUPPORTED_JPEG_FORMAT	:
			throw ExceptionRuntime("Unsupported JPEG format.");
		case HPDF_UNSUPPORTED_TYPE1_FONT	:
			throw ExceptionRuntime("Failed to parse .PFB file.");
		case HPDF_XREF_COUNT_ERR	:
			throw ExceptionRuntime("Internal error. Data consistency was lost.");
		case HPDF_ZLIB_ERROR	:
			throw ExceptionRuntime("Error while executing ZLIB function.");
		case HPDF_INVALID_PAGE_INDEX	:
			throw ExceptionRuntime("An invalid page index was passed.");
		case HPDF_INVALID_URI	:
			throw ExceptionRuntime("An invalid URI was set.");
			/*
		 case HPDF_PAGELAYOUT_OUT_OF_RANGE	:
			 throw ExceptionRuntime("An invalid page-layout was set.");
		 case HPDF_PAGEMODE_OUT_OF_RANGE	:
			 throw ExceptionRuntime("An invalid page-mode was set.");
		 case HPDF_PAGENUM_STYLE_OUT_OF_RANGE	:
			 throw ExceptionRuntime("An invalid page-num-style was set.");
			 */
		case HPDF_ANNOT_INVALID_ICON	:
			throw ExceptionRuntime("An invalid icon was set.");
		case HPDF_ANNOT_INVALID_BORDER_STYLE	:
			throw ExceptionRuntime("An invalid border-style was set.");
		case HPDF_PAGE_INVALID_DIRECTION	:
			throw ExceptionRuntime("An invalid page-direction was set.");
		case HPDF_INVALID_FONT	:
			throw ExceptionRuntime("An invalid font-handle was specified.");
	}
}

using namespace crn::PDF;

/*! Blank document creation
 * \throws	ExceptionRuntime	harupdf internal error
 */
Doc::Doc():
	doc(HPDF_New(error_handler, NULL)),
	has_pass(false)
{
	HPDF_SetCompressionMode(HPDF_Doc(doc), HPDF_COMP_ALL);
	SetCreator("libcrn");
}

/*! Document from attributes
 * \param[in]	attr	the PDF attributes 
 * \throws	ExceptionRuntime	harupdf internal error
 */
Doc::Doc(const Attributes &attr):
	doc(HPDF_New(error_handler, NULL)),
	has_pass(false)
{
	HPDF_SetCompressionMode(HPDF_Doc(doc), HPDF_COMP_ALL);
	SetAttributes(attr);
}

/*! Release internal data */
Doc::~Doc()
{ 
	HPDF_Free(HPDF_Doc(doc));
	doc = nullptr;
}

/*! Exports the document
 * \param[in]	file_name	the name of the PDF file
 * \throws	ExceptionRuntime	harupdf internal error
 */
void Doc::Save(const Path &file_name)
{
	HPDF_SaveToFile(HPDF_Doc(doc), file_name.CStr());
}

/*! Sets the initial display layout
 * \param[in]	layout	the page layout
 * \throws	ExceptionRuntime	harupdf internal error
 */
void Doc::SetPageLayout(Attributes::Layout layout)
{
	switch (layout)
	{
		case Attributes::Layout::ONE_PAGE:
			HPDF_SetPageLayout(HPDF_Doc(doc), HPDF_PAGE_LAYOUT_SINGLE);
			break;
		case Attributes::Layout::CONTINUOUS:
			HPDF_SetPageLayout(HPDF_Doc(doc), HPDF_PAGE_LAYOUT_ONE_COLUMN);
			break;
		case Attributes::Layout::OPEN_LEFT:
			HPDF_SetPageLayout(HPDF_Doc(doc), HPDF_PAGE_LAYOUT_TWO_COLUMN_LEFT);
			break;
		case Attributes::Layout::OPEN_RIGHT:
			HPDF_SetPageLayout(HPDF_Doc(doc), HPDF_PAGE_LAYOUT_TWO_COLUMN_RIGHT);
			break;
	}
}

/*! Gets the initial display layout
 * \return	the initial page layout
 */
Attributes::Layout Doc::GetPageLayout() const
{
	HPDF_PageLayout l = HPDF_GetPageLayout(HPDF_Doc(doc));
	switch (l)
	{
		case HPDF_PAGE_LAYOUT_SINGLE:
			return Attributes::Layout::ONE_PAGE;
		case HPDF_PAGE_LAYOUT_ONE_COLUMN:
			return Attributes::Layout::CONTINUOUS;
		case HPDF_PAGE_LAYOUT_TWO_COLUMN_LEFT:
			return Attributes::Layout::OPEN_LEFT;
		default:
			return Attributes::Layout::OPEN_RIGHT;
	}
}

/*! Gets a wrapper to the current page
 * \throws	ExceptionRuntime	harupdf internal error
 * \return	a wrapper to the current page
 */
Page Doc::GetCurrentPage()
{
	return Page(HPDF_GetCurrentPage(HPDF_Doc(doc)));
}

/*! Adds a page to the document 
 * \throws	ExceptionRuntime	harupdf internal error
 * \return	a wrapper to the added page
 */
Page Doc::AddPage()
{
	return Page(HPDF_AddPage(HPDF_Doc(doc)));
}

/*! Adds a page to the document 
 * \param[in]	target	the page before which the new page will be added
 * \throws	ExceptionRuntime	harupdf internal error
 * \return	a wrapper to the added page
 */
Page Doc::InsertPage(const Page& target)
{
	return Page(HPDF_InsertPage(HPDF_Doc(doc), HPDF_Page(target.page)));
}

/*! Adds a compressed image to the PDF
 * \param[in]	fname	the image file to add
 * \throws	ExceptionRuntime	harupdf internal error
 * \return	a wrapper to the added image
 */
Image Doc::AddJPEG(const Path &fname)
{
	return Image(HPDF_LoadJpegImageFromFile(HPDF_Doc(doc), fname.CStr()));
}

/*! Adds an uncompressed image to the PDF
 * \param[in]	fname	the image file to add
 * \param[in]	delay	shall the image be loaded only when saving the PDF? (default = yes)
 * \throws	ExceptionRuntime	harupdf internal error
 * \return	a wrapper to the added image
 */
Image Doc::AddPNG(const Path &fname, bool delay)
{
	if (delay)
	{
		return Image(HPDF_LoadPngImageFromFile2(HPDF_Doc(doc), fname.CStr()));
	}
	else
	{
		return Image(HPDF_LoadPngImageFromFile(HPDF_Doc(doc), fname.CStr()));
	}
}

/*! Sets the author name
 * \param[in]	s	the author name
 * \throws	ExceptionRuntime	harupdf internal error
 */
void Doc::SetAuthor(const StringUTF8 &s)
{
	HPDF_SetInfoAttr(HPDF_Doc(doc), HPDF_INFO_AUTHOR, s.CStr());
}

/*! Sets the creator name
 * \param[in]	s	the creator name
 * \throws	ExceptionRuntime	harupdf internal error
 */
void Doc::SetCreator(const StringUTF8 &s)
{
	HPDF_SetInfoAttr(HPDF_Doc(doc), HPDF_INFO_CREATOR, s.CStr());
}

/*! Sets the title
 * \param[in]	s	the title
 * \throws	ExceptionRuntime	harupdf internal error
 */
void Doc::SetTitle(const StringUTF8 &s)
{
	HPDF_SetInfoAttr(HPDF_Doc(doc), HPDF_INFO_TITLE, s.CStr());
}

/*! Sets the subject
 * \param[in]	s	the subject
 * \throws	ExceptionRuntime	harupdf internal error
 */
void Doc::SetSubject(const StringUTF8 &s)
{
	HPDF_SetInfoAttr(HPDF_Doc(doc), HPDF_INFO_SUBJECT, s.CStr());
}

/*! Sets the list of keywords
 * \param[in]	s	the list of keywords
 * \throws	ExceptionRuntime	harupdf internal error
 */
void Doc::SetKeywords(const StringUTF8 &s)
{
	HPDF_SetInfoAttr(HPDF_Doc(doc), HPDF_INFO_KEYWORDS, s.CStr());
}

/*! Sets the administration and (optionally) user passwords
 * \param[in]	admin	the administration password
 * \param[in]	user	the user password
 * \throws	ExceptionUninitialized	null admin password
 * \throws	ExceptionRuntime	harupdf internal error
 */
void Doc::SetPasswords(const StringUTF8 &admin, const StringUTF8 &user)
{
	if (admin.IsEmpty())
		throw ExceptionUninitialized("PDF admin password must not be empty.");
	HPDF_SetPassword(HPDF_Doc(doc), admin.CStr(), user.CStr());
	HPDF_SetEncryptionMode(HPDF_Doc(doc), HPDF_ENCRYPT_R3, 16);
	has_pass = true;
}

/*! Sets the permissions
 * \param[in]	can_copy	can the user copy/paste the PDF's content?
 * \param[in]	can_print	can the user print the PDF document?
 * \throws	ExceptionRuntime	harupdf internal error
 */
void Doc::SetPermissions(bool can_copy, bool can_print)
{
	if (!has_pass)
		throw ExceptionLogic("Cannot set permissions of a document that has no admin password.");
	HPDF_UINT perm = HPDF_ENABLE_READ|HPDF_ENABLE_EDIT_ALL|HPDF_ENABLE_EDIT;
	if (can_copy)
		perm |= HPDF_ENABLE_COPY;
	if (can_print)
		perm |= HPDF_ENABLE_PRINT;
	HPDF_SetPermission(HPDF_Doc(doc), perm);
}

/*! Configures the PDF
 * \param[in]	attr	the configuration object
 * \throws	ExceptionRuntime	harupdf internal error
 */
void Doc::SetAttributes(const Attributes &attr)
{
	SetPageLayout(attr.layout);
	SetAuthor(attr.author);
	SetCreator(attr.creator);
	SetTitle(attr.title);
	SetSubject(attr.subject);
	SetKeywords(attr.keywords);
	if (attr.owner_password.IsNotEmpty())
	{
		SetPasswords(attr.owner_password, attr.user_password);
		SetPermissions(attr.copyable, attr.printable);
	}
}

/*! Wrapper constructor
 * \param[in]	obj	the HPDF image
 */
PDF::Image::Image(void *obj):
	img(obj)
{
}

/*! Returns the width of the image
 * \throws	ExceptionRuntime	harupdf internal error
 * \return	the width of the image
 */
size_t PDF::Image::GetWidth() const
{
	return HPDF_Image_GetWidth(HPDF_Image(img));
}

/*! Returns the height of the image
 * \throws	ExceptionRuntime	harupdf internal error
 * \return	the height of the image
 */
size_t PDF::Image::GetHeight() const
{
	return HPDF_Image_GetHeight(HPDF_Image(img));
}

/*! Wrapper constructor
 * \param[in]	obj	the HPDF image
 */
Page::Page(void *obj):
	page(obj)
{
}

/*! \return	the width of the page */
double Page::GetWidth()
{
	return HPDF_Page_GetWidth(HPDF_Page(page));
}

/*! \return	the height of the page */
double Page::GetHeight()
{
	return HPDF_Page_GetHeight(HPDF_Page(page));
}

/*! Sets the width of the page
 * \throws	ExceptionRuntime	harupdf internal error
 * \param[in]	w	the width in pixels
 */
void Page::SetWidth(double w)
{
	HPDF_Page_SetWidth(HPDF_Page(page), HPDF_REAL(w));
}

/*! Sets the height of the page
 * \throws	ExceptionRuntime	harupdf internal error
 * \param[in]	h	the height in pixels
 */
void Page::SetHeight(double h)
{
	HPDF_Page_SetHeight(HPDF_Page(page), HPDF_REAL(h));
}

/*! Sets the pen color
 * \throws	ExceptionRuntime	harupdf internal error
 * \param[in]	color	color to draw lines
 */
void Page::SetPenColor(const crn::pixel::RGB8 &color)
{
	HPDF_Page_SetRGBStroke(HPDF_Page(page), HPDF_REAL(color.r / 255.0), HPDF_REAL(color.g / 255.0), HPDF_REAL(color.b / 255.0));
}

/*! Sets the fill color
 * \throws	ExceptionRuntime	harupdf internal error
 * \param[in]	color	color to fill shapes
 */
void Page::SetFillColor(const crn::pixel::RGB8 &color)
{
	HPDF_Page_SetRGBFill(HPDF_Page(page), HPDF_REAL(color.r / 255.0), HPDF_REAL(color.g / 255.0), HPDF_REAL(color.b / 255.0));
}

/*! Draws an image on the page
 * \throws	ExceptionRuntime	harupdf internal error
 *
 * \param[in]	img	the handle on the image (must be added to the same PDF file)
 * \param[in]	bbox	the position and size of the image
 */
void Page::DrawImage(Image &img, const Rect &bbox)
{
	HPDF_Page_DrawImage(HPDF_Page(page), HPDF_Image(img.img), HPDF_REAL(bbox.GetLeft()), HPDF_REAL(GetHeight()) - HPDF_REAL(bbox.GetBottom()), HPDF_REAL(bbox.GetWidth()), HPDF_REAL(bbox.GetHeight()));
}

/*! Draws a line on the page
 */
void Page::DrawLine(double x1, double y1, double x2, double y2)
{
	HPDF_Page_MoveTo(HPDF_Page(page), HPDF_REAL(x1), HPDF_REAL(GetHeight() - y1));
	HPDF_Page_LineTo(HPDF_Page(page), HPDF_REAL(x2), HPDF_REAL(GetHeight() - y2));
	HPDF_Page_Stroke(HPDF_Page(page));
}

/*! Draws a rectangle on the page
 * \throws	ExceptionRuntime	harupdf internal error
 * \param[in]	rect	the coordinates
 * \param[in]	fill	shall the rectangle be filled?
 */
void Page::DrawRect(const Rect &rect, bool fill)
{
	HPDF_Page_Rectangle(HPDF_Page(page), HPDF_REAL(rect.GetLeft()), HPDF_REAL(GetHeight() - rect.GetBottom()), HPDF_REAL(rect.GetWidth()), HPDF_REAL(rect.GetHeight()));
	if (fill)
		HPDF_Page_FillStroke(HPDF_Page(page));
	else
		HPDF_Page_Stroke(HPDF_Page(page));
}

#endif // CRN_USING_HARU
