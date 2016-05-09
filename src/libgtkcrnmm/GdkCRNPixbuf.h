/* Copyright 2010-2016 CoReNum, INSA-Lyon
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
 * file: GdkCRNPixbuf.h
 * \author Yann LEYDIER
 */

#ifndef GdkCRNPixbuf_HEADER
#define GdkCRNPixbuf_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>
#include <CRNImage/CRNImage.h>
#include <CRNImage/CRNPixel.h>

/*! \defgroup pixbuf Image data conversion
 * \ingroup gtkcrn */
namespace GdkCRN
{
	/*! \brief Creates a Gdk::Pixbuf from a crn::Image 
	 * \ingroup pixbuf */
	 Glib::RefPtr<Gdk::Pixbuf> PixbufFromCRNImage(const crn::ImageBase &img);
	/*! \brief Creates a Gdk::Pixbuf from a file
	 * \ingroup pixbuf */
	 Glib::RefPtr<Gdk::Pixbuf> PixbufFromFile(const crn::Path &p);
	/*! \brief Creates a crn::Image from a Gdk::Pixbuf 
	 * \ingroup pixbuf */
	 crn::ImageRGB CRNImageFromPixbuf(const Glib::RefPtr<Gdk::Pixbuf> &pb);

#ifdef CRN_USING_GTKMM3
	 /*! \brief Creates a Gdk::Color from a crn::Pixel
	 * \ingroup pixbuf */
	 Gdk::RGBA ColorFromCRNPixel(const crn::pixel::RGB8 &p);
	 /*! \brief Creates a crn::PixelRGB from a Gdk::Color
	 * \ingroup pixbuf */
	 crn::pixel::RGB8 CRNPixelRGBFromGdkColor(const Gdk::RGBA &color);
#else
	/*! \brief Creates a Gdk::Color from a crn::Pixel 
	 * \ingroup pixbuf */
	 Gdk::Color ColorFromCRNPixel(const crn::pixel::RGB8 &p);
	/*! \brief Creates a crn::PixelRGB from a Gdk::Color 
	 * \ingroup pixbuf */
	 crn::pixel::RGB8 CRNPixelRGBFromGdkColor(const Gdk::Color &color);
#endif
}

#endif


