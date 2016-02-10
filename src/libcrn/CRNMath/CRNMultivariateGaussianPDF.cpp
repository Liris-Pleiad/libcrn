/* Copyright 2008-2015 INSA Lyon, CoReNum
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
 * file: CRNMultivariateGaussianPDF.cpp
 * \author Jean DUONG, Yann LEYDIER
 */

#include <iostream>

#include <CRNException.h>
#include <CRNMath/CRNMatrixDouble.h>
#include <CRNMath/CRNSquareMatrixDouble.h>
#include <CRNMath/CRNMultivariateGaussianPDF.h>
#include <CRNStringUTF8.h>
#include <CRNi18n.h>

using namespace crn;

/*!
 * Constructor
 *
 * \param[in] 	mu		the mean pattern
 * \param[in] 	sigma	the covariance matrix
 */
MultivariateGaussianPDF::MultivariateGaussianPDF(const MatrixDouble& mu, const SquareMatrixDouble& sigma):
	dimension(mu.GetRows()),
	mean(mu),
	variance(sigma),
	inverse_variance(sigma)
{
	updateAuxiliaryAttributes();
}

/*!
 * Destructor
 */
MultivariateGaussianPDF::~MultivariateGaussianPDF()
{
}

/*! 
 * Set the number of features
 *
 * \param[in]	d	the new dimension
 */
void MultivariateGaussianPDF::SetDimension(size_t d) noexcept
{
	dimension = d;
	mean = MatrixDouble(d, 1, 0.0);
	variance = SquareMatrixDouble(d, 0.0);
	scale_factor = 0.0;
	inverse_variance = SquareMatrixDouble(d, 0.0);
}

/*! 
 * Set the mean of a given density function
 *
 * \param[in]	obj	the new mean pattern
 */
void MultivariateGaussianPDF::SetMean(const MatrixDouble &obj)
{
	mean = obj;
}

/*! 
 * Set the variance of a given density function
 *
 * \param[in]	obj	the new covariance matrix
 */
void MultivariateGaussianPDF::SetVariance(const SquareMatrixDouble &obj)
{
	variance = obj;
	
	updateAuxiliaryAttributes();
}

/*! 
 * Evaluate a pattern
 *
 * \throws	ExceptionDimension
 *
 * \param[in]	x	the pattern to evaluate
 *
 * \return	the probability for the given pattern
 */
double MultivariateGaussianPDF::ValueAt(const MatrixDouble &x) const
{
    if ((x.GetCols() != 1) || (x.GetRows() != dimension))
        throw ExceptionDimension(StringUTF8("MultivariateGaussianPDF::ValueAt(const MatrixDouble& x): ") +
                                 _("incompatible dimensions"));
    
	// Centered input data
	std::vector<double> y(dimension);
	
	for (size_t k = 0; k < dimension; ++k)
		y[k] = x[k][0] - mean[k][0];
	
	// Exponential computation
	double expo = 0.0;
	
	for (size_t r = 0; r < dimension; ++r)
	{
		double tmp = 0.0;
        
		for (size_t c = 0; c < dimension; ++c)
			tmp += inverse_variance[r][c] * y[c];
		
		expo += tmp * y[r];
	}
	
	expo /= -2.0;
	
	return scale_factor * exp(expo);
}

/*! 
 * Evaluate a pattern
 *
 * \throws	ExceptionDimension
 *
 * \param[in]	x	the pattern to evaluate
 *
 * \return	the probability for the given pattern
 */
double MultivariateGaussianPDF::ValueAt(const std::vector<double> &x) const
{	
	if (x.size() != dimension)
		throw ExceptionDimension(StringUTF8("MultivariateGaussianPDF::ValueAt(const std::vector<double> x): ") +
				_("incompatible dimensions"));
	
	// Centered input data
	std::vector<double> y(dimension);
	
	for (size_t k = 0; k < dimension; ++k)
		y[k] = x[k] - mean[k][0];
	
	// Exponential computation
	double expo = 0.0;
	
	for (size_t r = 0; r < dimension; ++r)
	{
		double tmp = 0.0;
		
		for (size_t c = 0; c < dimension; ++c)
			tmp += inverse_variance[r][c] * y[c];
		
		expo += tmp * y[r];
	}
	
	expo /= -2.0;	
	
	return scale_factor * exp(expo);
}

/*!
 * Internal
 */
void MultivariateGaussianPDF::updateAuxiliaryAttributes()
{
	scale_factor = 1.0 / (pow(2.0 * M_PI, double(dimension) / 2.0) *  sqrt(variance.Determinant()));
	inverse_variance = variance.MakeInverse();
}

/*! 
 * Dumps a summary to a string
 *
 * \return	the string
 */
String MultivariateGaussianPDF::ToString() const
{
	String s;

	s += U"Mean:\n";
	s += mean.ToString();
	s += U"\nVariance:\n";
	s += variance.ToString();
	
	return s;
}

/*! 
 * Check if PDF is valid (with finite values)
 *
 * \return	true if success, false else
 */
bool MultivariateGaussianPDF::IsValid() const
{
	if (dimension <= 1.0)
		return false;
	
	for (size_t r = 0; r < dimension; ++r)
	{
		double mk = mean[r][0];
        
		if (!(mk == mk))
			return false;
	}
	
	for (size_t r = 0; r < dimension; ++r)
		for (size_t c = 0; c < dimension; ++c)
		{
			double vrc = variance[r][c];
			
			if (!(vrc == vrc))
				return false;
		}
	
	return true;
}
