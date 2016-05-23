/* Copyright 2007-2015 Yann LEYDIER, INSA-Lyon, CoReNum, Université Paris Descartes
 *
 * This file is part of libCRN.
 *
 * libCRN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libCRN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libCRN.  If not, see <http://www.gnu.org/licenses/>.
 *
 * file: CRNImage.hpp
 * \author Yann LEYDIER
 */

#ifndef CRNIMAGEHPP
#define CRNIMAGEHPP

#include <iostream>

#include <CRNGeometry/CRNRect.h>
#include <CRNMath/CRNMatrixInt.h>
#include <CRNMath/CRNMatrixDouble.h>
#include <CRNGeometry/CRNPoint2DInt.h>
#include <CRNMath/CRNMatrixComplex.h>
#include <set>

#ifdef _MSC_VER // remove annoying warnings in Microsoft Visual C++
#	pragma warning(disable:4800)
#endif

namespace crn
{
	/**************************************************************************************
	 * Construction and copy
	 *************************************************************************************/

	/****************************************************************************/
	/*!
	 * Constructor for this class
	 *
	 * \throws	ExceptionRuntime	cannot allocate pixels
	 *
	 * \param[in]	w	width value to be set for this image
	 * \param[in]	h	height value to be set for this image
	 * \param[in]	val	default value for pixels
	 *
	 */
	template<typename T> Image<T>::Image(size_t w, size_t h, pixel_type val):
		ImageBase{w, h},
		pixels(w * h, val)
		{ }

	/****************************************************************************/
	/*!
	 * Constructor for this class
	 *
	 * \throws	ExceptionRuntime	cannot allocate pixels
	 *
	 * \param[in]	w	width value to be set for this image
	 * \param[in]	h	height value to be set for this image
	 * \param[in]	data	the data to copy to the new image
	 *
	 */
	template<typename T> Image<T>::Image(size_t w, size_t h, const pixel_type *data):
		ImageBase{w, h},
		pixels(data, data + w * h)
		{ }

	/*! Copy constructor
	 * \param[in]	img	the image to copy
	 */
	template<typename T> template<typename Y> Image<T>::Image(const Image<Y> &img):
		ImageBase(img.GetWidth(), img.GetHeight()),
		pixels(img.begin(), img.end())
	{ }

	/*! Crop constructor
	 * \param[in]	img	the image to copy
	 * \param[in]	bbox	the zone to copy
	 */
	template<typename T> template<typename Y> Image<T>::Image(const Image<Y> &img, const Rect &bbox):
		ImageBase(bbox.GetWidth(), bbox.GetHeight()),
		pixels(bbox.GetWidth() * bbox.GetHeight())
	{
		for (int y = 0; y < height; ++y)
			std::copy_n(img.begin() + bbox.GetLeft() + (y + bbox.GetTop()) * img.GetWidth(), width, pixels.begin() + y * width);
	}

	/*! Copy operator: pixel values are not cast
	 * \param[in]	img	the image to copy
	 * \return	a self reference
	 */
	template<typename T> template<typename Y> Image<T>& Image<T>::operator=(const Image<Y> &img)
	{
		width = img.GetWidth();
		height = img.GetHeight();
		pixels.resize(width * height);
		std::copy(img.begin(), img.end(), pixels.begin());
	}

	/*! Force copy operator: pixel values are cast
	 * \param[in]	img	the image to copy
	 */
	template<typename T> template<typename Y> void Image<T>::Assign(const Image<Y> &img)
	{
		width = img.GetWidth();
		height = img.GetHeight();
		pixels.resize(width * height);
		for (auto tmp : Range(img))
			pixels[tmp] = T(img.At(tmp));
	}

	/*! Saves as PNG file
	 * \throws	ExceptionInvalidArgument	unsupported pixel format
	 * \param[in]	fname	full path	to the image to save
	 */
	template<typename T> void Image<T>::SavePNG(const Path &fname) const
	{
		impl::SavePNG(*this, fname);
	}
	/*! Saves as JPEG file
	 * \throws	ExceptionInvalidArgument	unsupported pixel format
	 * \param[in]	fname	full path	to the image to save
	 * \param[in]	qual	compression quality [0..100]
	 */
	template<typename T> void Image<T>::SaveJPEG(const Path &fname, unsigned int qual) const
	{
		impl::SaveJPEG(*this, fname, Min(qual, 100u));
	}

	/*! Tests equality
	 * \param[in]	other	the image to compare
	 * \return	true if dimensions are equal and pixels are equal
	 */
	template<typename T> bool Image<T>::operator==(const Image &other) const
	{
		if ((GetWidth() != other.GetWidth()) || (GetHeight() != other.GetHeight()))
			return false;
		return std::equal(begin(), end(), other.begin());
	}

	/**************************************************************************************
	 * Arithmetic
	 *************************************************************************************/

	/*! Adds another image 
	 * \throws	ExceptionDimension	images have different sizes
	 * \param[in]	img	the image to add
	 * \return	a self reference
	 */
	template<typename T> Image<T>& Image<T>::operator+=(const Image<T> &img)
	{
		if ((GetWidth() != img.GetWidth()) || (GetHeight() != img.GetHeight()))
			throw ExceptionDimension("Image+=(Image): images have different sizes.");
		for (auto i : Range(img))
			At(i) = T(At(i) + img.At(i));
		return *this;
	}
	
	/*! Subtracts another image
	 * \throws	ExceptionDimension	images have different sizes
	 * \param[in]	img	the image to subtract
	 * \return	a self reference
	 */
	template<typename T> Image<T>& Image<T>::operator-=(const Image<T> &img)
	{
		if ((GetWidth() != img.GetWidth()) || (GetHeight() != img.GetHeight()))
			throw ExceptionDimension("Image-=(Image): images have different sizes.");
		for (auto i : Range(img))
			At(i) = T(At(i) - img.At(i));
		return *this;
	}
	
	/*! Multiplies all pixels
	 * \param[in]	f	the factor
	 * \return	a self reference
	 */
	template<typename T> Image<T>& Image<T>::operator*=(double f)
	{
		for (auto &v : pixels)
			v = T(DecimalType<T>(v) * f);
		return *this;
	}
	
	/*! Multiplies with another image's pixels
	 * \throws	ExceptionDimension	images have different sizes
	 * \param[in]	img	the image to multiply
	 * \return	a self reference
	 */
	template<typename T> Image<T>& Image<T>::operator*=(const Image<T> &img)
	{
		if ((GetWidth() != img.GetWidth()) || (GetHeight() != img.GetHeight()))
			throw ExceptionDimension("Image*=(Image): images have different sizes.");
		for (auto i : Range(img))
			At(i) = T(At(i) * img.At(i));
		return *this;
	}
	
	/*! Divides by another image's pixels
	 * \throws	ExceptionDimension	images have different sizes
	 * \param[in]	img	the image to divide with
	 * \return	a self reference
	 */
	template<typename T> Image<T>& Image<T>::operator/=(const Image<T> &img)
	{
		if ((GetWidth() != img.GetWidth()) || (GetHeight() != img.GetHeight()))
			throw ExceptionDimension("Image/=(Image): images have different sizes.");
		for (auto i : Range(img))
			At(i) = T(At(i) / img.At(i));
		return *this;
	}

	/**************************************************************************************
	 * Edition
	 *************************************************************************************/

	/*! Negative */
	template<typename T> void Image<T>::Negative()
	{
		for (size_t tmp = 0; tmp < width * height; ++tmp)
			pixels[tmp] = -pixels[tmp];
	}

	/*! Complement
	 * \param[in]	maxval	maximum value
	 */
	template<typename T> void Image<T>::Complement(T maxval)
	{
		for (size_t tmp = 0; tmp < width * height; ++tmp)
			pixels[tmp] = T(maxval - pixels[tmp]);
	}

	/*! Copies a part of an image
	 * \param[in]	src	the image to copy	
	 * \param[in]	srczone	the zone to copy
	 * \param[in]	dx	the abscissa where to copy
	 * \param[in]	dy	the ordinate where to copy
	 */
	template<typename T> template<typename Y> void Image<T>::Blit(const Image<Y> &src, const Rect &srczone, size_t dx, size_t dy)
	{
		if ((dx >= width) || (dy >= height)) // nothing to be done
			return;
		// source clipping
		auto bbox = srczone & src.GetBBox();
		// destination clipping
		if (dx + bbox.GetWidth() > width)
			bbox.SetWidth(int(width) - int(dx));
		if (dy + bbox.GetHeight() > height)
			bbox.SetWidth(int(height) - int(dy));
		// copy
		for (int y = 0; y < bbox.GetHeight(); ++y)
			std::copy_n(src.begin() + bbox.GetLeft() + (y + bbox.GetTop()) * src.GetWidth(), bbox.GetWidth(), pixels.begin() + dx + (dy + y) * width);
	}

	/****************************************************************************/
	/*!
	 * Flood fills a portion of the image.
	 *
	 * \warning Be careful of stack overflows!
	 *
	 * \throws	ExceptionDomain	coordinates out of bounds
	 *
	 * \param[in]	x	the beginning abscissa
	 * \param[in]	y	the beginning ordinate
	 * \param[in]	val	the value used to fill
	 * \param[in]	dist	the distance to use (DistanceType::D4 or DistanceType::D8)
	 */
	template<typename T> void Image<T>::FloodFill(size_t x, size_t y, const pixel_type &val, crn::DistanceType dist)
	{
		if ((x >= width) || (y >= height))
			throw crn::ExceptionDomain("Image::FloodFill(): Coordinates out of bounds.");
		size_t offset = x + y * width;
		pixel_type oldval(pixels[offset]);
		if (oldval == val)
			return; // do not fill twice
		pixels[offset] = val;
		if ((dist == crn::DistanceType::D4) || (dist == crn::DistanceType::D8))
		{
			if (x > 0)
				if (pixels[offset - 1] == oldval)
					FloodFill(x - 1, y, val, dist);
			if (x < width - 1)
				if (pixels[offset + 1] == oldval)
					FloodFill(x + 1, y, val, dist);
			if (y > 0)
				if (pixels[offset - width] == oldval)
					FloodFill(x, y - 1, val, dist);
			if (y < height - 1)
				if (pixels[offset + width] == oldval)
					FloodFill(x, y + 1, val, dist);
		}
		if (dist == crn::DistanceType::D8)
		{
			if ((x > 0) && (y > 0))
				if (pixels[offset - 1 - width] == oldval)
					FloodFill(x - 1, y - 1, val, dist);
			if ((x > 0) && (y < height - 1))
				if (pixels[offset - 1 + width] == oldval)
					FloodFill(x - 1, y + 1, val, dist);
			if ((x < width - 1) && (y > 0))
				if (pixels[offset + 1 - width] == oldval)
					FloodFill(x + 1, y - 1, val, dist);
			if ((x < width - 1) && (y < height - 1))
				if (pixels[offset + 1 + width] == oldval)
					FloodFill(x + 1, y + 1, val, dist);
		}
	}

	/****************************************************************************/
	/*!
	 * Fills a portion of the image.
	 *
	 * \throws	ExceptionDomain	coordinates out of bounds
	 *
	 * \param[in]  x  the beginning abscissa
	 * \param[in]  y  the beginning ordinate
	 * \param[in]  val  the value used to fill
	 * \param[in]  dist  the distance to use (DistanceType::D4 or DistanceType::D8)
	 */
	template<typename T> void Image<T>::ScanFill(size_t x, size_t y, const pixel_type &val, crn::DistanceType dist)
	{
		if ((x >= width) || (y >= height))
			throw crn::ExceptionDomain("Image::ScanFill(): Coordinates out of bounds.");
		pixel_type oldval = pixels[x + y * width];
		if (oldval == val)
			return; // do not fill twice
		auto todo = std::vector<std::pair<int, std::pair<int, int>>>{};
		int bx;
		for (bx = int(x); bx >= 0; --bx)
			if (pixels[bx + y * width] != oldval)
				break;
		bx += 1;
		int ex;
		for (ex = int(x); ex < int(width); ++ex)
			if (pixels[ex + y * width] != oldval)
				break;
		ex -= 1;
		todo.emplace_back(int(y), std::make_pair(bx, ex));
		while (!todo.empty())
		{
			auto pos(todo.back());
			todo.pop_back();
			auto checklimitup = pos.second.first - 1; // these two variables are used to avoid
			auto checklimitdown = pos.second.first - 1; // adding the same stream more than once
			for (int tx = pos.second.first; tx <= pos.second.second; ++tx)
			{
				// fill
				pixels[tx + pos.first * width] = val;
				// look up
				if (pos.first > 0)
				{
					if ((tx > checklimitup) && (pixels[tx + (pos.first - 1) * width] == oldval))
					{
						// add to queue
						for (bx = tx; bx >= 0; --bx)
							if (pixels[bx + (pos.first - 1) * width] != oldval)
								break;
						bx += 1;
						for (ex = tx; ex < width; ++ex)
							if (pixels[ex + (pos.first - 1) * width] != oldval)
								break;
						ex -= 1;
						todo.emplace_back(pos.first - 1, std::make_pair(bx, ex));
						checklimitup = ex;
					}
					if (dist == crn::DistanceType::D8)
					{
						// look up-left (only for the first pixel of the stream)
						if ((tx == pos.second.first) && (tx > 0) && (tx - 1 >= checklimitup))
						{
							if (pixels[tx - 1 + (pos.first - 1) * width] == oldval)
							{
								// add to queue
								for (bx = tx - 1; bx >= 0; --bx)
									if (pixels[bx + (pos.first - 1) * width] != oldval)
										break;
								bx += 1;
								for (ex = tx - 1; ex < width; ++ex)
									if (pixels[ex + (pos.first - 1) * width] != oldval)
										break;
								ex -= 1;
								todo.emplace_back(pos.first - 1, std::make_pair(bx, ex));
								checklimitup = ex;
							}
						}
						// look up-right (only for the last pixel of the stream)
						if ((tx == pos.second.second) && (tx < width - 1) && (tx + 1 > checklimitup))
						{
							if (pixels[tx + 1 + (pos.first - 1) * width] == oldval)
							{
								// add to queue
								for (bx = tx + 1; bx >= 0; --bx)
									if (pixels[bx + (pos.first - 1) * width] != oldval)
										break;
								bx += 1;
								for (ex = tx + 1; ex < width; ++ex)
									if (pixels[ex + (pos.first - 1) * width] != oldval)
										break;
								ex -= 1;
								todo.emplace_back(pos.first - 1, std::make_pair(bx, ex));
								checklimitup = ex;
							}
						}
					}
				} // look up
				// look down
				if ((pos.first < height - 1) && (tx > checklimitdown))
				{
					if (pixels[tx + (pos.first + 1) * width] == oldval)
					{
						// add to queue
						for (bx = tx; bx >= 0; --bx)
							if (pixels[bx + (pos.first + 1) * width] != oldval)
								break;
						bx += 1;
						for (ex = tx; ex < width; ++ex)
							if (pixels[ex + (pos.first + 1) * width] != oldval)
								break;
						ex -= 1;
						todo.emplace_back(pos.first + 1, std::make_pair(bx, ex));
						checklimitdown = ex;
					}
					if (dist == crn::DistanceType::D8)
					{
						// look up-left (only for the first pixel of the stream)
						if ((tx == pos.second.first) && (tx > 0) && (tx - 1 >= checklimitdown))
						{
							if (pixels[tx - 1 + (pos.first + 1) * width] == oldval)
							{
								// add to queue
								for (bx = tx - 1; bx >= 0; --bx)
									if (pixels[bx + (pos.first + 1) * width] != oldval)
										break;
								bx += 1;
								for (ex = tx - 1; ex < width; ++ex)
									if (pixels[ex + (pos.first + 1) * width] != oldval)
										break;
								ex -= 1;
								todo.emplace_back(pos.first + 1, std::make_pair(bx, ex));
								checklimitdown = ex;
							}
						}
						// look up-right (only for the last pixel of the stream)
						if ((tx == pos.second.second) && (tx < width - 1) && (tx + 1 > checklimitdown))
						{
							if (pixels[tx + 1 + (pos.first + 1) * width] == oldval)
							{
								// add to queue
								for (bx = tx + 1; bx >= 0; --bx)
									if (pixels[bx + (pos.first + 1) * width] != oldval)
										break;
								bx += 1;
								for (ex = tx + 1; ex < width; ++ex)
									if (pixels[ex + (pos.first + 1) * width] != oldval)
										break;
								ex -= 1;
								todo.emplace_back(pos.first + 1, std::make_pair(bx, ex));
								checklimitdown = ex;
							}
						}
					}
				} // look down
			}
		} // while there are streams left
	}

	/*! Draws a rectangle using a specified color 
	 * \param[in]	r	the rectangle to draw
	 * \param[in]	color	a pixel in any compatible format
	 * \param[in]	filled	is the rectangle filled? (default = no)
	 */
	template<typename T> void Image<T>::DrawRect(const Rect &r, pixel_type color, bool filled)
	{
		// clipping
		const auto clip = r & GetBBox();
		if (!clip.IsValid())
			return;

		// top line
		std::fill_n(pixels.begin() + clip.GetLeft() + clip.GetTop() * width, clip.GetWidth(), color);
		// bottom line
		std::fill_n(pixels.begin() + clip.GetLeft() + clip.GetBottom() * width, clip.GetWidth(), color);

		for (int y = clip.GetTop() + 1; y < clip.GetBottom(); ++y)
		{
			if (filled) // fill center
				std::fill_n(pixels.begin() + clip.GetLeft() + y * width, clip.GetWidth(), color);
			else // vertical lines
				At(clip.GetLeft(), y) = At(clip.GetRight(), y) = color;
		}
	}

	/*! Draws a line using a specified color
	 * \param[in]	x1	the beginning abscissa
	 * \param[in]	y1	the beginning ordinate
	 * \param[in]	x2	the ending abscissa
	 * \param[in]	y2	the ending ordinate
	 * \param[in]	color	a pixel in any compatible format
	 */
	template<typename T> void Image<T>::DrawLine(size_t x1, size_t y1, size_t x2, size_t y2, pixel_type color)
	{
		// compute increments
		int lg_delta, sh_delta, cycle, lg_step, sh_step;
		lg_delta = (int)x2 - (int)x1;
		sh_delta = (int)y2 - (int)y1;
		if (lg_delta < 0)
		{
			lg_delta = -lg_delta;
			lg_step = -1;
		}
		else 
			lg_step = 1;

		if (sh_delta < 0)
		{
			sh_delta = -sh_delta;
			sh_step = -1;
		}
		else 
			sh_step = 1;

		if (sh_delta < lg_delta)
		{
			cycle = lg_delta >> 1;
			while (x1 != x2)
			{
				if ((x1 >= 0) && (x1 < width) && (y1 >= 0) && (y1 < height))
					At(x1, y1) = color;

				x1 += lg_step;
				cycle = cycle + sh_delta;
				if (cycle > lg_delta)
				{
					y1 += sh_step;
					cycle = cycle - lg_delta;
				}
			}
		}
		else
		{
			cycle = sh_delta >> 1;
			int tmp = lg_delta;
			lg_delta = sh_delta;
			sh_delta = tmp;
			tmp = lg_step;
			lg_step = sh_step;
			sh_step = tmp;
			while (y1 != y2)
			{
				if ((x1 >= 0) && (x1 < width) && (y1 >= 0) && (y1 < height))
					At(x1, y1) = color;

				y1 += lg_step;
				cycle = cycle + sh_delta;
				if (cycle > lg_delta)
				{
					x1 += sh_step;
					cycle = cycle - lg_delta;
				}
			}
		}
	}

	/*****************************************************************************/
	/*!
	 * Scales the image
	 *
	 * \param[in]	w	the new width
	 * \param[in] h the new height
	 */
	template<typename T> void Image<T>::ScaleToSize(size_t w, size_t h)
	{
		const auto defaultvalue = pixels.front();
		const auto nullvalue = defaultvalue - defaultvalue;
		// width scaling
		auto wscale = std::vector<pixel_type>();
		auto step = (double)width / (double)w;

		if (step == 1)
		{ // just copy (assuming the values are base types)
			wscale.swap(pixels);
		}
		else
		{
			wscale = std::vector<pixel_type>(w * height, defaultvalue);
			for (auto x = size_t(0); x < w; ++x)
			{
				auto min = int(double(x) * step);
				auto coeffmin = 1 - (double(x) * step - double(min));

				auto max = int(double(x + 1) * step);
				auto coeffmax = double(x + 1) * step - double(max);

				if (step < 1.0)
				{
					max = min + 1;
					coeffmax = 1 - coeffmin;
				}
				if (max >= int(width))
					coeffmax = 0;
				for (size_t y = 0; y < height; ++y)
				{
					auto acc = DecimalType<pixel_type>(nullvalue);
					auto coeff = 0.0;
					auto yoff = y * width;

					acc += coeffmin * pixels[min + yoff];
					coeff += coeffmin;
					if (coeffmax)
					{
						acc += coeffmax * pixels[max + yoff];
						coeff += coeffmax;
					}
					for (int k = min + 1; k < max; ++k)
					{
						acc += pixels[k + yoff];
						coeff += 1;
					}
					// mean
					wscale[x + y * w] = T(acc / coeff);
				}
			}
		}

		// height scaling
		step = (double)height / (double)h;

		if (step == 1)
		{ // just copy (assuming the values are base types)
			pixels.swap(wscale);
		}
		else
		{
			pixels = std::vector<pixel_type>(w * h, defaultvalue);
			
			for (size_t y = 0; y < h; ++y)
			{
				auto min = int(double(y) * step);
				auto coeffmin = 1 - (double(y) * step - double(min));

				auto max = int(double(y + 1) * step);
				auto coeffmax = double(y + 1) * step - double(max);

				if (step < 1.0)
				{
					max = min + 1;
					coeffmax = 1 - coeffmin;
				}
				if (max >= int(height))
					coeffmax = 0;
				for (size_t x = 0; x < w; ++x)
				{
					auto acc = DecimalType<pixel_type>(nullvalue);
					auto coeff = 0.0;

					acc += coeffmin * wscale[x + min * w];
					coeff += coeffmin;
					if (coeffmax)
					{
						acc += coeffmax * wscale[x + max * w];
						coeff += coeffmax;
					}
					for (int k = min + 1; k < max; ++k)
					{
						acc += wscale[x + k * w];
						coeff += 1;
					}

					// mean
					pixels[x + y * w] = T(acc / coeff);
				}
			}
		}
		
		width = w;
		height = h;
	}

	/*****************************************************************************/
	/*!
	 * Flips the image
	 *
	 * \throws	ExceptionInvalidArgument	invalid orientation
	 * \param[in]	ori	the orientation of the flip
	 */
	template<typename T> void Image<T>::Flip(const Orientation &ori)
	{
		if (ori == Orientation::HORIZONTAL)
		{
			for (size_t y = 0; y < height; ++y)
			{
				std::reverse(pixels.begin() + y * width, pixels.begin() + (y + 1) * width);
			}
		}
		else if (ori == Orientation::VERTICAL)
		{
			auto tmp = std::vector<T>(width);
			for (size_t y = 0; y < height / 2; ++y)
			{
				std::copy_n(pixels.begin() + y * width, width, tmp.begin());
				std::copy_n(pixels.begin() + (height - 1 - y) * width, width, pixels.begin() + y * width);
				std::copy_n(tmp.begin(), width, pixels.begin() + (height - 1 - y) * width);
			}
		}
		else
			throw ExceptionInvalidArgument("void Image::Flip(const Orientation &ori): invalid orientation.");
	}

	/****************************************************************************/
	/*!
	 * Morphological dilatation
	 *
	 * \warning Invalidates proxy
	 *
	 * \throws	ExceptionDimension	even element
	 * \param[in]	strel		The structuring element
	 */
	template<typename T> template<typename CMP> void Image<T>::Dilate(const MatrixInt &strel, CMP cmp)
	{
		if (!(strel.GetRows() & 1) || !(strel.GetCols() & 1))
			throw ExceptionDimension("void Image::Dilate(const MatrixInt &strel): even matrix dimensions.");

		auto halfw = int(strel.GetCols()) / 2;
		auto halfh = int(strel.GetRows()) / 2;

		auto newpix = std::vector<pixel_type>(width * height, pixels.front());
		for (size_t y = 0; y < height; ++y)
		{
			for (size_t x = 0; x < width; ++x)
			{
				auto basey = int(y) - halfh;
				auto basex = int(x) - halfw;
				//auto pix = std::numeric_limits<pixel_type>::max();
				auto pix = [&strel, basex, basey, this]()
					{
						for (size_t cy = 0; cy < strel.GetRows(); cy++)
							for (size_t cx = 0; cx < strel.GetCols(); cx++)
							{
								if (!strel[cy][cx])
									continue;
								auto tx = basex + int(cx);
								if (tx < 0) continue;
								if (tx >= int(GetWidth())) continue;
								auto ty = basey + int(cy);
								if (ty < 0) continue;
								if (ty >= int(GetHeight())) continue;

								return pixels[tx + ty * width];
							}
						return pixels[basex + basey * width];
					}();
				for (size_t cy = 0; cy < strel.GetRows(); ++cy)
					for (size_t cx = 0; cx < strel.GetCols(); ++cx)
					{
						if (!strel[cy][cx])
							continue;
						auto tx = basex + int(cx);
						if (tx < 0) continue;
						if (tx >= int(GetWidth())) continue;
						auto ty = basey + int(cy);
						if (ty < 0) continue;
						if (ty >= int(GetHeight())) continue;

						pixel_type tmppix = pixels[tx + ty * width];
						if (cmp(tmppix, pix))
							pix = tmppix;
					}
				newpix[x + y * width] = pix;
			}
		}
		pixels.swap(newpix);
	}

	/****************************************************************************/
	/*!
	 * Morphological erosion.
	 *
	 * \warning Invalidates proxy
	 * \throws	ExceptionDimension	even element
	 *
	 * \param[in]	strel		The structuring element
	 * \return	true if success, false else
	 */
	template<typename T> template<typename CMP> void Image<T>::Erode(const MatrixInt &strel, CMP cmp)
	{
		if (!(strel.GetRows() & 1) || !(strel.GetCols() & 1))
			throw ExceptionDimension("void Image::Erode(MatrixInt &strel): even matrix dimensions.");

		auto halfw = int(strel.GetCols()) / 2;
		auto halfh = int(strel.GetRows()) / 2;

		auto newpix = std::vector<pixel_type>(width * height, pixels.front());
		for (size_t y = 0; y < height; y++)
			for (size_t x = 0; x < width; x++)
			{
				const auto basey = int(y) - halfh;
				const auto basex = int(x) - halfw;
				//auto pix = std::numeric_limits<pixel_type>::min();
				auto pix = [&strel, basex, basey, this]()
					{
						for (size_t cy = 0; cy < strel.GetRows(); cy++)
							for (size_t cx = 0; cx < strel.GetCols(); cx++)
							{
								if (!strel[cy][cx])
									continue;
								auto tx = basex + int(cx);
								if (tx < 0) continue;
								if (tx >= int(GetWidth())) continue;
								auto ty = basey + int(cy);
								if (ty < 0) continue;
								if (ty >= int(GetHeight())) continue;

								return pixels[tx + ty * width];
							}
						return pixels[basex + basey * width];
					}();
				for (size_t cy = 0; cy < strel.GetRows(); cy++)
					for (size_t cx = 0; cx < strel.GetCols(); cx++)
					{
						if (!strel[cy][cx])
							continue;
						auto tx = basex + int(cx);
						if (tx < 0) continue;
						if (tx >= int(GetWidth())) continue;
						auto ty = basey + int(cy);
						if (ty < 0) continue;
						if (ty >= int(GetHeight())) continue;

						pixel_type tmppix = pixels[tx + ty * width];
						if (!cmp(tmppix, pix))
							pix = tmppix;
					}
				newpix[x + y * width] = pix;
			}

		pixels.swap(newpix);
	}

	/*! Morphological dilatation (faster than normal dilatation for halfwin >= 8)
	 *
	 * \warning Invalidates proxy
	 *
	 * \param[in]	halfwin	the half window size (window width and height = 2*halfwin+1)
	 * \param[in]	index	the rank of the value to use on each column (0 = regular dilatation, halfwin = something closer to a median filter)
	 *
	 */
	template<typename T> template<typename CMP> void Image<T>::FastDilate(size_t halfwin, size_t index, CMP cmp)
	{
		if (halfwin == 0)
			return;
		if ((index == 0) && (halfwin < 8))
		{
			Dilate(MatrixInt{halfwin + halfwin + 1, halfwin + halfwin + 1, 1}, cmp);
			return;
		}
		// initialize windows
		auto wins = std::vector<std::multiset<pixel_type, CMP>>(width, std::multiset<pixel_type, CMP>(cmp));
		for (size_t x = 0; x < width; ++x)
			for (size_t y = 0; y < halfwin; ++y)
				wins[x].insert(At(x, y));

		auto newpix = std::vector<pixel_type>(width * height, pixels.front());
		for (size_t y = 0; y < height; ++y)
		{
			// update windows
			if (y > halfwin)
			{
				const auto ry = y - halfwin - 1;
				for (size_t x = 0; x < width; ++x)
				{
					auto it = wins[x].find(At(x, ry));
					if (it != wins[x].end())
						wins[x].erase(it);
				}
			}
			if (y + halfwin < height)
			{
				const auto ay = y + halfwin;
				for (size_t x = 0; x < width; ++x)
					wins[x].insert(At(x, ay));
			}				
			// compute mins
			auto mins = std::vector<pixel_type>(width, pixels.front());
			for (size_t x = 0; x < width; ++x)
			{
				auto cmin = wins[x].begin();
				std::advance(cmin, Min(index, wins[x].size() - 1));
				mins[x] = *cmin;
			}

			// compute
			for (size_t x = 0; x < width; ++x)
			{
				const auto bx = (x >= halfwin) ? x - halfwin : 0;
				const auto ex = Min(x + halfwin, width - 1);
				auto minval = mins[bx];
				for (auto tx = bx + 1; tx <= ex; ++tx)
					if (cmp(mins[tx], minval)) minval = mins[tx];
				newpix[x + y * width] = minval;
			} // for x
		} // for y
		pixels.swap(newpix);
	}

	/*! Morphological erosion (faster than normal erosion for halfwin >= 8)
	 *
	 * \warning Invalidates proxy
	 *
	 * \param[in]	halfwin	the half window size (window width and height = 2*halfwin+1)
	 * \param[in]	index	the rank of the value to use on each column (0 = regular erosion, halfwin = something closer to a median filter)
	 *
	 */
	template<typename T> template<typename CMP> void Image<T>::FastErode(size_t halfwin, size_t index, CMP cmp)
	{
		if (halfwin == 0)
			return;
		if ((index == 0) && (halfwin < 8))
		{
			Erode(MatrixInt{halfwin + halfwin + 1, halfwin + halfwin + 1, 1}, cmp);
			return;
		}
		// initialize windows
		auto wins = std::vector<std::multiset<pixel_type, CMP>>(width, std::multiset<pixel_type, CMP>(cmp));
		for (size_t x = 0; x < width; ++x)
			for (size_t y = 0; y < halfwin; ++y)
				wins[x].insert(At(x, y));

		auto newpix = std::vector<pixel_type>(width * height, pixels.front());
		for (size_t y = 0; y < height; ++y)
		{
			// update windows
			if (y > halfwin)
			{
				const auto ry = y - halfwin - 1;
				for (size_t x = 0; x < width; ++x)
				{
					auto it = wins[x].find(At(x, ry));
					if (it != wins[x].end())
						wins[x].erase(it);
				}
			}
			if (y + halfwin < height)
			{
				const auto ay = y + halfwin;
				for (size_t x = 0; x < width; ++x)
					wins[x].insert(At(x, ay));
			}
			// compute maxes
			auto maxes = std::vector<pixel_type>(width, pixels.front());
			for (size_t x = 0; x < width; ++x)
			{
				auto cmax = wins[x].rbegin();
				std::advance(cmax, Min(index, wins[x].size() - 1));
				maxes[x] = *cmax;
			}

			// compute
			for (size_t x = 0; x < width; ++x)
			{
				const auto bx = (x >= halfwin) ? x - halfwin : 0;
				const auto ex = Min(x + halfwin, width - 1);
				auto maxval = maxes[bx];
				for (auto tx = bx + 1; tx <= ex; ++tx)
					if (!cmp(maxes[tx], maxval)) maxval = maxes[tx];
				newpix[x + y * width] = maxval;
			} // for x
		} // for y
		pixels.swap(newpix);
	}

	/****************************************************************************/
	/*!
	 *
	 * Convolves the image with a matrix.
	 *
	 * \throws	ExceptionDimension	even matrix
	 * \throws	ExceptionDomain	matrix bigger than image
	 * \param[in]	mat	The convolution matrix
	 */
	template<typename T> void Image<T>::Convolve(const MatrixDouble &mat)
	{
		if (!(mat.GetRows() & 1) || !(mat.GetCols() & 1))
			throw ExceptionDimension("void Image::Convolve(MatrixDouble &mat): even matrix dimensions.");

		auto halfw = mat.GetCols() / 2;
		auto halfh = mat.GetRows() / 2;
		if ((halfh > height) || (halfw > width))
			throw ExceptionDomain("void Image::Convolve(MatrixDouble &mat): matrix bigger than the image!");

		auto oldimg = pixels;

		const auto nullvalue = pixels.front() - pixels.front();
		for (size_t y = 0; y < halfh; y++)
			for (size_t x = 0; x < width; x++)
			{ // top
				auto basey = int(y) - int(halfh);
				auto basex = int(x) - int(halfw);
				auto sum = DecimalType<pixel_type>(nullvalue);
				for (size_t cy = 0; cy < mat.GetRows(); cy++)
					for (size_t cx = 0; cx < mat.GetCols(); cx++)
					{
						auto tx = basex + int(cx);
						if (tx < 0) tx = 0;
						if (tx >= int(width)) tx = int(width) - 1;
						auto ty = basey + int(cy);
						if (ty < 0) ty = 0;
						if (ty >= int(height)) ty = int(height) - 1;
						sum += oldimg[tx + ty * width] * mat[cy][cx];
					}
				pixels[x + y * width] = pixel_type(sum);
			}
		for (int y = int(height) - int(halfh); y < int(height); y++)
			for (size_t x = 0; x < width; x++)
			{ // bottom
				auto basey = y - int(halfh);
				auto basex = int(x) - int(halfw);
				auto sum = DecimalType<pixel_type>(nullvalue);
				for (size_t cy = 0; cy < mat.GetRows(); cy++)
					for (size_t cx = 0; cx < mat.GetCols(); cx++)
					{
						auto tx = basex + int(cx);
						if (tx < 0) tx = 0;
						if (tx >= int(width)) tx = int(width) - 1;
						auto ty = basey + int(cy);
						if (ty < 0) ty = 0;
						if (ty >= int(height)) ty = int(height) - 1;
						sum += oldimg[tx + ty * width] * mat[cy][cx];
					}
				pixels[x + y * width] = pixel_type(sum);
			}
		for (int y = int(halfh); y < int(height) - int(halfh); y++)
			for (size_t x = 0; x < halfw; x++)
			{ // left
				auto basey = y - int(halfh);
				auto basex = int(x) - int(halfw);
				auto sum = DecimalType<pixel_type>(nullvalue);
				for (size_t cy = 0; cy < mat.GetRows(); cy++)
					for (size_t cx = 0; cx < mat.GetCols(); cx++)
					{
						auto tx = basex + int(cx);
						if (tx < 0) tx = 0;
						if (tx >= int(width)) tx = int(width) - 1;
						auto ty = basey + int(cy);
						if (ty < 0) ty = 0;
						if (ty >= int(height)) ty = int(height) - 1;
						sum += oldimg[tx + ty * width] * mat[cy][cx];
					}
				pixels[x + y * width] = pixel_type(sum);
			}
		for (int y = int(halfh); y < int(height) - int(halfh); y++)
			for (int x = int(width) - int(halfw); x < int(width); x++)
			{ // right
				auto basey = y - int(halfh);
				auto basex = x - int(halfw);
				auto sum = DecimalType<pixel_type>(nullvalue);
				for (size_t cy = 0; cy < mat.GetRows(); cy++)
					for (size_t cx = 0; cx < mat.GetCols(); cx++)
					{
						auto tx = basex + int(cx);
						if (tx < 0) tx = 0;
						if (tx >= int(width)) tx = int(width) - 1;
						auto ty = basey + int(cy);
						if (ty < 0) ty = 0;
						if (ty >= int(height)) ty = int(height) - 1;
						sum += oldimg[tx + ty * width] * mat[cy][cx];
					}
				pixels[x + y * width] = pixel_type(sum);
			}
		for (int y = int(halfh); y < int(height) - int(halfh); y++)
			for (int x = int(halfw); x < int(width) - int(halfw); x++)
			{
				auto basey = y - int(halfh);
				auto basex = x - int(halfw);
				auto sum = DecimalType<pixel_type>(nullvalue);
				for (size_t cy = 0; cy < mat.GetRows(); cy++)
					for (size_t cx = 0; cx < mat.GetCols(); cx++)
					{
						sum += oldimg[basex + cx + (basey + cy) * width] * mat[cy][cx];
					}
				pixels[x + y * width] = pixel_type(sum);
			}
	}

	/*! Gaussian blur
	 * \throws	ExceptionDomain	null or negative sigma
	 * \throws	ExceptionDimension	matrix bigger than image
	 * \param[in]	sigma	the standard deviation of the Gaussian
	 */
	template<typename T> void Image<T>::GaussianBlur(double sigma)
	{
        // Backup... Just in case...
        /*
		auto gauss = MatrixDouble::NewGaussianLine(sigma);
		gauss.NormalizeForConvolution();
		Convolve(gauss); // may throw
		gauss.Transpose();
		Convolve(gauss); // may throw
        */
        
        auto kernel = MatrixDouble::NewGaussianLine(sigma);
        
        kernel.NormalizeForConvolution();
        
        const auto kernel_size(kernel.GetCols());
        const auto radius(kernel_size / 2);
        std::vector<double> coeffs(radius + 1);
        auto index = size_t(0);
        
        for (auto k = radius; k < kernel_size; ++k)
        {
            coeffs[index] = kernel[0][k];
            ++index;
        }
        
        /********* "Horizontal" convolution *********/
        
        index = 0;
        auto left_bound = size_t(0);
        auto right_bound = width - 1;
        
        std::vector<pixel_type> tmp_row(width, pixels.front());
        auto back_shifted_index = size_t(0);
        
        for (auto r = 0; r < height; ++r)
        {
            auto tmp_id = 0;
            
            // Left margin
            
            for (auto c = size_t(0); c < radius; ++c)
            {
                auto weight = coeffs[0];
                auto local_cumul = pixels[index] * weight;
                
                auto index_left = int(index); // Because of "--"
                auto index_right = index_left;
                
                for (auto offset = size_t(1); offset <= radius; ++offset)
                {
                    --index_left;
                    index_left = Max(int(left_bound), index_left);
                    ++index_right;
                    weight = coeffs[offset];
                    
                    local_cumul += (pixels[index_left] + pixels[index_right]) * weight;
                }
                
                tmp_row[tmp_id] = pixel_type(local_cumul);
                ++tmp_id;
                ++index;
            }
            
            left_bound += width;
            
            // Central region
            
            for (auto c = radius; c < width - radius; ++c)
            {
                auto weight = coeffs[0];
                auto local_cumul = pixels[index] * weight;
                
                auto index_left = int(index); // Because of "--"
                auto index_right = index_left;
                
                for (auto offset = size_t(1); offset <= radius; ++offset)
                {
                    --index_left;
                    ++index_right;
                    weight = coeffs[offset];
                    
                    local_cumul += (pixels[index_left] + pixels[index_right]) * weight;
                }
                
                tmp_row[tmp_id] = pixel_type(local_cumul);
                ++tmp_id;
                ++index;
            }
            
            // Right margin
            
            for (auto c = width - radius; c < width; ++c)
            {
                auto weight = coeffs[0];
                auto local_cumul = pixels[index] * weight;
                
                auto index_left = int(index); // Because of "--"
                auto index_right = index_left;
                
                for (auto offset = 1; offset <= radius; ++offset)
                {
                    --index_left;
                    ++index_right;
                    index_right = Min(int(right_bound), int(index_right));
                    weight = coeffs[offset];
                    
                    local_cumul += (pixels[index_left] + pixels[index_right]) * weight;
                }
                
                tmp_row[tmp_id] = pixel_type(local_cumul);
                ++tmp_id;
                ++index;
            }
            
            right_bound += width;
            
            // Finalize current row
            
            for (auto k = 0; k < width; ++k)
            {
                pixels[back_shifted_index] = tmp_row[k];
                ++back_shifted_index;
            }
        }
        
        /********* "Vertical" convolution *********/
        
        std::vector<size_t> indices(kernel_size, 0);
        std::vector<pixel_type> tmp_col(height, pixels.front());
        auto weight = 0.0;
        
        for (auto c = 0; c < width; ++c)
        {
            index = 0;
            
            // Upper margin
            
            for (auto k = size_t(0); k <= radius; ++k)
                indices[k] = c;
            
            for (auto k = radius + 1; k < kernel_size; ++k)
                indices[k] = indices[k - 1] + width;
            
            for (auto r = size_t(0); r < radius; ++r)
            {
                weight = coeffs[0];
                auto local_cumul = pixels[indices[radius]] * weight;
                
                for (auto offset = 1; offset <= radius; ++offset)
                {
                    auto id_upper = indices[radius - offset];
                    auto id_lower = indices[radius + offset];
                    
                    weight = coeffs[offset];
                    local_cumul += (pixels[id_upper] + pixels[id_lower]) * weight;
                }
                
                tmp_col[index] = pixel_type(local_cumul);
                ++index;
                
                for (auto k = 0; k < kernel_size - 1; ++k)
                    indices[k] = indices[k + 1];
                
                indices[kernel_size - 1] += width;
                
            }
            
            // Central region
            
            for (auto r = radius; r < height - radius; ++r)
            {
                weight = coeffs[0];
                auto local_cumul = pixels[indices[radius]] * weight;
                
                for (auto offset = size_t(1); offset <= radius; ++offset)
                {
                    auto id_upper = indices[radius - offset];
                    auto id_lower = indices[radius + offset];
                    
                    weight = coeffs[offset];
                    local_cumul += (pixels[id_upper] + pixels[id_lower]) * weight;
                }
                
                tmp_col[index] = pixel_type(local_cumul);
                ++index;
                
                for (auto k = 0; k < kernel_size - 1; ++k)
                    indices[k] = indices[k + 1];
                
                indices[kernel_size - 1] += width;
            }
            
            // Lower margin
            
            indices[kernel_size - 1] = c + (height - 1) * width;
            
            for (auto r = height - radius; r < height; ++r)
            {
                weight = coeffs[0];
                auto local_cumul = pixels[indices[radius]] * weight;
                
                for (auto offset = size_t(1); offset <= radius; ++offset)
                {
                    auto id_upper = indices[radius - offset];
                    auto id_lower = indices[radius + offset];
                    
                    weight = coeffs[offset];
                    local_cumul += (pixels[id_upper] + pixels[id_lower]) * weight;
                }
                
                tmp_col[index] = pixel_type(local_cumul);
                ++index;
                
                for (auto k = size_t(0); k < kernel_size - 1; ++k)
                    indices[k] = indices[k + 1];
            }
            
            // Finalize current column
            
            index = c;
            
            for (auto k = size_t(0); k < height; ++k)
            {
                pixels[index] = tmp_col[k];
                index += width;
            }
        }
	}
}

template<typename T1, typename T2> crn::Image<crn::SumType<typename std::common_type<T1, T2>::type>> operator+(const crn::Image<T1> &i1, const crn::Image<T2> &i2)
{
	if ((i1.GetWidth() != i2.GetWidth()) || (i1.GetHeight() != i2.GetHeight()))
		throw crn::ExceptionDimension("operator+(Image, Image): images do not have the same sizes.");
	using R = crn::SumType<typename std::common_type<T1, T2>::type>;
	auto res = crn::Image<R>{i1.GetWidth(), i1.GetHeight()};
	for (auto i : crn::Range(i1))
		res.At(i) = R(i1.At(i)) + R(i2.At(i));
	return res;
}

template<typename T1, typename T2> crn::Image<crn::DiffType<typename std::common_type<T1, T2>::type>> operator-(const crn::Image<T1> &i1, const crn::Image<T2> &i2)
{
	if ((i1.GetWidth() != i2.GetWidth()) || (i1.GetHeight() != i2.GetHeight()))
		throw crn::ExceptionDimension("operator-(Image, Image): images do not have the same sizes.");
	using R = crn::DiffType<typename std::common_type<T1, T2>::type>;
	auto res = crn::Image<R>{i1.GetWidth(), i1.GetHeight()};
	for (auto i : crn::Range(i1))
		res.At(i) = R(i1.At(i)) - R(i2.At(i));
	return res;
}

template<typename T1, typename T2> crn::Image<crn::SumType<typename std::common_type<T1, T2>::type>> operator*(const crn::Image<T1> &i1, const crn::Image<T2> &i2)
{
	if ((i1.GetWidth() != i2.GetWidth()) || (i1.GetHeight() != i2.GetHeight()))
		throw crn::ExceptionDimension("operator*(Image, Image): images do not have the same sizes.");
	using R = crn::SumType<typename std::common_type<T1, T2>::type>;
	auto res = crn::Image<R>{i1.GetWidth(), i1.GetHeight()};
	for (auto i : crn::Range(i1))
		res.At(i) = R(i1.At(i)) * R(i2.At(i));
	return res;
}

template<typename T> crn::Image<crn::SumType<typename std::common_type<T, double>::type>> operator*(double d, const crn::Image<T> &i)
{
	using R = crn::SumType<typename std::common_type<T, double>::type>;
	auto res = crn::Image<R>{i.GetWidth(), i.GetHeight()};
	for (auto tmp : crn::Range(i))
		res.At(tmp) = R(i.At(tmp)) * R(d);
	return res;
}

template<typename T> crn::Image<crn::SumType<typename std::common_type<T, double>::type>> operator*(const crn::Image<T> &i, double d)
{
	return d * i;
}

template<typename T1, typename T2> crn::Image<crn::SumType<typename std::common_type<T1, T2>::type>> operator/(const crn::Image<T1> &i1, const crn::Image<T2> &i2)
{
	if ((i1.GetWidth() != i2.GetWidth()) || (i1.GetHeight() != i2.GetHeight()))
		throw crn::ExceptionDimension("operator/(Image, Image): images do not have the same sizes.");
	using R = crn::SumType<typename std::common_type<T1, T2>::type>;
	auto res = crn::Image<R>{i1.GetWidth(), i1.GetHeight()};
	for (auto i : Range(i1))
		res.At(i) = R(i1.At(i)) / R(i2.At(i));
	return res;
}
	
namespace crn
{
	/**************************************************************************************
	 * Characterization
	 *************************************************************************************/

	/*! Is the image binary (black & white)?
	 * \param[in]	img	an image
	 * \return	true if the image contains one or two values only, false else
	 */
	template<typename T> bool IsBitonal(const Image<T> &img)
	{
		auto values = std::set<T>{};
		for (const auto &p : img)
		{
			values.insert(p);
			if (values.size() > 2)
				return false;
		}
		return true;
	}

	/*! 
	 * \param[in]	img	an image
	 * \return	min and max pixel values
	 */
	template<typename T, typename CMP> std::pair<T, T> MinMax(const Image<T> &img, CMP cmp)
	{
		auto mM = std::minmax_element(img.begin(), img.end(), cmp);
		return std::make_pair(*mM.first, *mM.second);
	}

	/*! Estimates the ideal crop for the image
	 *
	 * \param[in]	img	an image
	 * \param[in]	bgval	the color value of the background
	 * \return	a rectangle that removes background flat border
	 */
	template<typename T> Rect AutoCrop(const Image<T> &img, const T &bgval)
	{
		auto bbox = img.GetBBox();
		// left
		for (int x = bbox.GetLeft(); x <= bbox.GetRight(); ++x)
		{
			bool bl = false;
			for (int y = bbox.GetTop(); y <= bbox.GetBottom(); ++y)
				if (img.At(x, y) != bgval)
				{
					bl = true;
					break;
				}
			if (bl)
			{
				bbox.SetLeft(x);
				break;
			}
		}
		// right
		for (int x = bbox.GetRight(); x >= bbox.GetLeft(); --x)
		{
			bool bl = false;
			for (int y = bbox.GetTop(); y <= bbox.GetBottom(); ++y)
				if (img.At(x, y) != bgval)
				{
					bl = true;
					break;
				}
			if (bl)
			{
				bbox.SetRight(x);
				break;
			}
		}
		// top
		for (int y = bbox.GetTop(); y <= bbox.GetBottom(); ++y)
		{
			bool bl = false;
			for (int x = bbox.GetLeft(); x <= bbox.GetRight(); ++x)
				if (img.At(x, y) != bgval)
				{
					bl = true;
					break;
				}
			if (bl)
			{
				bbox.SetTop(y);
				break;
			}
		}
		// bottom
		for (int y = bbox.GetBottom(); y >= bbox.GetTop(); --y)
		{
			bool bl = false;
			for (int x = bbox.GetLeft(); x <= bbox.GetRight(); ++x)
				if (img.At(x, y) != bgval)
				{
					bl = true;
					break;
				}
			if (bl)
			{
				bbox.SetBottom(y);
				break;
			}
		}
		return bbox;
	}

	/*! Creates a new image as the ideal crop for the image
	 *
	 * \param[in]	img	an image
	 * \param[in]	bgval	the color value of the background
	 * \return	a new image
	 */
	template<typename T> Image<T> MakeAutoCrop(const Image<T> &img, const T &bgval)
	{
		return Image<T>(img, AutoCrop(img, bgval));
	}

	/*! Best match between two images
	 *
	 * \warning	Uses FFT, the memory consumption might be huge!
	 *
	 * \param[in]	img1	first image
	 * \param[in]	img2	other image
	 * \param[in]	fill1	the fill color for the first image (to grow to power of 2 dimensions)
	 * \param[in]	fill2	the fill color for the second image (to grow to power of 2 dimensions)
	 * \return	the best position so that sum(difference(first[x, y] - second[x + p.X, y + p.Y])) is minimal
	 */
	template<typename T, typename Y> Point2DInt CrossCorrelation(const Image<T> &img1, const Image<Y> &img2, T fill1, Y fill2)
	{
		// find nearest power of 2 dimensions
		auto w = crn::Max(img1.GetWidth(), img2.GetWidth());
		auto h = crn::Max(img1.GetHeight(), img2.GetHeight());
		double logs = log2(int(w));
		while (logs != ceil(logs))
		{
			w += 1;
			logs = log2(int(w));
		}
		if (w < 2) w = 2;
		logs = log2(int(h));
		while (logs != ceil(logs))
		{
			h += 1;
			logs = log2(int(h));
		}
		if (h < 2) h = 2;
		// compute cross correlation
		auto c1 = MatrixComplex(h, w, std::complex<double>(fill1));
		for (size_t r = 0; r < img1.GetHeight(); ++r)
			for (size_t c = 0; c < img1.GetWidth(); ++c)
				c1[r][c] = std::complex<double>(img1.At(c, r));
		c1.FFT(true);
		auto c2 = MatrixComplex(h, w, std::complex<double>(fill2));
		for (size_t r = 0; r < img2.GetHeight(); ++r)
			for (size_t c = 0; c < img2.GetWidth(); ++c)
				c2[r][c] = std::complex<double>(img2.At(c, r));
		c2.FFT(true);
		for (size_t r = 0; r < h; ++r)
			for (size_t c = 0; c < w; ++c)
				c1[r][c] *= std::conj(c2[r][c]);
		c2 = MatrixComplex(1, 1); // free memory
		c1.FFT(true);
		// find best match
		Point2DInt p;
		double maxc = 0;
		for (size_t r = 0; r < h; ++r)
			for (size_t c = 0; c < w; ++c)
			{
				double corr = std::norm(c1[r][c]);
				if (corr > maxc)
				{
					p.X = int(c);
					p.Y = int(r);
					maxc = corr;
				}
			}
		int hw = (int)w/2;
		int hh = (int)h/2;
		if (p.X >= hw)
			p.X = (p.X % hw) - hw;
		if (p.Y >= hh)
			p.Y = (p.Y % hh) - hh;
		return p;
	}

	/**************************************************************************************
	 * Transformation
	 *************************************************************************************/

	/*! \return	The summed area table of the image */
	template<typename T> SummedAreaTable<SumType<T>> MakeSummedAreaTable(const Image<T> &img)
	{
		using SumType = SumType<T>;
		auto sum = SummedAreaTable<SumType>(img.GetWidth(), img.GetHeight());
		// first element
		sum.SetValue(0, 0, SumType(img.At(0)));
		// first line
		for (size_t x = 1; x < img.GetWidth(); ++x)
			sum.SetValue(x, 0, sum.GetValue(x - 1, 0) + SumType(img.At(x)));
		// first column
		for (size_t y = 1; y < img.GetHeight(); ++y)
			sum.SetValue(0, y, sum.GetValue(0, y - 1) + SumType(img.At(0, y)));
		// table: I(x,y) = i(x,y) + I(x-1,y) + I(x,y-1) - I(x-1,y-1)
		for (size_t y = 1; y < img.GetHeight(); ++y)
			for (size_t x = 1; x < img.GetWidth(); ++x)
				sum.SetValue(x, y, SumType(img.At(x, y)) + sum.GetValue(x - 1, y) + sum.GetValue(x, y - 1) - sum.GetValue(x - 1, y - 1));
		return sum;
	}

	namespace impl
	{
		/*! \brief Copies a row with a shift
		 * \param[in]	dest	the image to modify
		 * \param[in]	src	the source image
		 * \param[in]	row	the ordinate of the row to copy
		 * \param[in]	offset	the shift
		 * \param[in]	prevWeight	the weight of the previous pixel (for smoothing)
		 * \param[in]	bgColor	the fill color for the corners
		 */
		template<typename T> void ShiftCopyRow(Image<T> &dest, const Image<T> &src, size_t row, int offset, double prevWeight, const T &bgColor)
		{
			size_t rowoffset = row * dest.GetWidth();

			// fill the possible left gap
			for (int x = 0; x < offset; ++x)
				dest.At(x + rowoffset) = bgColor;

			auto oldPrevPix = DecimalType<T>(bgColor) * prevWeight;
			auto srcrowoffset = row * src.GetWidth();
			for (size_t x = 0; x < src.GetWidth(); ++x)
			{ // scan original pixels
				T pix = src.At(x + srcrowoffset); // do not use auto because of vector<bool>!
				auto prevPix = DecimalType<T>(pix) * prevWeight;
				// compute value to copy
				auto tmp = DecimalType<T>(pix);
				tmp += oldPrevPix;
				tmp -= prevPix;
				pix = T(tmp);
				// copy the new value
				if ((x + offset >= 0) && (x + offset < dest.GetWidth()))
				{
					dest.At(x + offset + rowoffset) = pix;
				}
				// remember the weighted previous pixel
				oldPrevPix = prevPix;
			}
			// fill the possible right gap
			size_t x = src.GetWidth() + offset;
			if (x < dest.GetWidth())
			{
				auto prevPix = DecimalType<T>(bgColor) * prevWeight;
				auto pix = bgColor;
				auto tmp = DecimalType<T>(pix);
				tmp += oldPrevPix;
				tmp -= prevPix;
				pix = T(tmp);
				dest.At(x + rowoffset) = pix;
			}
			for (++x; x < dest.GetWidth(); ++x)
			{
				dest.At(x + rowoffset) = bgColor;
			}
		}

		/*! \brief Copies a column with a shift
		 * \param[in]	dest	the image to modify
		 * \param[in]	src	the source image
		 * \param[in]	col	the abscissa of the column to copy
		 * \param[in]	offset	the shift
		 * \param[in]	prevWeight	the weight of the previous pixel (for smoothing)
		 * \param[in]	bgColor	the fill color for the corners
		 */
		template<typename T> void ShiftCopyColumn(Image<T> &dest, const Image<T> &src, size_t col, int offset, double prevWeight, const T &bgColor)
		{
			// fill the possible upper gap
			for (int y = 0; y < offset; ++y)
			{
				dest.At(col, y) = bgColor;
			}

			auto oldPrevPix = DecimalType<T>(bgColor) * prevWeight;
			for (size_t y = 0; y < src.GetHeight(); ++y)
			{ // scan original pixels
				T pix = src.At(col, y); // do not use auto because of vector<bool>!
				auto prevPix = DecimalType<T>(pix) * prevWeight;
				// compute value to copy
				auto tmp = DecimalType<T>(pix);
				tmp -= prevPix;
				tmp += oldPrevPix;
				pix = T(tmp);
				// copy the new value
				if ((y + offset >= 0) && (y + offset < dest.GetHeight()))
				{
					dest.At(col, y + offset) = pix;
				}
				// remember the weighted previous pixel
				oldPrevPix = prevPix;
			}
			// fill the possible bottom gap
			size_t y = src.GetHeight() + offset;
			if (y < dest.GetHeight())
			{
				auto prevPix = DecimalType<T>(bgColor) * prevWeight;
				auto pix = bgColor;
				auto tmp = DecimalType<T>(pix);
				tmp += oldPrevPix;
				tmp -= prevPix;
				pix = T(tmp);
				dest.At(col, y) = pix;
			}
			for (++y; y < dest.GetHeight(); ++y)
			{
				dest.At(col, y) = bgColor;
			}
		}

		/*! Performs a fast small rotation
		 * \param[in]	angle	the angle of the rotation
		 * \param[in]	bgColor	the fill color for the corners
		 * \return	a new rotated table
		 */
		template<typename T> Image<T> MakeSmallRotation(const Image<T> &img, const Angle<Degree> &angle, const T &bgColor)
		{
			if (angle.value == 0.0)
			{ // if no rotation, simply copy
				return img;
			}
			const auto radAngle = angle.Get<Radian>();
			const auto rotCos = cos(radAngle);
			const auto rotSin = sin(radAngle);
			const auto rotTan = tan(radAngle / 2.0);

			// first shear (horizontal)
			auto shear1 = Image<T>(img.GetWidth() + int(double(img.GetHeight()) * Abs(rotTan)), img.GetHeight());
			for (size_t y = 0; y < shear1.GetHeight(); ++y)
			{
				auto shear = 0.0;
				if (rotTan >= 0.0)
				{ // angle >= 0
					shear = (double(y) + 0.5) * rotTan;
				}
				else
				{ // angle < 0
					shear = (double(y) - double(shear1.GetHeight()) + 0.5) * rotTan;
				}
				ShiftCopyRow(shear1, img, y, int(floor(shear)), shear - floor(shear), bgColor);
			}

			// second shear (vertical)
			auto shear2 = Image<T>(shear1.GetWidth(), size_t(double(img.GetWidth()) * Abs(rotSin) + double(img.GetHeight()) * rotCos) + 1);
			auto shear = 0.0;
			if (rotSin > 0.0)
			{ // angle > 0
				shear = (double(img.GetWidth()) - 1.0) * rotSin;
			}
			else
			{ // angle <= 0
				shear = double(shear1.GetWidth() - img.GetWidth()) * rotSin;
			}
			for (size_t x = 0; x < shear2.GetWidth(); ++x, shear -= rotSin)
			{
				ShiftCopyColumn(shear2, shear1, x, int(floor(shear)), shear - floor(shear), bgColor);
			}
			shear1 = Image<T>{}; // free a bit of memory

			// third sear (horizontal)
			auto shear3 = Image<T>(size_t(double(img.GetHeight()) * Abs(rotSin) + double(img.GetWidth()) * rotCos) + 1, shear2.GetHeight());
			if (rotSin >= 0.0)
			{ // angle >= 0
				shear = (1.0 - double(img.GetWidth())) * rotSin * rotTan;
			}
			else
			{
				shear = ((1.0 - double(img.GetWidth())) * rotSin + 1.0 - double(shear3.GetHeight())) * rotTan;
			}
			for (size_t y = 0; y < shear3.GetHeight(); ++y, shear += rotTan)
			{
				ShiftCopyRow(shear3, shear2, y, int(floor(shear)), shear - floor(shear), bgColor);
			}

			return shear3;
		}
	}

	/*! Creates a rotated version of the image
	 * \param[in]	img	the image to rotate
	 * \param[in]	angle	the rotation angle
	 * \param[in]	bgColor	the fill color
	 * \return	a new image
	 */
	template<typename T> Image<T> MakeRotation(const Image<T> &img, const Angle<Degree> &angle, const T &bgColor)
	{
		auto rot = angle;
		while (rot.value >= 360.0)
			rot.value -= 360.0;
		while (rot.value < 0.0)
			rot.value += 360.0;
		if (rot.value == 0)
			return img;
		else if (rot.value == 90)
			return Make90Rotation(img);
		else if (rot.value == 180)
			return Make180Rotation(img);
		else if (rot.value == 270)
			return Make270Rotation(img);

		auto midimg = &img;
		auto tmpbuff = Image<T>{};
		if (rot.value > 225.0)
		{ // in ]225, 360[, rotate 270 then small rotation
			tmpbuff = Make270Rotation(img);
			midimg = &tmpbuff;
			rot.value -= 270.0;
		}
		else if (rot.value > 135.0)
		{ // in ]135, 225], rotate 180 then small rotation
			tmpbuff = Make180Rotation(img);
			midimg = &tmpbuff;
			rot.value -= 180.0;
		}
		else if (rot.value > 45.0)
		{ // in ]45, 135], rotate 90 then small rotation
			tmpbuff = Make90Rotation(img);
			midimg = &tmpbuff;
			rot.value -= 90.0;
		}
		// do the final rotation
		return impl::MakeSmallRotation(*midimg, rot, bgColor);
	}

	/*! Creates a rotated version of the image
	 * \param[in]	img	the image to rotate
	 * \return	a new image
	 */
	template<typename T> Image<T> Make90Rotation(const Image<T> &img)
	{
		auto newi = Image<T>(img.GetHeight(), img.GetWidth());
		FOREACHPIXEL(x, y, img)
			newi.At(y, img.GetWidth() - 1 - x) = img.At(x, y);
		return newi;
	}

	/*! Creates a rotated version of the image
	 * \param[in]	img	the image to rotate
	 * \return	a new image
	 */
	template<typename T> Image<T> Make180Rotation(const Image<T> &img)
	{
		auto newi = Image<T>(img.GetWidth(), img.GetHeight());
		const auto size = img.Size();
		for (size_t tmp = 0; tmp < size; ++tmp)
			newi.At(size - tmp) = img.At(tmp);
		return newi;
	}

	/*! Creates a rotated version of the image
	 * \param[in]	img	the image to rotate
	 * \return	a new image
	 */
	template<typename T> Image<T> Make270Rotation(const Image<T> &img)
	{
		auto newi = Image<T>(img.GetHeight(), img.GetWidth());
		FOREACHPIXEL(x, y, img)
			newi.At(img.GetHeight() - 1 - y, x) = img.At(x, y);
		return newi;
	}


}

#endif
