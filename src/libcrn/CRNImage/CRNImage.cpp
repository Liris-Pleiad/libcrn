/* Copyright 2006-2015 Yann LEYDIER, INSA-Lyon
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
 * file: CRNImage.cpp
 * \author Yann LEYDIER
 */

#include <CRNImage/CRNImage.h>
#include <CRNImage/CRNImageBW.h>
#include <CRNImage/CRNImageGray.h>
#include <CRNImage/CRNImageRGB.h>
#include <CRNException.h>
#include <CRNIO/CRNFileShield.h>
#include <CRNi18n.h>

#ifdef CRN_USING_GDKPB
#	include <gdk-pixbuf/gdk-pixbuf.h>
#endif

#ifdef CRN_USING_LIBPNG
#	include <png.h>
#endif

#ifdef CRN_USING_LIBJPEG
#  include <jpeglib.h>
#  include <setjmp.h>
#endif

using namespace crn;

/*! \returns the bounding box of the image */
Rect ImageBase::GetBBox() const noexcept
{
	return Rect(0, 0, int(width) - 1, int(height) - 1);
}

/*! \internal */
enum ColorType { RGB, GRAY, BW };

/*!
 * \internal
 * Closes a file if it is not null
 */
void fclose_if_not_null(FILE *f)
{
	if (f)
		fclose(f);
}

#ifdef CRN_USING_LIBPNG
// cf http://www.libpng.org/pub/png/libpng-1.2.5-manual.html
static std::pair<UImage, String> load_libpng(const Path &filename)
{
	// libpng does not support URIs
	auto fname = filename;
	fname.ToLocal();

	/* open image file */
	std::unique_ptr<FILE, decltype(fclose_if_not_null)*> fp(fopen(fname.CStr(), "rb"), fclose_if_not_null);
	if (!fp)
		return std::make_pair(UImage{}, String(_("Cannot open file ")) + fname);

	/* read magic number */
	png_byte magic[8];
	size_t readsize = fread(magic, 1, sizeof(magic), fp.get());
	if (!readsize)
		return std::make_pair(UImage{}, String(U"Empty file."));

	/* check for valid magic number */
	if (png_sig_cmp(magic, 0, sizeof(magic)))
		return std::make_pair(UImage{}, String(U"Not a PNG file."));


	/* create a png read struct */
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		return std::make_pair(UImage{}, String(_("Cannot create PNG reader.")));

	/* create a png info struct */
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return std::make_pair(UImage{}, String(_("Cannot create PNG info.")));
	}

	/* Set up the error handling */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return std::make_pair(UImage{}, String(_("Error while reading the PNG file ")) + fname);
	}

	/* setup libpng for using standard C fread() function with our FILE pointer */
	png_init_io(png_ptr, fp.get());
	/* tell libpng that we have already read the magic number */
	png_set_sig_bytes(png_ptr, sizeof(magic));

	/* Read informations */
	png_read_info(png_ptr, info_ptr);
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type, compression_type, filter_method;
	bool modified = false;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, &compression_type, &filter_method);

	ColorType read_type(RGB);

	auto col0 = pixel::BWBlack;
	auto col1 = pixel::BWWhite;
	if (bit_depth == 1)
	{ // binary image
		png_set_packing(png_ptr);
		modified = true;
		read_type = BW;
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE))
		{ // has a palette -_-
			// we convert it to black & white
			png_colorp pal;
			int numpal;
			png_get_PLTE(png_ptr, info_ptr, &pal, &numpal);
			if (numpal >= 2)
			{
				int m0 = pal[0].red + pal[0].green + pal[0].blue;
				int m1 = pal[1].red + pal[1].green + pal[1].blue;
				if (m1 < m0) // color 1 is darker than color 0
				{
					col0 = pixel::BWWhite;
					col1 = pixel::BWBlack;
				}
			}
		}
	}
	else if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{ // Grayscale image
		if (bit_depth < 8)
		{
			png_set_expand_gray_1_2_4_to_8(png_ptr);
			modified = true;
		}
		read_type = GRAY;
	}
	else if (color_type == PNG_COLOR_TYPE_PALETTE)
	{ // Changes palletted image to RGB
		png_set_palette_to_rgb(png_ptr);  // Just an alias of png_set_expand()
		modified = true;
	}
	// Transform 16 bits samples to 8 bits samples
	if (bit_depth == 16)
	{
		png_set_strip_16(png_ptr);
		modified = true;
	}
	// Remove alpha channel
	if (color_type & PNG_COLOR_MASK_ALPHA)
	{
		png_set_strip_alpha(png_ptr);
		modified = true;
	}

	// If any of the above were applied,
	if (modified)
	{
		// After setting the transformations, libpng can update your png_info structure to reflect any transformations
		// you've requested with this call. This is most useful to update the info structure's rowbytes field so you can
		// use it to allocate your image memory. This function will also update your palette with the correct screen_gamma
		// and background if these have been given with the calls above.
		png_read_update_info(png_ptr, info_ptr);

		// Just in case any of these have changed
		png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
	}

	int channels = png_get_channels(png_ptr, info_ptr);

	png_bytepp row_pointers;

	/* we can now allocate memory for storing pixel data */
	row_pointers = (png_bytep*)malloc(sizeof(png_byte*) * height);
	for (size_t i = 0; i < height ; ++i)
	{
		row_pointers[i] = (png_bytep)malloc(info_ptr->rowbytes);
	}
	/* Read PNG file */
	png_read_image(png_ptr, row_pointers);

	/* Conversion */
	UImage img;
	switch (read_type)
	{
		case RGB:
			{
				auto irgb = std::make_unique<ImageRGB>(size_t(width), size_t(height));
				for (size_t y = 0; y < height; y++)
					for (size_t x = 0; x < width; x++)
					{
						png_byte *pixel = &((row_pointers[y])[x*channels]);
						irgb->At(x, y) = {pixel[0], pixel[1], pixel[2]};
					}
				img = std::move(irgb);
			}
			break;
		case GRAY:
			{
				auto ig = std::make_unique<ImageGray>(size_t(width), size_t(height));
				for (size_t y = 0; y < height; y++)
					for (size_t x = 0; x < width; x++)
					{
						png_byte *pixel = &((row_pointers[y])[x]);
						ig->At(x, y) = *pixel;
					}
				img = std::move(ig);
			}
			break;
		case BW:
			{
				auto ibw = std::make_unique<ImageBW>(size_t(width), size_t(height));
				for (size_t y = 0; y < height; y++)
					for (size_t x = 0; x < width; x++)
					{
						png_byte *pixel = &((row_pointers[y])[x]);
						ibw->At(x, y) = (*pixel) ? col1 : col0;
					}
				img = std::move(ibw);
			}
			break;
	}

	/* read metadata */
/*
	png_textp txt;
	int nb = png_get_text(png_ptr, info_ptr, &txt, NULL);
	CRNVerbose(nb);
	for (int tmp = 0; tmp < nb; ++tmp)
	{
		String s(U"compression: ");
		s += txt[tmp].compression;
		s += U"\nkey: ";
		s += txt[tmp].key;
		s += U"\ntext: ";
		s += txt[tmp].text;
		s += U"\ntext length: ";
		s += txt[tmp].text_length;
#ifdef PNG_iTXt_SUPPORTED // not supported in 1.2 branch
		s += U"\ni text length: ";
		s += txt[tmp].itxt_length;
		s += U"\nlang: "
		s += txt[tmp].lang;
		s += U"\nlang key: "
		s += txt[tmp].lang_key;
#endif
		CRNVerbose(s);
	}
*/

	/* finish decompression and release memory */
	png_read_end(png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	for (size_t i = 0; i < height ; ++i)
	{
		free(row_pointers[i]);
	}
	free(row_pointers);

	return std::make_pair(std::move(img), String(U""));
}
#endif

#ifdef CRN_USING_LIBJPEG
struct crn_jpeg_error_mgr
{
  struct jpeg_error_mgr pub;	/* "public" fields */
  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct crn_jpeg_error_mgr * crn_jpeg_error_ptr;

void crn_jpeg_error_exit(j_common_ptr cinfo)
{
  crn_jpeg_error_ptr myerr = (crn_jpeg_error_ptr) cinfo->err;
  /* display the message. */
  //(*cinfo->err->output_message) (cinfo);
  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

static std::pair<UImage, String> load_libjpeg(const Path &filename)
{
	// libjpeg does not support URIs
	auto fname(filename);
	fname.ToLocal();

	/* open image file */
	std::unique_ptr<FILE, decltype(fclose_if_not_null)*> fp(fopen(fname.CStr(), "rb"), fclose_if_not_null);
	if (!fp)
		return std::make_pair(UImage{}, String(_("Cannot open file ")) + fname);

	struct jpeg_decompress_struct cinfo;
	struct crn_jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = crn_jpeg_error_exit;
	if (setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&cinfo);
		return std::make_pair(UImage{}, String(_("Not a JPEG file.")));
	}
	else
	{
		jpeg_create_decompress(&cinfo);
		jpeg_stdio_src(&cinfo, fp.get());
		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);
		int w = cinfo.output_width;
		int h = cinfo.output_height;
		int bpp = cinfo.out_color_components;
		UImageRGB irgb;
		UImageGray ig;
		if (bpp == 3)
		{
			irgb = std::make_unique<ImageRGB>(w, h);
		}
		else if (bpp == 1)
		{
			ig = std::make_unique<ImageGray>(w, h);
		}
		else
		{
			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			return std::make_pair(UImage{}, String(_("JPEG file contains unnatural bytes per pixel count.")));
		}
		JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, w * bpp, 1);
		int y = 0;
		while (cinfo.output_scanline < cinfo.output_height)
		{
			(void) jpeg_read_scanlines(&cinfo, buffer, 1);
			for (int x = 0; x < w; ++x)
			{
				if (bpp == 3)
				{
					irgb->At(x, y) = { buffer[0][3 * x], buffer[0][3 * x + 1], buffer[0][3 * x + 2] };
				}
				else
				{
					ig->At(x, y) = buffer[0][x];
				}
			}
			y += 1;
		}

		// read comments
		// TODO
		// jpeg_read_header() then cinfo->marker_list (linked list)

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		if (bpp == 3)
			return std::make_pair(std::move(irgb), String(U""));
		else
			return std::make_pair(std::move(ig), String(U""));
	}
}
#endif

#ifdef CRN_USING_GDKPB
static std::pair<UImage, String> load_gdkpixbuf(const Path &fname)
{
	GError *err = NULL;

	gsize i, o;
	gchar *filename;
	if (g_utf8_validate(fname.CStr(), -1, NULL))
	{
		filename = g_filename_from_utf8(fname.CStr(), -1, &i, &o, &err);
	}
	else
	{
		gchar *utfname;
		utfname = g_locale_to_utf8(fname.CStr(), -1, &i, &o, &err);
		if (!utfname)
		{
			String msg((char*)err->message);
			g_error_free(err);
			return std::make_pair(UImage{}, String(_("Cannot convert filename string. ")) + msg);
		}
		filename = g_filename_from_utf8(utfname, -1, &i, &o, &err);
		g_free(utfname);
	}
	if (!filename)
	{
		String msg((char*)err->message);
		g_error_free(err);
		return std::make_pair(UImage{}, String(_("Cannot convert filename string. ")) + msg);
	}
	GdkPixbuf *pb = gdk_pixbuf_new_from_file(filename, &err);
	g_free(filename);

	if (!pb)
	{
		String msg((char*)err->message);
		g_error_free(err);
		return std::make_pair(UImage{}, String(_("Cannot open image. ")) + msg);
	}
	int w = gdk_pixbuf_get_width(pb);
	int h = gdk_pixbuf_get_height(pb);
	int rs = gdk_pixbuf_get_rowstride(pb);
	int word = gdk_pixbuf_get_has_alpha(pb) ? 4 : 3;

	UImageRGB img(std::make_unique<ImageRGB>(w, h));
	guchar *oripix = gdk_pixbuf_get_pixels(pb);
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			const auto o2 = word * x + y * rs;
			img->At(x, y) = { oripix[o2], oripix[o2 + 1], oripix[o2 + 2] };
		}
	g_object_unref(pb);
	return std::make_pair(std::move(img), String(U""));
}
#endif

/*! \brief Loads an image from a file
 * \throws	ExceptionInvalidArgument	null file name
 * \throws	ExceptionIO	no decoder found
 * \param[in]	fname	full path to the image file
 * \return	a pointer on an image
 */
UImage crn::NewImageFromFile(const Path &fname)
{
	if (!fname)
		throw ExceptionInvalidArgument(StringUTF8("UImage NewImageFromFile(const Path &fname): ") +
				_("Null file name."));

	std::lock_guard<std::mutex> lock(crn::FileShield::GetMutex(fname)); // lock the file

	/**************************************************************
	 * try PNG
	 *************************************************************/
	std::pair<UImage, String> res(std::make_pair(UImage{}, String(U"")));
	String errors;
#ifdef CRN_USING_LIBPNG
	if (res.first.get() == nullptr)
	{
		res = load_libpng(fname);
		errors += U" " + res.second;
	}
#endif // CRN_USING_LIBPNG
#ifdef CRN_USING_LIBJPEG
	if (res.first.get() == nullptr)
	{
		res = load_libjpeg(fname);
		errors += U" " + res.second;
	}
#endif // CRN_USING_LIBJPEG
#ifdef CRN_USING_GDKPB
	if (res.first.get() == nullptr)
	{
		res = load_gdkpixbuf(fname);
		errors += U" " + res.second;
	}
#endif // CRN_USING_GDKPB
	if (res.first.get() == nullptr)
		throw ExceptionIO(StringUTF8("UImage NewImageFromFile(const Path &fname): ") +
				_("No decoder could open the file.") + errors.CStr());
	return std::move(res.first);
}

