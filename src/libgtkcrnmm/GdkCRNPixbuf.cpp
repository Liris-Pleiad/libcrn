/* Copyright 2010-2015 CoReNum, INSA-Lyon
 *
 * This file is part of libgtkcrnmm.
 *
 * libgtkcrnmm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libgtkcrnmm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libgtkcrnmm.  If not, see <http://www.gnu.org/licenses/>.
 *
 * file: GdkCRNPixbuf.cpp
 * \author Yann LEYDIER
 */

#include <libgtkcrnmm_config.h>
#include <GdkCRNPixbuf.h>
#include <CRNStringUTF8.h>
#include <CRNIO/CRNPath.h>
#include <CRNi18n.h>

using namespace crn;

static Glib::RefPtr<Gdk::Pixbuf> pixbufFromCRNImageRGB(const ImageRGB &img)
{
	Glib::RefPtr<Gdk::Pixbuf> pb(Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, int(img.GetWidth()), int(img.GetHeight())));
	size_t offset = 0;
	int offsetpb = 0;
	guint8 *pix = pb->get_pixels();
	for (size_t y = 0; y < img.GetHeight(); y++)
	{
		int pto = offsetpb;
		for (size_t x = 0; x < img.GetWidth(); x++)
		{
			pix[pto++] = img.At(offset).r;
			pix[pto++] = img.At(offset).g;
			pix[pto++] = img.At(offset++).b;
		}
		offsetpb += pb->get_rowstride();
	}
	return pb;
}

static Glib::RefPtr<Gdk::Pixbuf> pixbufFromCRNImageGray(const ImageGray &img)
{
	Glib::RefPtr<Gdk::Pixbuf> pb(Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, int(img.GetWidth()), int(img.GetHeight())));
	size_t offset = 0;
	int offsetpb = 0;
	guint8 *pix = pb->get_pixels();
	for (size_t y = 0; y < img.GetHeight(); y++)
	{
		int pto = offsetpb;
		for (size_t x = 0; x < img.GetWidth(); x++)
		{
			uint8_t v = img.At(offset++);
			pix[pto++] = v;
			pix[pto++] = v;
			pix[pto++] = v;
		}
		offsetpb += pb->get_rowstride();
	}
	return pb;
}

static Glib::RefPtr<Gdk::Pixbuf> pixbufFromCRNImageBW(const ImageBW &img)
{
	Glib::RefPtr<Gdk::Pixbuf> pb(Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, int(img.GetWidth()), int(img.GetHeight())));
	size_t offset = 0;
	int offsetpb = 0;
	guint8 *pix = pb->get_pixels();
	for (size_t y = 0; y < img.GetHeight(); y++)
	{
		int pto = offsetpb;
		for (size_t x = 0; x < img.GetWidth(); x++)
		{
			uint8_t v = img.At(offset++) ? 255 : 0;
			pix[pto++] = v;
			pix[pto++] = v;
			pix[pto++] = v;
		}
		offsetpb += pb->get_rowstride();
	}
	return pb;
}

/*!
 * Creates a Gdk::Pixbuf from a crn::Image
 *
 * \throws crn::ExceptionInvalidArgument	unsupported pixel format
 *
 * \param[in]  img  the image to convert
 * \return  a new Gdk::Pixbuf
 */
Glib::RefPtr<Gdk::Pixbuf> GdkCRN::PixbufFromCRNImage(const ImageBase &img)
{
	try
	{
		return pixbufFromCRNImageRGB(dynamic_cast<const ImageRGB&>(img));
	}
	catch (...) {}
	try
	{
		return pixbufFromCRNImageGray(dynamic_cast<const ImageGray&>(img));
	}
	catch (...) {}
	try
	{
		return pixbufFromCRNImageBW(dynamic_cast<const ImageBW&>(img));
	}
	catch (...) {}
	throw ExceptionInvalidArgument(crn::StringUTF8("GdkCRN::PixbufFromCRNImage(const ImageBase &img): ") + _("unsupported pixel format."));
}

/*! Creates a Gdk::Pixbuf from a file
 *
 * \param[in]  p	the path to the image
 * \return  a new Gdk::Pixbuf
 */
Glib::RefPtr<Gdk::Pixbuf> GdkCRN::PixbufFromFile(const Path &p)
{
#ifdef _MSC_VER
	auto ext = p.GetExtension();
	ext.ToLower();
	if (ext == "png" || ext == "jpeg" || ext == "jpg")
		return GdkCRN::PixbufFromCRNImage(*NewImageFromFile(p));
	else
		return Gdk::Pixbuf::create_from_file(p.CStr());
#else
	return Gdk::Pixbuf::create_from_file(p.CStr());
#endif
}

/*!
 * Creates a crn::Image from a Gdk::Pixbuf
 *
 * \throws crn::ExceptionInvalidArgument	null image
 *
 * \param[in]  pb  the pixbuf to convert
 * \return  a new SImageRGB or nullptr if failed
 */
ImageRGB GdkCRN::CRNImageFromPixbuf(const Glib::RefPtr<Gdk::Pixbuf> &pb)
{
	if (!pb)
	{
		throw ExceptionInvalidArgument(StringUTF8("SImage SImageFromPixbuf(const Glib::RefPtr<Gdk::Pixbuf> &pb): ") + _("Null pixbuf given."));
	}
	guint8 *pix = pb->get_pixels();
	auto imgrgb = ImageRGB(pb->get_width(), pb->get_height());
	int offset = 0;
	int offsetpb = 0;
	int alpha = pb->get_has_alpha() ? 1 : 0;
	for (size_t y = 0; y < imgrgb.GetHeight(); y++)
	{
		int tpo = offsetpb;
		for (size_t x = 0; x < imgrgb.GetWidth(); x++)
		{
			tpo += alpha;
			imgrgb.At(offset).r = pix[tpo++];
			imgrgb.At(offset).g = pix[tpo++];
			imgrgb.At(offset++).b = pix[tpo++];
		}
		offsetpb += pb->get_rowstride();
	}
	return imgrgb;
}

#ifdef CRN_USING_GTKMM3
/*!
* Creates a Gdk::Color from a crn::pixel::RGB8
*
* \param[in]  p  the pixel to convert
* \return  a Gdk::Color
*/
Gdk::RGBA GdkCRN::ColorFromCRNPixel(const crn::pixel::RGB8 &p)
{
	Gdk::RGBA col;
	col.set_rgba(p.r / 255.0, p.g / 255.0, p.b / 255.0);
	return col;
}

/*!
* Creates a crn::pixel::RGB8 from a Gdk::Color
*
* \param[in]  color  the color to convert
* \return  a crn::pixel::RGB8
*/
crn::pixel::RGB8 GdkCRN::CRNPixelRGBFromGdkColor(const Gdk::RGBA &color)
{
	return crn::pixel::RGB8(uint8_t(color.get_red() * 255), uint8_t(color.get_green() * 255), uint8_t(color.get_blue() * 255));
}

#else
/*!
 * Creates a Gdk::Color from a crn::pixel::RGB8
 *
 * \param[in]  p  the pixel to convert
 * \return  a Gdk::Color
 */
Gdk::Color GdkCRN::ColorFromCRNPixel(const crn::pixel::RGB8 &p)
{
	Gdk::Color col;
	col.set_rgb_p(p.r / 255.0, p.g / 255.0, p.b / 255.0);
	return col;
}

/*!
 * Creates a crn::pixel::RGB8 from a Gdk::Color
 *
 * \param[in]  color  the color to convert
 * \return  a crn::pixel::RGB8
 */
crn::pixel::RGB8 GdkCRN::CRNPixelRGBFromGdkColor(const Gdk::Color &color)
{
	return crn::pixel::RGB8(uint8_t(color.get_red_p() * 255), uint8_t(color.get_green_p() * 255), uint8_t(color.get_blue_p() * 255));
}
#endif
