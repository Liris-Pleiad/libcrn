/* Copyright 2006-2015 Yann LEYDIER, CoReNum
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
 * file: CRNMatrixInt.cpp
 * \author Yann LEYDIER, Jean DUONG
 */

#include <CRNi18n.h>
#include <CRNMath/CRNMatrixInt.h>
#include <CRNMath/CRNMath.h>
#include <CRNException.h>
#include <CRNStringUTF8.h>

using namespace crn;

// Special matrix constructors
#define MAX_Gauss_W 40
#define MULT 10

/*! 
 * Create a Gaussian mask given standard deviation
 *
 * \throws	ExceptionDomain	sigma is negative
 *
 * \param[in]	sigma	the standard deviation of the Gaussian
 *
 * \return	a new matrix
 */
MatrixInt MatrixInt::NewGaussian(double sigma)
{
	if (sigma < 0)
		throw ExceptionDomain(StringUTF8("MatrixInt::NewGaussian(double sigma): ") + 
				_("Negative standard deviation"));
	if (sigma <= 0.001)
		return MatrixInt(1, 1, 1);
	size_t d = (int)ceil(Thrice(sigma));

	MatrixInt mat(d + d + 1, d + d + 1);

	// fill central column
	for (size_t r = 0; r < d; r++)
		mat[r][d] = (int)(MULT * MeanGauss(double(d) - double(r), sigma));
	for (size_t r = d; r < mat.rows; r++)
		mat[r][d] = (int)(MULT * MeanGauss(double(r) - double(d), sigma));
	// fill central row
	for (size_t c = 0; c < d; c++)
		mat[d][c] = (int)(MULT * MeanGauss(double(d) - double(c), sigma));
	for (size_t c = d; c < mat.cols; c++)
		mat[d][c] = (int)(MULT * MeanGauss(double(c) - double(d), sigma));
	// fill first quadrant
	for (size_t r = 0; r < d; r++)
		for (size_t c = d + 1; c < mat.cols; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill second quadrant
	for (size_t r = 0; r < d; r++)
		for (size_t c = 0; c < d; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill third quadrant
	for (size_t r = d + 1; r < mat.rows; r++)
		for (size_t c = 0; c < d; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill fourth quadrant
	for (size_t r = d + 1; r < mat.rows; r++)
		for (size_t c = d + 1; c < mat.cols; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;

	return mat;
}

/*! 
 * Create a 3x3 Gaussian mask
 *
 * \return	a new matrix
 */
MatrixInt MatrixInt::NewGaussian3()
{
	MatrixInt mat(3, 3, 0);
	
	mat[0][0] = 1;
	mat[1][0] = 2;
	mat[2][0] = 1;
	mat[0][1] = 2;
	mat[1][1] = 4;
	mat[2][1] = 2;
	mat[0][2] = 1;
	mat[1][2] = 2;
	mat[2][2] = 1;

	return mat;
}

/*! 
 * Create a Gaussian Sobel X derivation mask
 *
 * \throws	ExceptionDomain	sigma is negative
 *
 * \param[in]	sigma	the standard deviation of the Gaussian
 *
 * \return	a new matrix
 */
MatrixInt MatrixInt::NewGaussianSobelX(double sigma)
{
	if (sigma < 0)
		throw ExceptionDomain(StringUTF8("MatrixInt::NewGaussianSobelX(double sigma): ") + 
				_("Negative standard deviation"));
	if (sigma <= 0.001)
		return NewSobelX();
	size_t d = (int)ceil(Thrice(sigma));

	MatrixInt mat(d + d + 1, d + d + 1);

	// fill central column
	for (size_t r = 0; r < d; r++)
		mat[r][d] = (int)(MULT * MeanGauss(double(d) - double(r), sigma));
	for (size_t r = d; r < mat.rows; r++)
		mat[r][d] = (int)(MULT * MeanGauss(double(r) - double(d), sigma));
	// fill central row
	for (size_t c = 0; c < d; c++)
		mat[d][c] = (int)(MULT * MeanGauss(double(d) - double(c), sigma));
	for (size_t c = d; c < mat.cols; c++)
		mat[d][c] = (int)(MULT * MeanGauss(double(c) - double(d), sigma));
	// fill first quadrant
	for (size_t r = 0; r < d; r++)
		for (size_t c = d + 1; c < mat.cols; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill second quadrant
	for (size_t r = 0; r < d; r++)
		for (size_t c = 0; c < d; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill third quadrant
	for (size_t r = d + 1; r < mat.rows; r++)
		for (size_t c = 0; c < d; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill fourth quadrant
	for (size_t r = d + 1; r < mat.rows; r++)
		for (size_t c = d + 1; c < mat.cols; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;

	// derivate
	for (size_t r = 0; r < mat.rows; r++)
		for (size_t c = 0; c < mat.cols; c++)
			mat[r][c] *= int(d) - int(c);

	return mat;
}

/*! 
 * Create a Gaussian Sobel Y derivation mask
 *
 * \throws	ExceptionDomain	sigma is negative
 *
 * \param[in]	sigma	the standard deviation of the Gaussian
 *
 * \return	a new matrix
 */
MatrixInt MatrixInt::NewGaussianSobelY(double sigma)
{
	if (sigma < 0)
		throw ExceptionDomain(StringUTF8("MatrixInt::NewGaussianSobelY(double sigma): ") + 
				_("Negative standard deviation"));
	if (sigma <= 0.001)
		return NewSobelY();
	size_t d = (int)ceil(Thrice(sigma));

	MatrixInt mat(d + d + 1, d + d + 1);

	// fill central column
	for (size_t r = 0; r < d; r++)
		mat[r][d] = (int)(MULT * MeanGauss(double(d) - double(r), sigma));
	for (size_t r = d; r < mat.rows; r++)
		mat[r][d] = (int)(MULT * MeanGauss(double(r) - double(d), sigma));
	// fill central row
	for (size_t c = 0; c < d; c++)
		mat[d][c] = (int)(MULT * MeanGauss(double(d) - double(c), sigma));
	for (size_t c = d; c < mat.cols; c++)
		mat[d][c] = (int)(MULT * MeanGauss(double(c) - double(d), sigma));
	// fill first quadrant
	for (size_t r = 0; r < d; r++)
		for (size_t c = d + 1; c < mat.cols; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill second quadrant
	for (size_t r = 0; r < d; r++)
		for (size_t c = 0; c < d; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill third quadrant
	for (size_t r = d + 1; r < mat.rows; r++)
		for (size_t c = 0; c < d; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill fourth quadrant
	for (size_t r = d + 1; r < mat.rows; r++)
		for (size_t c = d + 1; c < mat.cols; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// derivate
	for (size_t r = 0; r < mat.rows; r++)
		for (size_t c = 0; c < mat.cols; c++)
			mat[r][c] *= int(d) - int(r);

	return mat;
}

/*! 
 * Create a Sobel X derivation mask
 *
 * \return	a new matrix
 */
MatrixInt MatrixInt::NewSobelX()
{
	MatrixInt mat(3, 3, 0);

	mat[0][0] = 1;
	mat[1][0] = 2;
	mat[2][0] = 1;
	mat[0][2] = -1;
	mat[1][2] = -2;
	mat[2][2] = -1;

	return mat;
}

/*! 
 * Create a Sobel Y derivation mask
 *
 * \return	a new matrix
 */
MatrixInt MatrixInt::NewSobelY()
{
	MatrixInt mat(3, 3, 0);

	mat[0][0] = 1;
	mat[0][1] = 2;
	mat[0][2] = 1;
	mat[2][0] = -1;
	mat[2][1] = -2;
	mat[2][2] = -1;

	return mat;
}

/*! 
 * Create the first D4 DT mask
 *
 * \return	a new matrix
 */
MatrixInt MatrixInt::NewD4DT1()
{
	MatrixInt m(3, 3, -1);
	m[0][1] = 1;
	m[1][0] = 1;
	return m;
}

/*! 
 * Create the second D4 DT mask
 *
 * \return	a new matrix
 */
MatrixInt MatrixInt::NewD4DT2()
{
	MatrixInt m(3, 3, -1);
	m[1][1] = 0;
	m[2][1] = 1;
	m[1][2] = 1;
	return m;
}

/*! 
 * Create the first D8 DT mask
 *
 * \return	a new matrix
 */
MatrixInt MatrixInt::NewD8DT1()
{
	MatrixInt m(3, 3, -1);
	m[0][0] = 1;
	m[0][1] = 1;
	m[0][2] = 1;
	m[1][0] = 1;
	return m;
}

/*! 
 * Create the second D8 DT mask
 *
 * \return	a new matrix
 */
MatrixInt MatrixInt::NewD8DT2()
{
	MatrixInt m(3, 3, -1);
	m[1][1] = 0;
	m[1][2] = 1;
	m[2][0] = 1;
	m[2][1] = 1;
	m[2][2] = 1;
	return m;
}

/*! 
 * Create the first chamfer DT mask
 *
 * \return	a new matrix
 */
MatrixInt MatrixInt::NewChamferDT1()
{
	MatrixInt m(3, 3, -1);
	m[0][0] = 4;
	m[0][1] = 3;
	m[0][2] = 4;
	m[1][0] = 3;
	return m;
}

/*! 
 * Create the second chamfer DT mask
 *
 * \return	a new matrix
 */
MatrixInt MatrixInt::NewChamferDT2()
{
	MatrixInt m(3, 3, -1);
	m[1][1] = 0;
	m[1][2] = 3;
	m[2][0] = 4;
	m[2][1] = 3;
	m[2][2] = 4;
	return m;
}


