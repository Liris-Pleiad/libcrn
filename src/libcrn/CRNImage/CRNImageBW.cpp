/* Copyright 2006-2015 Yann LEYDIER, INSA-Lyon, CoReNum
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
 * file: CRNImageBW.cpp
 * \author Yann LEYDIER
 */

#include <CRNImage/CRNImageBW.h>
#include <CRNStatistics/CRNHistogram.h>
#include <CRNMath/CRNMatrixInt.h>
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
struct crn_jpeg_error_mgr {
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
#	undef max
#endif

void fclose_if_not_null(FILE *f);

using namespace crn;

#ifdef CRN_USING_LIBPNG
static std::pair<bool, String> save_png_libpng(const Path &filename, const ImageBW &img)
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
	int bit_depth = 1;
	int color_type = PNG_COLOR_TYPE_GRAY;
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
#if (PNG_LIBPNG_VER > 10300)
		row_pointers[i] = png_get_rowbytes(png_ptr, info_ptr);
#else
		row_pointers[i] = (png_bytep)calloc(1, info_ptr->rowbytes);
#endif
	}

	/* Conversion */
	//int channels = info_ptr->channels;
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			if (img.At(x, y))
				row_pointers[y][x / 8] = png_byte(row_pointers[y][x / 8] | (1 << (7 - x % 8)));

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
static std::pair<bool, String> save_png_gdkpixbuf(const Path &fname, const ImageBW &img)
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
			const uint8_t px = img.At(x, y) ? 255 : 0;
			int o2 = x + x + x + y * rs;
			ppix[o2] = px;
			ppix[o2 + 1] = px;
			ppix[o2 + 2] = px;
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

static std::pair<bool, String> save_png_gdiplus(const Path &filename, const ImageBW &img)
{
	// gdi+ does not support URIs
	Path winname(filename);
	winname.ToWindows();
	const auto newsize = winname.Size() + 1;
	auto wcstring = std::vector<wchar_t>(newsize);
	auto convertedChars = size_t(0);
	mbstowcs_s(&convertedChars, wcstring.data(), newsize, winname.CStr(), _TRUNCATE);

	Gdiplus::Bitmap *outbm = outbm = new Gdiplus::Bitmap(INT(img.GetWidth()), INT(img.GetHeight()), PixelFormat1bppIndexed);
	if (!outbm)
	{
		return std::make_pair(false, String(_("Cannot create bitmap")));
	}
	Gdiplus::BitmapData bitmapData;
	auto clip = Gdiplus::Rect(0, 0, outbm->GetWidth(), outbm->GetHeight());
	outbm->LockBits(&clip, Gdiplus::ImageLockModeWrite, PixelFormat1bppIndexed, &bitmapData);
	auto *pixels = (uint8_t*)bitmapData.Scan0;
	//#pragma omp parallel for
	FOREACHPIXEL(x, y, img)
	{
		size_t poffset = x / 8 + y * bitmapData.Stride;
		if (img.At(x, y))
		{
			uint8_t b = 1 << (7 - (x % 8));
			pixels[poffset] |= b;
		}
	}
	outbm->UnlockBits(&bitmapData);

	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	Gdiplus::Status stat = outbm->Save(wcstring.data(), &pngClsid, NULL);
	delete outbm;
	return std::make_pair(stat == Gdiplus::Ok, String{});
}
#endif
/*!
 * Saves the image to a PNG file
 *
 * \throws ExceptionInvalidArgument fname is empty or pixel format is not BW, Gray nor RGB.
 * \throws	ExceptionRuntime	no way to save png
 *
 * \param[in]	fname	the filename
 */
void crn::impl::SavePNG(const ImageBW &img, const Path &fname)
{
	if (!fname)
		throw ExceptionInvalidArgument(StringUTF8("void Image::SaveBW(const Path &fname): ") + _("Null file name."));

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
		throw ExceptionRuntime(StringUTF8("void crn::SaveBW(const ImageGray &img, const Path &fname): ") + 
				_("No library for saving image found or write permissions on the file or directory are not granted. No image will be saved.") + "\n" + StringUTF8(error) + "\n" + StringUTF8(fname));
}

#ifdef CRN_USING_LIBJPEG
static std::pair<bool, String> save_jpeg_libjpeg(const Path &filename, const ImageBW &img, int qual)
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
	cinfo.input_components = 1;
	cinfo.in_color_space = JCS_GRAYSCALE;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, qual, TRUE);
	jpeg_start_compress(&cinfo, TRUE);
	JSAMPROW scanline = new JSAMPLE[img.GetWidth()];
	for (size_t y = 0; y < img.GetHeight(); ++y)
	{
		for (size_t x = 0; x < img.GetWidth(); ++x)
		{
			scanline[x] = img.At(x, y) ? 255 : 0;
		}
		jpeg_write_scanlines(&cinfo, &scanline, 1);
	}
	delete scanline;
	// write comment
	jpeg_write_marker(&cinfo, JPEG_COM, (JOCTET*)"Saved by libcrn.", (unsigned int)(strlen("Saved by libcrn.")));

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	return std::make_pair(true, String(U""));
}
#endif

#ifdef CRN_USING_GDKPB
static std::pair<bool, String> save_jpeg_gdkpixbuf(const Path &fname, const ImageBW &img, int qual)
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
			const uint8_t px = img.At(x, y) ? 255 : 0;
			int o2 = x + x + x + y * rs;
			ppix[o2] = px;
			ppix[o2 + 1] = px;
			ppix[o2 + 2] = px;
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
static std::pair<bool, String> save_jpeg_gdiplus(const Path &filename, const ImageBW &img, int qual)
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
		const auto pix = img.At(x, y) ? uint8_t(255) : uint8_t(0);
		pixels[poffset + 2] = pix;
		pixels[poffset + 1] = pix;
		pixels[poffset] = pix;
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

/*!
 * Saves the image to a JPEG file
 *
 * \throws	ExceptionInvalidArgument fname is empty
 * \throws	ExceptionRuntime	no way to save jpeg
 *
 * \param[in]	fname	the filename
 * \param[in]	qual	the quality of the compression (from 0 to 100)
 */
void crn::impl::SaveJPEG(const ImageBW &img, const Path &fname, unsigned int qual)
{
	if (!fname)
		throw ExceptionInvalidArgument(StringUTF8("bool crn::SaveJPEG(const ImageBW &img, const Path &fname, int qual): ") + 
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
		throw ExceptionRuntime(StringUTF8("void crn::SaveJPEG(const ImageBW &img, const Path &fname, int qual): ") + 
				_("No library for saving image found or write permissions on the file or directory are not granted. No image will be saved.") + StringUTF8(error));
}

/*! Computes the left profile
 * \return	an histogram containing the profile
 */
Histogram crn::LeftProfile(const ImageBW &img)
{
	auto h = Histogram(img.GetHeight());
	for (size_t y = 0; y < img.GetHeight(); y++)
	{
		size_t x;
		for (x = 0; x < img.GetWidth(); x++)
			if (!img.At(x, y))
				break;
		h.SetBin(y, (unsigned int)x);
	}
	return h;
}

/*! Computes the right profile
 * \return	an histogram containing the profile
 */
Histogram crn::RightProfile(const ImageBW &img)
{
	auto h = Histogram(img.GetHeight());
	for (size_t y = 0; y < img.GetHeight(); y++)
	{
		int x;
		for (x = int(img.GetWidth()) - 1; x >= 0; --x)
			if (!img.At(x, y))
				break;
		h.SetBin(y, (unsigned int)(img.GetWidth() - 1 - x));
	}
	return h;
}

/*! Computes the top profile
 * \return	an histogram containing the profile
 */
Histogram crn::TopProfile(const ImageBW &img)
{
	auto h = Histogram(img.GetWidth());
	for (size_t x = 0; x < img.GetWidth(); x++)
	{
		size_t y;
		for (y = 0; y < img.GetHeight(); y++)
			if (!img.At(x, y))
				break;
		h.SetBin(x, (unsigned int)y);
	}
	return h;
}

/*! Computes the bottom profile
 * \return	an histogram containing the profile
 */
Histogram crn::BottomProfile(const ImageBW &img)
{
	auto h = Histogram(img.GetWidth());
	for (size_t x = 0; x < img.GetWidth(); x++)
	{
		int y;
		for (y = int(img.GetHeight()) - 1; y >= 0; --y)
			if (!img.At(x, y))
				break;
		h.SetBin(x, (unsigned int)(img.GetHeight() - 1 - y));
	}
	return h;
}

/*! Computes the horizontal projection
 * \return	an histogram containing the projection
 */
Histogram crn::HorizontalProjection(const ImageBW &img)
{
	auto h = Histogram(img.GetHeight());
	for (size_t y = 0; y < img.GetHeight(); y++)
	{
		unsigned int cnt = 0;
		for (size_t x = 0; x < img.GetWidth(); x++)
			if (!img.At(x, y))
				cnt += 1;
		h.SetBin(y, cnt);
	}
	return h;
}

/*! Computes the vertical projection
 * \return	an histogram containing the projection
 */
Histogram crn::VerticalProjection(const ImageBW &img)
{
	auto h = Histogram(img.GetWidth());
	for (size_t x = 0; x < img.GetWidth(); x++)
	{
		unsigned int cnt = 0;
		for (size_t y = 0; y < img.GetHeight(); y++)
			if (!img.At(x, y))
				cnt += 1;
		h.SetBin(x, cnt);
	}
	return h;
}

/*! Computes the vertical projection after rotation
 * \return	an histogram containing the projection
 */
Histogram crn::VerticalSlantedProjection(const ImageBW &img, const Angle<Radian> &theta)
{
	const auto cos_theta = theta.Cos();
	const auto sin_theta = theta.Sin();

	const auto head = double(img.GetWidth()) * cos_theta;
	const auto tail = double(img.GetHeight()) * sin_theta;

	auto hist = Histogram(size_t(tail + head) + 1);

	for (size_t l = 0; l < img.GetHeight(); l++)
	{
		auto previous_pixel = bool(img.At(0, l));

		if (!previous_pixel)
		{
			auto x = 0.0;
			auto y = double(img.GetHeight()) - double(l);

			auto x_prime = x * cos_theta - y * sin_theta;

			hist.IncBin((size_t)(x_prime + tail), 1);
		}

		for (size_t c = 1; c < img.GetWidth(); c++)
		{
			auto current_pixel = img.At(c, l);

			if (!current_pixel)
			{
				auto x = double(c);
				auto y = double(img.GetHeight()) - double(l);

				auto x_prime = x * cos_theta - y * sin_theta;

				hist.IncBin(size_t(x_prime + tail), 1);
			}

			previous_pixel = current_pixel;
		}
	}

	return hist;
}

/*!
 * Gets the mean horizontal black run
 *
 * \param[in]	img	the source image
 * \return The mean black run
 */
double crn::MeanBlackRun(const ImageBW &img) noexcept
{
	auto sum = 0L;
	auto cnt = 0L;
	for (size_t y = 0; y < img.GetHeight(); y++)
	{
		auto beg = size_t(0);
		for (size_t x = 1; x < img.GetWidth(); x++)
		{
			if (img.At(x - 1, y) && !img.At(x, y))
			{
				beg = x;
			}
			else if (!img.At(x - 1, y) && img.At(x, y))
			{
				sum += long(x) - long(beg);
				cnt += 1;
			}
		} // for x
	} // for y
	return (double)sum / (double)cnt;
}

/*!
 * Gets the mean horizontal white run.
 * This functions uses the mean black run in order to filter the page runs and only keep the intra-word runs.
 *
 * \param[in]	img	the source image
 * \param[in]	blackrun	The mean black run. If -1, then computes it. default = -1.
 * \return The mean white run
 */
double crn::MeanWhiteRun(const ImageBW &img, int blackrun) noexcept
{
	const auto RUNFACT = 2;
	if (blackrun == -1)
		blackrun = (int)MeanBlackRun(img);
	auto sum = 0L;
	auto cnt = 0L;
	for (size_t y = 0; y < img.GetHeight(); y++)
	{
		auto beg = size_t(0);
		for (size_t x = 1; x < img.GetWidth(); x++)
		{
			if (!img.At(x - 1, y) && img.At(x, y))
			{
				beg = x;
			}
			else if (img.At(x - 1, y) && !img.At(x, y))
			{
				if (int(x - beg) < blackrun * RUNFACT)
				{
					sum += long(x) - long(beg);
					cnt += 1;
				}
			}
		} // for x
	} // for y
	if (!cnt)
		return 0.0;
	else
		return (double)sum / (double)cnt;
}

/*!
 * Gets the mean vertical black run
 *
 * \param[in]	img	the source image
 * \return The mean black run
 */
double crn::MeanBlackVRun(const ImageBW &img) noexcept
{
	auto sum = 0L;
	auto cnt = 0L;
	for (size_t x = 0; x < img.GetWidth(); x++)
	{
		bool in = false;
		size_t beg = 0;
		for (size_t y = 1; y < img.GetHeight(); y++)
		{
			if (img.At(x, y - 1) && !img.At(x, y))
			{
				beg = y;
				in = true;
			}
			else if (!img.At(x, y - 1) && img.At(x, y))
			{
				if (in)
				{
					sum += long(y) - long(beg);
					cnt += 1;
				}
				in = false;
			}
		} // for x
	} // for y
	if (!cnt)
		return 0.0;
	return (double)sum / (double)cnt;
}

/*****************************************************************************/
/*!
 * \param[in]	img	the source image
 * Returns the number of black pixels
 */
size_t crn::CountBlackPixels(const ImageBW &img) noexcept
{
	auto cnt = size_t(0);
	for (auto p : img)
		if (!p)
			cnt += 1;
	return cnt;
}

/*****************************************************************************/
/*!
 * \param[in]	img	the source image
 * Returns the number of white pixels
 */
size_t crn::CountWhitePixels(const ImageBW &img) noexcept
{
	auto cnt = size_t(0);
	for (auto p : img)
		if (p)
			cnt += 1;
	return cnt;
}

/*****************************************************************************/
/*!
 * Removes isolated pixels
 *
 * \throws	ExceptionDomain	min_neighbors size is greater or equal to 8
 *
 * \param[in]	img	the image to modify
 * \param[in]	min_neighbors	if (\#black neighbors <= parameter) then remove black pixel
 * \return	the number of deleted pixels
 */
size_t crn::Regularize(ImageBW &img, size_t min_neighbors)
{
	if (min_neighbors > 7)
	{
		throw ExceptionDomain(crn::StringUTF8("Cleanup(ImageBW &img, size_t min_neighbors): ") + _("Min neighbors must be < 8."));
	}

	auto nb = size_t(0);
	auto npix = img;
	for (auto y = size_t(1); y < img.GetHeight() - 1; ++y)
		for (auto x = size_t(1); x < img.GetWidth() - 1; ++x)
		{
			if (!img.At(x, y))
			{
				size_t n = 0;
				if (!img.At(x - 1, y - 1))
					n += 1;
				if (!img.At(x, y - 1))
					n += 1;
				if (!img.At(x + 1, y - 1))
					n += 1;
				if (!img.At(x + 1, y))
					n += 1;
				if (!img.At(x + 1, y + 1))
					n += 1;
				if (!img.At(x, y + 1))
					n += 1;
				if (!img.At(x - 1, y + 1))
					n += 1;
				if (!img.At(x - 1, y))
					n += 1;
				if (n <= min_neighbors)
				{
					npix.At(x, y) = pixel::BWWhite;
					nb += 1;
				}
			}
		}

	img = std::move(npix);
	return nb;
}

/*****************************************************************************/
/*!
 * Creates an image containing the distance transform
 *
 * \throws	ExceptionDimension	matrices are of different sizes
 * \throws	ExceptionInvalidArgument	matrices have even size
 *
 * \param[in]	img	the source image
 * \param[in]	m1	1st pass matrix. Negative numbers are ignored.
 * \param[in]	m2	2nd pass matrix. Negative numbers are ignored.
 * \return	a new image containing the distance transform
 */
ImageIntGray crn::DistanceTransform(const ImageBW &img, const MatrixInt &m1, const MatrixInt &m2)
{
	if ((m1.GetRows() != m2.GetRows()) || (m1.GetCols() != m2.GetCols()))
	{
		throw ExceptionDimension(StringUTF8("ImageIntGray DistanceTransform(const ImageBW &img, const MatrixInt *m1, const MatrixInt *m2): ") + _("matrices of different sizes"));
	}
	if (!(m1.GetRows() & 1) || !(m1.GetCols() & 1))
	{
		throw ExceptionInvalidArgument(StringUTF8("ImageIntGray DistanceTransform(const ImageBW &img, const MatrixInt &m1, const MatrixInt &m2): ") + _("even matrix dimensions."));
	}
	auto hw = m1.GetCols() / 2;
	auto hh = m1.GetRows() / 2;

	ImageIntGray dt = ImageIntGray(img.GetWidth(), img.GetHeight());
	for (auto tmp : Range(img))
		dt.At(tmp) = img.At(tmp) ? 0 : 1;

	for (size_t y = hh; y < img.GetHeight() - hh; y++)
		for (size_t x = hw; x < img.GetWidth() - hw; x++)
			if (dt.At(x, y))
			{
				auto val = std::numeric_limits<int>::max();
				for (size_t l = 0; l < m1.GetRows(); l++)
					for (size_t c = 0; c < m1.GetCols(); c++)
					{
						auto mval = m1.At(l, c);
						if (mval >= 0)
						{
							int imval = dt.At(x + c - hw, y + l - hh);
							imval += mval;
							if (imval < val) val = imval;
						}
					}
				dt.At(x, y) = val;
			}

	for (size_t y = img.GetHeight() - hh - 1; y >= hh; y--)
		for (size_t x = img.GetWidth() - hw - 1; x >= hw; x--)
			if (dt.At(x, y))
			{
				int val = std::numeric_limits<int>::max();
				for (size_t l = 0; l < m2.GetRows(); l++)
					for (size_t c = 0; c < m2.GetCols(); c++)
					{
						int mval = m2.At(l, c);
						if (mval >= 0)
						{
							int imval = dt.At(x + c - hw, y + l - hh);
							imval += mval;
							if (imval < val) val = imval;
						}
					}
				dt.At(x, y) = val;
			}

	return dt;
}

