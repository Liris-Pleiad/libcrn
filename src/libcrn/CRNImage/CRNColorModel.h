/* Copyright 2008-2015 INSA Lyon, CoReNum
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
 * file: CRNColorModel.h
 * \author Yann LEYDIER
 */

#ifndef CRNCOLORMODEL_HEADER
#define CRNCOLORMODEL_HEADER

#include <CRNImage/CRNImageFormats.h>
#include <vector>

namespace crn
{
	/****************************************************************************/
	/*! \brief Class for a color model of an image
	 *
	 * \author  Yann LEYDIER
	 * \date    July 2008
	 * \version 0.3
	 * \ingroup imagergb
	 */
	struct ColorModel
	{
		ColorModel() {}
		ColorModel(const ImageRGB &img, size_t max = 15);
		ColorModel(const ColorModel&) = default;
		ColorModel(ColorModel&&) = default;
		ColorModel& operator=(const ColorModel&) = default;
		ColorModel& operator=(ColorModel&&) = default;

		pixel::RGB8 paper;
		pixel::RGB8 ink;
		std::vector<pixel::RGB8> other;
	};
}
#endif
