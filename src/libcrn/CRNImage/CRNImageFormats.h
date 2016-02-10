/* Copyright 2015 INSA-Lyon
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
 * file: CRNImageFormats.h
 * \author Yann LEYDIER
 */

#ifndef CRNIMAGEFORMATS_HEADER
#define CRNIMAGEFORMATS_HEADER

#include <CRNImage/CRNPixel.h>

namespace crn
{
	template<typename T> class Image;

	/*! \defgroup image Image formats
	 * \ingroup imaging */

	/****************************************************************************/
	/*! \brief Black and white image class
	 *
	 * This class is for black and white images.
	 * Values in pixels' vector represent luminosity values.
	 *
	 * \author  Yann LEYDIER
	 * \date    August 2007
	 * \version 0.4
	 * \ingroup imagebw
	 */
	using ImageBW = Image<pixel::BW>;
	CRN_ALIAS_SMART_PTR(ImageBW);

	/****************************************************************************/
	/*! \brief Grayscale image class
	 *
	 * This class is for grayscale images.
	 * Values in pixels' vector represent values for luminosity.
	 *
	 * \author  Yann LEYDIER
	 * \date    August 2007
	 * \version 0.4
	 * \ingroup imagegray
	 */
	using ImageGray = Image<uint8_t>;
	CRN_ALIAS_SMART_PTR(ImageGray);

	/****************************************************************************/
	/*! \brief Int grayscale image class
	 *
	 * This class is for grayscale images.
	 * Values in pixels' vector represent values for luminosity.
	 *
	 * \author  Yann LEYDIER
	 * \date    August 2007
	 * \version 0.4
	 * \ingroup imagegray
	 */
	using ImageIntGray = Image<int>;
	CRN_ALIAS_SMART_PTR(ImageIntGray);

	/****************************************************************************/
	/*! \brief double Grayscale image class
	 *
	 * This class is for grayscale images.
	 * Values in pixels' vector represent values for luminosity.
	 *
	 * \author  Yann LEYDIER
	 * \date    August 2007
	 * \version 0.4
	 * \ingroup imagegray
	 */
	using ImageDoubleGray = Image<double>;
	CRN_ALIAS_SMART_PTR(ImageDoubleGray);
	
	/****************************************************************************/
	/*! \brief Color image class
	 *
	 * This class is for color images.
	 * Values in pixels' vector represent red, green and blue channels (interlaced in this order).
	 *
	 * \author  Yann LEYDIER
	 * \date    August 2007
	 * \version 0.2
	 * \ingroup imagergb
	 */
	using ImageRGB = Image<pixel::RGB<uint8_t>>;
	CRN_ALIAS_SMART_PTR(ImageRGB)

	/****************************************************************************/
	/*! \brief Color image class
	 *
	 * This class is for color images.
	 * Values in pixels' vector represent red, green and blue channels (interlaced in this order).
	 *
	 * \author  Yann LEYDIER
	 * \date    August 2007
	 * \version 0.4
	 * \ingroup imagergb
	 */
	using ImageIntRGB = Image<pixel::RGB<int>>;
	CRN_ALIAS_SMART_PTR(ImageIntRGB)

	/****************************************************************************/
	/*! \brief Color image class
	 *
	 * This class is for color images.
	 * Values in pixels' vector represent hue, saturation and value.
	 *
	 * \author  Yann LEYDIER
	 * \date    August 2007
	 * \version 0.3
	 * \ingroup imageother
	 */
	using ImageHSV = Image<pixel::HSV>;
	CRN_ALIAS_SMART_PTR(ImageHSV)

	/****************************************************************************/
	/*! \brief Color image class
	 *
	 * This class is for color images.
	 * Values in pixels' vector represent Y (luma), Cr and Br (chroma).
	 *
	 * \author  Jean DUONG
	 * \date    January 2016
	 * \version 0.3
	 * \ingroup imageother
	 */
	using ImageYUV = Image<pixel::YUV>;
	CRN_ALIAS_SMART_PTR(ImageYUV)

	/****************************************************************************/
	/*! \brief 2D int vector image class
	 *
	 * This class is for 2D int vectors images.
	 *
	 * \author  Yann LEYDIER
	 * \date    August 2007
	 * \version 0.2
	 * \ingroup imageother
	 */
	using Image2DIntVector = Image<pixel::Cart2D<int>>;
	CRN_ALIAS_SMART_PTR(Image2DIntVector);

	/****************************************************************************/
	/*! \brief 2D int polar vector image class
	 *
	 * This class is for 2D int polar vectors images.
	 *
	 * \author  Yann LEYDIER
	 * \date    August 2007
	 * \version 0.2
	 * \ingroup imageother
	 */
	using Image2DIntPolarVector = Image<pixel::Polar2D<unsigned int, Angle<ByteAngle>>>;
	CRN_ALIAS_SMART_PTR(Image2DIntPolarVector);

	/****************************************************************************/
	/*! \brief Angle image class
	 *
	 * This class is for angle images.
	 * Values in pixels' vector represent values for the angle.
	 *
	 * \author  Yann LEYDIER
	 * \date    August 2007
	 * \version 0.3
	 * \ingroup imageother
	 */
	using ImageAngle = Image<typename ByteAngle::type>;
	CRN_ALIAS_SMART_PTR(ImageAngle);
	
	/****************************************************************************/
	/*! \brief XYZ image class
	 *
	 * This class is for angle images.
	 *
	 * \author  Yann LEYDIER
	 * \date    Apr. 2015
	 * \version 0.1
	 * \ingroup imageother
	 */
	using ImageXYZ = Image<pixel::XYZ>;
	CRN_ALIAS_SMART_PTR(ImageXYZ);
	
	/****************************************************************************/
	/*! \brief Color image class
	 *
	 * This class is for color images.
	 * Values in pixels' vector are represented in the L*a*b* color space.
	 *
	 * \author  Yann LEYDIER
	 * \date    July 2008
	 * \version 0.3
	 * \ingroup imageother
	 */
	using ImageLab = Image<pixel::Lab>;
	CRN_ALIAS_SMART_PTR(ImageLab);

	/****************************************************************************/
	/*! \brief Color image class
	 *
	 * This class is for color images.
	 * Values in pixels' vector are represented in the L*u*v* color space.
	 *
	 * \author  Loris EYNARD
	 * \date    July 2008
	 * \version 0.3
	 * \ingroup imageother
	 */
	using ImageLuv = Image<pixel::Luv>;
	CRN_ALIAS_SMART_PTR(ImageLuv);
	
}

#endif
