/* Copyright 2008-2014 INSA Lyon
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
 * file: CRNColorModel.cpp
 * \author Yann LEYDIER
 */

#include <CRNImage/CRNColorModel.h>
#include <CRNImage/CRNImage.h>
#include <CRNi18n.h>

using namespace crn;

static std::vector<int> local_max_cube(const std::vector<long> &cube)
{
	const auto size = cube.size();
	size_t GOFFSET = size;
	size_t BOFFSET = GOFFSET * size;

	// find local max
	auto mcube = std::vector<int>(size * size * size, 0);
	for (size_t b = 0; b < size; b++)
		for (size_t g = 0; g < size; g++)
			for (size_t r = 0; r < size; r++)
			{ // for each value in the cube
				long val = cube[r + g * GOFFSET + b * BOFFSET];
				for (int tb = Cap((int)b - 2, 0, (int)size - 1); tb <= Cap((int)b + 2, 0, (int)size - 1); tb++)
					for (int tg = Cap((int)g - 2, 0, (int)size - 1); tg <= Cap((int)g + 2, 0, (int)size - 1); tg++)
						for (int tr = Cap((int)r - 2, 0, (int)size - 1); tr <= Cap((int)r + 2, 0, (int)size - 1); tr++)
						{ // for each neighbor
							if ((tr == int(r)) && (tg == int(g)) && (tb == int(b)))
								continue;
							if (cube[tr + tg * GOFFSET + tb * BOFFSET] > val)
							{
								mcube[r + g * GOFFSET + b * BOFFSET] = -1;
							}
						}
			}
	bool mod = false;
	do
	{
		mod = false;
		for (size_t b = 0; b < size; b++)
			for (size_t g = 0; g < size; g++)
				for (size_t r = 0; r < size; r++)
				{ // for each value in the cube
					if (mcube[r + g * GOFFSET + b * BOFFSET] == -1)
						continue;
					long val = cube[r + g * GOFFSET + b * BOFFSET];
					for (int tb = Cap((int)b - 1, 0, (int)size - 1); tb <= Cap((int)b + 1, 0, (int)size - 1); tb++)
						for (int tg = Cap((int)g - 1, 0, (int)size - 1); tg <= Cap((int)g + 1, 0, (int)size - 1); tg++)
							for (int tr = Cap((int)r - 1, 0, (int)size - 1); tr <= Cap((int)r + 1, 0, (int)size - 1); tr++)
							{ // for each neighbor
								if ((tr == int(r)) && (tg == int(g)) && (tb == int(b)))
									continue;
								if ((cube[tr + tg * GOFFSET + tb * BOFFSET] == val) &&
										(mcube[tr + tg * GOFFSET + tb * BOFFSET] == -1))
								{
									mcube[r + g * GOFFSET + b * BOFFSET] = -1;
									mod = true;
								}
							}
				}
	} while (mod);

	return mcube;
}

/*****************************************************************************/
/*!
 * Constructor from RGB image
 * \param[in]	img	the image to analyze
 * \param[in]	max	maximum number of colors (default = 15)
 */
ColorModel::ColorModel(const ImageRGB &img, size_t max)
{
	const size_t CUBESIZE = 256;
	const size_t GOFFSET = CUBESIZE;
	const size_t BOFFSET = GOFFSET * CUBESIZE;

	// extract colorcube
	auto cube = std::vector<long>(CUBESIZE * CUBESIZE * CUBESIZE, 0);
	for (size_t tmp = 0; tmp < img.Size(); tmp++)
	{
		cube[img.At(tmp).r + img.At(tmp).g * GOFFSET + img.At(tmp).b * BOFFSET] += 1;
	}
	// search through scales
	std::vector<pixel::RGB8> colors;
	size_t bg = 0;
	for (size_t frac = 2; frac < 7; frac++)
	{
		// create scaled cube
		size_t ssize = CUBESIZE >> frac;
		size_t sGOFFSET = ssize;
		size_t sBOFFSET = sGOFFSET * ssize;
		auto scube = std::vector<long>(sGOFFSET * sBOFFSET, 0);
		for (size_t b = 0; b < CUBESIZE; b++)
			for (size_t g = 0; g < CUBESIZE; g++)
				for (size_t r = 0; r < CUBESIZE; r++)
					scube[(r >> frac) + (g >> frac) * sGOFFSET +
						(b >> frac) * sBOFFSET] += cube[r + g * GOFFSET + b * BOFFSET];

		auto mcube = local_max_cube(scube);

		// look for modes
		bg = 0;
		long bgcnt = 0;
		for (size_t b = 0; b < ssize; b++)
			for (size_t g = 0; g < ssize; g++)
				for (size_t r = 0; r < ssize; r++)
					if (mcube[r + g * sGOFFSET + b * sBOFFSET] == 0)
					{ // found a max
						// compute color
						long cnt = 0;
						long ar = 0, ag = 0, ab = 0;
						for (size_t tb = b << frac; tb < (b + 1) << frac; tb++)
							for (size_t tg = g << frac; tg < (g + 1) << frac; tg++)
								for (size_t tr = r << frac; tr < (r + 1) << frac; tr++)
								{
									cnt += cube[tr + tg * GOFFSET + tb * BOFFSET];
									ar += long(tr) * cube[tr + tg * GOFFSET + tb * BOFFSET];
									ag += long(tg) * cube[tr + tg * GOFFSET + tb * BOFFSET];
									ab += long(tb) * cube[tr + tg * GOFFSET + tb * BOFFSET];
								}
						// store color
						colors.emplace_back(uint8_t(ar / cnt), uint8_t(ag / cnt), uint8_t(ab / cnt));
						if (cnt > bgcnt)
						{
							bg = colors.size() - 1;
							bgcnt = cnt;
						}
					}

		if (colors.size() < max)
		{
			break;
		}

		colors.clear();
	}
	const auto &bgcol = colors[bg];
	std::sort(colors.begin(), colors.end(),
			[](const pixel::RGB8 &p1, const pixel::RGB8 &p2)
			{ return (p1.r + p1.g + p1.b) < (p2.r + p2.g + p2.b); }
			);
	bg = std::find(colors.begin(), colors.end(), bgcol) - colors.begin();

	for (size_t tmp = 0; tmp < colors.size(); tmp++)
	{
		if (tmp == 0)
			ink = colors[tmp];
		else if (tmp == bg)
			paper = colors[tmp];
		else
			other.push_back(colors[tmp]);
	}
}

