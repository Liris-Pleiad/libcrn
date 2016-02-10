/* Copyright 2013-2014 INSA Lyon
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
 * file: CRNSummedAreaTable.h
 * \author Yann LEYDIER
 */

#ifndef CRNSummedAreaTable_HEADER
#define CRNSummedAreaTable_HEADER

#include <vector>
#include <CRNGeometry/CRNRect.h>
#include <CRNException.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief A summed area table
	 *
	 * A summed area table. Used to compute fast square sums in an image
	 *
	 * \author  Yann LEYDIER
	 * \date    Jul. 2013
	 * \version 0.1
	 * \ingroup image
	 */
	template<typename T> class SummedAreaTable
	{
		public:
			/*! \brief Constructor */
			inline SummedAreaTable(size_t w, size_t h):width(w),height(h),data(w * h, T(0)) { }

			SummedAreaTable(const SummedAreaTable&) = default;
			SummedAreaTable(SummedAreaTable&&) = default;
			SummedAreaTable& operator=(const SummedAreaTable&) = default;
			SummedAreaTable& operator=(SummedAreaTable&&) = default;

			/*! \brief Gets the value of a bin */
			inline T GetValue(size_t x, size_t y) const { return data[x + y * width]; }
			/*! \brief Alters the value of a bin */
			inline void SetValue(size_t x, size_t y, T val) { data[x + y * width] = val; }

			/*! \brief Gets the sum of the original pixels in a rectangle
			 * \throws	ExceptionDimension	the rectangle exceeds the image's dimensions
			 * \param[in]	r	the rectangle to sum
			 * \return	the sum of the original pixels in the rectangle
			 */
			inline T GetSum(const crn::Rect &r) const
			{
				if ((r.GetLeft() < 0) || (r.GetTop() < 0) || (r.GetRight() >= int(width)) || (r.GetBottom() >= int(height)))
					throw ExceptionDimension("Rectangle out of bounds");
				return GetSum(r.GetLeft(), r.GetTop(), r.GetRight(), r.GetBottom());
			}

			/*! \brief Gets the sum of the original pixels in a rectangle
			 * \warning	No bound check is performed
			 * \param[in]	x1	left border
			 * \param[in]	y1	top border
			 * \param[in]	x2	right border
			 * \param[in]	y2	bottom border
			 * \return	the sum of the original pixels in the rectangle
			 */
			inline T GetSum(size_t x1, size_t y1, size_t x2, size_t y2) const
			{
				if ((x1 == 0) && (y1 == 0))
					return data[x2 + y2 * width];
				else if (x1 == 0)
					return T(data[x2 + y2 * width] - data[x2 + (y1 - 1) * width]);
				else if (y1 == 0)
					return T(data[x2 + y2 * width] - data[x1 - 1 + y2 * width]);
				else
					return T(data[x2 + y2 * width] + data[x1 - 1 + (y1 - 1) * width] - data[(x1 - 1) + y2 * width] - data[x2 + (y1 - 1) * width]);
			}

		private:
			size_t width, height;
			std::vector<T> data;
	};
}

#endif
