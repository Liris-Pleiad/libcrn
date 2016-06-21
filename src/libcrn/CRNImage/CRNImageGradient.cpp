/* Copyright 2006-2016 Yann LEYDIER, INSA-Lyon
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
 * file: CRNImageGradient.cpp
 * \author Yann LEYDIER
 */

#include <CRNImage/CRNImageGradient.h>
#include <CRNStringUTF8.h>
#include <CRNi18n.h>

using namespace crn;

/*! 
 * Computes the module significance threshold 
 *
 * \param[in]	img	an image representing the same view
 * \return	the new threshold value
 */
unsigned int ImageGradient::AutoMinModule(const ImageGray &img)
{
	if ((img.GetWidth() != GetWidth()) || (img.GetHeight() != GetHeight()))
	{
		throw ExceptionDimension(
				StringUTF8("unsigned int ImageGradient::AutoMinModule(const ImageGray &img)") + 
				_("Wrong image dimensions."));
	}

	auto S1 = 0.0, S2 = 0.0, C1 = 0.0, C2 = 0.0;
	auto max = MinMax(img).second;

	for (auto tmp : Range(img))
		{
			S1 += double(At(tmp).rho) * double(img.At(tmp));
			S2 += double(At(tmp).rho) * double(max - img.At(tmp));
			C1 += double(img.At(tmp));
			C2 += double(max - img.At(tmp));
		}

	auto moy1 = S1 / C1;
	auto moy2 = S2 / C2;
	thresh = (int)ceil((moy1 + moy2) / 2);
	
	return thresh;
}

/*****************************************************************************/
/*!
 * Converts to a gray image
 *
 * \return	the newly created image
 */
ImageGray ImageGradient::MakeImageGray() const
{
	ImageGray img = ImageGray(GetWidth(), GetHeight());
	for (auto tmp : Range(img))
		if (At(tmp).rho < thresh)
			img.At(tmp) = 127;
		else
			img.At(tmp) = At(tmp).theta.value;
	return img;
}

/*****************************************************************************/
/*!
 * Converts to a rgb image
 *
 * \param[in]	thres	shall we threshold the output?
 * \return	the newly created image
 */
ImageRGB ImageGradient::MakeImageRGB(bool thres) const
{
	auto min = std::numeric_limits<unsigned int>::max();
	auto max = std::numeric_limits<unsigned int>::min();
	for (auto tmp : Range(*this))
	{
		if (At(tmp).rho > max) max = At(tmp).rho;
		if (At(tmp).rho < min) min = At(tmp).rho;
	}
	max -= min;
	if (max == 0) max = 1;
	ImageRGB img = ImageRGB(GetWidth(), GetHeight());
	for (auto tmp : Range(img))
	{
		pixel::HSV p;
		p.h = At(tmp).theta.value;
		p.s = 255;
		if (thres && (At(tmp).rho < thresh))
			p.v = 0;
		else
			p.v = uint8_t(((At(tmp).rho - min) * 255) / max);
		img.At(tmp) = p;
	}
	return img;
}

/*! Creates an image representing the curvature of the gradients.
 *
 * A null curvature is assigned to pixels with a gradient module lower than the threshold.
 *
 * \return	an image with gray levels representing the curvature (0 = straight, 255 = half turn)
 */
ImageGray ImageGradient::MakeCurvature() const
{
	auto ans = ImageGray(GetWidth(), GetHeight(), uint8_t(0));
	for (size_t y = 2; y < GetHeight() - 2; y++)
		for (size_t x = 2; x < GetWidth() - 2; x++)
		{
			if (!IsSignificant(x, y))
			{
				ans.At(x, y) = 0;
				continue;
			}
			auto a = At(x, y).theta;
			const auto gx = a.Cos();
			const auto gy = a.Sin();
			// orthogonal, on the right
			auto px = double(x) - gy;
			auto py = double(y) + gx;
			auto cosine = 0.0, sine = 0.0;
			if (px == floor(px))
			{
				if (py == floor(py))
				{
					a = At(size_t(px), size_t(py)).theta;
					cosine = a.Cos();
					sine = a.Sin();
				}
				else
				{
					auto frac = py - floor(py);
					a = At(size_t(px), size_t(floor(py))).theta;
					cosine = a.Cos() * (1 - frac);
					sine = a.Sin() * (1 - frac);
					a = At(size_t(px), size_t(floor(py)) + 1).theta;
					cosine += a.Cos() * frac;
					sine += a.Sin() * frac;
				}
			}
			else
			{
				if (py == floor(py))
				{
					auto frac = px - floor(px);
					a = At(size_t(floor(px)), size_t(py)).theta;
					cosine = a.Cos() * (1 - frac);
					sine = a.Sin() * (1 - frac);
					a = At(size_t(floor(px)) + 1, size_t(py)).theta;
					cosine += a.Cos() * frac;
					sine += a.Sin() * frac;
				}
				else
				{
					auto fracx = px - floor(px);
					auto fracy = py - floor(py);
					a = At(size_t(floor(px)), size_t(floor(py))).theta;
					cosine = a.Cos() * (1 - fracx) * (1 - fracy);
					sine = a.Sin() * (1 - fracx) * (1 - fracy);
					a = At(size_t(floor(px)), size_t(floor(py)) + 1).theta;
					cosine += a.Cos() * (1 - fracx) * fracy;
					sine += a.Sin() * (1 - fracx) * fracy;
					a = At(size_t(floor(px)) + 1, size_t(floor(py)) + 1).theta;
					cosine += a.Cos() * fracx * fracy;
					sine += a.Sin() * fracx * fracy;
					a = At(size_t(floor(px)) + 1, size_t(floor(py))).theta;
					cosine += a.Cos() * fracx * (1 - fracy);
					sine += a.Sin() * fracx * (1 - fracy);
				}
			}
			const auto angle1 = Angle<ByteAngle>::Atan(sine, cosine);
			//uint8_t dist1 = AngularDistance<ByteAngle>(angle1, At(size_t(px), size_t(py)));
			// orthogonal, on the left
			px = double(x) + gy;
			py = double(y) - gx;
			cosine = 0.0;
			sine = 0.0;
			if (px == floor(px))
			{
				if (py == floor(py))
				{
					a = At(size_t(px), size_t(py)).theta;
					cosine = a.Cos();
					sine = a.Sin();
				}
				else
				{
					double frac = py - floor(py);
					a = At(size_t(px), size_t(floor(py))).theta;
					cosine = a.Cos() * (1 - frac);
					sine = a.Sin() * (1 - frac);
					a = At(size_t(px), size_t(floor(py)) + 1).theta;
					cosine += a.Cos() * frac;
					sine += a.Sin() * frac;
				}
			}
			else
			{
				if (py == floor(py))
				{
					double frac = px - floor(px);
					a = At(size_t(floor(px)), size_t(py)).theta;
					cosine = a.Cos() * (1 - frac);
					sine = a.Sin() * (1 - frac);
					a = At(size_t(floor(px)) + 1, size_t(py)).theta;
					cosine += a.Cos() * frac;
					sine += a.Sin() * frac;
				}
				else
				{
					double fracx = px - floor(px);
					double fracy = py - floor(py);
					a = At(size_t(floor(px)), size_t(floor(py))).theta;
					cosine = a.Cos() * (1 - fracx) * (1 - fracy);
					sine = a.Sin() * (1 - fracx) * (1 - fracy);
					a = At(size_t(floor(px)), size_t(floor(py)) + 1).theta;
					cosine += a.Cos() * (1 - fracx) * fracy;
					sine += a.Sin() * (1 - fracx) * fracy;
					a = At(size_t(floor(px)) + 1, size_t(floor(py)) + 1).theta;
					cosine += a.Cos() * fracx * fracy;
					sine += a.Sin() * fracx * fracy;
					a = At(size_t(floor(px)) + 1, size_t(floor(py))).theta;
					cosine += a.Cos() * fracx * (1 - fracy);
					sine += a.Sin() * fracx * (1 - fracy);
				}
			}
			const auto angle2 = Angle<ByteAngle>::Atan(sine, cosine);
			//uint8_t dist2 = AngularDistance<ByteAngle>(angle2, thetapixels[size_t(px) + size_t(py) * GetWidth()]);
			auto dist = AngularDistance<ByteAngle>(angle1, angle2);
			ans.At(x, y) = (uint8_t)Twice((int)dist);
		}
	return ans;
}

/*! \return	a mask of the gradients' module */
ImageBW ImageGradient::MakeMask() const
{
	auto res = ImageBW{GetWidth(), GetHeight(), pixel::BWBlack};
	for (auto tmp : Range(res))
		if (IsSignificant(tmp))
			res.At(tmp) = pixel::BWWhite;
	return res;
}

/*****************************************************************************/
/*!
 * Estimates the mean character width 
 *
 * \return	the estimation of the mean character width
 */
double ImageGradient::GetHRun() const noexcept
{
	const auto GR_WAIT = 1;
	const auto GR_WAITLEFT = 2;
	const auto GR_WAITRIGHT = 3;
	const auto GR_LEFT = 4;
	const auto GR_RIGHT = 5;
	auto mode = 0;
	std::vector<size_t> rightlist, leftlist;
	size_t acc;
	for (size_t y = 0; y < GetHeight(); ++y)
	{
		auto angle = At(0 ,y).theta;
		acc = 0;
		if (At(0, y) < thresh)
		{
			mode = GR_WAIT;
		}
		else if (AngularDistance<ByteAngle>(angle, Angle<ByteAngle>::RIGHT()) < 16)
		{
			mode = GR_WAITRIGHT;
		}
		else if (AngularDistance<ByteAngle>(angle, Angle<ByteAngle>::LEFT()) < 16)
		{
			mode = GR_WAITLEFT;
		}
		else
		{
			mode = GR_WAIT;
		}
		for (size_t x = 1; x < GetWidth(); x++)
		{
			auto angle = At(x, y).theta;
			auto pix = GR_WAIT;
			if (At(x, y).rho < thresh)
			{
				pix = GR_WAIT;
			}
			else if (AngularDistance<ByteAngle>(angle, Angle<ByteAngle>::RIGHT()) < 16)
			{
				pix = GR_RIGHT;
			}
			else if (AngularDistance<ByteAngle>(angle, Angle<ByteAngle>::LEFT()) < 16)
			{
				pix = GR_LEFT;
			}
			switch (mode)
			{
				case GR_WAITLEFT:
					switch (pix)
					{
						case GR_WAIT:
							mode = GR_WAIT;
							acc = 0;
							break;
						case GR_RIGHT:
							mode = GR_RIGHT;
							break;
					}
					break;
				case GR_WAITRIGHT:
					switch (pix)
					{
						case GR_WAIT:
							mode = GR_WAIT;
							acc = 0;
							break;
						case GR_LEFT:
							mode = GR_LEFT;
							break;
					}
					break;
				case GR_WAIT:
					switch (pix)
					{
						case GR_RIGHT:
							mode = GR_RIGHT;
							acc += 1;
							break;
						case GR_LEFT:
							mode = GR_LEFT;
							acc += 1;
							break;
					}
					break;
				case GR_LEFT:
					switch (pix)
					{
						case GR_WAIT:
							leftlist.push_back(acc);
							mode = GR_WAIT;
							acc = 0;
							break;
						case GR_RIGHT:
							leftlist.push_back(acc);
							mode = GR_RIGHT;
							acc = 1;
							break;
						case GR_LEFT:
							acc += 1;
							break;
					}
					break;
				case GR_RIGHT:
					switch (pix)
					{
						case GR_WAIT:
							rightlist.push_back(acc);
							mode = GR_WAIT;
							acc = 0;
							break;
						case GR_RIGHT:
							acc += 1;
							break;
						case GR_LEFT:
							rightlist.push_back(acc);
							mode = GR_LEFT;
							acc = 1;
							break;
					}
					break;
			}
		}
	}
	auto suml = 0.0, sumr = 0.0;
	for (auto & elem : leftlist)
		suml += double(elem);
	if (!leftlist.empty())
		suml /= double(leftlist.size());
	for (auto & elem : rightlist)
		sumr += double(elem);
	if (!rightlist.empty())
		sumr /= double(rightlist.size());
	return (sumr + suml);
}

/*****************************************************************************/
/*!
 * Estimates the mean character width 
 *
 * \return	the estimation of the mean character height
 */
double ImageGradient::GetVRun() const noexcept
{
const auto GR_WAIT = 1;
const auto GR_WAITTOP = 2;
const auto GR_WAITBOTTOM = 3;
const auto GR_TOP = 4;
const auto GR_BOTTOM = 5;
	auto mode = 0;
	std::vector<size_t> bottomlist, toplist;
	size_t acc;
	for (size_t x = 1; x < GetWidth(); x++)
	{
		auto angle = At(x).theta;
		acc = 0;
		if (At(x).rho < thresh)
		{
			mode = GR_WAIT;
		}
		else if (AngularDistance<ByteAngle>(angle, Angle<ByteAngle>::BOTTOM()) < 16)
		{
			mode = GR_WAITBOTTOM;
		}
		else if (AngularDistance<ByteAngle>(angle, Angle<ByteAngle>::TOP()) < 16)
		{
			mode = GR_WAITTOP;
		}
		else
		{
			mode = GR_WAIT;
		}
		for (size_t y = 0; y < GetHeight(); y++)
		{
			auto angle = At(x, y).theta;
			auto pix = GR_WAIT;
			if (At(x, y) < thresh)
			{
				pix = GR_WAIT;
			}
			else if (AngularDistance<ByteAngle>(angle, Angle<ByteAngle>::BOTTOM()) < 16)
			{
				pix = GR_BOTTOM;
			}
			else if (AngularDistance<ByteAngle>(angle, Angle<ByteAngle>::TOP()) < 16)
			{
				pix = GR_TOP;
			}
			switch (mode)
			{
				case GR_WAITTOP:
					switch (pix)
					{
						case GR_WAIT:
							mode = GR_WAIT;
							acc = 0;
							break;
						case GR_BOTTOM:
							mode = GR_BOTTOM;
							break;
					}
					break;
				case GR_WAITBOTTOM:
					switch (pix)
					{
						case GR_WAIT:
							mode = GR_WAIT;
							acc = 0;
							break;
						case GR_TOP:
							mode = GR_TOP;
							break;
					}
					break;
				case GR_WAIT:
					switch (pix)
					{
						case GR_BOTTOM:
							mode = GR_BOTTOM;
							acc += 1;
							break;
						case GR_TOP:
							mode = GR_TOP;
							acc += 1;
							break;
					}
					break;
				case GR_TOP:
					switch (pix)
					{
						case GR_WAIT:
							toplist.push_back(acc);
							mode = GR_WAIT;
							acc = 0;
							break;
						case GR_BOTTOM:
							toplist.push_back(acc);
							mode = GR_BOTTOM;
							acc = 1;
							break;
						case GR_TOP:
							acc += 1;
							break;
					}
					break;
				case GR_BOTTOM:
					switch (pix)
					{
						case GR_WAIT:
							bottomlist.push_back(acc);
							mode = GR_WAIT;
							acc = 0;
							break;
						case GR_BOTTOM:
							acc += 1;
							break;
						case GR_TOP:
							bottomlist.push_back(acc);
							mode = GR_TOP;
							acc = 1;
							break;
					}
					break;
			}
		}
	}
	auto sumt = 0.0, sumb = 0.0;
	for (auto & elem : toplist)
		sumt += double(elem);
	if (!toplist.empty())
		sumt /= double(toplist.size());
	for (auto & elem : bottomlist)
		sumb += double(elem);
	if (!bottomlist.empty())
		sumb /= double(bottomlist.size());
	return (sumt + sumb);
}

