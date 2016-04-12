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
 * file: CRNImageGradient.h
 * \author Yann LEYDIER
 */

#ifndef CRNIMAGEGRADIENT_HEADER
#define CRNIMAGEGRADIENT_HEADER

#include <CRNImage/CRNImage2D.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief Gradient image in polar form
	 *
	 * This class is for gradient images, in polar form
	 *
	 * \author  Yann LEYDIER
	 * \date    August 2007
	 * \version 0.4
	 * \ingroup imageother
	 */
	class ImageGradient : public Image<pixel::Polar2D<unsigned int, Angle<ByteAngle>>>
	{
		public:
#ifdef _MSC_VER
			/*! \brief Default constructor */
			ImageGradient(): Image<pixel::Polar2D<unsigned int, Angle<ByteAngle>>>() {}
			/*! \brief Constructor */
			ImageGradient(size_t w, size_t h, pixel_type val = pixel_type(0)) :
				Image<pixel::Polar2D<unsigned int, Angle<ByteAngle>>>(w, h, val) {}
			/*! \brief Constructor from data */
			ImageGradient(size_t w, size_t h, const pixel_type *data):
				Image<pixel::Polar2D<unsigned int, Angle<ByteAngle>>>(w, h, data) {}
			/*! \brief Copy constructor */
			template<typename Y> explicit ImageGradient(const Image<Y> &img) :
				Image<pixel::Polar2D<unsigned int, Angle<ByteAngle>>>(img) {}
			/*! \brief Crop constructor */
			template<typename Y> ImageGradient(const Image<Y> &img, const Rect &bbox) :
				Image<pixel::Polar2D<unsigned int, Angle<ByteAngle>>>(img, bbox) {}
#else
			using Image<pixel::Polar2D<unsigned int, Angle<ByteAngle>>>::Image;
#endif

			ImageGradient(const ImageGradient&) = default;
			ImageGradient(ImageGradient&&) = default;

			ImageGradient& operator=(const ImageGradient&) = default;
			ImageGradient& operator=(ImageGradient&&) = default;

			virtual ~ImageGradient() override {}

			/*! \brief Returns the module significance threshold */
			unsigned int GetMinModule() const noexcept { return thresh; }
			/*! \brief Sets the module significance threshold */
			void SetMinModule(unsigned int m) { thresh = m; }
			/*! \brief Computes the module significance threshold */
			unsigned int AutoMinModule(const ImageGray &img);

			/*! \brief Tests if a pixel has a significant gradient
			 * \warning	no bound checking is performed
			 * \param[in]	offset	the offset of the pixel
			 * \return	true if the pixel has a significant gradient module
			 */
			inline bool IsSignificant(size_t i) const { return At(i).rho >= thresh; }
			/*! \brief Tests if a pixel has a significant gradient
			 * \warning	no bound checking is performed
			 * \param[in]	x	the abscissa of the pixel
			 * \param[in]	y	the ordinate of the pixel
			 * \return	true if the pixel has a significant gradient module
			 */
			inline bool IsSignificant(size_t x, size_t y) const { return At(x, y).rho >= thresh; }

			/*! \brief Converts to a gray image */
			ImageGray MakeImageGray() const;
			/*! \brief Converts to a rgb image */
			ImageRGB MakeImageRGB(bool thres = false) const;
			/*! \brief Creates an image representing the curvature of the gradients */
			ImageGray MakeCurvature() const;
			/*! \brief Create a mask of the gradients' module */
			ImageBW MakeMask() const;

			/*! \brief Estimates the mean character width */
			double GetHRun() const noexcept;
			/*! \brief Estimates the mean character height */
			double GetVRun() const noexcept;
			
		private:
			unsigned int thresh;
	};

	CRN_ALIAS_SMART_PTR(ImageGradient)
}
#endif
