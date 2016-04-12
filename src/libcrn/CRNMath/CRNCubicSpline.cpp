/* Copyright 2011 CoReNum
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
 * file: CRNCubicSpline.cpp
 * \author Yann LEYDIER
 */

#include <CRNMath/CRNCubicSpline.h>
#include <CRNProtocols.h>

using namespace crn;

/*! Computes the coefficients of the spline */
void CubicSpline::computeCoeffs()
{
	std::sort(data.begin(), data.end(), Point2DDouble::Comparer(Direction::LEFT));
	// compute h and b
	std::vector<double> h(data.size() - 1);
	std::vector<double> b(h.size());
	for (size_t tmp = 0; tmp < h.size(); ++tmp)
	{
		h[tmp] = data[tmp + 1].X - data[tmp].X;
		b[tmp] = (data[tmp + 1].Y - data[tmp].Y) / h[tmp];
	}
	// Gaussian elimination
	std::vector<double> u(h.size());
	std::vector<double> v(h.size());
	u[1] = 2 * (h[0] + h[1]);
	v[1] = 6 * (b[1] - b[0]);
	for (size_t tmp = 2; tmp < u.size(); ++tmp)
	{
		u[tmp] = 2 * (h[tmp - 1] + h[tmp]) - crn::Sqr(h[tmp - 1]) / u[tmp - 1];
		v[tmp] = 6 * (b[tmp] - b[tmp - 1]) - h[tmp - 1] * v[tmp - 1] / u[tmp - 1];
	}
	// back substitution
	z.resize(data.size());
	z[z.size() - 1] = 0;
	for (int tmp = int(z.size()) - 2; tmp > 0; --tmp)
	{
		z[tmp] = (v[tmp] - h[tmp] * z[tmp + 1]) / u[tmp];
	}
	z[0] = 0;
}

double CubicSpline::operator[](double x) const
{
	if (extrapolation == Extrapolation::LINEAR)
	{
		if (x < data.front().X)
		{
			double dx = data[1].X - data[0].X;
			dx /= 10.0;
			if (dx > 0.01)
				dx = 0.01;
			double dy = (*this)[data.front().X + dx] - data.front().Y;
			double slope = dy / dx;
			return data.front().Y + slope * (x - data.front().X);
		}
		if (x > data.back().X)
		{
			double dx = data[data.size() - 1].X - data[data.size() - 2].X;
			dx /= 10.0;
			if (dx > 0.01)
				dx = 0.01;
			double dy = data.back().Y - (*this)[data.back().X - dx];
			double slope = dy / dx;
			return data.back().Y + slope * (x - data.back().X);
		}
	}
	size_t i;
	if (x >= data.back().X)
		i = z.size() - 2;
	else
	{
		for (i = 0; i < z.size() - 1; ++i)
		{
			if (x <= data[i + 1].X)
				break;
		}
	}
	double h = data[i + 1].X - data[i].X;
	double a = data[i].Y;
	double b = h * z[i + 1] / -6.0 - h * z[i] / 3.0 + (data[i + 1].Y - data[i].Y) / h;
	double c = z[i] / 2.0;
	double d = (z[i + 1] - z[i]) / (6.0 * h);
	double dx = x - data[i].X;
	return a + dx * (b + dx * (c + dx * d));
}

CRN_BEGIN_CLASS_CONSTRUCTOR(CubicSpline)
	Cloner::Register<CubicSpline>();
CRN_END_CLASS_CONSTRUCTOR(CubicSpline)
