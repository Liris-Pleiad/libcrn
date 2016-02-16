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
 * file: CRNGradientMatching.cpp
 * \author Yann LEYDIER
 */

#include <CRNFeature/CRNGradientMatching.h>
#include <CRNImage/CRNImageGradient.h>
#include <CRNImage/CRNImageGray.h>
#include <CRNImage/CRNImageBW.h>

using namespace crn;

GradientModel::GradientModel(const ImageGradient &igr):
	angles(ThetaChannel(igr)),
	mask(Threshold(RhoChannel(igr), igr.GetMinModule())),
	xbar(0),
	ybar(0)
{
	dmask = mask;
	auto sel = MatrixInt(3, 3, 1);
	sel[0][0] = sel[2][0] = sel[0][2] = sel[2][2] = 0;
	dmask.Erode(sel);
	auto npix = 0;
	FOREACHPIXEL(x, y, mask)
		if (mask.At(x, y))
		{
			npix += 1;
			xbar += int(x);
			ybar += int(y);
		}
	if (npix)
	{
		xbar /= npix;
		ybar /= npix;
	}
	else
	{
		xbar = int(igr.GetWidth()) / 2;
		ybar = int(igr.GetHeight()) / 2;
	}
}

static int diffgrad(const GradientModel &c1, const GradientModel &c2, int win) noexcept
{
	const auto deltax = c1.xbar - c2.xbar;
	const auto deltay = c1.ybar - c2.ybar;
	auto mindiff = std::numeric_limits<int>::max();

	for (auto y_d = deltay - win; y_d <= deltay + win; ++y_d)
		for (auto x_d = deltax - win; x_d <= deltax + win; ++x_d)
		{
			auto diff = 0;
			for (auto y = 0; y < int(c1.mask.GetHeight()); ++y)
			{
				for (auto x = 0; x < int(c1.mask.GetWidth()); ++x)
				{
					if (!c1.mask.At(x, y))
						continue;
					int d;
					if ((x >= x_d) && (x < x_d + int(c2.dmask.GetWidth())) &&
							(y >= y_d) && (y < y_d + int(c2.dmask.GetHeight())) &&
							c2.dmask.At(x - x_d, y - y_d))
					{
						d = AngularDistance(Angle<ByteAngle>(c1.angles.At(x, y)), Angle<ByteAngle>(c2.angles.At(x - x_d, y - y_d)));
					}
					else
					{
						d = 128;
					}
					diff += d;
					if (diff > mindiff)
						break;
				}
				if (diff > mindiff)
					break;
			}

			if (diff < mindiff)
			{
				mindiff = diff;
			}
		}
	return mindiff;
}

double GradientModel::Distance(const GradientModel &img1, const GradientModel &img2, size_t window) noexcept
{
	auto npix1 = 0;
	for (auto p : img1.mask)
		if (p)
			npix1 += 1;
	if (npix1 == 0) npix1 = 1;
	auto npix2 = 0;
	for (auto p : img2.mask)
		if (p)
			npix2 += 1;
	if (npix2 == 0) npix2 = 1;
	auto ndiff1 = diffgrad(img1, img2, int(window));
	auto ndiff2 = diffgrad(img2, img1, int(window));
	return Max(double(ndiff1) / npix1, double(ndiff2) / npix2);
}

