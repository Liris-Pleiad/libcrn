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
 * file: CRNImageGray.cpp
 * \author Yann LEYDIER
 */

#include <CRNImage/CRNImageGray.h>
#include <CRNData/CRNInt.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNBlock.h>
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
#endif

void fclose_if_not_null(FILE *f);

using namespace crn;

#ifdef CRN_USING_LIBPNG
static std::pair<bool, String> save_png_libpng(const Path &filename, const ImageGray &img)
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
			row_pointers[y][x] = img.At(x, y);

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
static std::pair<bool, String> save_png_gdkpixbuf(const Path &fname, const ImageGray &img)
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

static std::pair<bool, String> save_png_gdiplus(const Path &filename, const ImageGray &img)
{
	// gdi+ does not support URIs
	Path winname(filename);
	winname.ToWindows();
	const auto newsize = winname.Size() + 1;
	auto wcstring = std::vector<wchar_t>(newsize);
	auto convertedChars = size_t(0);
	mbstowcs_s(&convertedChars, wcstring.data(), newsize, winname.CStr(), _TRUNCATE);

	Gdiplus::Bitmap *outbm = new Gdiplus::Bitmap(INT(img.GetWidth()), INT(img.GetHeight()), PixelFormat8bppIndexed);
	if (!outbm)
	{
		return std::make_pair(false, String(_("Cannot create bitmap")));
	}
	Gdiplus::ColorPalette *pal = (Gdiplus::ColorPalette *)malloc(sizeof(Gdiplus::ColorPalette) + 255 * sizeof(Gdiplus::ARGB));
	pal->Count = 256;
	pal->Flags = Gdiplus::PaletteFlagsGrayScale;
	for (size_t tmp = 0; tmp < 256; ++tmp)
		pal->Entries[tmp] = Gdiplus::ARGB(0xFF000000 | tmp | (tmp << 8) | (tmp << 16));
	outbm->SetPalette(pal);
	Gdiplus::BitmapData bitmapData;
	outbm->LockBits(&Gdiplus::Rect(0, 0, outbm->GetWidth(), outbm->GetHeight()), Gdiplus::ImageLockModeWrite, PixelFormat8bppIndexed, &bitmapData);
	auto *pixels = (uint8_t*)bitmapData.Scan0;
	//#pragma omp parallel for
	FOREACHPIXEL(x, y, img)
	{
		pixels[x + y * bitmapData.Stride] = img.At(x, y);
	}
	outbm->UnlockBits(&bitmapData);

	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	Gdiplus::Status stat = outbm->Save(wcstring.data(), &pngClsid, nullptr);
	delete outbm;
	free(pal);
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
void crn::impl::SavePNG(const ImageGray &img, const Path &fname)
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
		throw ExceptionRuntime(StringUTF8("void crn::SavePNG(const ImageGray &img, const Path &fname): ") + 
				_("No library for saving image found or write permissions on the file or directory are not granted. No image will be saved.") + "\n" + StringUTF8(error) + "\n" + StringUTF8(fname));
}

#ifdef CRN_USING_LIBJPEG
static std::pair<bool, String> save_jpeg_libjpeg(const Path &filename, const ImageGray &img, int qual)
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
			scanline[x] = img.At(x, y);
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
static std::pair<bool, String> save_jpeg_gdkpixbuf(const Path &fname, const ImageGray &img, int qual)
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
			auto px = img.At(x, y);
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
static std::pair<bool, String> save_jpeg_gdiplus(const Path &filename, const ImageGray &img, int qual)
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
		const auto pix = img.At(x, y);
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
void crn::impl::SaveJPEG(const ImageGray &img, const Path &fname, unsigned int qual)
{
	if (!fname)
		throw ExceptionInvalidArgument(StringUTF8("bool crn::SaveJPEG(const ImageGray &img, const Path &fname, int qual): ") + 
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
		throw ExceptionRuntime(StringUTF8("void crn::SaveJPEG(const ImageGray &img, const Path &fname, int qual): ") + 
				_("No library for saving image found or write permissions on the file or directory are not granted. No image will be saved.") + StringUTF8(error));
}

/*! Replaces the pixels with their square root
 * \param[in]	img	the image to modify
 */
void crn::Sqrt(ImageDoubleGray &img) noexcept
{
	for (auto &px : img)
		px = sqrt(px);
}

/*! Stretches the histogram
 * \param[in]	img	the image to modify
 */
void crn::AutoContrast(ImageGray &img)
{
	int pb = 0, pw = 255;
	int ppb = 0, ppw = 255;
	auto h = MakeHistogram(img);
	do
	{
		unsigned int cumb = 0, cumw = 0;
		size_t nbb = 0, nbw = 0;
		for (int val = 0; val < int(h.Size()); ++val)
		{
			int db = Abs(pb - val);
			int dw = Abs(pw - val);
			if (db < dw)
			{
				cumb += val * h.GetBin(val);
				nbb += h.GetBin(val);
			}
			else
			{
				cumw += val * h.GetBin(val);
				nbw += h.GetBin(val);
			}
		}
		ppb = pb;
		ppw = pw;
		if (nbb)
			pb = cumb / int(nbb);
		if (nbw)
			pw = cumw / int(nbw);
	} while ((ppb != pb) || (ppw != pw));
	int diff = pw - pb;
	if (diff != 255)
	{
		for (auto tmp : Range(img))
		{
			if (img.At(tmp) < pb)
				img.At(tmp) = 0;
			else if (img.At(tmp) > pw)
				img.At(tmp) = 255;
			else
			{
				img.At(tmp) = uint8_t(((img.At(tmp) - pb) * 255) / diff);
			}
		}
	}
}

/*! Computes the horizontal projection
 * \return	an histogram containing the projection
 */
Histogram crn::HorizontalProjection(const ImageGray &img)
{
	auto h = Histogram(img.GetHeight());
	for (size_t y = 0; y < img.GetHeight(); y++)
	{
		unsigned int cnt = 0;
		for (size_t x = 0; x < img.GetWidth(); x++)
			cnt += img.At(x, y);
		h.SetBin(y, cnt / 255);
	}
	return h;
}

/*! Computes the vertical projection
 * \return	an histogram containing the projection
 */
Histogram crn::VerticalProjection(const ImageGray &img)
{
	auto h = Histogram(img.GetWidth());
	for (size_t x = 0; x < img.GetWidth(); x++)
	{
		unsigned int cnt = 0;
		for (size_t y = 0; y < img.GetHeight(); y++)
			cnt += img.At(x, y);
		h.SetBin(x, cnt / 255);
	}
	return h;
}


/*! Creates an histogram from the pixels
 * \param[in]	img	the image to binarize
 * \return	the histogram
 */
Histogram crn::MakeHistogram(const ImageGray &img)
{
	auto h = Histogram(256);
	for (auto v : img)
		h.IncBin(v);
	return h;
}

/*****************************************************************************/
/*!
 * Computes the mean text line x-height
 *
 * \param[in]	img	the source image
 * \return	the mean text line x-height
 */
size_t crn::EstimateLinesXHeight(const ImageGray &img, unsigned int xdiv)
{
	auto ig = ImageGray(img.GetWidth() / xdiv, img.GetHeight());
	FOREACHPIXEL(x, y, ig)
	{
		uint8_t minv = 255;
		for (size_t tx = x * xdiv; tx < Min(img.GetWidth(), (x + 1) * xdiv); ++tx)
		{
			if (img.At(tx, y) < minv)
				minv = img.At(tx, y);
		}
		ig.At(x, y) = minv;
	}
	auto ibw = Fisher(ig);
	ig = ImageGray{}; // free memory
	auto m1 = MatrixInt(1, 3, 1);
	ibw.Dilate(m1);
	auto m2 = MatrixInt(1, 5, 1);
	ibw.Erode(m2);
	auto h = Histogram(250);
	for (size_t x = 0; x < ibw.GetWidth(); ++x)
	{
		size_t y = 0;
		bool stop = false;
		do
		{
			while (ibw.At(x, y))
			{
				y += 1;
				if (y >= ibw.GetHeight())
				{
					stop = true;
					break;
				}
			}
			if (stop)
				break;
			auto by = y;
			while (!ibw.At(x, y))
			{
				y += 1;
				if (y >= ibw.GetHeight())
				{
					stop = true;
					break;
				}
			}
			h.IncBin(Min(size_t(249), y - by));
		} while (!stop);
	}
	h.AverageSmoothing(2);
	return int(Max(size_t(floor(h.Mean())), h.Argmax()));
}

/*! \warning	very slow
 * \param[in]	img	the source image
 * \return	the median distance between two baselines */
size_t crn::EstimateLeading(const ImageGray &img)
{
	const auto sw = StrokesWidth(img);

	auto ly = ImageIntGray(img.GetWidth(), img.GetHeight());
	for (auto tmp : Range(ly))
		ly.At(tmp) = img.At(tmp);
	ly.Dilate(crn::MatrixInt{2 * sw + 1, 1, 1});
	ly.Dilate(crn::MatrixInt{1, 6 * sw + 1, 1});
	ly.Erode(crn::MatrixInt{1, 16 * sw + 1, 1});
	ly.Convolve(crn::MatrixDouble::NewGaussianLine(double(sw)));
	auto gy = crn::MatrixDouble::NewGaussianLineDerivative(double(sw)/3.0);
	gy.Transpose();
	ly.Convolve(gy);
	for (auto tmp : Range(ly))
		if (ly.At(tmp) < 0)
			ly.At(tmp) *= -1;
		else
			ly.At(tmp) = 0;

	auto bl = Fisher(ly);
	auto histo = crn::Histogram{img.GetHeight()};
	for (size_t x = 0; x < img.GetWidth(); ++x)
	{
		bool w = bl.At(x, 0);
		size_t by = 0;
		for (size_t y = 1; y < img.GetHeight(); ++y)
		{
			if (bl.At(x, y))
			{
				if (!w)
				{
					w = true;
				}
			}
			else
			{
				if (w)
				{
					histo.IncBin(y - by);
					by = y;
					w = false;
				}
			}
		}
	}
	return histo.MedianValue();
}

/*!
 * Estimates the mean skew of the document's lines
 * \param[in]	img	the source image
 * \return	the mean angle of the document's lines
 */
Angle<Radian> crn::EstimateSkew(const ImageGray &img)
{
	size_t xh = EstimateLinesXHeight(img);
	size_t div = xh / 2;
	if (div == 0)
		return 0;
	// create a thumbnail
	auto ig = ImageGray(img.GetWidth() / div, img.GetHeight());
	FOREACHPIXEL(x, y, ig)
	{
		uint8_t minv = 255;
		for (size_t tx = x * div; tx < Min(img.GetWidth(), (x + 1) * div); ++tx)
		{
			if (img.At(tx, y) < minv)
				minv = img.At(tx, y);
		}
		ig.At(x, y) = minv;
	}
	// binarize and filter
	auto ibw = std::make_shared<ImageBW>(Fisher(ig));
	ig = ImageGray{}; // free memory
	auto m = MatrixInt(1, 3, 1);
	ibw->Dilate(m);
	auto m2 = MatrixInt(1, 9, 1);
	ibw->Erode(m2);
	// extract lines
	auto lb = Block::New(ibw);
	auto mask = lb->ExtractCC(U"cc");
	lb->FilterMinOr(U"cc", 3, 3);
	long U = 0, V = 0;
	for (auto cco : lb->GetTree(U"cc"))
	{
		auto cc = std::static_pointer_cast<Block>(cco);
		auto bbox = cc->GetAbsoluteBBox();
		auto val = cc->GetName().ToInt();
		auto maxval = int(Min(bbox.GetWidth(), bbox.GetHeight()));
		Point2DInt topleft, topright, bottomleft, bottomright;
		bool tl = false, tr = false, bl = false, br = false;
		for (int tmp = 1; tmp < maxval; ++tmp)
		{
			for (int off = 0; off <= tmp; ++off)
			{
				if (!tl)
					if (mask->At(bbox.GetLeft() + off, bbox.GetTop() + tmp - off) == val)
					{
						topleft.X = bbox.GetLeft() + off;
						topleft.Y = bbox.GetTop() + tmp - off;
						tl = true;
					}
				if (!tr)
					if (mask->At(bbox.GetRight() - off, bbox.GetTop() + tmp - off) == val)
					{
						topright.X = bbox.GetRight() - off;
						topright.Y = bbox.GetTop() + tmp - off;
						tr = true;
					}
				if (!bl)
					if (mask->At(bbox.GetLeft() + off, bbox.GetBottom() - tmp + off) == val)
					{
						bottomleft.X = bbox.GetLeft() + off;
						bottomleft.Y = bbox.GetBottom() - tmp + off;
						bl = true;
					}
				if (!br)
					if (mask->At(bbox.GetRight() - off, bbox.GetBottom() - tmp + off) == val)
					{
						bottomright.X = bbox.GetRight() - off;
						bottomright.Y = bbox.GetBottom() - tmp + off;
						br = true;
					}
			}
			if (tl && tr && bl && br)
				break;
		}
		if (tr && tl)
		{
			U += long(div) * (topright.X - topleft.X);
			V += topright.Y - topleft.Y;
		}
		if (br && bl)
		{
			U += long(div) * (bottomright.X - bottomleft.X);
			V += bottomright.Y - bottomleft.Y;
		}
	}
	return Angle<Radian>::Atan(double(-V), double(U)); // Y axis is inverted
}

/*! Default constructor
 * \param[in]  t  the threshold to use in [0..255]
 */
Gray2BWThreshold::Gray2BWThreshold(uint8_t t)
{
	UserData.Set(U"threshold", std::make_shared<Int>(t));
}

ImageBW Gray2BWThreshold::Binarize(const ImageGray &img)
{
	return Threshold(img, uint8_t(*std::static_pointer_cast<Int>(UserData[U"threshold"])));
}

/*! Default constructor
 * \param[in]  halfwin  the half size of the window. Real size will be 2 * halfwin + 1
 * \param[in]  k  the parameter k
 */
Gray2BWNiblack::Gray2BWNiblack(size_t halfwin, double k)
{
	UserData.Set(U"halfwin", std::make_shared<Int>(int(halfwin)));
	UserData.Set(U"k", std::make_shared<Real>(k));
}

ImageBW Gray2BWNiblack::Binarize(const ImageGray &img)
{
	return Niblack(img, *std::static_pointer_cast<Int>(UserData[U"halfwin"]), 
			*std::static_pointer_cast<Real>(UserData[U"k"]));
}

/*! Default constructor
 * \param[in]  halfwin  the half size of the window. Real size will be 2 * halfwin + 1
 * \param[in]  k  the parameter k
 */
Gray2BWSauvola::Gray2BWSauvola(size_t halfwin, double k)
{
	UserData.Set(U"halfwin", std::make_shared<Int>(int(halfwin)));
	UserData.Set(U"k", std::make_shared<Real>(k));
}

ImageBW Gray2BWSauvola::Binarize(const ImageGray &img)
{
	return Sauvola(img, *std::static_pointer_cast<Int>(UserData[U"halfwin"]), 
			*std::static_pointer_cast<Real>(UserData[U"k"]));
}

/*! Default constructor
 * \param[in]  classes  the total number of classes
 * \param[in]  black_classes  the number of classes that will be labelled black
 */
Gray2BWkMeansHisto::Gray2BWkMeansHisto(size_t classes, size_t black_classes)
{
	UserData.Set(U"classes", std::make_shared<Int>(int(classes)));
	UserData.Set(U"black_classes", std::make_shared<Int>(int(black_classes)));
}

ImageBW Gray2BWkMeansHisto::Binarize(const ImageGray &img)
{
	return kMeansHisto(img, *std::static_pointer_cast<Int>(UserData[U"classes"]), 
			*std::static_pointer_cast<Int>(UserData[U"black_classes"]));
}

/*! Default constructor
 * \param[in]  area  the area to search the local min
 */
Gray2BWLocalMin::Gray2BWLocalMin(size_t area)
{
	UserData.Set(U"area", std::make_shared<Int>(int(area)));
}

ImageBW Gray2BWLocalMin::Binarize(const ImageGray &img)
{
	return LocalMin(img, *std::static_pointer_cast<Int>(UserData[U"area"]));
}

/*! Default constructor
 * \param[in]  area  the area to search the local max
 */
Gray2BWLocalMax::Gray2BWLocalMax(size_t area)
{
	UserData.Set(U"area", std::make_shared<Int>(int(area)));
}

ImageBW Gray2BWLocalMax::Binarize(const ImageGray &img)
{
	return LocalMax(img, *std::static_pointer_cast<Int>(UserData[U"area"]));
}

/*! Default constructor
*/
Gray2BWFisher::Gray2BWFisher()
{
}

ImageBW Gray2BWFisher::Binarize(const ImageGray &img)
{
	return Fisher(img);
}

/*! Default constructor
*/
Gray2BWEntropy::Gray2BWEntropy()
{
}

ImageBW Gray2BWEntropy::Binarize(const ImageGray &img)
{
	return Entropy(img);
}

/*! Default constructor
*/
Gray2BWOtsu::Gray2BWOtsu()
{
}

ImageBW Gray2BWOtsu::Binarize(const ImageGray &img)
{
	return Otsu(img);
}

ImageBW crn::MakeImageBW(const ImageGray &img)
{
	auto act = std::dynamic_pointer_cast<Gray2BW>(DefaultAction::GetAction(U"Gray2BW"));
	if (!act)
	{
		act = std::make_shared<Gray2BWFisher>();
		DefaultAction::SetAction(U"Gray2BW", act);
	}
	return act->Binarize(img);
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Gray2BWThreshold)
	CRN_DATA_FACTORY_REGISTER(U"Gray2BWThreshold", Gray2BWThreshold)
CRN_END_CLASS_CONSTRUCTOR(Gray2BWThreshold)

CRN_BEGIN_CLASS_CONSTRUCTOR(Gray2BWNiblack)
	CRN_DATA_FACTORY_REGISTER(U"Gray2BWNiblack", Gray2BWNiblack)
CRN_END_CLASS_CONSTRUCTOR(Gray2BWNiblack)

CRN_BEGIN_CLASS_CONSTRUCTOR(Gray2BWSauvola)
	CRN_DATA_FACTORY_REGISTER(U"Gray2BWSauvola", Gray2BWSauvola)
CRN_END_CLASS_CONSTRUCTOR(Gray2BWSauvola)

CRN_BEGIN_CLASS_CONSTRUCTOR(Gray2BWkMeansHisto)
	CRN_DATA_FACTORY_REGISTER(U"Gray2BWkMeansHisto", Gray2BWkMeansHisto)
CRN_END_CLASS_CONSTRUCTOR(Gray2BWkMeansHisto)

CRN_BEGIN_CLASS_CONSTRUCTOR(Gray2BWLocalMin)
	CRN_DATA_FACTORY_REGISTER(U"Gray2BWLocalMin", Gray2BWLocalMin)
CRN_END_CLASS_CONSTRUCTOR(Gray2BWLocalMin)

CRN_BEGIN_CLASS_CONSTRUCTOR(Gray2BWLocalMax)
	CRN_DATA_FACTORY_REGISTER(U"Gray2BWLocalMax", Gray2BWLocalMax)
CRN_END_CLASS_CONSTRUCTOR(Gray2BWLocalMax)

CRN_BEGIN_CLASS_CONSTRUCTOR(Gray2BWFisher)
	CRN_DATA_FACTORY_REGISTER(U"Gray2BWFisher", Gray2BWFisher)
CRN_END_CLASS_CONSTRUCTOR(Gray2BWFisher)

CRN_BEGIN_CLASS_CONSTRUCTOR(Gray2BWEntropy)
	CRN_DATA_FACTORY_REGISTER(U"Gray2BWEntropy", Gray2BWEntropy)
CRN_END_CLASS_CONSTRUCTOR(Gray2BWEntropy)

CRN_BEGIN_CLASS_CONSTRUCTOR(Gray2BWOtsu)
	CRN_DATA_FACTORY_REGISTER(U"Gray2BWOtsu", Gray2BWOtsu)
CRN_END_CLASS_CONSTRUCTOR(Gray2BWOtsu)

