/* Copyright 2006-2015 Yann LEYDIER, CoReNum, INSA-Lyon
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
 * file: CRNImageRGB.cpp
 * \author Yann LEYDIER
 */

#include <CRNImage/CRNImageRGB.h>
#include <CRNData/CRNDataFactory.h>
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
struct crn_jpeg_error_mgr
{
  struct jpeg_error_mgr pub;	/* "public" fields */
  jmp_buf setjmp_buffer;	/* for return to caller */
};
typedef struct crn_jpeg_error_mgr * crn_jpeg_error_ptr;
void crn_jpeg_error_exit(j_common_ptr cinfo);
#endif

#ifdef CRN_USING_GDIPLUS
#	include <windows.h>
#	include <objidl.h>
#	include <gdiplus.h>
#	undef RegisterClass
#endif

void fclose_if_not_null(FILE *f);

using namespace crn;

/*************************************************************************************
 * File operations
 ************************************************************************************/

#ifdef CRN_USING_LIBPNG
static std::pair<bool, String> save_png_libpng(const Path &filename, const ImageRGB &img)
{
	// libpng does not support URIs
	Path fname(filename);
	fname.ToLocal();

	std::unique_ptr<FILE, decltype(fclose_if_not_null)*> fp(fopen(fname.CStr(), "wb"), fclose_if_not_null);
	if (!fp)
	{
		return std::make_pair(false, String(_("Cannot create file ")) + U"<" + fname + U">");
	}

	/* Create png struct & info */
	png_structp png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		return std::make_pair(false, String(_("Cannot create the PNG structure.")));
	}
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return std::make_pair(false, String(_("Cannot create the PNG info.")));
	}

	/* Set up the error handling */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return std::make_pair(false, String(_("Error while generating the PNG image.")));
	}

	/* setup libpng for using standard C fwrite() function with our FILE pointer */
	png_init_io(png_ptr, fp.get());

	/* Fill in the png_info structure */
	int width = int(img.GetWidth());
	int height = int(img.GetHeight());
	int bit_depth = 8;
	int color_type = PNG_COLOR_TYPE_RGB;
	int interlace_type = PNG_INTERLACE_NONE;
	int compression_type = PNG_COMPRESSION_TYPE_DEFAULT;
	int filter_method = PNG_FILTER_TYPE_DEFAULT;
	png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type, interlace_type, compression_type, filter_method);

	/* Using the low-level write interface */
	png_write_info(png_ptr, info_ptr);
	png_bytepp row_pointers;
	row_pointers = (png_bytep*)malloc(sizeof(png_byte*) * height);
	for (int i = 0; i < height ; ++i)
	{
		row_pointers[i] = (png_bytep)calloc(1, info_ptr->rowbytes);
	}

	/* Conversion */
	int channels = info_ptr->channels;
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			const auto px = img.At(x, y);
			row_pointers[y][x * channels] = px.r;
			row_pointers[y][x * channels + 1] = px.g;
			row_pointers[y][x * channels + 2] = px.b;
		}

	png_write_image(png_ptr, row_pointers);

	// TODO more options
	png_text txt[1];
	txt[0].compression = PNG_TEXT_COMPRESSION_NONE;
	txt[0].key = (char*)"creator";
	txt[0].text = (char*)"libcrn";
	txt[0].text_length = strlen(txt[0].text);
	png_set_text(png_ptr, info_ptr, txt, 1);

	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);

	// free
	for (int i = 0; i < height ; ++i)
	{
		free(row_pointers[i]);
	}
	free(row_pointers);
	return std::make_pair(true, String(""));

}
#endif

#ifdef CRN_USING_GDKPB
static std::pair<bool, String> save_png_gdkpixbuf(const Path &fname, const ImageRGB &img)
{
	GdkPixbuf *pb = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, 
			int(img.GetWidth()), int(img.GetHeight()));
	if (!pb)
	{
		return std::make_pair(false, String(_("Cannot create temporary buffer.")));
	}

	int w = gdk_pixbuf_get_width(pb);
	int h = gdk_pixbuf_get_height(pb);
	int rs = gdk_pixbuf_get_rowstride(pb);
	guchar *ppix = gdk_pixbuf_get_pixels(pb);
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			const auto px = img.At(x, y);
			int o2 = x + x + x + y * rs;
			ppix[o2] = px.r;
			ppix[o2 + 1] = px.g;
			ppix[o2 + 2] = px.b;
		}

	GError *err = NULL;
	gchar *utfname;
	gsize i;
	utfname = g_locale_to_utf8(fname.CStr(), -1, NULL, &i, NULL);
	gchar *filename;
	filename = g_filename_from_utf8(utfname, -1, NULL, &i, NULL);
	g_free(utfname);
	gchar *tinfo = g_locale_to_utf8("tEXt::Source", -1, NULL, &i, NULL);
	gchar *tinfo2 = g_locale_to_utf8("Saved by libcrn.", -1, NULL, &i, NULL);
	bool ok = gdk_pixbuf_save(pb, filename, "png", &err, tinfo, tinfo2, NULL); 
	g_free(filename);
	g_free(tinfo);
	g_free(tinfo2);
	g_object_unref(pb);
	String out(U"");
	if (!ok)
	{
		out = String(_("Cannot save file. ")) + err->message;
		g_error_free(err);
	}
	return std::make_pair(ok, out);
}
#endif

#ifdef CRN_USING_GDIPLUS
static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == nullptr)
		return -1;  // Failure

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

static std::pair<bool, String> save_png_gdiplus(const Path &filename, const ImageRGB &img)
{
	// gdi+ does not support URIs
	Path winname(filename);
	winname.ToWindows();
	const auto newsize = winname.Size() + 1;
	auto wcstring = std::vector<wchar_t>(newsize);
	auto convertedChars = size_t(0);
	mbstowcs_s(&convertedChars, wcstring.data(), newsize, winname.CStr(), _TRUNCATE);

	Gdiplus::Bitmap *outbm = new Gdiplus::Bitmap(INT(img.GetWidth()), INT(img.GetHeight()), PixelFormat24bppRGB);
	if (!outbm)
	{
		return std::make_pair(false, String(_("Cannot create bitmap")));
	}
	Gdiplus::BitmapData bitmapData;
	auto clip = Gdiplus::Rect(0, 0, outbm->GetWidth(), outbm->GetHeight());
	outbm->LockBits(&clip, Gdiplus::ImageLockModeWrite, PixelFormat24bppRGB, &bitmapData);
	auto *pixels = (uint8_t*)bitmapData.Scan0;
//#pragma omp parallel for
	FOREACHPIXEL(x, y, img)
	{
		size_t poffset = 3 * x + y * bitmapData.Stride;
		const auto &pix = img.At(x, y);
		pixels[poffset + 2] = pix.r;
		pixels[poffset + 1] = pix.g;
		pixels[poffset] = pix.b;
	}
	outbm->UnlockBits(&bitmapData);
	
	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	Gdiplus::Status stat = outbm->Save(wcstring.data(), &pngClsid, nullptr);
	delete outbm;
	return std::make_pair(stat == Gdiplus::Ok, String{});
}

#endif

/*! Saves as PNG file
 * \throws ExceptionInvalidArgument fname is empty
 * \throws	ExceptionRuntime	no way to save png
 * \param[in]	img	the image to save
 * \param[in]	fname	full path	to the image to save
 */
void crn::impl::SavePNG(const ImageRGB &img, const Path &fname)
{
	if (!fname)
		throw ExceptionInvalidArgument(StringUTF8("void Image::SavePNG(const Path &fname): ") + _("Null file name."));

	std::lock_guard<std::mutex> lock(crn::FileShield::GetMutex(fname)); // lock the file

	std::pair<bool, String> res(std::make_pair(false, String(U"")));
	String error;
#ifdef CRN_USING_LIBPNG
	if (res.first == false)
	{
		res = save_png_libpng(fname, img);
		error += U" " + res.second;
	}
#endif
#ifdef CRN_USING_GDIPLUS
	if (res.first == false)
	{
		res = save_png_gdiplus(fname, img);
		error += U" " + res.second;
	}
#endif
#ifdef CRN_USING_GDKPB
	if (res.first == false)
	{
		res = save_png_gdkpixbuf(fname, img);
		error += U" " + res.second;
	}
#endif
	if (res.first == false)
		throw ExceptionRuntime(StringUTF8("void Image::SavePNG(const Path &fname): ") + 
				_("No library for saving image found or write permissions on the file or directory are not granted. No image will be saved.") + "\n" + StringUTF8(error) + "\n" + StringUTF8(fname));
}

#ifdef CRN_USING_LIBJPEG
static std::pair<bool, String> save_jpeg_libjpeg(const Path &filename, const ImageRGB &img, int qual)
{
	// libjpeg does not support URIs
	Path fname(filename);
	fname.ToLocal();

	std::unique_ptr<FILE, decltype(fclose_if_not_null)*> fp(fopen(fname.CStr(), "wb"), fclose_if_not_null);
	if (!fp)
	{
		return std::make_pair(false, String(_("Cannot create file ")) + U"<" + fname + U">");
	}

	crn_jpeg_error_mgr jerr;
	struct jpeg_compress_struct cinfo;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = crn_jpeg_error_exit;
	if (setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_compress(&cinfo);
		return std::make_pair(false, String(_("Cannot create jpeg file structure.")));
	}

	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, fp.get());
	cinfo.image_width = int(img.GetWidth());
	cinfo.image_height = int(img.GetHeight());
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, qual, TRUE);
	jpeg_start_compress(&cinfo, TRUE);
	JSAMPROW scanline;
	scanline = new JSAMPLE[3 * img.GetWidth()];
	for (size_t y = 0; y < img.GetHeight(); ++y)
	{
		for (size_t x = 0; x < img.GetWidth(); ++x)
		{
			const auto &p = img.At(x, y);
			scanline[3 * x] = p.r;
			scanline[3 * x + 1] = p.g;
			scanline[3 * x + 2] = p.b;
		}
		jpeg_write_scanlines(&cinfo, &scanline, 1);
	}
	delete scanline;
	// write comment
	jpeg_write_marker(&cinfo, JPEG_COM, (JOCTET*)"Saved by libcrn.", (unsigned int)strlen("Saved by libcrn.")); 

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	return std::make_pair(true, String(U""));
}
#endif

#ifdef CRN_USING_GDKPB
static std::pair<bool, String> save_jpeg_gdkpixbuf(const Path &fname, const ImageRGB &img, int qual)
{
	GdkPixbuf *pb = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, 
			int(img.GetWidth()), int(img.GetHeight()));
	if (!pb)
	{
		return std::make_pair(false, String(_("Cannot create temporary buffer.")));
	}

	int w = gdk_pixbuf_get_width(pb);
	int h = gdk_pixbuf_get_height(pb);
	int rs = gdk_pixbuf_get_rowstride(pb);
	guchar *ppix = gdk_pixbuf_get_pixels(pb);
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			const auto px = img.At(x, y);
			int o2 = x + x + x + y * rs;
			ppix[o2] = px.r;
			ppix[o2 + 1] = px.g;
			ppix[o2 + 2] = px.b;
		}

	GError *err = NULL;
	gchar *utfname;
	gsize i;
	utfname = g_locale_to_utf8(fname.CStr(), -1, NULL, &i, NULL);
	gchar *filename;
	filename = g_filename_from_utf8(utfname, -1, NULL, &i, NULL);
	g_free(utfname);
	gchar *tinfo = g_locale_to_utf8("quality", -1, NULL, &i, NULL);
	char qt[16];
	sprintf(qt, "%i", qual);
	gchar *tinfo2 = g_locale_to_utf8(qt, -1, NULL, &i, NULL);
	bool ok = gdk_pixbuf_save(pb, filename, "jpeg", &err, tinfo, tinfo2, NULL); 
	g_free(filename);
	g_free(tinfo);
	g_free(tinfo2);
	g_object_unref(pb);
	String res(U"");
	if (!ok)
	{
		res = String(_("Cannot save file. ")) + err->message;
		g_error_free(err);
	}
	return std::make_pair(ok, res);
}
#endif

#ifdef CRN_USING_GDIPLUS
static std::pair<bool, String> save_jpeg_gdiplus(const Path &filename, const ImageRGB &img, int qual)
{
	// gdi+ does not support URIs
	Path winname(filename);
	winname.ToWindows();
	const auto newsize = winname.Size() + 1;
	auto wcstring = std::vector<wchar_t>(newsize);
	auto convertedChars = size_t(0);
	mbstowcs_s(&convertedChars, wcstring.data(), newsize, winname.CStr(), _TRUNCATE);

	Gdiplus::Bitmap *outbm = new Gdiplus::Bitmap(INT(img.GetWidth()), INT(img.GetHeight()), PixelFormat24bppRGB);
	if (!outbm)
	{
		return std::make_pair(false, String(_("Cannot create bitmap")));
	}
	Gdiplus::BitmapData bitmapData;
	auto clip = Gdiplus::Rect(0, 0, outbm->GetWidth(), outbm->GetHeight());
	outbm->LockBits(&clip, Gdiplus::ImageLockModeWrite, PixelFormat24bppRGB, &bitmapData);
	auto *pixels = (uint8_t*)bitmapData.Scan0;
//#pragma omp parallel for
	FOREACHPIXEL(x, y, img)
	{
		size_t poffset = 3 * x + y * bitmapData.Stride;
		const auto &pix = img.At(x, y);
		pixels[poffset + 2] = pix.r;
		pixels[poffset + 1] = pix.g;
		pixels[poffset] = pix.b;
	}
	outbm->UnlockBits(&bitmapData);
	CLSID jpegClsid;
	GetEncoderClsid(L"image/jpeg", &jpegClsid);
	Gdiplus::EncoderParameters encoderParameters;
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
	encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	ULONG quality = qual;
	encoderParameters.Parameter[0].Value = &quality;
	outbm->Save(wcstring.data(), &jpegClsid, &encoderParameters);
	delete outbm;
	return std::make_pair(true, String{});
}
#endif

/*! Saves as JPEG file
 * \throws	ExceptionInvalidArgument fname is empty
 * \throws	ExceptionRuntime	no way to save jpeg
 * \param[in]	img	the image to save
 * \param[in]	fname	full path	to the image to save
 * \param[in]	qual	compression quality [0..100]
 */
void crn::impl::SaveJPEG(const ImageRGB &img, const Path &fname, unsigned int qual)
{
	if (!fname)
		throw ExceptionInvalidArgument(StringUTF8("bool SaveJPEG(const ImageRGB &img, const Path &fname, int qual): ") + 
				_("Null file name."));

	std::lock_guard<std::mutex> lock(crn::FileShield::GetMutex(fname)); // lock the file

	std::pair<bool, String> res(std::make_pair(false, String(U"")));
	String error;
#ifdef CRN_USING_LIBJPEG
	if (res.first == false)
	{
		res = save_jpeg_libjpeg(fname, img, qual);
		error += U" " + res.second;
	}
#endif
#ifdef CRN_USING_GDIPLUS
	if (res.first == false)
	{
		res = save_jpeg_gdiplus(fname, img, qual);
		error += U" " + res.second;
	}
#endif
#ifdef CRN_USING_GDKPB
	if (res.first == false)
	{
		res = save_jpeg_gdkpixbuf(fname, img, qual);
		error += U" " + res.second;
	}
#endif
	if (res.first == false)
		throw ExceptionRuntime(StringUTF8("void Image::SaveJPEG(const Path &fname, int qual): ") + 
				_("No library for saving image found or write permissions on the file or directory are not granted. No image will be saved.") + StringUTF8(error));
}

/*************************************************************************************
 * Edition
 ************************************************************************************/

/*! Smooths and enhances the contrast
 * \throws	ExceptionInvalidArgument	iter is null
 * \param[in]	img	the image to modify
 * \param[in]	iter	the number of iterations
 * \param[in]	maxdist	the maximal distance between to colors of the same class
 */
void crn::EdgePreservingFilter(ImageRGB &img, size_t iter, uint8_t maxdist)
{
	if (iter == 0)
		throw ExceptionInvalidArgument(StringUTF8("EdgePreservingFilter(ImageRGB &img, size_t iter, uint8_t maxdist): ") + _("The number of iterations is null."));
	// buffer
	auto newpix = ImageRGB(img.GetWidth(), img.GetHeight());
	// reference buffer
	auto refpix = img;

	const int lin[8] = {-1,-1,-1, 0, 1, 1, 1, 0};
	const int col[8] = {-1, 0, 1, 1, 1, 0,-1,-1};
	for (size_t tmp = 0; tmp < iter; ++tmp)
	{ // for each iter
		for (int y = 0; y < int(img.GetHeight()); ++y)
			for (int x = 0; x < int(img.GetWidth()); ++x)
			{
				int tr = 0, tg = 0, tb = 0, mr = 0, mg = 0, mb = 0;
				for (size_t d = 0; d < 8; ++d)
				{ // for each neighbour
					int xx = x + col[d];
					int yy = y + lin[d];
					if ((xx >= 0) && (xx < int(img.GetWidth())) && (yy >= 0) && (yy < int(img.GetHeight())))
					{
						int dr = Max(0, 1 - Abs(refpix.At(x, y).r - img.At(xx, yy).r) / maxdist);
						tr += dr;
						mr += dr * img.At(xx, yy).r;
						int dg = Max(0, 1 - Abs(refpix.At(x, y).g - img.At(xx, yy).g) / maxdist);
						tg += dg;
						mg += dg * img.At(xx, yy).g;
						int db = Max(0, 1 - Abs(refpix.At(x, y).b - img.At(xx, yy).b) / maxdist);
						tb += db;
						mb += db * img.At(xx, yy).b;
					}
				}
				// update buffer
				if (tr) mr /= tr;
				newpix.At(x, y).r = uint8_t(mr);
				if (tg) mg /= tg;
				newpix.At(x, y).g = uint8_t(mg);
				if (tb) mb /= tb;
				newpix.At(x, y).b = uint8_t(mb);
			}
		// swap buffer and reference buffer
		std::swap(newpix, refpix);
	}
	img = std::move(refpix);
}

/*************************************************************************************
 * Conversion
 ************************************************************************************/

/*****************************************************************************/
/*!
 * Saturation estimation from RGB data
 *
 * \param[in]	img	the source image
 * \return Gray image representing  saturation for this image
 */
ImageGray crn::SChannel(const ImageRGB &img)
{
	auto ri = ImageGray(img.GetWidth(), img.GetHeight());
	for (size_t tmp = 0; tmp < img.Size(); tmp++)
	{
		const auto px = img.At(tmp);
		auto max = Max(px.r, px.g, px.b);
		auto min = Min(px.r, px.g, px.b);
		auto sat = 0;
		if (max > 0)
			sat = 255 - ((255 * min) / max);
		ri.At(tmp) = uint8_t(sat);
	}
	return ri;
}

/*****************************************************************************/
/*!
 * Computes the pseudo-luminance channel U' = L * (1 - S)
 *
 * \param[in]	img	the source image
 * \return A gray image representing pseudo-luminance for this image
 */
ImageGray crn::LPrime(const ImageRGB &img)
{
	auto g = YChannel(img);
	for (size_t tmp = 0; tmp < img.Size(); tmp++)
	{
		const auto px = img.At(tmp);
		int max = Max(px.r, px.g, px.b);
		int min = Min(px.r, px.g, px.b);
		auto sat = 0.0;
		if (max > 0)
			sat = 1.0 - ((double)min / max);
		auto s = (int)(sat * 255);
		auto v = int(g.At(tmp));
		v *= 255 - s;
		v /= 255;
		g.At(tmp) = (uint8_t)Cap(v, 0, 255);
	}
	return g;
}

/*****************************************************************************/
/*!
 * Computes the pseudo-saturation channel
 * Psat(x,y) = max(abs(b(x,y) - g(x,y)), max(abs(r(x,y) - g(x,y)), abs(r(x,y) - b(x,y))))
 *
 * \param[in]	img	the source image
 * \return A gray image representing pseudo-saturation for this image
 */
ImageGray crn::PseudoSaturation(const ImageRGB &img)
{
	ImageGray pseudo_saturation_img = ImageGray(img.GetWidth(), img.GetHeight());

	for (auto k : Range(img))
	{
		const auto px = img.At(k);
		pseudo_saturation_img.At(k) = (uint8_t) Max(abs(px.b - px.g), abs(px.r - px.g), abs(px.r - px.b));
	}

	return pseudo_saturation_img;
}


CRN_BEGIN_CLASS_CONSTRUCTOR(RGB2GrayRed)
	CRN_DATA_FACTORY_REGISTER(U"RGB2GrayRed", RGB2GrayRed)
CRN_END_CLASS_CONSTRUCTOR(RGB2GrayRed)

CRN_BEGIN_CLASS_CONSTRUCTOR(RGB2GrayGreen)
	CRN_DATA_FACTORY_REGISTER(U"RGB2GrayGreen", RGB2GrayGreen)
CRN_END_CLASS_CONSTRUCTOR(RGB2GrayGreen)

CRN_BEGIN_CLASS_CONSTRUCTOR(RGB2GrayBlue)
	CRN_DATA_FACTORY_REGISTER(U"RGB2GrayBlue", RGB2GrayBlue)
CRN_END_CLASS_CONSTRUCTOR(RGB2GrayBlue)

CRN_BEGIN_CLASS_CONSTRUCTOR(RGB2GraySaturation)
	CRN_DATA_FACTORY_REGISTER(U"RGB2GraySaturation", RGB2GraySaturation)
CRN_END_CLASS_CONSTRUCTOR(RGB2GraySaturation)

CRN_BEGIN_CLASS_CONSTRUCTOR(RGB2GrayValue)
	CRN_DATA_FACTORY_REGISTER(U"RGB2GrayValue", RGB2GrayValue)
CRN_END_CLASS_CONSTRUCTOR(RGB2GrayValue)

CRN_BEGIN_CLASS_CONSTRUCTOR(RGB2GrayY)
	CRN_DATA_FACTORY_REGISTER(U"RGB2GrayY", RGB2GrayY)
CRN_END_CLASS_CONSTRUCTOR(RGB2GrayY)

CRN_BEGIN_CLASS_CONSTRUCTOR(RGB2GrayL)
	CRN_DATA_FACTORY_REGISTER(U"RGB2GrayU", RGB2GrayL)
CRN_END_CLASS_CONSTRUCTOR(RGB2GrayL)

CRN_BEGIN_CLASS_CONSTRUCTOR(RGB2GrayLPrime)
	CRN_DATA_FACTORY_REGISTER(U"RGB2GrayLPrime", RGB2GrayLPrime)
CRN_END_CLASS_CONSTRUCTOR(RGB2GrayLPrime)

