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
 * file: CRNDifferential.h
 * \author Yann LEYDIER
 */

#ifndef CRNDIFFERENTIAL_HEADER
#define CRNDIFFERENTIAL_HEADER

#include <CRNImage/CRNImageGradient.h>
#include <CRNImage/CRNImageGray.h>
#include <mutex>

/*! \defgroup diff	PDE
 * \ingroup	image */

namespace crn
{
	class Differential;
	CRN_ALIAS_SMART_PTR(Differential)
}
namespace crn
{
	/****************************************************************************/
	/*! \brief Differential computation on images.
	 *
	 * This class extracts differential features from an image.
	 * \attention Heavy memory usage (up to 40 times the original image size)!
	 *
	 * \author 	Yann LEYDIER
	 * \date		Jan 2015
	 * \version 0.5
	 * \ingroup	diff
	 */
	class Differential
	{
		public:
			enum class RGBProjection { ABSMAX, SUM };
			/*! \brief Convolution with Gaussian derivatives */
			static Differential NewGaussian(const ImageRGB &src, RGBProjection proj, double sigma);
			/*! \brief Alternate half derivatives */
			static Differential NewHalfDiff(const ImageRGB &src, RGBProjection proj);
			/*! \brief AbsMax half derivatives */
			static Differential NewHalfDiffAbsMax(const ImageRGB &src, RGBProjection proj);
			/*! \brief AbsMin half derivatives */
			static Differential NewHalfDiffAbsMin(const ImageRGB &src, RGBProjection proj);

			/*! \brief Convolution with Gaussian derivatives */
			template<typename T> static Differential NewGaussian(const Image<T> &src, double sigma)
			{ auto i = ImageDoubleGray(src); return NewGaussian(i, sigma); }
			/*! \brief Alternate half derivatives */
			template<typename T> static Differential NewHalfDiff(const Image<T> &src)
			{ auto i = ImageDoubleGray(src); return NewHalfDiff(i); }
			/*! \brief AbsMax half derivatives */
			template<typename T> static Differential NewHalfDiffAbsMax(const Image<T> &src)
			{ auto i = ImageDoubleGray(src); return NewHalfDiffAbsMax(i); }
			/*! \brief AbsMin half derivatives */
			template<typename T> static Differential NewHalfDiffAbsMin(const Image<T> &src)
			{ auto i = ImageDoubleGray(src); return NewHalfDiffAbsMin(i); }

			/*! \brief Convolution with Gaussian derivatives */
			static Differential NewGaussian(const ImageDoubleGray &src, double sigma);
			/*! \brief Alternate half derivatives */
			static Differential NewHalfDiff(const ImageDoubleGray &src);
			/*! \brief AbsMax half derivatives */
			static Differential NewHalfDiffAbsMax(const ImageDoubleGray &src);
			/*! \brief AbsMin half derivatives */
			static Differential NewHalfDiffAbsMin(const ImageDoubleGray &src);

			/*! \brief Destructor */
			~Differential() = default;

			Differential(const Differential &) = delete;
			Differential(Differential &&) = default;
			Differential& operator=(const Differential &) = delete;
			Differential& operator=(Differential &&) = default;

			/*! \brief Returns a reference to the internal x derivate */
			ImageDoubleGray& GetLx() noexcept { return lx; }
			/*! \brief Returns a reference to the internal x derivate */
			const ImageDoubleGray& GetLx() const { return lx; }
			/*! \brief Returns a reference to the internal y derivate */
			ImageDoubleGray& GetLy() noexcept { return ly; }
			/*! \brief Returns a reference to the internal y derivate */
			const ImageDoubleGray& GetLy() const { return ly; }
			/*! \brief Returns a reference to the internal first derivatives norm */
			ImageDoubleGray& GetLx2Ly2() noexcept { return lx2ly2; }
			/*! \brief Returns a reference to the internal first derivatives norm */
			const ImageDoubleGray& GetLx2Ly2() const { return lx2ly2; }
			/*! \brief Returns a reference to the internal xx derivate */
			const ImageDoubleGray& GetLxx();
			/*! \brief Returns a reference to the internal xy derivate */
			const ImageDoubleGray& GetLxy();
			/*! \brief Returns a reference to the internal yx derivate */
			const ImageDoubleGray& GetLyx();
			/*! \brief Returns a reference to the internal yy derivate */
			const ImageDoubleGray& GetLyy();

			/*! \brief Diffuses the gradient */
			void Diffuse(size_t maxiter, double maxdiv = std::numeric_limits<double>::max());

			/*! \brief Returns the derivative of the normal to the isophotes */
			ImageDoubleGray MakeLw();
			/*! \brief Returns the second derivative of the tangent to the isophote */
			ImageDoubleGray MakeLvv();
			/*! \brief Returns the second derivative of the normal to the isophote  */
			ImageDoubleGray MakeLww();
			/*! \brief Returns the cross derivative of the tangent and normal to the isophote */
			ImageDoubleGray MakeLvw();
			
			/*! \brief Returns the first eigenvalue of the Hessian as an image */
			ImageDoubleGray MakeKappa1();
			/*! \brief Returns the second eigenvalue of the Hessian as an image */
			ImageDoubleGray MakeKappa2();
			/*! \brief Returns the Hessian corner image */
			ImageDoubleGray MakeHessianCorner(double s = 0.1);
			/*! \brief Returns the Laplacian image */
			ImageDoubleGray MakeLaplacian();
			/*! \brief Returns the isophote curvature image */
			ImageDoubleGray MakeIsophoteCurvature();
			/*! \brief Returns the flowline curvature image */
			ImageDoubleGray MakeFlowlineCurvature();
			/*! \brief Returns the edge image */
			ImageDoubleGray MakeEdge();
			/*! \brief Returns the corner image */
			ImageDoubleGray MakeCorner();
			/*! \brief Returns the Gaussian curvature image */
			ImageDoubleGray MakeGaussianCurvature();
			/*! \brief Returns the gradient image */
			ImageGradient MakeImageGradient();
			/*! \brief Returns the gradient module image */
			ImageDoubleGray MakeGradientModule();
			/*! \brief Returns the gradient curvature image */
			ImageGray MakeGradientCurvature();
			/*! \brief Returns Canny's edge detector image */
			ImageDoubleGray MakeCanny();
			/*! \brief Creates the continuous image skeleton */
			ImageDoubleGray MakeDivergence();

			/*! \brief Computes the best threshold for lx2ly2 */
			double AutoThreshold();
			/*! \brief Gets the threshold for lx2ly2 */
			double GetThreshold() const noexcept { return thres; }
			/*! \brief Sets the threshold for lx2ly2 */
			void SetThreshold(double d) noexcept { thres = d; }
			/*! \brief Is a pixel significant */
			bool IsSignificant(size_t i) const noexcept { return GetLx2Ly2().At(i) > thres; }
			/*! \brief Is a pixel significant */
			bool IsSignificant(size_t x, size_t y) const noexcept { return GetLx2Ly2().At(x, y) > thres; }
			/*! \brief Cleans an image of non-significant values
			 * \throws	ExceptionDimension	wrong image dimensions
			 * \param[in]	img	the image to clean
			 * \param[in]	val	the value that represents non-significant pixels
			 */
			template<typename T> void FillNonSignificant(Image<T> &img, T val = T(0)) const
			{
				if ((img.GetWidth() != lx2ly2.GetWidth()) || (img.GetHeight() != lx2ly2.GetHeight()))
					throw ExceptionDimension("The image does not have the same dimensions as the differential.");
				for (auto tmp : Range(img))
				{
					if (!IsSignificant(tmp))
						img.At(tmp) = val;
				}
			}

		private:
			/*! \brief Constructor */
			Differential(ImageGray &&src, ImageDoubleGray &&xdiff, ImageDoubleGray &&ydiff, ImageDoubleGray &&xxdiff, ImageDoubleGray &&yydiff);
			/*! \brief Constructor */
			Differential(ImageGray &&src, ImageDoubleGray &&xdiff, ImageDoubleGray &&ydiff);

			/*! \brief internal. */
			void updateLx2ly2();

			ImageGray srcigray; /*!< a grayscale copy of the source */

			ImageDoubleGray lx; /*!< x first derivative */
			ImageDoubleGray ly; /*!< y first derivative */
			ImageDoubleGray lxx; /*!< x second derivative */
			ImageDoubleGray lxy; /*!< x & y derivative */
			ImageDoubleGray lyx; /*!< y & x derivative */
			ImageDoubleGray lyy; /*!< y second derivative */
			ImageDoubleGray lx2ly2; /*!< the square gradient module */
			double thres; /*!< the square gradient module threshold */
			std::unique_ptr<std::mutex> lazydata; /*!< protects the lazy computation of data */
	};
}
#endif
