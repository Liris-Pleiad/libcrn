/* Copyright 2015 Université Paris Descartes
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
 * file: CRNGradientMatching.h
 * \author Yann LEYDIER
 */

#ifndef CRNGradientMatching_H
#define CRNGradientMatching_H

#include <CRNImage/CRNImage.h>

namespace crn
{
	class ImageGradient;
	/****************************************************************************/
	/*! \brief Gradient descriptor
	 *
	 * \author 	Yann LEYDIER
	 * \date		October 2015
	 * \version 0.1
	 */
	struct GradientModel
	{
		GradientModel(const ImageGradient &igr);
		GradientModel(const GradientModel&) = default;
		GradientModel(GradientModel&&) = default;
		GradientModel& operator=(const GradientModel&) = default;
		GradientModel& operator=(GradientModel&&) = default;

		ImageAngle angles;
		ImageBW mask;
		ImageBW dmask;
		int xbar, ybar;

		static double Distance(const GradientModel &img1, const GradientModel &img2, size_t window) noexcept;
	};
}

#endif

