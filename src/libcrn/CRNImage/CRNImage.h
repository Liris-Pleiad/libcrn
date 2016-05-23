/* Copyright 2006-2016 Yann LEYDIER, INSA-Lyon, CoReNum, ENS-Lyon
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
 * file: CRNImage.h
 * \author Yann LEYDIER
 */

#ifndef CRNIMAGE_HEADER
#define CRNIMAGE_HEADER

#include <CRNMath/CRNMath.h>
#include <CRNException.h>
#include <CRNString.h>
#include <CRNImage/CRNSummedAreaTable.h>
#include <vector>
#include <type_traits>

namespace crn
{
	/*! \defgroup image Image */
	/*@{*/
	/*! \brief Convenience macro to sweep an image */
#define FOREACHPIXEL(x, y, img) for (size_t y = 0; y < (img).GetHeight(); y++) for (size_t x = 0; x < (img).GetWidth(); x++)

	class Rect;
	class MatrixInt;
	class MatrixDouble;
	class Path;
	class Point2DInt;

	/*! \brief Base class for images */
	class ImageBase
	{
		public:
			virtual ~ImageBase();

			/**************************************************************************************
			 * Construction and copy
			 *************************************************************************************/
			ImageBase(size_t w, size_t h): width(w), height(h) {}
			ImageBase(const ImageBase&) = default;
			ImageBase(ImageBase&&) = default;
			ImageBase& operator=(const ImageBase&) = default;
			ImageBase& operator=(ImageBase&&) = default;

			/*! \brief Saves as PNG file */
			virtual void SavePNG(const Path &fname) const = 0;
			/*! \brief Saves as JPEG file */
			virtual void SaveJPEG(const Path &fname, unsigned int qual) const = 0;

			/**************************************************************************************
			 * Accessors
			 *************************************************************************************/

			/*! \returns the width of the image */
			size_t GetWidth() const noexcept { return width; }
			/*! \returns the height of the image */
			size_t GetHeight() const noexcept { return height; }
			/*! \returns the bounding box of the image */
			Rect GetBBox() const noexcept;
			/*! \returns the number of pixels in the image */
			size_t Size() const noexcept { return width * height; }

			/**************************************************************************************
			 * Edition
			 *************************************************************************************/

			/*! \brief Scales the image */
			virtual void ScaleToSize(size_t w, size_t h) = 0;

			/**************************************************************************************
			 * Transformation
			 *************************************************************************************/

		protected:
			size_t width, height;

#ifdef CRN_USING_GDIPLUS
			struct gditoken;
			static std::unique_ptr<gditoken> gdiinit;
			CRN_DECLARE_CLASS_CONSTRUCTOR(ImageBase)
#endif
	};

	using SImage = std::shared_ptr<ImageBase>;
	using SCImage = std::shared_ptr<const ImageBase>;
	using WImage = std::weak_ptr<ImageBase>;
	using WCImage = std::weak_ptr<const ImageBase>;
	using UImage = std::unique_ptr<ImageBase>;
	using UCImage = std::unique_ptr<const ImageBase>;

	class Path;
	/*! \brief Loads an image from a file */
	UImage NewImageFromFile(const Path &fname);

	/****************************************************************************/
	/*! \brief Abstract class for images
	 *
	 * This class is the highest ancestor for all image classes.
	 *
	 * \author  Yann LEYDIER
	 * \date    August 2007
	 * \version 0.4
	 */
	template<class T> class Image: public ImageBase
	{
		public:
			using pixel_type = T;
			
			/**************************************************************************************
			 * Construction and copy
			 *************************************************************************************/

			/*! \brief Constructor */
			Image(size_t w, size_t h, pixel_type val = pixel_type(0));
			/*! \brief Default constructor */
			Image():Image{1, 1} { }
			/*! \brief Constructor from data */
			Image(size_t w, size_t h, const pixel_type *data);
			
			/*! \brief Copy constructor */
			Image(const Image &img) = default;
			/*! \brief Copy constructor */
			template<typename Y> explicit Image(const Image<Y> &img);
			/*! \brief Crop constructor */
			template<typename Y> Image(const Image<Y> &img, const Rect &bbox);
			/*! \brief Move constructor */
			Image(Image &&img) = default;

			/*! \brief Destructor */
			virtual ~Image() override = default;

			/*! \brief Copy operator */
			Image& operator=(const Image &img) = default;
			/*! \brief Copy operator */
			template<typename Y> Image& operator=(const Image<Y> &img);
			/*! \brief Force copy operator (pixel cast) */
			template<typename Y> void Assign(const Image<Y> &img);
			/*! \brief Move operator */
			Image& operator=(Image &&img) = default;

			/*! \brief Swaps two images */
			void Swap(Image &other)
			{
				std::swap(width, other.width);
				std::swap(height, other.height);
				std::swap(pixels, other.pixels);
			}

			/*! \brief Saves as PNG file */
			virtual void SavePNG(const Path &fname) const override;
			/*! \brief Saves as JPEG file */
			virtual void SaveJPEG(const Path &fname, unsigned int qual) const override;

			/*! \brief	Converts to matrix */
			template<typename MAT> MAT ToMatrix() const
			{
				auto mat = MAT{GetHeight(), GetWidth()};
				std::copy(begin(), end(), mat[0]);
				return mat;
			}

			/**************************************************************************************
			 * Accessors
			 *************************************************************************************/

			typename std::vector<pixel_type>::iterator begin() { return pixels.begin(); }
			typename std::vector<pixel_type>::const_iterator begin() const { return pixels.begin(); }
			typename std::vector<pixel_type>::iterator end() { return pixels.end(); }
			typename std::vector<pixel_type>::const_iterator end() const { return pixels.end(); }
			typename std::vector<pixel_type>::const_iterator cbegin() const { return pixels.begin(); }
			typename std::vector<pixel_type>::const_iterator cend() const { return pixels.end(); }

			/*! \brief Gets a pointer to the pixels */
			inline typename std::vector<pixel_type>::pointer GetPixels() noexcept { return pixels.data(); }
			/*! \brief Gets a const pointer to the pixels */
			inline typename std::vector<pixel_type>::const_pointer GetPixels() const noexcept { return pixels.data(); }

			/*! \brief Returns a reference to a pixel */
			inline typename std::vector<pixel_type>::reference At(size_t x, size_t y) noexcept { return pixels[x + y * width]; }
			/*! \brief Returns a reference to a pixel */
			inline typename std::vector<pixel_type>::reference At(size_t offset) noexcept { return pixels[offset]; }
			/*! \brief Returns a reference to a pixel */
			inline typename std::vector<pixel_type>::const_reference At(size_t x, size_t y) const noexcept { return pixels[x + y * width]; }
			/*! \brief Returns a reference to a pixel */
			inline typename std::vector<pixel_type>::const_reference At(size_t offset) const noexcept { return pixels[offset]; }

			/*! \brief Tests equality */
			bool operator==(const Image &other) const;
			/*! \brief Tests equality */
			bool operator!=(const Image &other) const { return !(*this == other); }

			/**************************************************************************************
			 * Arithmetic
			 *************************************************************************************/

			/*! \brief Adds another image */
			Image& operator+=(const Image &img);
			/*! \brief Subtracts another image */
			Image& operator-=(const Image &img);
			/*! \brief Multiplies all pixels */
			Image& operator*=(double f);
			/*! \brief Multiplies with another image's pixels */
			Image& operator*=(const Image &img);
			/*! \brief Divides by another image's pixels */
			Image& operator/=(const Image &img);

			/**************************************************************************************
			 * Edition
			 *************************************************************************************/

			/*! \brief Negative */
			void Negative();
			/*! \brief Complement */
			void Complement(pixel_type maxval = std::numeric_limits<pixel_type>::max());

			/*! \brief Copies a part of an image */
			template<typename Y> void Blit(const Image<Y> &src, const Rect &srczone, size_t dx, size_t dy);
			
			/*! \brief Flood fills a portion of the image */
			void FloodFill(size_t x, size_t y, const pixel_type &val, crn::DistanceType dist = crn::DistanceType::D4);
			/*! \brief Fills a portion of the image */
			void ScanFill(size_t x, size_t y, const pixel_type &val, crn::DistanceType dist = crn::DistanceType::D4);

			/*! \brief Draws a rectangle using a specified color */
			void DrawRect(const Rect &r, pixel_type color, bool filled = false);
			/*! \brief Draws a line using a specified color */
			void DrawLine(size_t x1, size_t y1, size_t x2, size_t y2, pixel_type color);

			/*! \brief Scales the image */
			virtual void ScaleToSize(size_t w, size_t h) override;

			/*! \brief Flips the image */
			void Flip(const Orientation &ori);

			/*! \brief Morphological dilatation */
			template<typename CMP = std::less<pixel_type>> void Dilate(const MatrixInt &strel, CMP cmp = std::less<pixel_type>{});
			/*! \brief Morphological erosion */
			template<typename CMP = std::less<pixel_type>> void Erode(const MatrixInt &strel, CMP cmp = std::less<pixel_type>{});
			/*! \brief Morphological dilatation */
			template<typename CMP = std::less<pixel_type>> void FastDilate(size_t halfwin, size_t index = 0, CMP cmp = std::less<pixel_type>{});
			/*! \brief Morphological erosion */
			template<typename CMP = std::less<pixel_type>> void FastErode(size_t halfwin, size_t index = 0, CMP cmp = std::less<pixel_type>{});

			/*! \brief Convolution */
			void Convolve(const MatrixDouble &mat);
			/*! \brief Gaussian blur */
			void GaussianBlur(double sigma);

		protected:
			std::vector<pixel_type> pixels; /*!< the pixels */
	};

	/**************************************************************************************
	 * Characterization
	 *************************************************************************************/

	/*! \brief Is the image binary (black & white)? */
	template<typename T> bool IsBitonal(const Image<T> &img);

	/*! \brief Returns min and max pixel values */
	template<typename T, typename CMP = std::less<T>> std::pair<T, T> MinMax(const Image<T> &img, CMP cmp = CMP{});

	/*! \brief Estimates the ideal crop for the image */
	template<typename T> Rect AutoCrop(const Image<T> &img, const T &bgval);
	/*! \brief Creates a new image as the ideal crop for the image */
	template<typename T> Image<T> MakeAutoCrop(const Image<T> &img, const T &bgval);

	/*! \brief Best match between two images */
	template<typename T, typename Y> Point2DInt CrossCorrelation(const Image<T> &img1, const Image<Y> &img2, T fill1 = T(0), Y fill2 = Y(0));

	/**************************************************************************************
	 * Transformation
	 *************************************************************************************/

	/*! \brief Converts the image to a type that has a smaller pixel range */
	template<typename IMG, typename T> IMG Downgrade(const Image<T> &img);

	/*! \brief Creates a summed area table of the image */
	template<typename T> SummedAreaTable<SumType<T>> MakeSummedAreaTable(const Image<T> &img);

	/*! \brief Creates a rotated version of the image */
	template<typename T> Image<T> MakeRotation(const Image<T> &img, const Angle<Degree> &angle, const T &bgColor);
	/*! \brief Creates a rotated version of the image */
	template<typename T> Image<T> Make90Rotation(const Image<T> &img);
	/*! \brief Creates a rotated version of the image */
	template<typename T> Image<T> Make180Rotation(const Image<T> &img);
	/*! \brief Creates a rotated version of the image */
	template<typename T> Image<T> Make270Rotation(const Image<T> &img);

	template<typename T> inline auto Size(const Image<T> &img) noexcept(noexcept(img.Size())) -> decltype(img.Size()) { return img.Size(); }
	/*@}*/
} // namespace crn

/*! \addtogroup image */
/*@{*/
template<typename T1, typename T2> crn::Image<crn::SumType<typename std::common_type<T1, T2>::type>> operator+(const crn::Image<T1> &i1, const crn::Image<T2> &i2);
template<typename T1, typename T2> crn::Image<crn::DiffType<typename std::common_type<T1, T2>::type>> operator-(const crn::Image<T1> &i1, const crn::Image<T2> &i2);
template<typename T1, typename T2> crn::Image<crn::SumType<typename std::common_type<T1, T2>::type>> operator*(const crn::Image<T1> &i1, const crn::Image<T2> &i2);
template<typename T> crn::Image<crn::SumType<typename std::common_type<T, double>::type>> operator*(double d, const crn::Image<T> &i);
template<typename T> crn::Image<crn::SumType<typename std::common_type<T, double>::type>> operator*(const crn::Image<T> &i, double d);
template<typename T1, typename T2> crn::Image<crn::SumType<typename std::common_type<T1, T2>::type>> operator/(const crn::Image<T1> &i1, const crn::Image<T2> &i2);
/*@}*/

#include <CRNImage/CRNImageFormats.h>

namespace crn
{
	/*! Converts the image to a type that has a smaller pixel range
	 * \param[in]	img	the source image
	 * \return	a new image
	 */
	template<typename IMG, typename T> IMG Downgrade(const Image<T> &img)
	{
		using new_pixel = typename IMG::pixel_type;
		using common_pixel = typename std::common_type<T, new_pixel>::type;

		auto res = IMG(img.GetWidth(), img.GetHeight());
		const auto mM = MinMax(img);
		if ((common_pixel(mM.first) < common_pixel(std::numeric_limits<new_pixel>::lowest())) || 
				(common_pixel(mM.second) > common_pixel(std::numeric_limits<new_pixel>::max())))
		{
			for (auto tmp : Range(img))
			{
				auto val = common_pixel(img.At(tmp)) - common_pixel(mM.first);
				val = (val * common_pixel(std::numeric_limits<new_pixel>::max())) / common_pixel(mM.second - mM.first);
				res.At(tmp) = new_pixel(val);
			}
		}
		else
		{
			for (auto tmp : Range(img))
				res.At(tmp) = new_pixel(img.At(tmp));
		}
		return res;
	}

	/*! Converts the image to a type that has a smaller pixel range
	 * \ingroup	imaging
	 * \param[in]	img	the source image
	 * \return	a new image
	 */
	template<typename IMG, typename T> IMG Downgrade(const Image<pixel::RGB<T>> &img)
	{
		using new_pixel = typename IMG::pixel_type;
		using new_base = typename new_pixel::type;
		using common_base = typename std::common_type<T, typename new_pixel::type>::type;
		using common_pixel = pixel::RGB<common_base>;

		auto res = IMG(img.GetWidth(), img.GetHeight());
		auto m = std::numeric_limits<T>::max();
		auto M = std::numeric_limits<T>::lowest();
		for (const auto &p : img)
		{
			if (p.r < m) m = p.r;
			if (p.g < m) m = p.g;
			if (p.b < m) m = p.b;
			if (p.r > M) M = p.r;
			if (p.g > M) M = p.g;
			if (p.b > M) M = p.b;
		}
		if ((common_base(m) < common_base(std::numeric_limits<new_base>::lowest())) || 
				(common_base(M) > common_base(std::numeric_limits<new_base>::max())))
		{
			for (auto tmp : Range(img))
			{
				auto val = common_pixel(img.At(tmp)) - common_pixel(m);
				val = (val * common_base(std::numeric_limits<new_base>::max())) / common_base(M - m);
				res.At(tmp) = new_pixel(val);
			}
		}
		else
		{
			for (auto tmp : Range(img))
				res.At(tmp) = new_pixel(img.At(tmp));
		}
		return res;
	}

	/*************************************************************************************
	 * File operations
	 ************************************************************************************/

	namespace impl
	{
		/*! \brief Saves as PNG file */
		void SavePNG(const ImageBW &img, const Path &fname);
		/*! \brief Saves as JPEG file */
		void SaveJPEG(const ImageBW &img, const Path &fname, unsigned int qual);

		/*! \brief Saves as PNG file */
		void SavePNG(const ImageGray &img, const Path &fname);
		/*! \brief Saves as JPEG file */
		void SaveJPEG(const ImageGray &img, const Path &fname, unsigned int qual);

		/*! \brief Saves as PNG file */
		void SavePNG(const ImageRGB &img, const Path &fname);
		/*! \brief Saves as JPEG file */
		void SaveJPEG(const ImageRGB &img, const Path &fname, unsigned int qual);

		/*! Saves as PNG file
		 * \param[in]	img	the image to save
		 * \param[in]	fname	full path	to the image to save
		 */
		template<typename T> void SavePNG(const Image<pixel::RGB<T>> &img, const Path &fname)
		{
			SavePNG(Downgrade<ImageRGB>(img), fname);
		}
		/*! Saves as JPEG file
		 * \param[in]	img	the image to save
		 * \param[in]	fname	full path	to the image to save
		 * \param[in]	qual	compression quality [0..100]
		 */
		template<typename T> void SaveJPEG(const Image<pixel::RGB<T>> &img, const Path &fname, unsigned int qual)
		{
			SaveJPEG(Downgrade<ImageRGB>(img), fname, qual);
		}

		/*! Saves as PNG file
		 * \param[in]	img	the image to save
		 * \param[in]	fname	full path	to the image to save
		 */
		template<typename T> void SavePNG(const Image<T> &img, const Path &fname, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
		{
			SavePNG(Downgrade<ImageGray>(img), fname);
		}
		/*! Saves as JPEG file
		 * \param[in]	img	the image to save
		 * \param[in]	fname	full path	to the image to save
		 * \param[in]	qual	compression quality [0..100]
		 */
		template<typename T> void SaveJPEG(const Image<T> &img, const Path &fname, unsigned int qual, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
		{
			SaveJPEG(Downgrade<ImageGray>(img), fname, qual);
		}

		/*! Saves as PNG file
		 * \throws	ExceptionInvalidArgument	unsupported pixel format
		 * \param[in]	img	the image to save
		 * \param[in]	fname	full path	to the image to save
		 */
		template<typename T> void SavePNG(const Image<T> &img, const Path &fname, typename std::enable_if<!std::is_arithmetic<T>::value>::type *dummy = nullptr)
		{
			throw ExceptionInvalidArgument("SavePNG(): pixel format not supported.");
		}

		/*! Saves as JPEG file
		 * \throws	ExceptionInvalidArgument	unsupported pixel format
		 * \param[in]	img	the image to save
		 * \param[in]	fname	full path	to the image to save
		 * \param[in]	qual	compression quality [0..100]
		 */
		template<typename T> void SaveJPEG(const Image<T> &img, const Path &fname, unsigned int qual, typename std::enable_if<!std::is_arithmetic<T>::value>::type *dummy = nullptr)
		{
			throw ExceptionInvalidArgument("SaveJPEG(): pixel format not supported.");
		}
	}

}

#include <CRNImage/CRNImage.hpp>

#endif
