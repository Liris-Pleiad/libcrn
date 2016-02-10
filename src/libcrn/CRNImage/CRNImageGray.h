/* Copyright 2006-2015 Yann LEYDIER, CoReNum, INSA-Lyon
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
 * file: CRNImageGray.h
 * \author Yann LEYDIER
 */

#ifndef CRNIMAGEGRAY_HEADER
#define CRNIMAGEGRAY_HEADER

#include <CRNImage/CRNImage.h>
#include <CRNStatistics/CRNHistogram.h>
#include <CRNAI/CRNkMeans.h>
#include <CRNData/CRNRealCoeff.h>
#include <CRNUtils/CRNDefaultAction.h>

/*! \defgroup imagegray Gray images 
 * \ingroup	image
 */

namespace crn
{
	/*! \addtogroup imagegray */
	/*@{*/
	
	/*************************************************************************************
	 * Edition
	 ************************************************************************************/

	/*! \brief Replaces the pixels with their square root */
	void Sqrt(ImageDoubleGray &img) noexcept;

	/*! \brief Replaces each pixel by its absolute value */
	template<typename T> void Abs(Image<T> &img, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr) noexcept
	{
		for (auto &px : img)
			px = Abs(px);
	}

	/*! Stretches the histogram */
	void AutoContrast(ImageGray &img);

	/*************************************************************************************
	 * Characterization
	 ************************************************************************************/

	/*! Creates an histogram from the pixels
	 * \param[in]	img	the image to binarize
	 * \return	the histogram
	 */
	template<typename T> Histogram MakeHistogram(const Image<T> &img, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
	{
		const auto mM = MinMax(img);
		auto h = Histogram(size_t(mM.second - mM.first + 1));
		for (auto v : img)
			h.IncBin(size_t(v - mM.first));
		return h;
	}

	/*! \brief Creates an histogram from the pixels */
	Histogram MakeHistogram(const ImageGray &img);

	/*! \brief Computes the horizontal projection */
	Histogram HorizontalProjection(const ImageGray &img);
	/*! \brief Computes the vertical projection */
	Histogram VerticalProjection(const ImageGray &img);

	/*****************************************************************************/
	/*!
	 * Computes the mean stroke width
	 *
	 * \param[in]	img	the image to binarize
	 * \param[in]	maxval	if the mean stroke width is > maxval, then defaultval is returned
	 * \param[in]	defaultval	the value returned if no mean stroke width can be computed
	 * \return	the mean stroke width
	 */
	template<typename T> size_t StrokesWidth(const Image<T> &img, size_t maxval = 50, size_t defaultval = 0, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
	{
		auto strokeswidth = size_t(0);
		auto pacc = SumType<T>(0);
		for (strokeswidth = 1; strokeswidth < Min(maxval, img.GetWidth()); strokeswidth++)
		{
			auto acc = SumType<T>(0);
			for (size_t y = 0; y < img.GetHeight(); y++)
			{
				auto yoffset = y * img.GetWidth();
				auto tacc = SumType<T>(0);
				for (size_t x = 0; x < img.GetWidth(); x++)
				{
					auto tx = int(x) - int(strokeswidth);
					if (tx < 0) tx += int(img.GetWidth());
					if (tx >= int(img.GetWidth())) tx -= int(img.GetWidth());
					tacc += Abs(img.At(x + yoffset) - img.At(tx + yoffset));
				}
				acc += tacc;
			}
			if (pacc != 0)
			{
				if ((DecimalType<T>(Abs(acc - pacc)) / DecimalType<T>(pacc)) < 0.1)
				{
					return strokeswidth;
				}
			}
			pacc = acc;
		}
		return defaultval;
	}

	/*****************************************************************************/
	/*!
	 * Computes the mean stroke height
	 *
	 * \param[in]	img	the image to binarize
	 * \param[in]	maxval	if the mean stroke height is > maxval, then defaultval is returned
	 * \param[in]	defaultval	the value returned if no mean stroke height can be computed
	 * \return	the mean stroke height
	 */
	template<typename T> size_t StrokesHeight(const Image<T> &img, size_t maxval = 50, size_t defaultval = 0, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
	{
		auto strokesheight = size_t(0);
		auto pacc = SumType<T>(0);
		for (strokesheight = 1; strokesheight < Min(maxval, img.GetHeight()); strokesheight++)
		{
			auto acc = SumType<T>(0);
			for (size_t y = 0; y < img.GetHeight(); y++)
			{
				auto tacc = SumType<T>(0);
				for (size_t x = 0; x < img.GetWidth(); x++)
				{
					auto ty = int(y) - int(strokesheight);
					if (ty < 0) ty += int(img.GetHeight());
					if (ty >= int(img.GetHeight())) ty -= int(img.GetHeight());
					tacc += Abs(img.At(x, y) - img.At(x, ty));
				}
				acc += tacc;
			}
			if (pacc != 0)
			{
				if ((DecimalType<T>(Abs(acc - pacc)) / DecimalType<T>(pacc)) < 0.1)
				{
					return strokesheight;
				}
			}
			pacc = acc;
		}
		return defaultval;
	}

	/*! \brief Computes the mean text line x-height */
	size_t EstimateLinesXHeight(const ImageGray &img, unsigned int xdiv = 16);
	/*! \brief Computes the median distance between two baselines */
	size_t EstimateLeading(const ImageGray &img);
	/*! \brief Estimates the mean skew of the document's lines */
	Angle<Radian> EstimateSkew(const ImageGray &img);

	/*************************************************************************************
	 * Conversion
	 ************************************************************************************/

	/*! Exports to an RGB image with random colors
	 * \param[in]	img	the source image
	 * \return	the newly created image
	 */ 
	template<typename T> ImageRGB RandomColors(const Image<T> &img, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
	{
		auto irgb = ImageRGB(img.GetWidth(), img.GetHeight());
		for (auto tmp : Range(img))
		{
			auto v = img.At(tmp);
			if (v)
			{
				irgb.At(tmp) = { 
					uint8_t(int(32 + v * 33) & 0xFF), 
					uint8_t(int(32 + v * 55) & 0xFF), 
					uint8_t(int(32 + v * 77) & 0xFF)
				};
			}
		}
		return irgb;
	}

	/****************************************************************************/
	/*!
	 * Creates a BW image using a fixed threshold.
	 * Pixels strictly inferior to the threshold become black, the others white.
	 *
	 * \param[in]	img	the image to binarize
	 * \param[in]	thresh	the threshold
	 * \return	the newly created image
	 */
	template<typename T, typename CMP = std::less<T>> ImageBW Threshold(const Image<T> &img, T thresh, CMP cmp = std::less<T>{})
	{
		auto out = ImageBW(img.GetWidth(), img.GetHeight());
		for (auto i : Range(img))
			out.At(i) = cmp(img.At(i), thresh) ? pixel::BWBlack : pixel::BWWhite;
		return out;
	}

	/****************************************************************************/
	/*!
	 * Creates a BW image using Niblack's algorithm
	 * the binarisation use a threshold on a square window of neighbouring pixels
	 * the threshold <i>t</i> is calculated by
	 * 		- mean <i>m</i>
	 * 		- standard deviation <i>s</i>
	 * 		- parameter <i>k</i>
	 * Pixels strictly inferior to the threshold become black, the others white.
	 *
	 * <i>t</i> = <i>m</i> + <i>k</i>*<i>s</i>
	 *
	 * \param[in]	img	the image to binarize
	 * \param[in] halfwin	size of the square window of neighbouring pixels, from the border of the window to the central pixel
	 * \param[in] k				parameter <i>k</i>
	 * \return	the newly created image
	 */
	template<typename T> ImageBW Niblack(const Image<T> &img, size_t halfwin, double k = 0.5, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
	{
		auto out = ImageBW(img.GetWidth(), img.GetHeight());
		FOREACHPIXEL(x, y, img)
		{
			// adjust window
			auto x1 = (x < halfwin) ? 0 : x - halfwin;
			auto y1 = (y < halfwin) ? 0 : y - halfwin;
			auto x2 = x + halfwin;
			if (x2 >= img.GetWidth()) x2 = img.GetWidth() - 1;
			auto y2 = y + halfwin;
			if (y2 >= img.GetHeight()) y2 = img.GetHeight() - 1;

			// compute stats
			auto sum = DecimalType<T>(0.0);
			auto sqrsum = DecimalType<T>(0.0);
			for (auto ty = y1; ty <= y2; ty++)
				for (auto tx = x1; tx <= x2; tx++)
				{
					auto p = DecimalType<T>(img.At(tx, ty));
					sum += p;
					sqrsum += Sqr(p);
				}
			auto size = (x2 - x1 + 1) * (y2 - y1 + 1);
			auto m = sum / double(size); // mean
			auto sd = sqrt(sqrsum / double(size) - Sqr(m)); // std deviation

			// compute threshold
			auto t = m  + k * sd ; // original formula

			out.At(x, y) = (img.At(x, y) < T(t)) ? pixel::BWBlack : pixel::BWWhite;
		}
		return out;
	}

	/****************************************************************************/
	/*!
	 * Creates a BW image using Sauvola's algorithm
	 * the binarisation use a threshold on a square window of neighbouring pixels
	 * the threshold <i>t</i> is calculated by
	 * 		- mean <i>m</i>
	 * 		- standard deviation <i>s</i>
	 * 		- dynamic range of std.dev. <i>R</i>
	 * 		- parameter <i>k</i>
	 * Pixels strictly inferior to the threshold become black, the others white.
	 *
	 * <i>t</i> = <i>m</i> * (1 + <i>k</i> * (<i>s</i> / <i>R</i> - 1))
	 *
	 * \param[in]	img	the image to binarize
	 * \param[in] halfwin	size of the square window of neighbouring pixels, from the border of the window to the central pixel
	 * \param[in] k				parameter <i>k</i>
	 * \return	the newly created image
	 */
	template<typename T> ImageBW Sauvola(const Image<T> &img, size_t halfwin, double k = 0.5, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
	{
		// compute range
		const auto mM = MinMax(img);
		const auto dynRge = (DecimalType<T>(mM.second) - DecimalType<T>(mM.first)) * 0.5;

		auto out = ImageBW(img.GetWidth(), img.GetHeight());
		FOREACHPIXEL(x, y, img)
		{
			// adjust window
			auto x1 = (x < halfwin) ? 0 : x - halfwin;
			auto y1 = (y < halfwin) ? 0 : y - halfwin;
			auto x2 = x + halfwin;
			if (x2 >= img.GetWidth()) x2 = img.GetWidth() - 1;
			auto y2 = y + halfwin;
			if (y2 >= img.GetHeight()) y2 = img.GetHeight() - 1;

			// compute stats
			//auto mGL = std::numeric_limits<double>::max(); // min gray
			auto sum = DecimalType<T>(0.0);
			auto sqrsum = DecimalType<T>(0.0);
			for (auto ty = y1; ty <= y2; ty++)
				for (auto tx = x1; tx <= x2; tx++)
				{
					auto p = DecimalType<T>(img.At(tx, ty));
					sum += p;
					sqrsum += Sqr(p);
					//if (p < mGL) mGL = p;
				}
			auto size = (x2 - x1 + 1) * (y2 - y1 + 1);
			auto m = sum / double(size); // mean
			auto sd = sqrt(sqrsum / double(size) - Sqr(m)); // std deviation

			// compute threshold
			//auto t = m - k * (1 - sd / dynRge) * (m - mGL); // Wolf's formula
			auto t = m * (1 + k * (sd / dynRge - 1)); // original formula

			out.At(x, y) = (img.At(x, y) < T(t)) ? pixel::BWBlack : pixel::BWWhite;
		}

		return out;
	}

	/****************************************************************************/
	/*!
	 * Creates a BW image through a classification of the histogram
	 *
	 * \param[in]	img	the image to binarize
	 * \param[in]	classes	the total number of classes to compute
	 * \param[in]	black_classes	the total number of classes that represent the black pixels
	 * \return	the newly created image or nullptr if the image was not allocated
	 */
	template<typename T> ImageBW kMeansHisto(const Image<T> &img, size_t classes, size_t black_classes, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
	{
		ImageBW out = ImageBW(img.GetWidth(), img.GetHeight(), pixel::BWBlack);

		auto histo = MakeHistogram(img);
		if (histo.Size() < 3)
			return Fisher(img);

		auto km = kMeans{};
		km.AddSamples(histo);
		auto min = 0.0;
		auto max = double(histo.Size()) - 1;
		for (size_t tmp = 1; tmp < histo.Size(); tmp++)
			if (histo.GetBin(tmp) != 0)
			{
				if (histo.GetBin(tmp - 1) == 0)
					min = double(tmp);
				break;
			}
		for (auto tmp = int(histo.Size()) - 2; tmp >= 0; tmp--)
			if (histo.GetBin(tmp) != 0)
			{
				if (histo.GetBin(tmp + 1) == 0)
					max = double(tmp);
				break;
			}

		if (max <= min)
		{
			max = double(histo.Size()) - 1;
			min = 0;
		}

		for (size_t tmp = 0; tmp < classes; tmp++)
		{
			RealCoeff rc(min + double(tmp + 1) * (max - min) / double(classes + 1), 1);
			km.AddPrototype(rc);
		}
		km.Run();
		double threshold = double(
				std::static_pointer_cast<Real>(km.GetPrototypes()[black_classes - 1])->GetValue() +
				std::static_pointer_cast<Real>(km.GetPrototypes()[black_classes])->GetValue()) / 2.0;

		for (auto tmp : Range(img))
		{
			if (double(img.At(tmp)) > threshold)
				out.At(tmp) = pixel::BWWhite;
			else
				out.At(tmp) = pixel::BWBlack;
		}

		return out;
	}

	/****************************************************************************/
	/*!
	 * Creates a BW image the local minima
	 *
	 * \param[in]	img	the image to binarize
	 * \param[in]	area	the half window size (window size = 1 + 2 * area).
	 * \return	the newly created image or nullptr if the image was not allocated
	 */
	template<typename T> ImageBW LocalMin(const Image<T> &img, size_t area = 1, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
	{
		auto ibw = ImageBW(img.GetWidth(), img.GetHeight(), pixel::BWBlack);
		// find plateaux
		auto offset = size_t(0);
		for (int y = 0; y < int(img.GetHeight()); y++)
			for (int x = 0; x < int(img.GetWidth()); x++)
			{
				auto val = img.At(offset);
				for (int ty = Max(y - int(area), 0); ty <= Min(int(img.GetHeight()) - 1, y + int(area)); ty++)
					for (int tx = Max(x - int(area), 0); tx <= Min(int(img.GetWidth()) - 1, x + int(area)); tx++)
					{
						if (img.At(tx, ty) < val)
						{
							ibw.At(offset) = pixel::BWWhite;
							break; // slightly useless...
						}
					}
				offset += 1;
			}
		// propagate
		auto mod = false;
		do
		{
			mod = false;
			auto offset = size_t(0);
			for (int y = 0; y < int(img.GetHeight()); y++)
				for (int x = 0; x < int(img.GetWidth()); x++)
				{
					if (ibw.At(offset) == pixel::BWBlack)
					{
						auto val = img.At(offset);
						for (int ty = Max(y - int(area), 0); ty <= Min(int(img.GetHeight()) - 1, y + int(area)); ty++)
							for (int tx = Max(x - int(area), 0); tx <= Min(int(img.GetWidth()) - 1, x + int(area)); tx++)
							{
								if ((ibw.At(tx, ty) == pixel::BWWhite) &&
										(img.At(tx, ty) == val))
								{
									ibw.At(offset) = pixel::BWWhite;
									mod = true;
									break; // slightly useless...
								}
							}
					}
					offset += 1;
				}
		} while (mod);

		return ibw;
	}

	/****************************************************************************/
	/*!
	 * Creates a BW image the local maxima
	 *
	 * \param[in]	img	the image to binarize
	 * \param[in]	area	the half window size (window size = 1 + 2 * area).
	 * \return	the newly created image
	 */
	template<typename T> ImageBW LocalMax(const Image<T> &img, size_t area = 1, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
	{
		ImageBW ibw = ImageBW(img.GetWidth(), img.GetHeight(), pixel::BWBlack);
		// find plateaux
		auto offset = size_t(0);
		for (int y = 0; y < int(img.GetHeight()); y++)
			for (int x = 0; x < int(img.GetWidth()); x++)
			{
				auto val = img.At(offset);
				for (int ty = Max(y - int(area), 0); ty <= Min(int(img.GetHeight()) - 1, y + int(area)); ty++)
					for (int tx = Max(x - int(area), 0); tx <= Min(int(img.GetWidth()) - 1, x + int(area)); tx++)
					{
						if (img.At(tx, ty) > val)
						{
							ibw.At(offset) = pixel::BWWhite;
							break; // slightly useless...
						}
					}
				offset += 1;
			}
		// propagate
		auto mod = false;
		do
		{
			mod = false;
			auto offset = size_t(0);
			for (int y = 0; y < int(img.GetHeight()); y++)
				for (int x = 0; x < int(img.GetWidth()); x++)
				{
					if (ibw.At(offset) == pixel::BWBlack)
					{
						auto val = img.At(offset);
						for (int ty = Max(y - int(area), 0); ty <= Min(int(img.GetHeight()) - 1, y + int(area)); ty++)
							for (int tx = Max(x - int(area), 0); tx <= Min(int(img.GetWidth()) - 1, x + int(area)); tx++)
							{
								if ((ibw.At(tx, ty) == pixel::BWWhite) &&
										(img.At(tx, ty) == val))
								{
									ibw.At(offset) = pixel::BWWhite;
									mod = true;
									break; // slightly useless...
								}
							}
					}
					offset += 1;
				}
		} while (mod);

		return ibw;
	}

	/****************************************************************************/
	/*!
	 * Creates a BW image using the Fisher criterion
	 *
	 * \param[in]	img	the image to binarize
	 * \return	the newly created image or nullptr if the image was not allocated
	 */
	template<typename T> ImageBW Fisher(const Image<T> &img, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
	{
		const auto mM = MinMax(img);
		auto scaled = false;
		auto h = Histogram{};
		if (mM.second < 10)
		{
			auto clone = img;
			clone *= 100.0;
			h = MakeHistogram(clone);
			scaled = true;
		}
		else
		{
			h = MakeHistogram(img);
		}
		const auto t = h.Fisher();
		auto thresh = T{};
		if (scaled)
			thresh = T(mM.first + T(double(t) / 100.0));
		else
			thresh = T(mM.first + T(t));
		return Threshold(img, thresh);
	}

	/****************************************************************************/
	/*!
	 * Creates a BW image using the entropy threshold
	 *
	 * \param[in]	img	the image to binarize
	 * \return	the newly created image or nullptr if the image was not allocated
	 */
	template<typename T> ImageBW Entropy(const Image<T> &img, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
	{
		const auto mM = MinMax(img);
		bool scaled = false;
		auto h = Histogram{};
		if (mM.second < 10)
		{
			auto clone = img;
			clone *= 100.0;
			h = MakeHistogram(clone);
			scaled = true;
		}
		else
		{
			h = MakeHistogram(img);
		}
		const auto t = h.EntropyThreshold();
		auto thresh = T{};
		if (scaled)
			thresh = T(mM.first + T(double(t) / 100.0));
		else
			thresh = T(mM.first + T(t));
		return Threshold(img, thresh);
	}

	/****************************************************************************/
	/*!
	 * Creates a BW image using the Otsu's algorithm
	 *
	 * \param[in]	img	the image to binarize
	 * \return	the newly created image or nullptr if the image was not allocated
	 */
	template<typename T> ImageBW Otsu(const Image<T> &img, typename std::enable_if<std::is_arithmetic<T>::value>::type *dummy = nullptr)
	{
		auto h = MakeHistogram(img);

		const auto hsize = h.Size();
		auto mu = std::vector<double>(hsize, 0.0);
		auto w = std::vector<double>(hsize, 0.0);
		auto s2 = std::vector<double>(hsize, 0.0);
		auto hnorm = std::vector<double>(hsize, 0.0);

		const auto cumul = double(h.CumulateBins());
		for (size_t k = 0; k < hsize; ++k)
		{
			hnorm[k] = double(h.GetBin(k))/ cumul;
		}

		for (size_t k = 0; k < hsize; ++k)
		{
			for (size_t j = 0; j < k; ++j)
			{
				mu[k] += double(j) * hnorm[j];
				w[k] += hnorm[j];
			}
		}
		for (size_t k = 0; k < hsize; ++k)
		{
			s2[k] = w[k] * (1 - w[k]) * (mu[hsize-1] * w[k] - mu[k]) * (mu[hsize-1] * w[k] - mu[k]);
		}

		auto thresh = T{};
		auto maxs2 = 0.0;
		for (size_t k = 0;k < hsize; k++)
		{
			if (s2[k] > maxs2)
			{
				maxs2 = s2[k];
				thresh = T(k);
			}
		}

		return Threshold(img, thresh);
	}

	/*! \brief Binarization action
	 * \ingroup action
	 */
	class Gray2BW: public Action
	{
		public:
			virtual ~Gray2BW() override { }
			/*! \brief Action = binarize a gray image */
			virtual ImageBW Binarize(const ImageGray &img) = 0;
	};

	/*! \brief Threshold binarization action
	 * \ingroup action
	 */
	class Gray2BWThreshold: public Gray2BW
	{
		public:
			/*! \brief Default constructor */
			Gray2BWThreshold(uint8_t t = 127);
			virtual ~Gray2BWThreshold() override { }
			virtual const String& GetClassName() const override { static const String cn(U"Gray2BWThreshold"); return cn; }
			virtual ImageBW Binarize(const ImageGray &img) override;
			CRN_DECLARE_CLASS_CONSTRUCTOR(Gray2BWThreshold)
				CRN_SERIALIZATION_CONSTRUCTOR(Gray2BWThreshold)
	};

	/*! \brief Niblack binarization action
	 * \ingroup action
	 */
	class Gray2BWNiblack: public Gray2BW
	{
		public:
			/*! \brief Default constructor */
			Gray2BWNiblack(size_t halfwin = 3, double k = 0.5);
			virtual ~Gray2BWNiblack() override { }
			virtual const String& GetClassName() const override { static const String cn(U"Gray2BWNiblack"); return cn; }
			virtual ImageBW Binarize(const ImageGray &img) override;
			CRN_DECLARE_CLASS_CONSTRUCTOR(Gray2BWNiblack)
				CRN_SERIALIZATION_CONSTRUCTOR(Gray2BWNiblack)
	};

	/*! \brief Sauvola binarization action
	 * \ingroup action
	 */
	class Gray2BWSauvola: public Gray2BW
	{
		public:
			/*! \brief Default constructor */
			Gray2BWSauvola(size_t halfwin = 3, double k = 0.5);
			virtual ~Gray2BWSauvola() override { }
			virtual const String& GetClassName() const override { static const String cn(U"Gray2BWSauvola"); return cn; }
			virtual ImageBW Binarize(const ImageGray &img) override;
			CRN_DECLARE_CLASS_CONSTRUCTOR(Gray2BWSauvola)
				CRN_SERIALIZATION_CONSTRUCTOR(Gray2BWSauvola)
	};

	/*! \brief k-means histo binarization action
	 * \ingroup action
	 */
	class Gray2BWkMeansHisto: public Gray2BW
	{
		public:
			/*! \brief Default constructor */
			Gray2BWkMeansHisto(size_t classes = 5, size_t black_classes = 3);
			virtual ~Gray2BWkMeansHisto() override { }
			virtual const String& GetClassName() const override { static const String cn(U"Gray2BWkMeansHisto"); return cn; }
			virtual ImageBW Binarize(const ImageGray &img) override;
			CRN_DECLARE_CLASS_CONSTRUCTOR(Gray2BWkMeansHisto)
				CRN_SERIALIZATION_CONSTRUCTOR(Gray2BWkMeansHisto)
	};

	/*! \brief Local min binarization action
	 * \ingroup action
	 */
	class Gray2BWLocalMin: public Gray2BW
	{
		public:
			/*! \brief Default constructor */
			Gray2BWLocalMin(size_t area = 1);
			virtual ~Gray2BWLocalMin() override { }
			virtual const String& GetClassName() const override { static const String cn(U"Gray2BWLocalMin"); return cn; }
			virtual ImageBW Binarize(const ImageGray &img) override;
			CRN_DECLARE_CLASS_CONSTRUCTOR(Gray2BWLocalMin)
				CRN_SERIALIZATION_CONSTRUCTOR(Gray2BWLocalMin)
	};

	/*! \brief Local max binarization action
	 * \ingroup action
	 */
	class Gray2BWLocalMax: public Gray2BW
	{
		public:
			/*! \brief Default constructor */
			Gray2BWLocalMax(size_t area = 1);
			virtual ~Gray2BWLocalMax() override { }
			virtual const String& GetClassName() const override { static const String cn(U"Gray2BWLocalMax"); return cn; }
			virtual ImageBW Binarize(const ImageGray &img) override;
			CRN_DECLARE_CLASS_CONSTRUCTOR(Gray2BWLocalMax)
				CRN_SERIALIZATION_CONSTRUCTOR(Gray2BWLocalMax)
	};

	/*! \brief Fisher binarization action
	 * \ingroup action
	 */
	class Gray2BWFisher: public Gray2BW
	{
		public:
			/*! \brief Default constructor */
			Gray2BWFisher();
			virtual ~Gray2BWFisher() override { }
			virtual const String& GetClassName() const override { static const String cn(U"Gray2BWFisher"); return cn; }
			virtual ImageBW Binarize(const ImageGray &img) override;
			CRN_DECLARE_CLASS_CONSTRUCTOR(Gray2BWFisher)
				CRN_SERIALIZATION_CONSTRUCTOR(Gray2BWFisher)
	};

	/*! \brief Entropy binarization action
	 * \ingroup action
	 */
	class Gray2BWEntropy: public Gray2BW
	{
		public:
			/*! \brief Default constructor */
			Gray2BWEntropy();
			virtual ~Gray2BWEntropy() override { }
			virtual const String& GetClassName() const override { static const String cn(U"Gray2BWEntropy"); return cn; }
			virtual ImageBW Binarize(const ImageGray &img) override;
			CRN_DECLARE_CLASS_CONSTRUCTOR(Gray2BWEntropy)
				CRN_SERIALIZATION_CONSTRUCTOR(Gray2BWEntropy)
	};

	/*! \brief Otsu binarization action
	 * \ingroup action
	 */
	class Gray2BWOtsu: public Gray2BW
	{
		public:
			/*! \brief Default constructor */
			Gray2BWOtsu();
			virtual ~Gray2BWOtsu() override { }
			virtual const String& GetClassName() const override { static const String cn(U"Gray2BWOtsu"); return cn; }
			virtual ImageBW Binarize(const ImageGray &img) override;
			CRN_DECLARE_CLASS_CONSTRUCTOR(Gray2BWOtsu)
				CRN_SERIALIZATION_CONSTRUCTOR(Gray2BWOtsu)
	};

	CRN_ALIAS_SMART_PTR(Gray2BW)

	ImageBW MakeImageBW(const ImageGray &img);
	/*@}*/

}
#endif
