/* Copyright 2016 CoReNum
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
 * file: CRNCV.h
 * \author Yann LEYDIER
 */

#ifndef CRNCV_HEADER
#define CRNCV_HEADER

#include "opencv2/core/mat.hpp"
#include <CRNImage/CRNImage.h>

namespace crn
{
	template<typename T> struct CVMatType {};
	template<> struct CVMatType<uint8_t> { static constexpr int val = CV_8UC1; };
	template<> struct CVMatType<int8_t> { static constexpr int val = CV_8SC1; };
	template<> struct CVMatType<uint16_t> { static constexpr int val = CV_16UC1; };
	template<> struct CVMatType<int16_t> { static constexpr int val = CV_16SC1; };
	template<> struct CVMatType<int32_t> { static constexpr int val = CV_32SC1; };
	template<> struct CVMatType<float> { static constexpr int val = CV_32FC1; };
	template<> struct CVMatType<double> { static constexpr int val = CV_64FC1; };
	template<typename T> struct CVMatType<crn::pixel::RGB<T>> { static constexpr int val = CV_MAKETYPE(CVMatType<T>::val, 3); };
	template<> struct CVMatType<crn::pixel::HSV> { static constexpr int val = CV_8UC3; };
	template<typename T> struct CVMatType<crn::pixel::Cart2D<T>> { static constexpr int val = CV_MAKETYPE(CVMatType<T>::val, 2); };
	template<> struct CVMatType<crn::pixel::XYZ> { static constexpr int val = CV_64FC3; };
	template<> struct CVMatType<crn::pixel::YUV> { static constexpr int val = CV_64FC3; };
	template<> struct CVMatType<crn::pixel::Lab> { static constexpr int val = CV_64FC3; };
	template<> struct CVMatType<crn::pixel::Luv> { static constexpr int val = CV_64FC3; };

	/*! \brief Wraps an image to an OpenCV matrix 
	 *
	 * Wraps an image to an OpenCV matrix. The returned cv::Mat uses the same memory as the crn::Image. In other words, any modification made to the cv::Mat is immediately passed on the crn::Image and vice-versa.
	 *
	 * \ingroup image
	 */
	template<typename T> cv::Mat WrapCVMat(crn::Image<T> &img)
	{
		return cv::Mat{int(img.GetWidth()), int(img.GetHeight()), CVMatType<T>::val, reinterpret_cast<unsigned char *>(img.GetPixels())};
	}

}

#endif

