/* Copyright 2011-2014 CoReNum, INSA-Lyon
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
 * file: CRNPolynomialRegression.cpp
 * \author Yann LEYDIER
 */

#include <CRNMath/CRNPolynomialRegression.h>
#include <CRNMath/CRNSquareMatrixDouble.h>

using namespace crn;

/*! Translates the polynomial
 * \param[in]	increment	the vertical shift
 */
void PolynomialRegression::TranslateY(int increment)
{
	coefficients[0] += increment;
	for (Point2DDouble &p : data)
	{
		p.Y += increment;
	}
}

/*! Computes the coefficients of the spline */
void PolynomialRegression::computeCoeffs()
{
	std::sort(data.begin(), data.end(), Point2DDouble::Comparer(Direction::LEFT));
	// create matrices
	MatrixDouble X(data.size(), dimension + 1);
	MatrixDouble Y(data.size(), size_t(1));
	for (size_t tmp = 0; tmp < data.size(); ++tmp)
	{
		X[tmp][0] = 1;
		for (size_t d = 1; d <= dimension; ++d)
		{
			X[tmp][d] = X[tmp][d - 1] * data[tmp].X;
		}
		Y[tmp][0] = data[tmp].Y;
	}
	// Y = X ⋅ A + ε ⇒ Â = (tX ⋅ X)^-1 ⋅ tX ⋅ Y
	SquareMatrixDouble tmpMat(X.MakeCovariance());
	tmpMat.Mult((double)data.size()); // covariance is normalized, we don't want this
	tmpMat = tmpMat.MakeGaussJordanInverse();
	X.Transpose();
	MatrixDouble res(tmpMat);
	res.Mult(X);
	res.Mult(Y);
	for (size_t d = 0; d <= dimension; ++d)
	{
		coefficients[d] = res[d][0];
	}
}

double PolynomialRegression::operator[](double x) const
{
	if (extrapolation == Extrapolation::LINEAR)
	{
		if (x < data.front().X)
		{
			double x0 = data[0].X;
			double x1 = x0 + (data[1].X - x0) / 5;
			double y0 = (*this)[x0];
			double y1 = (*this)[x1];
			double slope = (y1 - y0) / (x1 - x0);
			return y0 + slope * (x - x0);
		}
		if (x > data.back().X)
		{
			double xn = data.back().X;
			double xn1 = xn - (xn - data[data.size() - 2].X) / 5;
			double yn = (*this)[xn];
			double yn1 = (*this)[xn1];
			double slope = (yn - yn1) / (xn - xn1);
			return yn + slope * (x - xn);
		}
	}
	double y = coefficients[0];
	double xx = 1;
	for (size_t d = 1; d <= dimension; ++d)
	{
		xx *= x;
		y += coefficients[d] * xx;
	}
	return y;
}

